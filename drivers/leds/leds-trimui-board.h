/* SPDX-License-Identifier: GPL-2.0-only */
/* Included by leds-sunxi.c so the original LEDC transport remains shared. */
#include <linux/gpio.h>
#include <linux/kthread.h>
#include "leds-trimui-effects.h"
#include "leds-trimui-help.h"

extern int trimui_axp2202_get_bat_cap(void);
extern int trimui_set_speaker_enable(int enable);
extern int trimui_speaker_mute;

enum { EFFECT_M, EFFECT_L, EFFECT_R, EFFECT_F1, EFFECT_F2, EFFECT_COUNT };
static struct trimui_led_effect trimui_effect[EFFECT_COUNT];
static u32 trimui_frame[128], trimui_wire_frame[128];
static u32 trimui_anim_frames[14400];
static unsigned int trimui_limit = 128, trimui_scale_m = 40;
static unsigned int trimui_scale_lr = 40, trimui_scale_f = 40;
static int trimui_led_enable = 1, trimui_effect_enable;
static int trimui_frames_enable, trimui_frames_cycles, trimui_frames_fps;
static int trimui_frames_len, trimui_frame_index, trimui_cycles_left;
static int trimui_mask_m, trimui_mask_lr, trimui_mask_f;
static int trimui_low_battery, trimui_suspended, trimui_motor_voltage;
static int trimui_mute_requested;
static struct regulator *trimui_motor_regulator;
static struct task_struct *trimui_led_task;
static DEFINE_MUTEX(trimui_frame_lock);

static void check_battery(void)
{
	trimui_low_battery = trimui_axp2202_get_bat_cap() < 16;
}

static void commit_static_frame(void)
{
	struct sunxi_led *led = sunxi_led;
	unsigned int i, scale, rgb, r, g, b;
	unsigned long flags;
	mutex_lock(&trimui_frame_lock);
	for (i = 0; i < led->led_count && i < 128; i++) {
		scale = i < 8 ? trimui_scale_m : i < 10 ? trimui_scale_f : trimui_scale_lr;
		if (trimui_low_battery && scale > 3)
			scale = 3;
		rgb = trimui_frame[i];
		r = (((rgb >> 16) & 255) * scale) >> 8;
		g = (((rgb >> 8) & 255) * scale) >> 8;
		b = ((rgb & 255) * scale) >> 8;
		trimui_wire_frame[i] = i < 10 ? (g << 16) | (r << 8) | b :
			(r << 16) | (g << 8) | b;
	}
	if (trimui_suspended || !trimui_led_enable)
		memset(trimui_wire_frame, 0, sizeof(trimui_wire_frame));
	spin_lock_irqsave(&led->lock, flags);
	led->length = min_t(unsigned int, led->led_count, 128);
	memcpy(led->data, trimui_wire_frame, led->length * sizeof(u32));
	spin_unlock_irqrestore(&led->lock, flags);
	sunxi_ledc_trans_data(led);
	sunxi_ledc_complete(led);
	mutex_unlock(&trimui_frame_lock);
}

static int trimui_ledc_run_thread(void *unused)
{
	unsigned int i, battery_ticks = 0;
	u32 rgb;
	trimui_scale_m = 14;
	trimui_effect_enable = 1;
	/* The Brick transmits 14 physical RGB packages; the shared DT says 23. */
	sunxi_led->led_count = 14;
	for (i = 0; i < EFFECT_COUNT; i++) {
		struct trimui_led_effect *e = &trimui_effect[i];
		e->type = 2;
		e->duration = 2000;
		e->cycles = 5;
		e->target_rgb = i >= EFFECT_F1 ? 0x00ff33 : 0xffffff;
		gen_effect(e, 2, e->target_rgb, e->target_rgb);
		e->active_cycles = e->cycles;
	}
	while (!kthread_should_stop()) {
		if (trimui_effect_enable) {
			for (i = 0; i < EFFECT_COUNT; i++) {
				if (!is_effect_actived(&trimui_effect[i]))
					continue;
				rgb = step_one_frame(&trimui_effect[i]);
				switch (i) {
				case EFFECT_M: {
					unsigned int j;
					for (j = 0; j < 8; j++) trimui_frame[j] = rgb;
					break;
				}
				case EFFECT_L: trimui_frame[10] = trimui_frame[11] = rgb; break;
				case EFFECT_R: trimui_frame[12] = trimui_frame[13] = rgb; break;
				case EFFECT_F1: trimui_frame[9] = rgb; break;
				case EFFECT_F2: trimui_frame[8] = rgb; break;
				}
			}
			commit_static_frame();
		} else if (trimui_frames_enable) {
			if (trimui_frames_len > 0 && trimui_frames_len <= 14400 / 14) {
				u32 *frame = trimui_anim_frames + trimui_frame_index * 14;
				if (trimui_mask_m) memcpy(trimui_frame, frame, 8 * sizeof(u32));
				if (trimui_mask_f) memcpy(trimui_frame + 8, frame + 8, 2 * sizeof(u32));
				if (trimui_mask_lr) memcpy(trimui_frame + 10, frame + 10, 4 * sizeof(u32));
				if (++trimui_frame_index >= trimui_frames_len) {
					trimui_frame_index = 0;
					if (trimui_cycles_left > 0) trimui_cycles_left--;
					else if (trimui_cycles_left >= -1) {
						trimui_frames_cycles = 0;
						trimui_frames_len = 0;
					}
				}
				commit_static_frame();
			}
		}
		if (battery_ticks++ > 625) {
			battery_ticks = 0;
			check_battery();
		}
		msleep(16);
	}
	return 0;
}

static int trimui_effect_index(const char *suffix)
{
	if (!strcmp(suffix, "m")) return EFFECT_M;
	if (!strcmp(suffix, "l") || !strcmp(suffix, "lr")) return EFFECT_L;
	if (!strcmp(suffix, "r")) return EFFECT_R;
	if (!strcmp(suffix, "f1")) return EFFECT_F1;
	if (!strcmp(suffix, "f2")) return EFFECT_F2;
	return -1;
}

static int *trimui_integer_attribute(const char *name)
{
	if (!strcmp(name, "enable")) return &trimui_led_enable;
	if (!strcmp(name, "effect_enable")) return &trimui_effect_enable;
	if (!strcmp(name, "anim_frames_len")) return &trimui_frames_len;
	if (!strcmp(name, "anim_frames_fps")) return &trimui_frames_fps;
	if (!strcmp(name, "anim_frames_cycles")) return &trimui_frames_cycles;
	if (!strcmp(name, "anim_frames_enable")) return &trimui_frames_enable;
	if (!strcmp(name, "anim_frames_mask_m_enable")) return &trimui_mask_m;
	if (!strcmp(name, "anim_frames_mask_lr_enable")) return &trimui_mask_lr;
	if (!strcmp(name, "anim_frames_mask_f1f2_enable")) return &trimui_mask_f;
	return NULL;
}

static ssize_t trimui_led_attr_show(struct class *class,
				  struct class_attribute *attr, char *buffer)
{
	const char *name = attr->attr.name, *suffix;
	struct trimui_led_effect *e;
	int *value, index, i, count = 0;
	if (!strcmp(name, "tg5040_exterme_brightness")) return sprintf(buffer, "%u\n", trimui_limit);
	if (!strcmp(name, "max_scale")) return sprintf(buffer, "%u\n", trimui_scale_m);
	if (!strcmp(name, "max_scale_lr")) return sprintf(buffer, "%u\n", trimui_scale_lr);
	if (!strcmp(name, "max_scale_f1f2")) return sprintf(buffer, "%u\n", trimui_scale_f);
	if (!strcmp(name, "voltage")) return sprintf(buffer, "%d\n",
		IS_ERR_OR_NULL(trimui_motor_regulator) ? 0 : regulator_get_voltage(trimui_motor_regulator));
	if (!strcmp(name, "mute")) return sprintf(buffer, "%d\n", trimui_mute_requested);
	value = trimui_integer_attribute(name);
	if (value) return sprintf(buffer, "%d\n", *value);
	if (!strcmp(name, "frame") || !strcmp(name, "anim_frames")) {
		count = sunxi_led->led_count * 4;
		memcpy(buffer, !strcmp(name, "frame") ? trimui_frame : trimui_anim_frames, count);
		return count;
	}
	if (!strcmp(name, "frame_hex") || !strcmp(name, "anim_frames_hex")) {
		u32 *data = !strcmp(name, "frame_hex") ? trimui_frame : trimui_anim_frames;
		for (i = 0; i < sunxi_led->led_count; i++)
			count += sprintf(buffer + count, "%06X ", data[i] & 0xffffff);
		return count + sprintf(buffer + count, "\n");
	}
	if (!strcmp(name, "effect_names"))
		return sprintf(buffer, "TRIMUI LED Animation Effect names:\n0 - disable\n1 - linear\n2 - breath\n3 - sniff\n4 - static\n5 - blink1\n6 - blink2\n7 - blink3\n");
	if (!strcmp(name, "help")) {
		sprintf(buffer, "%s\n", trimui_led_help);
		return sizeof(trimui_led_help);
	}
	if (!strncmp(name, "effect_duration_", 16)) suffix = name + 16;
	else if (!strncmp(name, "effect_cycles_", 14)) suffix = name + 14;
	else if (!strncmp(name, "effect_rgb_hex_", 15)) suffix = name + 15;
	else if (!strncmp(name, "effect_", 7)) suffix = name + 7;
	else return -EINVAL;
	index = trimui_effect_index(suffix);
	if (index < 0) return -EINVAL;
	e = &trimui_effect[index];
	if (!strncmp(name, "effect_duration_", 16)) return sprintf(buffer, "%d\n", e->duration);
	if (!strncmp(name, "effect_cycles_", 14)) return sprintf(buffer, "%d\n", e->cycles);
	if (!strncmp(name, "effect_rgb_hex_", 15)) return sprintf(buffer, "%06X \n", e->target_rgb & 0xffffff);
	return sprintf(buffer, "%d\n", e->type);
}

static ssize_t trimui_led_attr_store(struct class *class,
		struct class_attribute *attr, const char *buffer, size_t count)
{
	const char *name = attr->attr.name, *suffix;
	int *value, parsed, index, i, last, colors;
	unsigned int rgb;
	if (!strcmp(name, "frame") || !strcmp(name, "anim_frames")) {
		u32 *data = !strcmp(name, "frame") ? trimui_frame : trimui_anim_frames;
		size_t capacity = !strcmp(name, "frame") ? sunxi_led->led_count * 4 : sizeof(trimui_anim_frames);
		memcpy(data, buffer, min(count, capacity));
		return count;
	}
	if (!strcmp(name, "frame_hex") || !strcmp(name, "anim_frames_hex")) {
		int animation = !strcmp(name, "anim_frames_hex");
		u32 *data = animation ? trimui_anim_frames : trimui_frame;
		int offset = animation ? max(trimui_frames_len, 0) * 14 : 0;
		int capacity = animation ? 14400 : sunxi_led->led_count;
		colors = min_t(size_t, count / 7, max(capacity - offset, 0));
		for (i = 0; i < colors; i++) {
			rgb = 0;
			sscanf(buffer + 7 * i, "%x", &rgb);
			data[offset + i] = rgb;
		}
		if (animation) trimui_frames_len += colors / 14;
		return count;
	}
	if (strncmp(name, "effect_rgb_hex_", 15) && kstrtoint(buffer, 0, &parsed)) return count;
	if (!strcmp(name, "tg5040_exterme_brightness")) { trimui_limit = min_t(unsigned int, parsed, 255); return count; }
	if (!strcmp(name, "max_scale")) { trimui_scale_m = min_t(unsigned int, parsed, trimui_limit); return count; }
	if (!strcmp(name, "max_scale_lr")) { trimui_scale_lr = min_t(unsigned int, parsed, trimui_limit); return count; }
	if (!strcmp(name, "max_scale_f1f2")) { trimui_scale_f = min_t(unsigned int, parsed, trimui_limit); return count; }
	if (!strcmp(name, "voltage")) {
		trimui_motor_voltage = parsed;
		if (!IS_ERR_OR_NULL(trimui_motor_regulator)) regulator_set_voltage(trimui_motor_regulator, parsed, parsed + 100000);
		return count;
	}
	if (!strcmp(name, "mute")) {
		trimui_mute_requested = parsed;
		trimui_speaker_mute = !!parsed;
		trimui_set_speaker_enable(!parsed);
		return count;
	}
	value = trimui_integer_attribute(name);
	if (value) {
		*value = parsed;
		if (value == &trimui_frames_enable) {
			trimui_frame_index = 0; trimui_cycles_left = trimui_frames_cycles - 1;
			if (!parsed) trimui_frames_len = 0;
		} else if (value == &trimui_frames_cycles) {
			trimui_frames_enable = 0; trimui_frame_index = trimui_cycles_left = 0;
		}
		return count;
	}
	if (!strncmp(name, "effect_rgb_hex_", 15)) {
		rgb = 0;
		sscanf(buffer, "%x", &rgb);
		suffix = name + 15;
	} else if (!strncmp(name, "effect_duration_", 16)) suffix = name + 16;
	else if (!strncmp(name, "effect_cycles_", 14)) suffix = name + 14;
	else if (!strncmp(name, "effect_", 7)) suffix = name + 7;
	else return count;
	index = trimui_effect_index(suffix);
	if (index < 0) return count;
	last = !strcmp(suffix, "lr") ? EFFECT_R : index;
	for (i = index; i <= last; i++) {
		struct trimui_led_effect *e = &trimui_effect[i];
		if (!strncmp(name, "effect_duration_", 16)) e->duration = parsed;
		else if (!strncmp(name, "effect_cycles_", 14)) e->cycles = parsed;
		else if (!strncmp(name, "effect_rgb_hex_", 15)) e->target_rgb = rgb;
		else {
			e->type = (unsigned int)parsed > 7 ? 0 : parsed;
			gen_effect(e, e->type, e->target_rgb, e->current_rgb);
			e->index = 0; e->active_cycles = e->cycles;
		}
	}
	return count;
}

static struct class_attribute trimui_led_anim_attrs[] = {
	__ATTR(help, 0664, trimui_led_attr_show, NULL),
	__ATTR(enable, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_names, 0664, trimui_led_attr_show, NULL),
	__ATTR(max_scale, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(max_scale_lr, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(max_scale_f1f2, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(frame, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(frame_hex, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_duration_m, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_duration_l, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_duration_r, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_duration_f1, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_duration_f2, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_duration_lr, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_cycles_m, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_cycles_l, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_cycles_r, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_cycles_f1, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_cycles_f2, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_cycles_lr, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_rgb_hex_m, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_rgb_hex_l, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_rgb_hex_r, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_rgb_hex_f1, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_rgb_hex_f2, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_rgb_hex_lr, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_m, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_l, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_r, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_f1, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_f2, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_lr, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(effect_enable, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(anim_frames, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(anim_frames_hex, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(anim_frames_len, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(anim_frames_fps, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(anim_frames_cycles, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(anim_frames_enable, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(anim_frames_mask_m_enable, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(anim_frames_mask_f1f2_enable, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR(anim_frames_mask_lr_enable, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR_NULL,
};
static struct class trimui_led_anim_class = {
	.name = "led_anim", .owner = THIS_MODULE, .class_attrs = trimui_led_anim_attrs,
};
static struct class_attribute trimui_motor_attrs[] = {
	__ATTR(voltage, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR_NULL,
};
static struct class trimui_motor_class = {
	.name = "motor", .owner = THIS_MODULE, .class_attrs = trimui_motor_attrs,
};
static struct class_attribute trimui_speaker_attrs[] = {
	__ATTR(mute, 0664, trimui_led_attr_show, trimui_led_attr_store),
	__ATTR_NULL,
};
static struct class trimui_speaker_class = {
	.name = "speaker", .owner = THIS_MODULE, .class_attrs = trimui_speaker_attrs,
};

static void trimui_board_exit(void)
{
	if (!IS_ERR_OR_NULL(trimui_led_task)) {
		kthread_stop(trimui_led_task);
		trimui_led_task = NULL;
	}
	class_unregister(&trimui_speaker_class);
	class_unregister(&trimui_motor_class);
	class_unregister(&trimui_led_anim_class);
	if (!IS_ERR_OR_NULL(trimui_motor_regulator))
		regulator_put(trimui_motor_regulator);
	trimui_motor_regulator = NULL;
	gpio_direction_output(107, 0);
	gpio_free(107);
}

static int trimui_board_init(void)
{
	unsigned int i;
	int result;
	for (i = 0; i < EFFECT_COUNT; i++) {
		trimui_effect[i].duration = 2000;
		trimui_effect[i].cycles = 1;
	}
	result = gpio_request(107, "led_power");
	if (result)
		return result;
	gpio_direction_output(107, 1);
	result = class_register(&trimui_led_anim_class);
	if (result)
		goto gpio;
	result = class_register(&trimui_motor_class);
	if (result)
		goto animation;
	trimui_motor_regulator = regulator_get(NULL, "axp2202-bldo3");
	result = class_register(&trimui_speaker_class);
	if (result)
		goto motor;
	trimui_led_task = kthread_run(trimui_ledc_run_thread, NULL, "trimui_ledc");
	if (!IS_ERR(trimui_led_task))
		return 0;
	result = PTR_ERR(trimui_led_task);
	class_unregister(&trimui_speaker_class);
motor:
	if (!IS_ERR_OR_NULL(trimui_motor_regulator))
		regulator_put(trimui_motor_regulator);
	trimui_motor_regulator = NULL;
	class_unregister(&trimui_motor_class);
animation:
	class_unregister(&trimui_led_anim_class);
gpio:
	gpio_direction_output(107, 0);
	gpio_free(107);
	return result;
}
