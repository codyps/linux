/* SPDX-License-Identifier: GPL-2.0-only */
/* TG3040 #860 board LED callbacks and state recovered from the shipped Image. */
#include <linux/gpio.h>
#include <linux/kthread.h>
#include "leds-trimui-effects.h"

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

static void trimui_led_power_enable(int enable)
{
	/* Both shipped callers pass zero. GCC partially inlines this condition
	 * and leaves the same 32-byte out-of-line GPIO helper found in stock.
	 * The uncalled enable path's original C is not recoverable from this
	 * clone; the shipped resume path drives the GPIO high directly.
	 * Stock ignores GPIO direction/output failures at shutdown.
	 * TODO: handle those failures separately from this reconstruction.
	 */
	if (!enable)
		gpio_direction_output(107, 0);
}
static void commit_static_frame(void) __asm__("commit_static_frame.constprop.16"); /* OEM clone name, not original C. */
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
 /* OEM branches on the pixel group before its single wire-frame store.
  * A volatile view keeps that branch in this compiler and preserves the
  * same one-store-per-pixel observation, including unchecked overflow.
  * TODO: bound the count in a separate behavior change. */
 volatile u32 *out;
 mutex_lock(access);
 state=RELOC_HIDE(&trimui_transfer_state,0);
 /* OEM starts one pixel ahead, then reads this pixel with negative byte
  * offsets. Preserve that address shape and the unchecked count. */
 src=state->rgb+4; out=state->wire;
 /* Stock snapshots these values after locking, before conversion. */
 count=READ_ONCE(trimui_transfer_config.count);
 low=READ_ONCE(state->low_battery);
 /* Stock trusts the count beyond both arrays and its 32-entry scale table.
  * TODO: bound it in a separate behavior change. */
 for(i=0;i<count;i++) {
  /* Stock observes the scale before the three color-byte reads. */
  scale=READ_ONCE(*trimui_scale_by_led[i]);
  /* OEM skips the clamp entirely when the low-battery snapshot is clear. */
  if(low) {
   if(scale>3)scale=3;
  }
  /* Preserve byte observation order, wrap, and arithmetic blue shift.
   * TODO: normalize channel widths only as a separate fix. */
  blue=(u32)src[-4]*scale; red=(u32)src[-2]*scale; green=(u32)src[-3]*scale;
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
	/* Keep these effect roots live through their generator calls.  Stock
	 * retains the left pointer in x22 and F2 pointer in x18.  The register
	 * bindings recover the daemon's 824-byte body size with the OEM GCC.
	 */
	register struct trimui_led_effect *left asm("x22") = &trimui_effect_l;
	register struct trimui_led_effect *front2 asm("x18") = &trimui_effect_f2;
	asm volatile("" : "+r"(left));
	asm volatile("" : "+r"(front2));
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
	gen_effect(left, 2, 0xffffff, 0xffffff);
	gen_effect(&trimui_effect_r, 2, 0xffffff, 0xffffff);
	gen_effect(&trimui_effect_f1, 2, 0x00ff33, 0x00ff33);
	gen_effect(front2, 2, 0x00ff33, 0x00ff33);
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
			if (is_effect_actived(left)) {
				rgb = step_one_frame(left);
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
			if (is_effect_actived(front2))
				trimui_frame[8] = step_one_frame(front2);
		} else if (trimui_frames_enable) {
			int offset = trimui_frame_index * 14;
			if (trimui_frames_len <= 0)
				goto battery;
			/* TODO: bound the animation length/index in a corrective patch.
			 * Stock's 32-bit offset wraps; sysfs can read before/after anim_frames.
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

/* Keep these small readers in the order found in the stock KALLSYMS table.
 * GCC otherwise reorders them even when their definitions appear in this order.
 * This preserves the linked callback addresses and the OEM's existing reads.
 */
static __attribute__((no_reorder)) ssize_t show_tg5040_max_scale(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_limit);
}

static __attribute__((no_reorder)) ssize_t show_frames_override_lr_enable(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_mask_lr);
}

static __attribute__((no_reorder)) ssize_t show_frames_override_f1f2_enable(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_mask_f);
}

static __attribute__((no_reorder)) ssize_t show_frames_override_m_enable(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_mask_m);
}

static __attribute__((no_reorder)) ssize_t show_frames_enable(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_frames_enable);
}

static __attribute__((no_reorder)) ssize_t show_frames_cycles(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_frames_cycles);
}

static __attribute__((no_reorder)) ssize_t show_frames_fps(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_frames_fps);
}

static __attribute__((no_reorder)) ssize_t show_frames_len(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_frames_len);
}

static __attribute__((no_reorder)) ssize_t show_effect_enable(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_enable);
}

static __attribute__((no_reorder)) ssize_t show_effect_f2(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_f2.type);
}

static __attribute__((no_reorder)) ssize_t show_effect_f1(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_f1.type);
}

static __attribute__((no_reorder)) ssize_t show_effect_r(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_r.type);
}

static __attribute__((no_reorder)) ssize_t show_effect_l(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_l.type);
}

static __attribute__((no_reorder)) ssize_t show_effect_lr(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_l.type);
}

static __attribute__((no_reorder)) ssize_t show_effect_m(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_m.type);
}

static __attribute__((no_reorder)) ssize_t show_cycles(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_l.cycles);
}

static __attribute__((no_reorder)) ssize_t show_cycles_l(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_l.cycles);
}

static __attribute__((no_reorder)) ssize_t show_cycles_f2(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_f2.cycles);
}

static __attribute__((no_reorder)) ssize_t show_cycles_f1(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_f1.cycles);
}

static __attribute__((no_reorder)) ssize_t show_cycles_r(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_r.cycles);
}

static __attribute__((no_reorder)) ssize_t show_cycles_m(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_m.cycles);
}

static __attribute__((no_reorder)) ssize_t show_duration(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_l.duration);
}

static __attribute__((no_reorder)) ssize_t show_duration_l(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_l.duration);
}

static __attribute__((no_reorder)) ssize_t show_duration_f2(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_f2.duration);
}

static __attribute__((no_reorder)) ssize_t show_duration_f1(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_f1.duration);
}

static __attribute__((no_reorder)) ssize_t show_duration_r(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_r.duration);
}

static __attribute__((no_reorder)) ssize_t show_duration_m(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_effect_m.duration);
}

static __attribute__((no_reorder)) ssize_t show_max_scale_f1f2(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_scale_f);
}

static __attribute__((no_reorder)) ssize_t show_max_scale_lr(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_scale_lr);
}

static __attribute__((no_reorder)) ssize_t show_max_scale(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_scale_m);
}

static __attribute__((no_reorder)) ssize_t show_led_enable(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_led_enable);
}

static __attribute__((no_reorder)) ssize_t show_speaker_mute(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_mute_requested);
}

/* Stock ignores conversion failures, still returns count, and still performs
 * the callback's subsequent side effects. TODO: report parse errors in a fix.
 */
static ssize_t store_tg5040_max_scale(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_limit);
	if (trimui_limit > 255) trimui_limit = 255;
	return count;
}

static ssize_t store_led_enable(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_led_enable);
	return count;
}

static ssize_t store_max_scale(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_scale_m);
	if (trimui_scale_m > trimui_limit) trimui_scale_m = trimui_limit;
	return count;
}

static ssize_t store_max_scale_lr(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	kstrtoint(buffer, 0, (int *)&trimui_scale_lr);
	if (trimui_scale_lr > trimui_limit) trimui_scale_lr = trimui_limit;
	return count;
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
	/* Keep both OEM roots live before shifting the signed count. GCC emits
	 * the same address and count order without hard-coded ADRP/ADD text. */
	source = (u8 *)RELOC_HIDE(&trimui_transfer_state, 0) + 0x7cc;
	asm volatile("" : : "r"(source), "r"(size));
	size <<= 2;
	memcpy(buffer, source, size);
	return size;
}

static ssize_t store_frame(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	/* Stock compares count with the buffer address, not its size.
	 * TODO: use sizeof(frame) in a separate corrective patch. */
	/* Integer address form keeps the OEM frame/count save order. */
	u8 *frame = (u8 *)((unsigned long)&trimui_transfer_state + 0x7cc);
	if (count < (unsigned long)frame)
		memcpy(frame, buffer, count);
	else
		memcpy(frame, buffer, (int)(trimui_led_count << 2));
	return count;
}

static ssize_t store_frame_hex(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	register int i asm("w23") = 0; int bytes = (int)count;
	int rounded;
	const char *position = buffer;
	register u32 *pixels asm("x20");
	/* Integer root preserves the OEM frame address and x20 role.
	 * It also retains GCC's separate page and 0x7cc ADDs. */
	pixels = (u32 *)((unsigned long)&trimui_transfer_state + 0x7cc);
	/* Keep this line count for later OEM __LINE__ diagnostics. */
	if (bytes > (int)trimui_led_count * 7)
		bytes = trimui_led_count * 7;
	rounded = bytes / 7 * 7;
	/* Stock compares a signed 32-bit difference between the end and the
	 * advancing input pointer. Keep its wraparound and unchecked destination.
	 * TODO: bound the destination and use a non-wrapping length separately. */
	while ((int)((u32)(unsigned long)buffer + (u32)rounded -
		     (u32)(unsigned long)position) > 0) {
		int parsed = sscanf(position, "%X", &pixels[i]); /* OEM ignores errors. */
		position += 7; asm volatile("" : : "r"(parsed)); /* Pin index after call. */
		i++;
		/* Preserve stock's two-step end-minus-cursor comparison. */
		asm volatile("" : "+r"(rounded));
	}
	return count;
}

/* OEM scalar setters form the field address before saving the return count.
 * F1/F2 use the BSS pointer immediately before the LED state. The L/R/M
 * setters use interior effect-frame addresses as roots; these frames are
 * not read by the setters. The parser errors are ignored and count is
 * returned even for bad input; TODO: report errors in a separate fix. */
_Static_assert(sizeof(sunxi_led) +
	__builtin_offsetof(struct trimui_stock_led_state,
			   effects[1].frames[102]) == 0x1100,
	       "right-effect address root");
_Static_assert(sizeof(sunxi_led) +
	__builtin_offsetof(struct trimui_stock_led_state,
			   effects[2].frames[222]) == 0x2200,
	       "left-effect address root");
_Static_assert(sizeof(sunxi_led) +
	__builtin_offsetof(struct trimui_stock_led_state,
			   effects[3].frames[342]) == 0x3300,
	       "middle-effect address root");
static ssize_t store_duration_m(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	unsigned char *anchor = (unsigned char *)&trimui_led_state.effects[3].frames[342];
	int *value;
	size_t saved_count;

	asm volatile("" : "+r"(anchor));
	value = (int *)(anchor + sizeof(sunxi_led) +
		__builtin_offsetof(struct trimui_stock_led_state, effects[4].duration) - 0x3300);
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_duration_l(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	unsigned char *anchor = (unsigned char *)&trimui_led_state.effects[2].frames[222];
	int *value;
	size_t saved_count;

	asm volatile("" : "+r"(anchor));
	value = (int *)(anchor + sizeof(sunxi_led) +
		__builtin_offsetof(struct trimui_stock_led_state, effects[3].duration) - 0x2200);
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_duration_r(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	unsigned char *anchor = (unsigned char *)&trimui_led_state.effects[1].frames[102];
	int *value;
	size_t saved_count;

	asm volatile("" : "+r"(anchor));
	value = (int *)(anchor + sizeof(sunxi_led) +
		__builtin_offsetof(struct trimui_stock_led_state, effects[2].duration) - 0x1100);
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_duration_f1(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	unsigned char *anchor = (unsigned char *)&sunxi_led;
	int *value;
	size_t saved_count;

	asm volatile("" : "+r"(anchor));
	value = (int *)(anchor + sizeof(sunxi_led) +
		__builtin_offsetof(struct trimui_stock_led_state, effects[1].duration));
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_duration_f2(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	unsigned char *anchor = (unsigned char *)&sunxi_led;
	int *value;
	size_t saved_count;

	asm volatile("" : "+r"(anchor));
	value = (int *)(anchor + sizeof(sunxi_led) +
		__builtin_offsetof(struct trimui_stock_led_state, effects[0].duration));
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_duration(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	struct trimui_effect_pair_view *pair;
	struct trimui_led_effect *left;

	/* Keep the C pair root and parser target live before saving count. */
	pair = &trimui_effect_pair;
	asm volatile("" : "+r"(pair));
	left = &pair->left;
	asm volatile("" : "+r"(count) : "r"(&left->duration));

	/* OEM copies the stale value to the right on parse failure too.
	 * TODO: reject invalid input only in a separate behavior change. */
	kstrtoint(buffer, 0, &left->duration);
	trimui_effect_r.duration = left->duration;
	return count;
}

static ssize_t store_cycles_m(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	unsigned char *anchor = (unsigned char *)&trimui_led_state.effects[3].frames[342];
	int *value;
	size_t saved_count;

	asm volatile("" : "+r"(anchor));
	value = (int *)(anchor + sizeof(sunxi_led) +
		__builtin_offsetof(struct trimui_stock_led_state, effects[4].cycles) - 0x3300);
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_cycles_l(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	unsigned char *anchor = (unsigned char *)&trimui_led_state.effects[2].frames[222];
	int *value;
	size_t saved_count;

	asm volatile("" : "+r"(anchor));
	value = (int *)(anchor + sizeof(sunxi_led) +
		__builtin_offsetof(struct trimui_stock_led_state, effects[3].cycles) - 0x2200);
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_cycles_r(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	unsigned char *anchor = (unsigned char *)&trimui_led_state.effects[1].frames[102];
	int *value;
	size_t saved_count;

	asm volatile("" : "+r"(anchor));
	value = (int *)(anchor + sizeof(sunxi_led) +
		__builtin_offsetof(struct trimui_stock_led_state, effects[2].cycles) - 0x1100);
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_cycles_f1(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	unsigned char *anchor = (unsigned char *)&sunxi_led;
	int *value;
	size_t saved_count;

	asm volatile("" : "+r"(anchor));
	value = (int *)(anchor + sizeof(sunxi_led) +
		__builtin_offsetof(struct trimui_stock_led_state, effects[1].cycles));
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_cycles_f2(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	unsigned char *anchor = (unsigned char *)&sunxi_led;
	int *value;
	size_t saved_count;

	asm volatile("" : "+r"(anchor));
	value = (int *)(anchor + sizeof(sunxi_led) +
		__builtin_offsetof(struct trimui_stock_led_state, effects[0].cycles));
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_cycles(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	struct trimui_effect_pair_view *pair;
	struct trimui_led_effect *left;

	/* Match the duration setter's C root and callback argument order. */
	pair = &trimui_effect_pair;
	asm volatile("" : "+r"(pair));
	left = &pair->left;
	asm volatile("" : "+r"(count) : "r"(&left->cycles));

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

static noinline u32 get_rgb_hex(struct class *class, const char *buffer)
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
	u32 rgb = get_rgb_hex(class, buffer);
	trimui_effect_m.target_rgb = rgb;
	return count;
}

static ssize_t show_effect_rgb_hex_l(struct class *class, struct class_attribute *attr, char *buffer)
{
	return dump_rgb_hex(buffer, trimui_effect_l.target_rgb);
}

static ssize_t store_effect_rgb_hex_l(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	u32 rgb = get_rgb_hex(class, buffer);
	trimui_effect_l.target_rgb = rgb;
	return count;
}

static ssize_t show_effect_rgb_hex_r(struct class *class, struct class_attribute *attr, char *buffer)
{
	return dump_rgb_hex(buffer, trimui_effect_r.target_rgb);
}

static ssize_t store_effect_rgb_hex_r(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	u32 rgb = get_rgb_hex(class, buffer);
	trimui_effect_r.target_rgb = rgb;
	return count;
}

static ssize_t show_effect_rgb_hex_f1(struct class *class, struct class_attribute *attr, char *buffer)
{
	return dump_rgb_hex(buffer, trimui_effect_f1.target_rgb);
}

static ssize_t store_effect_rgb_hex_f1(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	u32 rgb = get_rgb_hex(class, buffer);
	trimui_effect_f1.target_rgb = rgb;
	return count;
}

static ssize_t show_effect_rgb_hex_f2(struct class *class, struct class_attribute *attr, char *buffer)
{
	return dump_rgb_hex(buffer, trimui_effect_f2.target_rgb);
}

static ssize_t store_effect_rgb_hex_f2(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	u32 rgb = get_rgb_hex(class, buffer);
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
 u32 rgb=get_rgb_hex(class, buffer);
 /* Stock publishes left before right without synchronization, even after
  * malformed input produced zero. TODO: reject invalid input and protect
  * the pair only in a separate behavior change. */
 WRITE_ONCE(trimui_target_l,rgb);
 WRITE_ONCE(trimui_target_r,rgb);
 return count;
}

/* OEM ignores kstrtoint failures in all six effect setters below. Malformed
 * input keeps the previous type, but still regenerates the effect and resets
 * its playback state; preserve that observable bug.
 */
#define TRIMUI_EFFECT_AT(root, offset) \
	((struct trimui_led_effect *)((root) + (offset)))

/* The OEM forms the effect-M address from the page containing this byte.
 * Keep the address schedule; all parser, generator, and state effects below
 * remain in C. The alias does not allocate storage. */
extern unsigned char trimui_m_page;
asm(".local trimui_m_page\n.type trimui_m_page,%object\n"
    ".set trimui_m_page,trimui_led_state+0x2830\n.size trimui_m_page,1\n");
static ssize_t store_effect_m(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	register unsigned char *page asm("x19");
	register unsigned char *base asm("x20");
	register size_t saved asm("x21") = count;
	register struct trimui_led_effect *genptr asm("x14");
	register struct trimui_led_effect *callptr asm("x0");
	struct trimui_led_effect *effect;
	int parsed_type;
	u32 current_color, target_color;

	base = (unsigned char *)&trimui_m_page + 0xac8;
	page = (unsigned char *)&trimui_m_page;
	effect = (struct trimui_led_effect *)(base + 0x9a8);
	/* OEM ignores parse failure, then clamps any type above 7. */
	kstrtoint(buffer, 0, (int *)&effect->type);
	if ((unsigned int)effect->type > 7)
		effect->type = 0;
	page += 0xac8;
	asm volatile("" : "+r"(page));
	effect = (struct trimui_led_effect *)(page + 0x9a8);
	/* Retain OEM's separate x14 address calculation for the generator. */
	genptr = (struct trimui_led_effect *)(page + 0x9a8);
	asm volatile("" : "+r"(genptr));
	callptr = genptr;
	asm volatile("" : "+r"(callptr));
	parsed_type = READ_ONCE(effect->type);
	current_color = READ_ONCE(effect->current_rgb);
	target_color = READ_ONCE(effect->target_rgb);
	gen_effect(callptr, parsed_type, target_color, current_color);
	/* OEM resets the index before reading cycles, even if state changes. */
	WRITE_ONCE(effect->index, 0);
	WRITE_ONCE(effect->active_cycles, READ_ONCE(effect->cycles));
	return saved;
}

extern unsigned char trimui_l_page;
asm(".local trimui_l_page\n.type trimui_l_page,%object\n"
    ".set trimui_l_page,trimui_led_state+0x1830\n.size trimui_l_page,1\n");
static ssize_t store_effect_l(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	register unsigned char *page asm("x19");
	register unsigned char *base asm("x20");
	register size_t saved asm("x21") = count;
	register struct trimui_led_effect *genptr asm("x14");
	register struct trimui_led_effect *callptr asm("x0");
	struct trimui_led_effect *effect;
	int parsed_type;
	u32 current_color, target_color;

	base = (unsigned char *)&trimui_l_page + 0x9c8;
	page = (unsigned char *)&trimui_l_page;
	effect = (struct trimui_led_effect *)(base + 0xb88);
	/* OEM ignores parse failure, then clamps any type above 7. */
	kstrtoint(buffer, 0, (int *)&effect->type);
	if ((unsigned int)effect->type > 7)
		effect->type = 0;
	page += 0x9c8;
	asm volatile("" : "+r"(page));
	effect = (struct trimui_led_effect *)(page + 0xb88);
	genptr = (struct trimui_led_effect *)(page + 0xb88);
	asm volatile("" : "+r"(genptr));
	callptr = genptr;
	asm volatile("" : "+r"(callptr));
	parsed_type = READ_ONCE(effect->type);
	current_color = READ_ONCE(effect->current_rgb);
	target_color = READ_ONCE(effect->target_rgb);
	gen_effect(callptr, parsed_type, target_color, current_color);
	WRITE_ONCE(effect->index, 0);
	WRITE_ONCE(effect->active_cycles, READ_ONCE(effect->cycles));
	return saved;
}

extern unsigned char trimui_r_page;
asm(".local trimui_r_page\n.type trimui_r_page,%object\n"
    ".set trimui_r_page,trimui_led_state+0x830\n.size trimui_r_page,1\n");
static ssize_t store_effect_r(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	register unsigned char *page asm("x19");
	register unsigned char *base asm("x20");
	register size_t saved asm("x21") = count;
	register struct trimui_led_effect *genptr asm("x14");
	register struct trimui_led_effect *callptr asm("x0");
	struct trimui_led_effect *effect;
	int parsed_type;
	u32 current_color, target_color;

	base = (unsigned char *)&trimui_r_page + 0x8c8;
	page = (unsigned char *)&trimui_r_page;
	effect = (struct trimui_led_effect *)(base + 0xd68);
	/* OEM ignores parse failure, then clamps any type above 7. */
	kstrtoint(buffer, 0, (int *)&effect->type);
	if ((unsigned int)effect->type > 7)
		effect->type = 0;
	page += 0x8c8;
	asm volatile("" : "+r"(page));
	effect = (struct trimui_led_effect *)(page + 0xd68);
	genptr = (struct trimui_led_effect *)(page + 0xd68);
	asm volatile("" : "+r"(genptr));
	callptr = genptr;
	asm volatile("" : "+r"(callptr));
	parsed_type = READ_ONCE(effect->type);
	current_color = READ_ONCE(effect->current_rgb);
	target_color = READ_ONCE(effect->target_rgb);
	gen_effect(callptr, parsed_type, target_color, current_color);
	WRITE_ONCE(effect->index, 0);
	WRITE_ONCE(effect->active_cycles, READ_ONCE(effect->cycles));
	return saved;
}
#undef TRIMUI_EFFECT_AT

static ssize_t store_effect_f1(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	register unsigned char *root asm("x20") = (unsigned char *)&sunxi_led;
	struct trimui_led_effect *effect = (struct trimui_led_effect *)(root + 0xf48);
	register struct trimui_led_effect *argument asm("x14");
	register size_t saved_count asm("x21") = count;
	register int parser_base asm("w1") = 0;

	/* The OEM uses the pointer slot eight bytes before trimui_led_state as
	 * its address root. Keep its parser and generator pointer lifetimes. */
	/* OEM ignores parse errors, then clamps any type above 7 to zero. */
	kstrtoint(buffer, parser_base, (int *)&effect->type);
	if ((unsigned int)effect->type > 7)
		effect->type = 0;
	argument = effect;
	asm volatile("" : "+r"(argument));
	/* Keep the OEM x0 setup before its reread of type. */
	{
		register struct trimui_led_effect *call asm("x0") = argument;

		asm volatile("" : "+r"(call));
		gen_effect(call, effect->type, effect->target_rgb, effect->current_rgb);
	}
	WRITE_ONCE(effect->index, 0);
	WRITE_ONCE(effect->active_cycles, READ_ONCE(effect->cycles));
	return saved_count;
}

static ssize_t store_effect_f2(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	register unsigned char *root asm("x20") = (unsigned char *)&sunxi_led;
	struct trimui_led_effect *effect = (struct trimui_led_effect *)(root + 0x28);
	register struct trimui_led_effect *argument asm("x14");
	register size_t saved_count asm("x21") = count;
	register int parser_base asm("w1") = 0;

	/* The OEM uses the pointer slot eight bytes before trimui_led_state as
	 * its address root. Keep its parser and generator pointer lifetimes. */
	/* OEM ignores parse errors, then clamps any type above 7 to zero. */
	kstrtoint(buffer, parser_base, (int *)&effect->type);
	if ((unsigned int)effect->type > 7)
		effect->type = 0;
	argument = effect;
	asm volatile("" : "+r"(argument));
	/* Keep the OEM x0 setup before its reread of type. */
	{
		register struct trimui_led_effect *call asm("x0") = argument;

		asm volatile("" : "+r"(call));
		gen_effect(call, effect->type, effect->target_rgb, effect->current_rgb);
	}
	WRITE_ONCE(effect->index, 0);
	WRITE_ONCE(effect->active_cycles, READ_ONCE(effect->cycles));
	return saved_count;
}

#define TRIMUI_PAIR_LEFT(root) \
	((struct trimui_led_effect *)((root) + 0xb88))
#define TRIMUI_PAIR_RIGHT(root) \
	((struct trimui_led_effect *)((root) + 0xd68))
static ssize_t store_effect_lr(struct class *class, struct class_attribute *attr,
			       const char *buffer, size_t count)
{
	register unsigned char *page asm("x19");
	register unsigned char *left_root asm("x20");
	register unsigned char *right_root asm("x18");
	register struct trimui_led_effect *left_argument asm("x15");
	register struct trimui_led_effect *right_argument asm("x14");
	register struct trimui_led_effect *left_call asm("x0");
	register size_t saved_count asm("x21") = count;
	register int parser_base asm("w1") = 0;
	int right_cycles, left_type, left_target, left_current;
	int right_type, right_target, right_current;

	/* OEM ignores parser errors and still regenerates both effects. */
	left_root = (unsigned char *)&trimui_l_page + 0x9c8;
	page = (unsigned char *)&trimui_l_page;
	kstrtoint(buffer, parser_base, (int *)&TRIMUI_PAIR_LEFT(left_root)->type);
	if ((unsigned int)TRIMUI_PAIR_LEFT(left_root)->type > 7)
		TRIMUI_PAIR_LEFT(left_root)->type = 0;
	page += 0x9c8;
	asm volatile("" : "+r"(page));
	right_root = (unsigned char *)&trimui_led_state + 0x10f8;
	left_argument = TRIMUI_PAIR_LEFT(page);
	right_argument = TRIMUI_PAIR_RIGHT(right_root);
	asm volatile("" : "+r"(left_argument), "+r"(right_argument));
	left_call = left_argument;
	asm volatile("" : "+r"(left_call));
	left_type = READ_ONCE(TRIMUI_PAIR_LEFT(page)->type);
	left_current = READ_ONCE(TRIMUI_PAIR_LEFT(page)->current_rgb);
	left_target = READ_ONCE(TRIMUI_PAIR_LEFT(page)->target_rgb);
	WRITE_ONCE(TRIMUI_PAIR_RIGHT(right_root)->type, left_type);
	gen_effect(left_call, left_type, left_target, left_current);
	/* OEM carries x14 across the first generator call. This relies on the
	 * pinned gen_effect body not writing x14; a generic ABI call may clobber it. */
	right_type = READ_ONCE(TRIMUI_PAIR_RIGHT(right_root)->type);
	right_current = READ_ONCE(TRIMUI_PAIR_RIGHT(right_root)->current_rgb);
	right_target = READ_ONCE(TRIMUI_PAIR_RIGHT(right_root)->target_rgb);
	gen_effect(right_argument, right_type, right_target, right_current);
	WRITE_ONCE(TRIMUI_PAIR_LEFT(page)->index, 0);
	WRITE_ONCE(TRIMUI_PAIR_LEFT(page)->active_cycles,
		READ_ONCE(TRIMUI_PAIR_LEFT(page)->cycles));
	/* OEM reads right cycles before resetting right index. */
	right_cycles = READ_ONCE(TRIMUI_PAIR_RIGHT(right_root)->cycles);
	WRITE_ONCE(TRIMUI_PAIR_RIGHT(right_root)->index, 0);
	/* OEM's ordinary final store is scheduled after the x19/x20 restore.
	 * WRITE_ONCE moves it before the restore with this compiler. */
	TRIMUI_PAIR_RIGHT(right_root)->active_cycles = right_cycles;
	return saved_count;
}
#undef TRIMUI_PAIR_LEFT
#undef TRIMUI_PAIR_RIGHT

/* This view begins eight bytes before the LED state. The adjacent scalar
 * fields share the address root used by the stock sysfs setters. */
struct trimui_frame_control_view {
	u8 unused[8];
	int mask_lr, mask_f, mask_m;
	int enable, cycles, fps, len, effect_enable;
};
extern struct trimui_frame_control_view trimui_frame_controls;

static ssize_t store_effect_enable(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	unsigned char *anchor = (unsigned char *)&sunxi_led;
	int *value;
	size_t saved_count;

	asm volatile("" : "+r"(anchor));
	value = (int *)(anchor + sizeof(sunxi_led) +
		__builtin_offsetof(struct trimui_stock_led_state, effect_enable));
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
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
	/* Stock keeps the animation index in a 64-bit register. */
	register long i asm("x21") = 0;
	int count = 0;
	char *position = buffer;
	u32 *pixels = trimui_anim_frames;
	/* Stock advances by the cumulative length, producing sparse output.
	 * At 34 LEDs the newline lands beyond a 4 KiB sysfs page.
	 * TODO: advance by the last write and bound output in a separate fix. */
	while ((int)i < (int)trimui_led_count) {
		count += 7;
		/* Keep the destination in x0 before setting the format argument. */
		{
			register char *destination asm("x0") = position;
			asm("" : "+r"(destination));
			sprintf(destination, "%06X ", pixels[i] & 0xffffff);
		}
		position += count;
		/* Keep the cumulative count live across the next LED-count read. */
		asm volatile("" : "+r"(count));
		i++;
	}
	strcpy(position, "\n");
	return count + 1;
}

/* OEM text and its animation/format/help literals follow this declaration
 * order. Keep the three functions together when changing the recovered C.
 */
static __attribute__((no_reorder)) ssize_t store_anim_frames_hex(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	register long i asm("x24");
	int colors = (int)(count / 7);
	register const char *position asm("x25") = buffer;
	unsigned int offset;
	register const char *format asm("x26") = "%X";
	register u32 *pixels asm("x22");
	register u32 *root asm("x1");
	register u32 *append asm("x23");

	/* These register lifetimes reproduce the OEM loop and pointer schedule. */
	if (colors > 4200)
		colors = 4200;
	/* Stock wraps the frame offset in 32 bits once, then sign-extends it
	 * before adding the 64-bit loop index. Recomputing offset + i in an
	 * int changes the destination around the sign boundary. The append
	 * range is still unchecked; TODO: reject out-of-range frame counts. */
	offset = (unsigned int)trimui_frame_controls.len * 14u;
	/* Stock forms this root after the frame offset multiplication. */
	asm("adrp %0, trimui_led_state+0x4ff0\n\t"
	    "add %0, %0, :lo12:trimui_led_state+0x4ff0"
	    : "=&r"(root));
	pixels = root;
	append = pixels + (int)offset;
	/* Stock ignores append capacity and clears the start of the buffer,
	 * even when sscanf writes at a later append offset. TODO: validate the
	 * append and clear its actual destination in a separate fix. */
	for (i = 0; (int)i < colors; ) {
		unsigned long byte_offset = (unsigned long)i << 2;
		const char *scan = position;

		position += 7;
		i++;
		*(u32 *)((char *)pixels + byte_offset) = 0;
		sscanf(scan, format, (u32 *)((char *)append + byte_offset));
		asm volatile("" : "+r"(position));
	}
	trimui_frame_controls.len += colors / 14;
	printk("\n anim frames:%d, pixels:%d\n", trimui_frame_controls.len, colors);
	return count;
}

static __attribute__((no_reorder)) ssize_t show_frame_hex(struct class *class, struct class_attribute *attr, char *buffer)
{
	/* Stock advances the index in x22 but compares its low signed word.
	 * A mutable LED count can change between loop iterations. */
	register long i asm("x22") = 0;
	int count = 0;
	register char *position asm("x21") = buffer;
	register u32 *pixels asm("x20") =
		/* OEM page-ADD order; 0x7cc remains a separate ADD. */ (u32 *)((unsigned long)&trimui_transfer_state + 0x7cc);
	register const char *format_page asm("x24");
	/* Keep the OEM frame root and late format-page offset while retaining
	 * the compiler's pooled format literal. */
	asm("adrp %0, %1" : "=r"(format_page) : "S"("%06X "));
	/* Stock advances by the cumulative length, producing sparse output.
	 * At 34 LEDs the newline lands beyond a 4 KiB sysfs page.
	 * TODO: advance by the last write and bound output in a separate fix. */
	while ((int)i < (int)trimui_led_count) {
		count += 7;
		{
			const char *format;
			/* Stock puts the destination in x0 before forming the
			 * pooled format address. This constrains only argument order;
			 * the OEM sparse-output and overflow behavior remains.
			 */
			register char *destination asm("x0") = position;
			asm("" : "+r"(destination));
			asm("add %0, %1, :lo12:%2"
			    : "=r"(format) : "r"(format_page), "S"("%06X "));
			sprintf(destination, format, pixels[i] & 0xffffff);
		}
		position += count;
		/* Keep the cumulative count live across the next LED-count read. */
		asm volatile("" : "+r"(count));
		i++;
	}
	strcpy(position, "\n");
	return count + 1;
}

static __attribute__((no_reorder)) ssize_t help_show_effect(struct class *class, struct class_attribute *attr, char *buffer)
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

static ssize_t store_frames_len(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	unsigned char *anchor = (unsigned char *)&sunxi_led;
	int *value;
	size_t saved_count;

	asm volatile("" : "+r"(anchor));
	value = (int *)(anchor + sizeof(sunxi_led) +
		__builtin_offsetof(struct trimui_stock_led_state, frames_len));
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_frames_fps(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	unsigned char *anchor = (unsigned char *)&sunxi_led;
	int *value;
	size_t saved_count;

	asm volatile("" : "+r"(anchor));
	value = (int *)(anchor + sizeof(sunxi_led) +
		__builtin_offsetof(struct trimui_stock_led_state, frames_fps));
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
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
	int parser_base = 0;

	/* Keep OEM's parser zero, BSS root, and saved-count schedule. */
	asm volatile("" : "+r"(parser_base));
	control = &trimui_frame_controls;
	asm volatile("" : "+r"(control));
	result = count;
	asm volatile("" : "+r"(result));
	/* OEM ignores parse errors and clears enable regardless.  TODO: reject
	 * invalid input only in a separate behavior-changing fix. */
	kstrtoint(buffer, parser_base, &control->cycles);
	/* OEM materializes the counter root before clearing enable.  This C
	 * reference retains the same instruction order with the pinned compiler. */
	counter = &trimui_frame_counters;
	control->enable = 0;
	/* Stock issues two separate 32-bit resets.  A frame worker can observe
	 * the intermediate state; TODO: synchronize only in a separate fix. */
	WRITE_ONCE(counter->index, 0);
	WRITE_ONCE(counter->left, 0);
	return result;
}

static ssize_t store_frames_enable(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
 struct trimui_frame_control_view *control;
 size_t result;
 int parser_base = 0;
 int left;
 /* Preserve the OEM parser and count-save order around this C BSS root.
  * GCC otherwise moves the count save ahead of the address calculation. */
 asm volatile("" : "+r"(parser_base));
 control = &trimui_frame_controls;
 asm volatile("" : "+r"(control));
 asm volatile("mov %0, %1" : "=r"(result) : "r"(count));
 /* Stock ignores parse errors and still resets/decrements state.
  * TODO: reject errors only in a separate behavior change. */
 kstrtoint(buffer,parser_base,&control->enable);
 left=control->cycles-1;
 trimui_frame_counters.index=0;
 trimui_frame_counters.left=left;
 if(!control->enable)control->len=0;
 return result;
}

static ssize_t store_frames_override_m_enable(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	unsigned char *anchor = (unsigned char *)&sunxi_led;
	int *value;
	size_t saved_count;

	asm volatile("" : "+r"(anchor));
	value = (int *)(anchor + sizeof(sunxi_led) +
		__builtin_offsetof(struct trimui_stock_led_state, mask_m));
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_frames_override_f1f2_enable(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	unsigned char *anchor = (unsigned char *)&sunxi_led;
	int *value;
	size_t saved_count;

	asm volatile("" : "+r"(anchor));
	value = (int *)(anchor + sizeof(sunxi_led) +
		__builtin_offsetof(struct trimui_stock_led_state, mask_f));
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_frames_override_lr_enable(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	unsigned char *anchor = (unsigned char *)&sunxi_led;
	int *value;
	size_t saved_count;

	asm volatile("" : "+r"(anchor));
	value = (int *)(anchor + sizeof(sunxi_led) +
		__builtin_offsetof(struct trimui_stock_led_state, mask_lr));
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t show_motor_voltage(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_motor_regulator ? regulator_get_voltage(trimui_motor_regulator) : 0);
}

static ssize_t store_motor_voltage(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	struct trimui_motor_state_view *state;
	register size_t saved_count asm("x20");

	/* OEM keeps this state pointer live across kstrtoint and then re-reads
	 * voltage from the same object. Keep the pointer live without adding a
	 * memory access or changing the stock NULL/error-pointer handling.
	 * Saving count after forming the state root preserves OEM ADRP/ADD/MOV
	 * order without changing the stale-value behavior on parse failure. */
	state = &trimui_motor_state;
	asm("" : "+r"(state));
	asm("mov %0, %1" : "=r"(saved_count) : "r"(count), "r"(state));
	kstrtoint(buffer, 0, &state->voltage);
	/* Stock passes NULL/error pointers through. TODO: check regulator_get in a separate fix. */
	regulator_set_voltage(state->regulator, state->voltage,
		state->voltage + 100000);
	return saved_count;
}

static ssize_t store_speaker_mute(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	unsigned char *root = (unsigned char *)&trimui_transfer_state;
	int *requested;
	register size_t saved_count asm("x20");

	/* Stock addresses mute through the transfer-state root, then offsets
	 * 0x7c8 into the shared LED state. Keep that root across the parser.
	 * Save count after forming the root: OEM emits ADRP/ADD before MOV x20.
	 * The explicit register move only constrains this setup order. */
	asm("" : "+r"(root));
	asm("mov %0, %1" : "=r"(saved_count) : "r"(count), "r"(root));
	requested = (int *)(root + 0x7c8);
	/* OEM ignores parse failure and may use the prior mute request. */
	kstrtoint(buffer, 0, requested);
	if (*requested) {
		trimui_speaker_mute = 1;
		trimui_set_speaker_enable(0);
	} else {
		trimui_speaker_mute = 0;
		trimui_set_speaker_enable(1);
	}
	printk("speaker force mute:%d\n", trimui_speaker_mute);
	return saved_count;
}

/* Stock places this table after the motor, speaker and animation attributes.
 * GCC 7 emits these static declarations in reverse order within this unit.
 */
static struct class_attribute led_class_attrs[] = {
	__ATTR(light, 0644, led_show, NULL),
	__ATTR(tg5040_exterme_brightness, 0664,
	       show_tg5040_max_scale, store_tg5040_max_scale),
	__ATTR_NULL,
};

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

/* OEM probe holds these four effect addresses across the memset calls.
 * They view existing BSS: no independent storage or change to the state ABI.
 * Original declaration boundaries are unknown.
 */
extern struct trimui_led_effect trimui_probe_effect_f2;
extern struct trimui_led_effect trimui_probe_effect_f1;
extern struct trimui_led_effect trimui_probe_effect_r;
extern struct trimui_led_effect trimui_probe_effect_l;
asm(".local trimui_probe_effect_f2\n.type trimui_probe_effect_f2,%object\n"
    ".set trimui_probe_effect_f2,trimui_led_state+0x20\n"
    ".size trimui_probe_effect_f2,0xf20\n"
    ".local trimui_probe_effect_f1\n.type trimui_probe_effect_f1,%object\n"
    ".set trimui_probe_effect_f1,trimui_led_state+0xf40\n"
    ".size trimui_probe_effect_f1,0xf20\n"
    ".local trimui_probe_effect_r\n.type trimui_probe_effect_r,%object\n"
    ".set trimui_probe_effect_r,trimui_led_state+0x1e60\n"
    ".size trimui_probe_effect_r,0xf20\n"
    ".local trimui_probe_effect_l\n.type trimui_probe_effect_l,%object\n"
    ".set trimui_probe_effect_l,trimui_led_state+0x2d80\n"
    ".size trimui_probe_effect_l,0xf20\n");

static int trimui_board_init(void)
{
	struct task_struct *task;
	int result;

	BUILD_BUG_ON(offsetof(struct trimui_stock_led_state, frame) != 0x4bc4);
	BUILD_BUG_ON(offsetof(struct trimui_stock_led_state, motor_regulator) != 0x4dd8);
	BUILD_BUG_ON(offsetof(struct trimui_stock_led_state, wire_frame) != 0x4de4);
	BUILD_BUG_ON(offsetof(struct trimui_stock_led_state, anim_frames) != 0x4ff0);
	BUILD_BUG_ON(offsetof(struct trimui_stock_led_state, effects[0]) != 0x20);
	BUILD_BUG_ON(sizeof(struct trimui_led_effect) != 0xf20);
	memset(trimui_frame, 0, sizeof(trimui_frame));
	memset(&trimui_effect_m, 0, sizeof(trimui_effect_m));
	memset(&trimui_probe_effect_l, 0, sizeof(trimui_probe_effect_l));
	memset(&trimui_probe_effect_r, 0, sizeof(trimui_probe_effect_r));
	memset(&trimui_probe_effect_f1, 0, sizeof(trimui_probe_effect_f1));
	memset(&trimui_probe_effect_f2, 0, sizeof(trimui_probe_effect_f2));
	/* Preserve the OEM store order. The LED class is already visible while
	 * probe initializes these globals, so a reader can observe partial state. */
	WRITE_ONCE(trimui_probe_effect_f2.duration, 2000);
	WRITE_ONCE(trimui_effect_m.duration, 2000);
	WRITE_ONCE(trimui_probe_effect_f1.duration, 2000);
	WRITE_ONCE(trimui_probe_effect_r.duration, 2000);
	WRITE_ONCE(trimui_probe_effect_l.duration, 2000);
	WRITE_ONCE(trimui_probe_effect_f1.cycles, 1);
	WRITE_ONCE(trimui_probe_effect_f2.cycles, 1);
	WRITE_ONCE(trimui_effect_m.cycles, 1);
	WRITE_ONCE(trimui_probe_effect_r.cycles, 1);
	WRITE_ONCE(trimui_probe_effect_l.cycles, 1);
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
