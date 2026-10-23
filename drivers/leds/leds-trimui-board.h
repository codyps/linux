/* SPDX-License-Identifier: GPL-2.0-only */
/* TG3040 #860 board LED callbacks and state recovered from the shipped Image. */
#include <linux/gpio.h>
#include <linux/kthread.h>
#include "leds-trimui-effects.h"
#include "leds-trimui-help.h"

extern int trimui_axp2202_get_bat_cap(void);
extern int trimui_set_speaker_enable(int enable);
extern int trimui_speaker_mute;

/* The four names follow the debugfs operation tables in stock .rodata.
 * Keep their byte order as one object; the paired no_reorder attributes on
 * those tables prevent GCC from moving this object ahead of them.
 */
static const struct {
	char motor[19];
	char speaker[18];
	char enable[21];
	char resume[23];
} trimui_name_cluster __attribute__((no_reorder)) = {
	.motor = "init_motor_voltage",
	.speaker = "init_speaker_mute",
	.enable = "led_regulator_enable",
	.resume = "sunxi_led_resume_noirq",
};

/* Preserve the recovered relative BSS layout, including overflow victims.
 * Stock state starts at ffffff8008bf47d0; effects are F2,F1,R,L,M in memory.
 * TODO: split and bound these buffers in a separate corrective change.
 */
static struct trimui_stock_led_state {
	int mask_lr, mask_f, mask_m, frames_enable;
	int frames_cycles, frames_fps, frames_len, effect_enable;
	struct trimui_led_effect effects[5];
	int mute;
	u32 frame[128];
	int low_battery, frame_index, cycles_left, motor_voltage;
	struct regulator *motor_regulator;
	int suspended;
	u32 wire_frame[128];
	u8 animation_alignment[12];
	u32 anim_frames[14400];
} trimui_led_state;

static unsigned int trimui_limit = 128, trimui_scale_f = 40;
static unsigned int trimui_scale_lr = 40, trimui_scale_m = 40;
static int trimui_led_enable = 1;
/* Stock has 32 scale pointers although the TG3040 device tree uses 23 LEDs.
 * Entries 23..31 still select the middle scale. Keep the unchecked indexing
 * beyond 32 as shipped; TODO: validate the device-tree count in a separate fix.
 */
static unsigned int * const trimui_scale_by_led[32] = {
	&trimui_scale_m, &trimui_scale_m, &trimui_scale_m, &trimui_scale_m,
	&trimui_scale_m, &trimui_scale_m, &trimui_scale_m, &trimui_scale_m,
	&trimui_scale_f, &trimui_scale_f,
	&trimui_scale_lr, &trimui_scale_lr, &trimui_scale_lr, &trimui_scale_lr,
	&trimui_scale_m, &trimui_scale_m, &trimui_scale_m, &trimui_scale_m,
	&trimui_scale_m, &trimui_scale_m, &trimui_scale_m, &trimui_scale_m,
	&trimui_scale_m, &trimui_scale_m, &trimui_scale_m, &trimui_scale_m, &trimui_scale_m, &trimui_scale_m, &trimui_scale_m, &trimui_scale_m, &trimui_scale_m, &trimui_scale_m,
};
#define trimui_mask_lr trimui_led_state.mask_lr
#define trimui_mask_f trimui_led_state.mask_f
#define trimui_mask_m trimui_led_state.mask_m
#define trimui_frames_enable trimui_led_state.frames_enable
#define trimui_frames_cycles trimui_led_state.frames_cycles
#define trimui_frames_fps trimui_led_state.frames_fps
#define trimui_frames_len trimui_led_state.frames_len
#define trimui_effect_enable trimui_led_state.effect_enable
#define trimui_effect_f2 trimui_led_state.effects[0]
#define trimui_effect_f1 trimui_led_state.effects[1]
#define trimui_effect_r trimui_led_state.effects[2]
#define trimui_effect_l trimui_led_state.effects[3]
#define trimui_effect_m trimui_led_state.effects[4]
#define trimui_mute_requested trimui_led_state.mute
#define trimui_frame trimui_led_state.frame
#define trimui_low_battery trimui_led_state.low_battery
#define trimui_frame_index trimui_led_state.frame_index
#define trimui_cycles_left trimui_led_state.cycles_left
#define trimui_motor_voltage trimui_led_state.motor_voltage
#define trimui_motor_regulator trimui_led_state.motor_regulator
#define trimui_suspended trimui_led_state.suspended
#define trimui_wire_frame trimui_led_state.wire_frame
#define trimui_anim_frames trimui_led_state.anim_frames
/* Stock forms one address below the left effect, then retains the left
 * pointer across both paired setters' parser calls. This is a view only. */
struct trimui_effect_pair_view {
	u8 before_left[0xb88];
	struct trimui_led_effect left;
};
extern struct trimui_effect_pair_view trimui_effect_pair;
asm(".local trimui_effect_pair\n.type trimui_effect_pair,%object\n"
    ".set trimui_effect_pair,trimui_led_state+0x21f8\n"
    ".size trimui_effect_pair,0x1aa8\n");
/* Compatible views of rooted stock fields; no independent allocation.
 * Original C declarations are unknown. The config alias inherits the
 * four-byte ELF extent of its scalar root, not the full C view below. */
struct trimui_transfer_state_view {
 u8 prefix[0x7cc]; u8 rgb[512]; int low_battery;
 u8 unobserved[0x18]; int suspended; u32 wire[128];
};
struct trimui_transfer_config_view { u8 prefix[16]; int enable; u32 count,debug; };
extern struct trimui_transfer_state_view trimui_transfer_state;
extern struct trimui_transfer_config_view trimui_transfer_config;
asm(".local trimui_transfer_state\n.type trimui_transfer_state,%object\n.set trimui_transfer_state,trimui_led_state+0x43f8\n.size trimui_transfer_state,0xbec\n"
    ".local trimui_transfer_config\n.type trimui_transfer_config,%object\n.set trimui_transfer_config,trimui_limit\n.size trimui_transfer_config,4\n");
/* Stock retains two distinct stores, including on negative capacity errors.
 * Keep its out-of-line helper and the same state view used by frame output.
 * TODO: report power-supply read failures in a separate behavior change.
 */
static noinline void check_battery(void)
{
	if (trimui_axp2202_get_bat_cap() < 16)
		trimui_transfer_state.low_battery = 1;
	else
		trimui_transfer_state.low_battery = 0;
}

static noinline void trimui_led_power_disable(void)
{
	/* Stock ignores GPIO direction/output failures, including at shutdown.
	 * TODO: handle those failures separately from this reconstruction.
	 */
	gpio_direction_output(107, 0);
}

static void commit_static_frame(void)
{
 /* Stock keeps this initial mutex even if the global device changes.
  * TODO: define device lifetime/locking in a separate behavior change. */
 struct mutex *access=&sunxi_led->pcdev_group[0].r.cdev.led_access;
 struct sunxi_led *led;
 static const char frame_name[]="sunxi_set_led_rgb32_frame";
 unsigned int i,count,scale,red,green; int low,j; s32 blue;
 unsigned long flags,diff_time;
 struct timeval start,end;
 struct trimui_transfer_state_view *state;
 const volatile u8 *src;
 u32 *out;
 mutex_lock(access);
 state=RELOC_HIDE(&trimui_transfer_state,0);
 src=state->rgb; out=state->wire;
 /* Stock snapshots these values after locking, before conversion. */
 count=READ_ONCE(trimui_transfer_config.count);
 low=READ_ONCE(state->low_battery);
 /* Stock trusts the count beyond both arrays and its 32-entry scale table.
  * TODO: bound it in a separate behavior change. */
 for(i=0;i<count;i++) {
  scale=*trimui_scale_by_led[i];
  if(low && scale>3)scale=3;
  /* Preserve byte observation order, wrap, and arithmetic blue shift.
   * TODO: normalize channel widths only as a separate fix. */
  blue=(u32)src[0]*scale; red=(u32)src[2]*scale; green=(u32)src[1]*scale;
  if(i<10)out[i]=((red<<8)&0xff0000)|(green&0xff00)|(blue>>8);
  else out[i]=((green<<8)&0xff0000)|(red&0xff00)|(blue>>8);
  src+=4;
 }
 if(state->suspended || !trimui_transfer_config.enable)
  memset(state->wire,0,sizeof(state->wire));
 /* Stock reloads the device here while retaining the initial mutex. */
 led=READ_ONCE(sunxi_led);
 do_gettimeofday(&start);
 /* Lock/transfer/completion results remain ignored as shipped. */
 spin_lock_irqsave(&led->lock,flags);
 led->length=count;
 for(j=0;(unsigned int)j<READ_ONCE(led->length);j++) {
  u32 *dest=READ_ONCE(led->data);
  u32 pixel=READ_ONCE(state->wire[j]);
  WRITE_ONCE(dest[j],pixel);
 }
 spin_unlock_irqrestore(&led->lock,flags);
 sunxi_ledc_trans_data(led);
 if(trimui_transfer_config.debug & DEBUG_INFO2) {
  printk(KERN_WARNING "%s()%d - dump reg:\n",frame_name,2744);
  led_dump_reg(led,0,0x30);
 }
 sunxi_ledc_complete(led);
 do_gettimeofday(&end);
 diff_time=1000000*(end.tv_sec-start.tv_sec)+end.tv_usec-start.tv_usec;
 if(trimui_transfer_config.debug & DEBUG_INFO1)
  printk(KERN_WARNING "num = %03u, time = %ld us\n",led->length,diff_time);
 mutex_unlock(access);
}

static int trimui_ledc_run_thread(void *unused)
{
	unsigned int battery_ticks = 0;
	u32 rgb;
	trimui_scale_m = 14;
	/* Stock publishes this control as a separate 32-bit store before the
	 * adjacent first effect type.  A plain store lets GCC merge the two.
	 */
	WRITE_ONCE(trimui_effect_enable, 1);
	/* Stock exposes all five cycle counts before any effect shape.  Sysfs
	 * readers can observe these intermediate values during startup.
	 */
#define INITIAL_CYCLES(e) WRITE_ONCE((e).cycles, 5)
	INITIAL_CYCLES(trimui_effect_m);
	INITIAL_CYCLES(trimui_effect_r);
	INITIAL_CYCLES(trimui_effect_l);
	INITIAL_CYCLES(trimui_effect_f2);
	INITIAL_CYCLES(trimui_effect_f1);
#undef INITIAL_CYCLES
	/* Stock commits type/duration together as one little-endian 64-bit
	 * store.  Keep that width and the M,R,L,F2,F1 publication sequence.
	 */
#define INITIAL_SHAPE(e) WRITE_ONCE(*(u64 *)&(e).type, ((u64)2000 << 32) | 2)
	INITIAL_SHAPE(trimui_effect_m);
	INITIAL_SHAPE(trimui_effect_r);
	INITIAL_SHAPE(trimui_effect_l);
	WRITE_ONCE(trimui_effect_m.target_rgb, 0xffffff);
	WRITE_ONCE(trimui_effect_r.target_rgb, 0xffffff);
	WRITE_ONCE(trimui_effect_l.target_rgb, 0xffffff);
	INITIAL_SHAPE(trimui_effect_f2);
	WRITE_ONCE(trimui_effect_f2.target_rgb, 0x00ff33);
	INITIAL_SHAPE(trimui_effect_f1);
	WRITE_ONCE(trimui_effect_f1.target_rgb, 0x00ff33);
#undef INITIAL_SHAPE
	/* The OEM generator receives these fixed boot colors.  It does not
	 * reread target_rgb between initialization and each call.
	 */
	gen_effect(&trimui_effect_l, 2, 0xffffff, 0xffffff);
	gen_effect(&trimui_effect_r, 2, 0xffffff, 0xffffff);
	gen_effect(&trimui_effect_f1, 2, 0x00ff33, 0x00ff33);
	gen_effect(&trimui_effect_f2, 2, 0x00ff33, 0x00ff33);
	gen_effect(&trimui_effect_m, 2, 0xffffff, 0xffffff);
	/* Keep the OEM's odd reset order.  A concurrent control read can see
	 * these partially reset effects; do not collapse this into a loop.
	 */
	WRITE_ONCE(trimui_effect_r.index, 0);
	WRITE_ONCE(trimui_effect_l.active_cycles, READ_ONCE(trimui_effect_l.cycles));
	WRITE_ONCE(trimui_effect_r.active_cycles, READ_ONCE(trimui_effect_r.cycles));
	WRITE_ONCE(trimui_effect_f1.active_cycles, READ_ONCE(trimui_effect_f1.cycles));
	WRITE_ONCE(trimui_effect_f2.active_cycles, READ_ONCE(trimui_effect_f2.cycles));
	rgb = READ_ONCE(trimui_effect_m.cycles);
	WRITE_ONCE(trimui_effect_l.index, 0);
	WRITE_ONCE(trimui_effect_m.index, 0);
	WRITE_ONCE(trimui_effect_f2.index, 0);
	WRITE_ONCE(trimui_effect_f1.index, 0);
	WRITE_ONCE(trimui_effect_m.active_cycles, rgb);
	while (!kthread_should_stop()) {
		if (trimui_effect_enable) {
			if (is_effect_actived(&trimui_effect_l)) {
				rgb = step_one_frame(&trimui_effect_l);
				trimui_frame[10] = trimui_frame[11] = rgb;
			}
			if (is_effect_actived(&trimui_effect_r)) {
				rgb = step_one_frame(&trimui_effect_r);
				trimui_frame[12] = trimui_frame[13] = rgb;
			}
			if (is_effect_actived(&trimui_effect_m)) {
				unsigned int i;
				rgb = step_one_frame(&trimui_effect_m);
				for (i = 0; i < 8; i++) trimui_frame[i] = rgb;
			}
			if (is_effect_actived(&trimui_effect_f1))
				trimui_frame[9] = step_one_frame(&trimui_effect_f1);
			if (is_effect_actived(&trimui_effect_f2))
				trimui_frame[8] = step_one_frame(&trimui_effect_f2);
		} else if (trimui_frames_enable) {
			int offset = trimui_frame_index * 14;
			if (trimui_frames_len <= 0)
				goto battery;
			/* TODO: bound the animation length/index in a corrective patch.
			 * Stock reads beyond anim_frames when sysfs supplies such state.
			 */
			if (++trimui_frame_index >= trimui_frames_len) {
				trimui_frame_index = 0;
				if (trimui_cycles_left > 0) trimui_cycles_left--;
				else if (trimui_cycles_left >= -1) {
					trimui_frames_cycles = 0;
					trimui_frames_len = 0;
				}
			}
			if (trimui_mask_m) memcpy(trimui_frame, trimui_anim_frames + offset, 8 * sizeof(u32));
			if (trimui_mask_lr) {
				/* Stock transfers each LED pair before reading the next.
				 * The two-copy sequence can observe an intervening frame edit.
				 */
				memcpy(trimui_frame + 10, trimui_anim_frames + offset + 10, 2 * sizeof(u32));
				barrier();
				memcpy(trimui_frame + 12, trimui_anim_frames + offset + 12, 2 * sizeof(u32));
			}
			if (trimui_mask_f) {
				u32 f1 = READ_ONCE(trimui_anim_frames[offset + 9]);
				u32 f2 = READ_ONCE(trimui_anim_frames[offset + 8]);
				WRITE_ONCE(trimui_frame[8], f2);
				WRITE_ONCE(trimui_frame[9], f1);
			}
		}
		commit_static_frame();
 battery:
		if (battery_ticks > 625) {
			battery_ticks = 0;
			check_battery();
		}
		battery_ticks++;
		msleep(16);
	}
	return 0;
}

/* Stock ignores conversion failures, still returns count, and still performs
 * the callback's subsequent side effects. TODO: report parse errors in a fix.
 */
static ssize_t show_tg5040_max_scale(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_limit);
}

static ssize_t store_tg5040_max_scale(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_limit);
	if (trimui_limit > 255) trimui_limit = 255;
	return count;
}

static ssize_t help_show(struct class *class, struct class_attribute *attr, char *buffer)
{
	sprintf(buffer, "%s\n", trimui_led_help);
	return sizeof(trimui_led_help);
}

static ssize_t show_led_enable(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_led_enable);
}

static ssize_t store_led_enable(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_led_enable);
	return count;
}



static ssize_t help_show_effect(struct class *class, struct class_attribute *attr, char *buffer)
{
	char line[32];
	unsigned long i;

	strcpy(buffer, "TRIMUI LED Animation Effect names:\n");
	for (i = 0; i < ARRAY_SIZE(trimui_effect_names); i++) {
		sprintf(line, "%d - %s\n", (int)i, trimui_effect_names[i]);
		strcat(buffer, line);
	}
	return strlen(buffer);
}

static ssize_t show_max_scale(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_scale_m);
}

static ssize_t store_max_scale(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_scale_m);
	if (trimui_scale_m > trimui_limit) trimui_scale_m = trimui_limit;
	return count;
}

static ssize_t show_max_scale_lr(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_scale_lr);
}

static ssize_t store_max_scale_lr(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_scale_lr);
	if (trimui_scale_lr > trimui_limit) trimui_scale_lr = trimui_limit;
	return count;
}

static ssize_t show_max_scale_f1f2(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_scale_f);
}

static ssize_t store_max_scale_f1f2(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_scale_f);
	if (trimui_scale_f > trimui_limit) trimui_scale_f = trimui_limit;
	return count;
}

static ssize_t show_frame(struct class *class, struct class_attribute *attr, char *buffer)
{
	/* OEM shifts a signed int before memcpy's size_t conversion. A corrupt
	 * led_count can wrap the shift into a negative, enormous copy length. */
	u8 *source;
	register int size asm("w19") = trimui_led_count;
	/* Keep the observed count live until after the rooted address is formed. */
	asm volatile("adrp %0, trimui_transfer_state\n\t"
		     "add %0, %0, :lo12:trimui_transfer_state\n\t"
		     "add %0, %0, #0x7cc"
		     : "=&r"(source), "+r"(size) : : "memory");
	size <<= 2;
	memcpy(buffer, source, size);
	return size;
}

static ssize_t store_frame(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	/* Stock compares count with the buffer address, not its size.
	 * TODO: use sizeof(frame) in a separate corrective patch. */
	unsigned long length = count;
	if (count >= (unsigned long)trimui_frame)
		length = (int)(trimui_led_count << 2);
	memcpy(trimui_frame, buffer, length);
	return count;
}

static ssize_t show_frame_hex(struct class *class, struct class_attribute *attr, char *buffer)
{
	int i = 0, count = 0;
	char *position = buffer;
	u32 *pixels = (u32 *)trimui_transfer_state.rgb;
	/* Stock advances by the cumulative length, producing sparse output.
	 * At 34 LEDs the newline lands beyond a 4 KiB sysfs page.
	 * TODO: advance by the last write and bound output in a separate fix. */
	while (i < (int)trimui_led_count) {
		count += 7;
		sprintf(position, "%06X ", pixels[i] & 0xffffff);
		position += count;
		/* Keep the cumulative count live across the next LED-count read. */
		asm volatile("" : "+r"(count));
		i++;
	}
	strcpy(position, "\n");
	return count + 1;
}

static ssize_t store_frame_hex(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	int i = 0, bytes = (int)count;
	int rounded;
	const char *position = buffer;
	u32 *pixels;
	asm("adrp %0, trimui_transfer_state\n\t"
	    "add %0, %0, :lo12:trimui_transfer_state\n\t"
	    "add %0, %0, #0x7cc"
	    : "=&r"(pixels));
	if (bytes > (int)trimui_led_count * 7)
		bytes = trimui_led_count * 7;
	rounded = bytes / 7 * 7;
	/* Stock compares a signed 32-bit difference between the end and the
	 * advancing input pointer. Keep its wraparound and unchecked destination.
	 * TODO: bound the destination and use a non-wrapping length separately. */
	while ((int)((u32)(unsigned long)buffer + (u32)rounded -
		     (u32)(unsigned long)position) > 0) {
		sscanf(position, "%X", &pixels[i]);
		position += 7;
		i++;
		/* Preserve stock's two-step end-minus-cursor comparison. */
		asm volatile("" : "+r"(rounded));
	}
	return count;
}

static ssize_t show_duration(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_l.duration);
}

static ssize_t show_duration_m(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_m.duration);
}

static ssize_t store_duration_m(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_effect_m.duration);
	return count;
}

static ssize_t show_duration_l(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_l.duration);
}

static ssize_t store_duration_l(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_effect_l.duration);
	return count;
}

static ssize_t show_duration_r(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_r.duration);
}

static ssize_t store_duration_r(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_effect_r.duration);
	return count;
}

static ssize_t show_duration_f1(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_f1.duration);
}

static ssize_t store_duration_f1(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_effect_f1.duration);
	return count;
}

static ssize_t show_duration_f2(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_f2.duration);
}

static ssize_t store_duration_f2(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_effect_f2.duration);
	return count;
}

static ssize_t store_duration(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	struct trimui_effect_pair_view *pair;
	struct trimui_led_effect *left;

	asm("adrp %0, trimui_effect_pair\n\t"
	    "add %0, %0, :lo12:trimui_effect_pair"
	    : "=&r"(pair));
	left = &pair->left;

	/* OEM copies the stale value to the right on parse failure too.
	 * TODO: reject invalid input only in a separate behavior change. */
	kstrtoint(buffer, 0, &left->duration);
	trimui_effect_r.duration = left->duration;
	return count;
}

static ssize_t show_cycles(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_l.cycles);
}

static ssize_t show_cycles_m(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_m.cycles);
}

static ssize_t store_cycles_m(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_effect_m.cycles);
	return count;
}

static ssize_t show_cycles_l(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_l.cycles);
}

static ssize_t store_cycles_l(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_effect_l.cycles);
	return count;
}

static ssize_t show_cycles_r(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_r.cycles);
}

static ssize_t store_cycles_r(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_effect_r.cycles);
	return count;
}

static ssize_t show_cycles_f1(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_f1.cycles);
}

static ssize_t store_cycles_f1(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_effect_f1.cycles);
	return count;
}

static ssize_t show_cycles_f2(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_f2.cycles);
}

static ssize_t store_cycles_f2(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_effect_f2.cycles);
	return count;
}

static ssize_t store_cycles(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	struct trimui_effect_pair_view *pair;
	struct trimui_led_effect *left;

	asm("adrp %0, trimui_effect_pair\n\t"
	    "add %0, %0, :lo12:trimui_effect_pair"
	    : "=&r"(pair));
	left = &pair->left;

	/* OEM copies the stale value to the right on parse failure too.
	 * TODO: reject invalid input only in a separate behavior change. */
	kstrtoint(buffer, 0, &left->cycles);
	trimui_effect_r.cycles = left->cycles;
	return count;
}

/* Stock writes the terminating newline at the fixed byte offset seven and
 * returns eight, independently of sprintf's return value.  Keep both calls.
 */
static noinline int dump_rgb_hex(char *buffer, u32 rgb)
{
	sprintf(buffer, "%06X ", rgb & 0xffffff);
	strcpy(buffer + 7, "\n");
	return 8;
}

static noinline u32 get_rgb_hex(const char *buffer)
{
	u32 rgb = 0;

	/* Stock ignores conversion failure and publishes the initialized zero.
	 * TODO: report malformed input in a separate behavior-changing fix.
	 */
	sscanf(buffer, "%X", &rgb);
	return rgb;
}

static ssize_t show_effect_rgb_hex_m(struct class *class, struct class_attribute *attr, char *buffer)
{
	return dump_rgb_hex(buffer, trimui_effect_m.target_rgb);
}

static ssize_t store_effect_rgb_hex_m(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	u32 rgb = get_rgb_hex(buffer);
	trimui_effect_m.target_rgb = rgb;
	return count;
}

static ssize_t show_effect_rgb_hex_l(struct class *class, struct class_attribute *attr, char *buffer)
{
	return dump_rgb_hex(buffer, trimui_effect_l.target_rgb);
}

static ssize_t store_effect_rgb_hex_l(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	u32 rgb = get_rgb_hex(buffer);
	trimui_effect_l.target_rgb = rgb;
	return count;
}

static ssize_t show_effect_rgb_hex_r(struct class *class, struct class_attribute *attr, char *buffer)
{
	return dump_rgb_hex(buffer, trimui_effect_r.target_rgb);
}

static ssize_t store_effect_rgb_hex_r(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	u32 rgb = get_rgb_hex(buffer);
	trimui_effect_r.target_rgb = rgb;
	return count;
}

static ssize_t show_effect_rgb_hex_f1(struct class *class, struct class_attribute *attr, char *buffer)
{
	return dump_rgb_hex(buffer, trimui_effect_f1.target_rgb);
}

static ssize_t store_effect_rgb_hex_f1(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	u32 rgb = get_rgb_hex(buffer);
	trimui_effect_f1.target_rgb = rgb;
	return count;
}

static ssize_t show_effect_rgb_hex_f2(struct class *class, struct class_attribute *attr, char *buffer)
{
	return dump_rgb_hex(buffer, trimui_effect_f2.target_rgb);
}

static ssize_t store_effect_rgb_hex_f2(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	u32 rgb = get_rgb_hex(buffer);
	trimui_effect_f2.target_rgb = rgb;
	return count;
}

static ssize_t show_effect_rgb_hex_lr(struct class *class, struct class_attribute *attr, char *buffer)
{
	return dump_rgb_hex(buffer, trimui_effect_l.target_rgb);
}

/* Scalar views of recovered fields; these aliases allocate no storage.
 * Their original C declarations are unknown. */
extern u32 trimui_target_l,trimui_target_r;
__asm__(".local trimui_target_l\n.type trimui_target_l,%object\n.set trimui_target_l,trimui_led_state+0x2d9c\n.size trimui_target_l,4\n"
        ".local trimui_target_r\n.type trimui_target_r,%object\n.set trimui_target_r,trimui_led_state+0x1e7c\n.size trimui_target_r,4\n");
static ssize_t store_effect_rgb_hex_lr(struct class *class,struct class_attribute *attr,const char *buffer,size_t count)
{
 u32 rgb=get_rgb_hex(buffer);
 /* Stock publishes left before right without synchronization, even after
  * malformed input produced zero. TODO: reject invalid input and protect
  * the pair only in a separate behavior change. */
 WRITE_ONCE(trimui_target_l,rgb);
 WRITE_ONCE(trimui_target_r,rgb);
 return count;
}

static ssize_t show_effect_m(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_m.type);
}

static ssize_t store_effect_m(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_effect_m.type);
	if ((unsigned int)trimui_effect_m.type > 7) trimui_effect_m.type = 0;
	gen_effect(&trimui_effect_m, trimui_effect_m.type, trimui_effect_m.target_rgb, trimui_effect_m.current_rgb);
	/* OEM resets the index before reading cycles, even if state changes. */
	WRITE_ONCE(trimui_effect_m.index, 0);
	WRITE_ONCE(trimui_effect_m.active_cycles, READ_ONCE(trimui_effect_m.cycles));
	return count;
}

static ssize_t show_effect_l(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_l.type);
}

static ssize_t store_effect_l(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_effect_l.type);
	if ((unsigned int)trimui_effect_l.type > 7) trimui_effect_l.type = 0;
	gen_effect(&trimui_effect_l, trimui_effect_l.type, trimui_effect_l.target_rgb, trimui_effect_l.current_rgb);
	WRITE_ONCE(trimui_effect_l.index, 0);
	WRITE_ONCE(trimui_effect_l.active_cycles, READ_ONCE(trimui_effect_l.cycles));
	return count;
}

static ssize_t show_effect_r(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_r.type);
}

static ssize_t store_effect_r(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_effect_r.type);
	if ((unsigned int)trimui_effect_r.type > 7) trimui_effect_r.type = 0;
	gen_effect(&trimui_effect_r, trimui_effect_r.type, trimui_effect_r.target_rgb, trimui_effect_r.current_rgb);
	WRITE_ONCE(trimui_effect_r.index, 0);
	WRITE_ONCE(trimui_effect_r.active_cycles, READ_ONCE(trimui_effect_r.cycles));
	return count;
}

static ssize_t show_effect_f1(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_f1.type);
}

static ssize_t store_effect_f1(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_effect_f1.type);
	if ((unsigned int)trimui_effect_f1.type > 7) trimui_effect_f1.type = 0;
	gen_effect(&trimui_effect_f1, trimui_effect_f1.type, trimui_effect_f1.target_rgb, trimui_effect_f1.current_rgb);
	WRITE_ONCE(trimui_effect_f1.index, 0);
	WRITE_ONCE(trimui_effect_f1.active_cycles, READ_ONCE(trimui_effect_f1.cycles));
	return count;
}

static ssize_t show_effect_f2(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_f2.type);
}

static ssize_t store_effect_f2(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_effect_f2.type);
	if ((unsigned int)trimui_effect_f2.type > 7) trimui_effect_f2.type = 0;
	gen_effect(&trimui_effect_f2, trimui_effect_f2.type, trimui_effect_f2.target_rgb, trimui_effect_f2.current_rgb);
	WRITE_ONCE(trimui_effect_f2.index, 0);
	WRITE_ONCE(trimui_effect_f2.active_cycles, READ_ONCE(trimui_effect_f2.cycles));
	return count;
}

static ssize_t show_effect_lr(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_l.type);
}

static ssize_t store_effect_lr(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	int right_cycles;

	kstrtoint(buffer, 0, (int *)&trimui_effect_l.type);
	if ((unsigned int)trimui_effect_l.type > 7) trimui_effect_l.type = 0;
	trimui_effect_r.type = trimui_effect_l.type;
	gen_effect(&trimui_effect_l, trimui_effect_l.type, trimui_effect_l.target_rgb, trimui_effect_l.current_rgb);
	gen_effect(&trimui_effect_r, trimui_effect_r.type, trimui_effect_r.target_rgb, trimui_effect_r.current_rgb);
	WRITE_ONCE(trimui_effect_l.index, 0);
	WRITE_ONCE(trimui_effect_l.active_cycles, READ_ONCE(trimui_effect_l.cycles));
	/* OEM reads right cycles before resetting right index, unlike the
	 * five single-effect setters. Keep this observable ordering. */
	right_cycles = READ_ONCE(trimui_effect_r.cycles);
	WRITE_ONCE(trimui_effect_r.index, 0);
	WRITE_ONCE(trimui_effect_r.active_cycles, right_cycles);
	return count;
}

/* This view begins eight bytes before the LED state. The adjacent scalar
 * fields share the address root used by the stock sysfs setters. */
struct trimui_frame_control_view {
	u8 unused[0x14];
	int enable, cycles, fps, len, effect_enable;
};
extern struct trimui_frame_control_view trimui_frame_controls;

static ssize_t show_effect_enable(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_enable);
}

static ssize_t store_effect_enable(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	struct trimui_frame_control_view *control = &trimui_frame_controls;
	asm volatile("" : "+r"(control));
	kstrtoint(buffer, 0, &control->effect_enable);
	return count;
}

static ssize_t show_anim_frames(struct class *class, struct class_attribute *attr, char *buffer)
{
	int size = trimui_led_count << 2;
	memcpy(buffer, trimui_anim_frames, size);
	return size;
}

static ssize_t store_anim_frames(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	/* Stock falls back to one LED frame, rather than clamping to capacity.
	 * TODO: reject oversized writes separately instead of preserving this. */
	if (count < sizeof(trimui_anim_frames))
		memcpy(trimui_anim_frames, buffer, count);
	else
		memcpy(trimui_anim_frames, buffer, (int)(trimui_led_count << 2));
	return count;
}

static ssize_t show_anim_frames_hex(struct class *class, struct class_attribute *attr, char *buffer)
{
	int i = 0, count = 0;
	char *position = buffer;
	u32 *pixels = trimui_anim_frames;
	/* Stock advances by the cumulative length, producing sparse output.
	 * At 34 LEDs the newline lands beyond a 4 KiB sysfs page.
	 * TODO: advance by the last write and bound output in a separate fix. */
	while (i < (int)trimui_led_count) {
		count += 7;
		sprintf(position, "%06X ", pixels[i] & 0xffffff);
		position += count;
		/* Keep the cumulative count live across the next LED-count read. */
		asm volatile("" : "+r"(count));
		i++;
	}
	strcpy(position, "\n");
	return count + 1;
}

static ssize_t store_anim_frames_hex(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	int i, colors = (int)(count / 7);
	int offset = trimui_frames_len * 14;
	u32 *pixels;
	/* Stock materializes the animation array base before indexed stores. */
	asm("adrp %0, trimui_led_state+0x4ff0\n\t"
	    "add %0, %0, :lo12:trimui_led_state+0x4ff0"
	    : "=&r"(pixels));
	if (colors > 4200)
		colors = 4200;
	/* Stock ignores append capacity and clears the start of the buffer,
	 * even when sscanf writes at a later append offset. TODO: validate the
	 * append and clear its actual destination in a separate fix. */
	for (i = 0; i < colors; i++) {
		pixels[i] = 0;
		sscanf(buffer + 7 * i, "%X", &pixels[offset + i]);
	}
	trimui_frames_len += colors / 14;
	printk("\n anim frames:%d, pixels:%d\n", trimui_frames_len, colors);
	return count;
}

static ssize_t show_frames_len(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_frames_len);
}

static ssize_t store_frames_len(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	struct trimui_frame_control_view *control = &trimui_frame_controls;
	asm volatile("" : "+r"(control));
	kstrtoint(buffer, 0, &control->len);
	return count;
}

static ssize_t show_frames_fps(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_frames_fps);
}

static ssize_t store_frames_fps(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	struct trimui_frame_control_view *control = &trimui_frame_controls;
	asm volatile("" : "+r"(control));
	kstrtoint(buffer, 0, &control->fps);
	return count;
}

static ssize_t show_frames_cycles(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_frames_cycles);
}

/* Compatible address views, not independent allocations or recovered types.
 * Only the explicitly rooted control/counter fields are accessed. */
struct trimui_frame_counter_view { u8 unused[0x9d0]; int index,left; };
/* Voltage and regulator share the same stock BSS root as the frame counters. */
struct trimui_motor_state_view {
 u8 unused[0x9d8]; int voltage;
 struct regulator *regulator;
};
extern struct trimui_frame_counter_view trimui_frame_counters;
extern struct trimui_motor_state_view trimui_motor_state;
__asm__(".local trimui_frame_controls\n.type trimui_frame_controls,%object\n.set trimui_frame_controls,trimui_led_state-8\n.size trimui_frame_controls,40\n"
        ".local trimui_frame_counters\n.type trimui_frame_counters,%object\n.set trimui_frame_counters,trimui_led_state+0x43f8\n.size trimui_frame_counters,0x9d8\n");
__asm__(".local trimui_motor_state\n.type trimui_motor_state,%object\n.set trimui_motor_state,trimui_led_state+0x43f8\n.size trimui_motor_state,0x9e8\n");
static ssize_t store_frames_cycles(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	struct trimui_frame_control_view *control;
	struct trimui_frame_counter_view *counter;
	size_t result;

	/* These compatible aliases recover stock's two BSS address roots and
	 * keep the count in a callee-saved register across the parser call. */
	asm("adrp %0, trimui_frame_controls\n\t"
	    "add %0, %0, :lo12:trimui_frame_controls\n\t"
	    "mov %1, %2"
	    : "=&r"(control), "=r"(result) : "r"(count));
	/* OEM ignores parse errors and clears enable regardless.  TODO: reject
	 * invalid input only in a separate behavior-changing fix. */
	kstrtoint(buffer, 0, &control->cycles);
	/* Materialize the counter root before clearing enable as stock does.
	 * The memory clobber keeps these observable operations ordered. */
	asm volatile("adrp %0, trimui_frame_counters\n\t"
		     "add %0, %0, :lo12:trimui_frame_counters"
		     : "=&r"(counter) : : "memory");
	control->enable = 0;
	/* Stock issues two separate 32-bit resets.  A frame worker can observe
	 * the intermediate state; TODO: synchronize only in a separate fix. */
	WRITE_ONCE(counter->index, 0);
	WRITE_ONCE(counter->left, 0);
	return result;
}

static ssize_t show_frames_enable(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_frames_enable);
}

static ssize_t store_frames_enable(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
 struct trimui_frame_control_view *control;
 size_t result;
 int left;
 /* Compatible native address/count lifetime, not recovered original C.
  * These symbolic relocations allocate no data and add no locking. */
 asm("adrp %0, trimui_frame_controls\n\tadd %0, %0, :lo12:trimui_frame_controls\n\tmov %1, %2"
     : "=&r"(control), "=r"(result) : "r"(count));
 /* Stock ignores parse errors and still resets/decrements state.
  * TODO: reject errors only in a separate behavior change. */
 kstrtoint(buffer,0,&control->enable);
 left=control->cycles-1;
 trimui_frame_counters.index=0;
 trimui_frame_counters.left=left;
 if(!control->enable)control->len=0;
 return result;
}

static ssize_t show_frames_override_m_enable(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_mask_m);
}

static ssize_t store_frames_override_m_enable(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_mask_m);
	return count;
}

static ssize_t show_frames_override_f1f2_enable(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_mask_f);
}

static ssize_t store_frames_override_f1f2_enable(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_mask_f);
	return count;
}

static ssize_t show_frames_override_lr_enable(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_mask_lr);
}

static ssize_t store_frames_override_lr_enable(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_mask_lr);
	return count;
}

static ssize_t show_motor_voltage(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_motor_regulator ? regulator_get_voltage(trimui_motor_regulator) : 0);
}

static ssize_t store_motor_voltage(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	struct trimui_motor_state_view *state;

	/* OEM keeps this state pointer live across kstrtoint and then re-reads
	 * voltage from the same object. Keep the pointer live without adding a
	 * memory access or changing the stock NULL/error-pointer handling. */
	state = &trimui_motor_state;
	asm("" : "+r"(state));
	kstrtoint(buffer, 0, &state->voltage);
	/* Stock passes NULL/error pointers through. TODO: check regulator_get in a separate fix. */
	regulator_set_voltage(state->regulator, state->voltage,
		state->voltage + 100000);
	return count;
}

static ssize_t show_speaker_mute(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_mute_requested);
}

static ssize_t store_speaker_mute(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	/* OEM ignores parse failure and may use the prior mute request. */
	kstrtoint(buffer, 0, (int *)&trimui_mute_requested);
	if (trimui_mute_requested) {
		trimui_speaker_mute = 1;
		trimui_set_speaker_enable(0);
	} else {
		trimui_speaker_mute = 0;
		trimui_set_speaker_enable(1);
	}
	printk("speaker force mute:%d\n", trimui_speaker_mute);
	return count;
}

static struct class_attribute trimui_led_anim_attrs[] = {
	__ATTR(help, 0664, help_show, NULL),
	__ATTR(enable, 0664, show_led_enable, store_led_enable),
	__ATTR(effect_names, 0664, help_show_effect, NULL),
	__ATTR(max_scale, 0664, show_max_scale, store_max_scale),
	__ATTR(max_scale_lr, 0664, show_max_scale_lr, store_max_scale_lr),
	__ATTR(max_scale_f1f2, 0664, show_max_scale_f1f2, store_max_scale_f1f2),
	__ATTR(frame, 0664, show_frame, store_frame),
	__ATTR(frame_hex, 0664, show_frame_hex, store_frame_hex),
	__ATTR(effect_duration_m, 0664, show_duration_m, store_duration_m),
	__ATTR(effect_duration_l, 0664, show_duration_l, store_duration_l),
	__ATTR(effect_duration_r, 0664, show_duration_r, store_duration_r),
	__ATTR(effect_duration_f1, 0664, show_duration_f1, store_duration_f1),
	__ATTR(effect_duration_f2, 0664, show_duration_f2, store_duration_f2),
	__ATTR(effect_duration_lr, 0664, show_duration, store_duration),
	__ATTR(effect_cycles_m, 0664, show_cycles_m, store_cycles_m),
	__ATTR(effect_cycles_l, 0664, show_cycles_l, store_cycles_l),
	__ATTR(effect_cycles_r, 0664, show_cycles_r, store_cycles_r),
	__ATTR(effect_cycles_f1, 0664, show_cycles_f1, store_cycles_f1),
	__ATTR(effect_cycles_f2, 0664, show_cycles_f2, store_cycles_f2),
	__ATTR(effect_cycles_lr, 0664, show_cycles, store_cycles),
	__ATTR(effect_rgb_hex_m, 0664, show_effect_rgb_hex_m, store_effect_rgb_hex_m),
	__ATTR(effect_rgb_hex_l, 0664, show_effect_rgb_hex_l, store_effect_rgb_hex_l),
	__ATTR(effect_rgb_hex_r, 0664, show_effect_rgb_hex_r, store_effect_rgb_hex_r),
	__ATTR(effect_rgb_hex_f1, 0664, show_effect_rgb_hex_f1, store_effect_rgb_hex_f1),
	__ATTR(effect_rgb_hex_f2, 0664, show_effect_rgb_hex_f2, store_effect_rgb_hex_f2),
	__ATTR(effect_rgb_hex_lr, 0664, show_effect_rgb_hex_lr, store_effect_rgb_hex_lr),
	__ATTR(effect_m, 0664, show_effect_m, store_effect_m),
	__ATTR(effect_l, 0664, show_effect_l, store_effect_l),
	__ATTR(effect_r, 0664, show_effect_r, store_effect_r),
	__ATTR(effect_f1, 0664, show_effect_f1, store_effect_f1),
	__ATTR(effect_f2, 0664, show_effect_f2, store_effect_f2),
	__ATTR(effect_lr, 0664, show_effect_lr, store_effect_lr),
	__ATTR(effect_enable, 0664, show_effect_enable, store_effect_enable),
	__ATTR(anim_frames, 0664, show_anim_frames, store_anim_frames),
	__ATTR(anim_frames_hex, 0664, show_anim_frames_hex, store_anim_frames_hex),
	__ATTR(anim_frames_len, 0664, show_frames_len, store_frames_len),
	__ATTR(anim_frames_fps, 0664, show_frames_fps, store_frames_fps),
	__ATTR(anim_frames_cycles, 0664, show_frames_cycles, store_frames_cycles),
	__ATTR(anim_frames_enable, 0664, show_frames_enable, store_frames_enable),
	__ATTR(anim_frames_mask_m_enable, 0664, show_frames_override_m_enable, store_frames_override_m_enable),
	__ATTR(anim_frames_mask_f1f2_enable, 0664, show_frames_override_f1f2_enable, store_frames_override_f1f2_enable),
	__ATTR(anim_frames_mask_lr_enable, 0664, show_frames_override_lr_enable, store_frames_override_lr_enable),
	__ATTR_NULL,
};
static struct class trimui_led_anim_class = {
	.name = "led_anim", .owner = THIS_MODULE, .class_attrs = trimui_led_anim_attrs,
};
static struct class_attribute trimui_motor_attrs[] = {
	__ATTR(voltage, 0664, show_motor_voltage, store_motor_voltage),
	__ATTR_NULL,
};
static struct class trimui_motor_class = {
	.name = "motor", .owner = THIS_MODULE, .class_attrs = trimui_motor_attrs,
};
static struct class_attribute trimui_speaker_attrs[] = {
	__ATTR(mute, 0664, show_speaker_mute, store_speaker_mute),
	__ATTR_NULL,
};
static struct class trimui_speaker_class = {
	.name = "speaker", .owner = THIS_MODULE, .class_attrs = trimui_speaker_attrs,
};


static int trimui_board_init(void)
{
	struct task_struct *task;
	int result;
	unsigned int i;

	BUILD_BUG_ON(offsetof(struct trimui_stock_led_state, frame) != 0x4bc4);
	BUILD_BUG_ON(offsetof(struct trimui_stock_led_state, motor_regulator) != 0x4dd8);
	BUILD_BUG_ON(offsetof(struct trimui_stock_led_state, wire_frame) != 0x4de4);
	BUILD_BUG_ON(offsetof(struct trimui_stock_led_state, anim_frames) != 0x4ff0);
	memset(trimui_frame, 0, sizeof(trimui_frame));
	memset(&trimui_effect_m, 0, sizeof(trimui_effect_m));
	memset(&trimui_effect_l, 0, sizeof(trimui_effect_l));
	memset(&trimui_effect_r, 0, sizeof(trimui_effect_r));
	memset(&trimui_effect_f1, 0, sizeof(trimui_effect_f1));
	memset(&trimui_effect_f2, 0, sizeof(trimui_effect_f2));
	for (i = 0; i < 5; i++) {
		trimui_led_state.effects[i].duration = 2000;
		trimui_led_state.effects[i].cycles = 1;
	}
	result = gpio_request(107, "led_5v");
	/* Stock continues after GPIO and thread errors. TODO: unwind and return
	 * those errors in a separate fix, and stop the thread on remove. */
	if (result < 0)
		printk("trimui_ledc_init error! led_5v: %d\n", 107);
	gpio_direction_output(107, 1);
	task = kthread_run(trimui_ledc_run_thread, NULL, "ledc daemon");
	if (IS_ERR(task))
		printk("ledc thread error\n");
	result = class_register(&trimui_led_anim_class);
	if (result < 0) {
		/* Stock reuses the probe diagnostic array, including pointer identity. */
		printk(KERN_ERR "%s()%d - class_register err\n", trimui_ro_writes.sunxi_led_probe, 3329);
		class_unregister(&trimui_led_anim_class);
		return -EFAULT;
	}
	result = class_register(&trimui_motor_class);
	if (result < 0) {
		printk(KERN_ERR "%s()%d - class_register err\n", trimui_name_cluster.motor, 3158);
		class_unregister(&trimui_motor_class);
	}
	trimui_motor_regulator = regulator_get(NULL, "axp2202-bldo3");
	result = class_register(&trimui_speaker_class);
	if (result < 0) {
		printk(KERN_ERR "%s()%d - class_register err\n", trimui_name_cluster.speaker, 3218);
		class_unregister(&trimui_speaker_class);
	}
	return 0;
}
