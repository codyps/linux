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
struct trimui_stock_led_state {
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
};

static int trimui_mask_lr __attribute__((used, no_reorder, aligned(4)));
static int trimui_mask_f __attribute__((used, no_reorder, aligned(4)));
static int trimui_mask_m __attribute__((used, no_reorder, aligned(4)));
static int trimui_frames_enable __attribute__((used, no_reorder, aligned(4)));
static int trimui_frames_cycles __attribute__((used, no_reorder, aligned(4)));
static int trimui_frames_fps __attribute__((used, no_reorder, aligned(4)));
static int trimui_frames_len __attribute__((used, no_reorder, aligned(4)));
static int trimui_effect_enable __attribute__((used, no_reorder, aligned(4)));
static struct trimui_led_effect trimui_effect_f2 __attribute__((used, no_reorder, aligned(4)));
static struct trimui_led_effect trimui_effect_f1 __attribute__((used, no_reorder, aligned(4)));
static struct trimui_led_effect trimui_effect_r __attribute__((used, no_reorder, aligned(4)));
static struct trimui_led_effect trimui_effect_l __attribute__((used, no_reorder, aligned(4)));
static struct trimui_led_effect trimui_effect_m __attribute__((used, no_reorder, aligned(4)));
static int trimui_mute_requested __attribute__((used, no_reorder, aligned(4)));
static u32 trimui_frame[128] __attribute__((used, no_reorder, aligned(4)));
static int trimui_low_battery __attribute__((used, no_reorder, aligned(4)));
static int trimui_frame_index __attribute__((used, no_reorder, aligned(4)));
static int trimui_cycles_left __attribute__((used, no_reorder, aligned(4)));
static int trimui_motor_voltage __attribute__((used, no_reorder, aligned(4)));
static struct regulator * trimui_motor_regulator __attribute__((used, no_reorder, aligned(8)));
static int trimui_suspended __attribute__((used, no_reorder, aligned(4)));
/* Recovered stock lockdep-disabled layout: five empty key objects have
 * the same address as the wire buffer. These are real GNU C objects, not
 * aliases onto another allocation. Original declaration spelling is unknown.
 */
_Static_assert(sizeof(struct lock_class_key) == 0, "stock empty lock keys");
static struct lock_class_key trimui_wait_key __attribute__((used, no_reorder));
static struct lock_class_key trimui_main_key __attribute__((used, no_reorder));
static struct lock_class_key trimui_anim_key __attribute__((used, no_reorder));
static struct lock_class_key trimui_motor_key __attribute__((used, no_reorder));
static struct lock_class_key trimui_speaker_key __attribute__((used, no_reorder));
static u32 trimui_wire_frame[128] __attribute__((used, no_reorder, aligned(4)));
static u8 trimui_animation_alignment[12] __attribute__((used, no_reorder, aligned(1)));
static u32 trimui_anim_frames[14400] __attribute__((used, no_reorder, aligned(4)));

/* Config scalars live in the typed trimui_config object. */
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
#define trimui_transfer_config trimui_config
/* Stock retains two distinct stores, including on negative capacity errors.
 * Keep its out-of-line helper and the same state view used by frame output.
 * TODO: report power-supply read failures in a separate behavior change.
 */
static noinline void check_battery(void)
{
	if (trimui_axp2202_get_bat_cap() < 16)
		trimui_low_battery = 1;
	else
		trimui_low_battery = 0;
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
#include "leds-trimui-frame-byte.h"
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
 const u8 *src;
 /* OEM branches on the pixel group before its single wire-frame store.
  * A volatile view keeps that branch in this compiler and preserves the
  * same one-store-per-pixel observation, including unchecked overflow.
  * TODO: bound the count in a separate behavior change. */
 volatile u32 *out;
 mutex_lock(access);
 /* OEM starts one pixel ahead, then reads this pixel with negative byte
  * offsets. Preserve that address shape and the unchecked count. */
 src=((u8 *)trimui_frame)+4; out=trimui_wire_frame;
 /* Stock snapshots these values after locking, before conversion. */
 count=READ_ONCE(trimui_transfer_config.count);
 low=READ_ONCE(trimui_low_battery);
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
  blue=TRIMUI_FRAME_BYTE(src-4)*scale; red=TRIMUI_FRAME_BYTE(src-2)*scale; green=TRIMUI_FRAME_BYTE(src-3)*scale;
  u32 high,partial; if(i<10){ high=(red<<8)&0xff0000; partial=TRIMUI_PACK_CAPTURE((green&0xff00)|(blue>>8)); }
  else { high=(green<<8)&0xff0000; partial=TRIMUI_PACK_CAPTURE((red&0xff00)|(blue>>8)); } out[i]=high|partial;
  src+=4;
 }
 if(trimui_suspended || !trimui_transfer_config.enable)
  memset(trimui_wire_frame,0,sizeof(trimui_wire_frame));
 /* Stock reloads the device here while retaining the initial mutex. */
 led=READ_ONCE(sunxi_led);
 do_gettimeofday(&start);
 /* Lock/transfer/completion results remain ignored as shipped. */
 spin_lock_irqsave(&led->lock,flags);
 led->length=count;
 for(j=0;(unsigned int)j<READ_ONCE(led->length);j++) { /* OEM signed wrap past INT_MAX can address before wire. */
  u32 *dest=READ_ONCE(led->data);
  /* OEM sign-extends the wrapped 32-bit index before both addresses.
   * Ordinary array indexing let this compiler widen only the destination
   * induction variable and lose that behavior after INT_MAX. Keep the
   * byte offset numeric so it also preserves the OEM backward/fault
   * address rather than forming an out-of-array intermediate C pointer.
   * TODO: bound the transfer only in a separate behavior change. */
  unsigned long offset=(unsigned long)(long)(s32)(u32)j << 2;
  u32 pixel=READ_ONCE(*(u32 *)((unsigned long)trimui_wire_frame+offset));
  WRITE_ONCE(*(u32 *)((unsigned long)dest+offset),pixel);
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

__attribute__((target("tune=cortex-a53")))
__attribute__((optimize("reorder-blocks-algorithm=stc")))
static int trimui_ledc_run_thread(void *unused)
{
	/* Startup roots only; recompute F2 after caller-clobbering services. */
	struct trimui_led_effect *left = &(trimui_effect_l);
	struct trimui_led_effect *front2 = &(trimui_effect_f2);

	asm volatile("" : "+r"(front2));
	int battery_ticks = 0; /* OEM signed comparison, including negative values. */
	u32 rgb;
	u32 boot_white = 0xffffff;
	u32 boot_orange = 0x00ff33;
	asm("" : : "r"(boot_orange)); /* Keep the stock worker instruction span. */
	trimui_scale_m = 14;
	/* Stock publishes this control as a separate 32-bit store before the
	 * adjacent first effect type.  A plain store lets GCC merge the two.
	 */
	WRITE_ONCE(trimui_effect_enable, 1);
	/* Stock exposes all five cycle counts before any effect shape.  Sysfs
	 * readers can observe these intermediate values during startup.
	 */
#define INITIAL_CYCLES(e) WRITE_ONCE((e).cycles, 5)
	INITIAL_CYCLES((trimui_effect_m));
	INITIAL_CYCLES(trimui_effect_r);
	INITIAL_CYCLES((trimui_effect_l));
	INITIAL_CYCLES((trimui_effect_f2));
	INITIAL_CYCLES((trimui_effect_f1));
#undef INITIAL_CYCLES
	/* Stock commits type/duration together as one little-endian 64-bit
	 * store.  Keep that width and the M,R,L,F2,F1 publication sequence.
	 */
#define INITIAL_SHAPE(e) WRITE_ONCE(*(u64 *)&(e).type, ((u64)2000 << 32) | 2)
	INITIAL_SHAPE((trimui_effect_m));
	INITIAL_SHAPE(trimui_effect_r);
	INITIAL_SHAPE((trimui_effect_l));
	WRITE_ONCE((trimui_effect_m).target_rgb, boot_white);
	WRITE_ONCE(trimui_effect_r.target_rgb, boot_white);
	WRITE_ONCE((trimui_effect_l).target_rgb, boot_white);
	INITIAL_SHAPE((trimui_effect_f2));
	WRITE_ONCE((trimui_effect_f2).target_rgb, boot_orange);
	INITIAL_SHAPE((trimui_effect_f1));
	WRITE_ONCE((trimui_effect_f1).target_rgb, boot_orange);
#undef INITIAL_SHAPE
	/* The OEM generator receives these fixed boot colors.  It does not
	 * reread target_rgb between initialization and each call.
	 */
	gen_effect(left, 2, boot_white, boot_white);
	gen_effect(&trimui_effect_r, 2, boot_white, boot_white);
	gen_effect(&(trimui_effect_f1), 2, boot_orange, boot_orange);
	gen_effect(front2, 2, boot_orange, boot_orange);
	gen_effect(&(trimui_effect_m), 2, boot_white, boot_white);
	/* Keep the OEM's odd reset order.  A concurrent control read can see
	 * these partially reset effects; do not collapse this into a loop.
	 */
	WRITE_ONCE(trimui_effect_r.index, 0);
	WRITE_ONCE((trimui_effect_l).active_cycles, READ_ONCE((trimui_effect_l).cycles));
	WRITE_ONCE(trimui_effect_r.active_cycles, READ_ONCE(trimui_effect_r.cycles));
	WRITE_ONCE((trimui_effect_f1).active_cycles, READ_ONCE((trimui_effect_f1).cycles));
	WRITE_ONCE(front2->active_cycles, READ_ONCE(front2->cycles));
	rgb = READ_ONCE((trimui_effect_m).cycles);
	WRITE_ONCE((trimui_effect_l).index, 0);
	WRITE_ONCE((trimui_effect_m).index, 0);
	WRITE_ONCE(front2->index, 0);
	WRITE_ONCE((trimui_effect_f1).index, 0);
	WRITE_ONCE((trimui_effect_m).active_cycles, rgb);
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
			s32 offset;
			const u32 *source;
			if (trimui_frames_len <= 0)
				goto battery;
			/* Stock wraps offset at 32 bits, then widens before adding channels.
			 * TODO: bound length/index separately; these reads remain unchecked.
			 */
			/* Retain the OEM shared source address, including signed
			 * backward addresses after 32-bit index multiplication wraps. */
			offset = (s32)((u32)trimui_frame_index * 14U);
			source = (const u32 *)((unsigned long)trimui_anim_frames +
				((unsigned long)(long)offset << 2));
			if (++trimui_frame_index >= trimui_frames_len) {
				trimui_frame_index = 0;
				if (trimui_cycles_left > 0) trimui_cycles_left--;
				else if (trimui_cycles_left >= -1) {
					trimui_frames_cycles = 0;
					trimui_frames_len = 0;
				}
			}
			if (trimui_mask_m) memcpy(trimui_frame, source, 8 * sizeof(u32));
			if (trimui_mask_lr) {
				/* Stock transfers each LED pair before reading the next.
				 * The two-copy sequence can observe an intervening frame edit.
				 */
				memcpy(trimui_frame + 10, source + 10, 2 * sizeof(u32));
				barrier();
				memcpy(trimui_frame + 12, source + 12, 2 * sizeof(u32));
			}
			if (trimui_mask_f) {
				u32 f1 = READ_ONCE(source[9]);
				u32 f2 = READ_ONCE(source[8]);
				/* OEM publishes these with one paired 32-bit STP after
				 * both reads. Separate WRITE_ONCE stores emitted two STRs.
				 * Keep the adjacent ordinary stores and verify the pair;
				 * this does not promise atomic publication of both pixels.
				 * TODO: synchronize animation/frame edits separately. */
				trimui_frame[8] = f2;
				trimui_frame[9] = f1;
			}
		}
		commit_static_frame();
 battery:
		if (battery_ticks > 625) {
			battery_ticks = 0;
			check_battery();
		}
		battery_ticks++; /* OEM reset-before-increment: first poll at 627, then every 626 loops. */
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
	source = (u8 *)trimui_frame;
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
	u8 *frame = (u8 *)trimui_frame;
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
	pixels = trimui_frame;
	/* Keep this line count for later OEM __LINE__ diagnostics. */
	if (bytes > (int)trimui_led_count * 7)
		bytes = trimui_led_count * 7;
	rounded = bytes / 7 * 7;
	/* Stock compares a signed 32-bit difference between the end and the
	 * advancing input pointer. Keep its wraparound and unchecked destination.
	 * TODO: bound the destination and use a non-wrapping length separately. */
	while ((int)((u32)(unsigned long)buffer + (u32)rounded -
		     (u32)(unsigned long)position) > 0) {
		int parsed; register u32 *slot asm("x2") = &pixels[i];

		register const char *scan asm("x0") = position;
		asm volatile("" : "+r"(scan) : "r"(slot)); parsed = sscanf(scan, "%X", slot);
		position += 7; asm volatile("" : : "r"(parsed)); /* Pin index after call. */
		i++;
		/* Preserve stock's two-step end-minus-cursor comparison. */
		asm volatile("" : "+r"(rounded));
	}
	return count;
}
/* OEM scalar setters form field addresses before saving count. F1/F2 use the
 * BSS pointer before LED state; L/R/M use unread interior effect-frame roots.
 * Parser errors are ignored and count returned; TODO: report errors. */
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
	int *value;
	size_t saved_count;

	value = &trimui_effect_m.duration;
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_duration_l(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	int *value;
	size_t saved_count;

	value = &trimui_effect_l.duration;
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_duration_r(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	int *value;
	size_t saved_count;

	value = &trimui_effect_r.duration;
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_duration_f1(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	int *value;
	size_t saved_count;


	value = &trimui_effect_f1.duration;

	saved_count = count;

	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_duration_f2(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	int *value;
	size_t saved_count;


	value = &trimui_effect_f2.duration;

	saved_count = count;

	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_duration(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	struct trimui_led_effect *left;

	/* Keep the C pair root and parser target live before saving count. */
	left = &trimui_effect_l;
	asm volatile("" : "+r"(count) : "r"(&left->duration));

	/* OEM copies the stale value to the right on parse failure too.
	 * TODO: reject invalid input only in a separate behavior change. */
	kstrtoint(buffer, 0, &left->duration);
	trimui_effect_r.duration = left->duration;
	return count;
}

static ssize_t store_cycles_m(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	int *value;
	size_t saved_count;

	value = &trimui_effect_m.cycles;
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_cycles_l(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	int *value;
	size_t saved_count;

	value = &trimui_effect_l.cycles;
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_cycles_r(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	int *value;
	size_t saved_count;

	value = &trimui_effect_r.cycles;
	asm volatile("" : "+r"(value));
	saved_count = count;
	asm volatile("" : "+r"(saved_count));
	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_cycles_f1(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	int *value;
	size_t saved_count;


	value = &trimui_effect_f1.cycles;

	saved_count = count;

	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_cycles_f2(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	int *value;
	size_t saved_count;


	value = &trimui_effect_f2.cycles;

	saved_count = count;

	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_cycles(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	struct trimui_led_effect *left;

	/* Match the duration setter's C root and callback argument order. */
	left = &trimui_effect_l;
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


_Static_assert(__builtin_offsetof(struct trimui_stock_led_state,
				 effects[3].target_rgb) == 0x2d9c,
	       "left RGB target offset");
_Static_assert(__builtin_offsetof(struct trimui_stock_led_state,
				 effects[2].target_rgb) == 0x1e7c,
	       "right RGB target offset");
/* GNU C symbol names bind these declarations to existing u32 members.
 * Each four-byte interval is fully inside one real effect object; there is
 * no additional allocation or oversized view. The bindings retain the OEM
 * direct store relocations, not a claim about original C spelling. */
extern u32 trimui_rgb_target_l __asm__("trimui_effect_l+28");
extern u32 trimui_rgb_target_r __asm__("trimui_effect_r+28");
_Static_assert(__builtin_offsetof(struct trimui_led_effect, target_rgb) == 28,
	       "RGB target binding offset");
_Static_assert(sizeof(trimui_rgb_target_l) == sizeof(trimui_effect_l.target_rgb),
	       "left target binding width");
_Static_assert(sizeof(trimui_rgb_target_r) == sizeof(trimui_effect_r.target_rgb),
	       "right target binding width");
_Static_assert(28 + sizeof(trimui_rgb_target_l) <= sizeof(trimui_effect_l),
	       "left target binding extent");
_Static_assert(28 + sizeof(trimui_rgb_target_r) <= sizeof(trimui_effect_r),
	       "right target binding extent");
static ssize_t store_effect_rgb_hex_lr(struct class *class,struct class_attribute *attr,const char *buffer,size_t count)
{
 u32 rgb=get_rgb_hex(class, buffer);
 /* Stock publishes left before right without synchronization, even after
  * malformed input produced zero. TODO: reject invalid input and protect
  * the pair only in a separate behavior change. */
 WRITE_ONCE(trimui_rgb_target_l,rgb);
 WRITE_ONCE(trimui_rgb_target_r,rgb);
 return count;
}

/* OEM ignores kstrtoint failures in all six effect setters below. Malformed
 * input keeps the previous type, but still regenerates the effect and resets
 * its playback state; preserve that observable bug.
 */
#include "leds-trimui-scalar-roots.h"
#define MIDDLE_FIELD(root, member) \
 (*(__typeof__(trimui_effect_m.member) *)((root) + 2472 + __builtin_offsetof(struct trimui_led_effect, member)))
_Static_assert(__builtin_offsetof(struct trimui_led_effect, frames[342]) == 1400, "middle member root");
_Static_assert(1400 + sizeof(trimui_middle_member_root) <= sizeof(trimui_effect_l) &&
	       sizeof(struct trimui_led_effect) - 1400 == 2472,
	       "middle member extent and numeric field delta");
static ssize_t store_effect_m(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	register unsigned long page asm("x19");
	register unsigned long base asm("x20");
	register size_t saved asm("x21") = count;
	register struct trimui_led_effect *genptr asm("x14");
	register struct trimui_led_effect *callptr asm("x0");
	int parsed_type;
	u32 current_color, target_color;
	base = (unsigned long)&trimui_middle_member_root;
	asm volatile("" : "+r"(base));
	kstrtoint(buffer, 0, (int *)&MIDDLE_FIELD(base, type));
	if ((unsigned int)MIDDLE_FIELD(base, type) > 7)
		MIDDLE_FIELD(base, type) = 0;
	page = (unsigned long)&trimui_middle_member_root;
	asm volatile("" : "+r"(page));
	genptr = (struct trimui_led_effect *)(page + 2472);
	asm volatile("" : "+r"(genptr));
	callptr = genptr;
	asm volatile("" : "+r"(callptr));
	parsed_type = READ_ONCE(MIDDLE_FIELD(page, type));
	current_color = READ_ONCE(MIDDLE_FIELD(page, current_rgb));
	target_color = READ_ONCE(MIDDLE_FIELD(page, target_rgb));
	gen_effect(callptr, parsed_type, target_color, current_color);
	/* OEM resets index before reading cycles, including after parse failure. */
	WRITE_ONCE(MIDDLE_FIELD(page, index), 0);
	WRITE_ONCE(MIDDLE_FIELD(page, active_cycles), READ_ONCE(MIDDLE_FIELD(page, cycles)));
	return saved;
}

_Static_assert(__builtin_offsetof(struct trimui_stock_led_state,
				 effects[1].frames[564]) == 0x1830,
	       "left LED page root");
static ssize_t store_effect_l(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	register unsigned long page asm("x19");
	register unsigned long base asm("x20");
	register size_t saved asm("x21") = count;
	register struct trimui_led_effect *genptr asm("x14");
	register struct trimui_led_effect *callptr asm("x0");
	int parsed_type;
	u32 current_color, target_color;
	base = (unsigned long)&trimui_left_member_root;
	asm volatile("" : "+r"(base));
	kstrtoint(buffer, 0, (int *)&TRIMUI_EFFECT_MEMBER(base, 2952, type));
	if ((unsigned int)TRIMUI_EFFECT_MEMBER(base, 2952, type) > 7)
		TRIMUI_EFFECT_MEMBER(base, 2952, type) = 0;
	page = (unsigned long)&trimui_left_member_root;
	asm volatile("" : "+r"(page));
	genptr = (struct trimui_led_effect *)(page + 2952);
	asm volatile("" : "+r"(genptr));
	callptr = genptr; asm volatile("" : "+r"(callptr));
	parsed_type = READ_ONCE(TRIMUI_EFFECT_MEMBER(page, 2952, type));
	current_color = READ_ONCE(TRIMUI_EFFECT_MEMBER(page, 2952, current_rgb));
	target_color = READ_ONCE(TRIMUI_EFFECT_MEMBER(page, 2952, target_rgb));
	gen_effect(callptr, parsed_type, target_color, current_color);
	WRITE_ONCE(TRIMUI_EFFECT_MEMBER(page, 2952, index), 0);
	WRITE_ONCE(TRIMUI_EFFECT_MEMBER(page, 2952, active_cycles), READ_ONCE(TRIMUI_EFFECT_MEMBER(page, 2952, cycles)));
	return saved;
}

_Static_assert(__builtin_offsetof(struct trimui_stock_led_state,
				 effects[0].frames[508]) == 0x830,
	       "right LED page root");
static ssize_t store_effect_r(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	register unsigned long page asm("x19");
	register unsigned long base asm("x20");
	register size_t saved asm("x21") = count;
	register struct trimui_led_effect *genptr asm("x14");
	register struct trimui_led_effect *callptr asm("x0");
	int parsed_type;
	u32 current_color, target_color;
	base = (unsigned long)&trimui_right_member_root;
	asm volatile("" : "+r"(base));
	kstrtoint(buffer, 0, (int *)&TRIMUI_EFFECT_MEMBER(base, 3432, type));
	if ((unsigned int)TRIMUI_EFFECT_MEMBER(base, 3432, type) > 7)
		TRIMUI_EFFECT_MEMBER(base, 3432, type) = 0;
	page = (unsigned long)&trimui_right_member_root;
	asm volatile("" : "+r"(page));
	genptr = (struct trimui_led_effect *)(page + 3432);
	asm volatile("" : "+r"(genptr));
	callptr = genptr; asm volatile("" : "+r"(callptr));
	parsed_type = READ_ONCE(TRIMUI_EFFECT_MEMBER(page, 3432, type));
	current_color = READ_ONCE(TRIMUI_EFFECT_MEMBER(page, 3432, current_rgb));
	target_color = READ_ONCE(TRIMUI_EFFECT_MEMBER(page, 3432, target_rgb));
	gen_effect(callptr, parsed_type, target_color, current_color);
	WRITE_ONCE(TRIMUI_EFFECT_MEMBER(page, 3432, index), 0);
	WRITE_ONCE(TRIMUI_EFFECT_MEMBER(page, 3432, active_cycles), READ_ONCE(TRIMUI_EFFECT_MEMBER(page, 3432, cycles)));
	return saved;
}
#undef MIDDLE_FIELD

static ssize_t store_effect_f1(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
 register unsigned long page asm("x19"), base asm("x20");
 register struct trimui_led_effect *argument asm("x14");
 register size_t saved_count asm("x21") = count;
 register int parser_base asm("w1") = 0;
 u32 type, current_color, target;
 /* OEM ignores parse errors; type above7 becomes0. Preserve separate reads. */
 base=(unsigned long)&trimui_front_member_root; asm volatile("" : "+r"(base));
 kstrtoint(buffer,parser_base,(int *)&TRIMUI_EFFECT_MEMBER(base,3912,type));
 if ((unsigned int)TRIMUI_EFFECT_MEMBER(base,3912,type)>7) TRIMUI_EFFECT_MEMBER(base,3912,type)=0;
 page=(unsigned long)&trimui_front_member_root; asm volatile("" : "+r"(page));
 argument=(struct trimui_led_effect *)(page+3912); asm volatile("" : "+r"(argument));
 {
  register struct trimui_led_effect *call asm("x0")=argument;
  asm volatile("" : "+r"(call));
  /* No paired load: OEM observes type/current_color/target in this order. */
  type=READ_ONCE(TRIMUI_EFFECT_MEMBER(page,3912,type)); current_color=READ_ONCE(TRIMUI_EFFECT_MEMBER(page,3912,current_rgb)); target=READ_ONCE(TRIMUI_EFFECT_MEMBER(page,3912,target_rgb));
  gen_effect(call,type,target,current_color);
 }
 WRITE_ONCE(TRIMUI_EFFECT_MEMBER(page,3912,index),0);
 WRITE_ONCE(TRIMUI_EFFECT_MEMBER(page,3912,active_cycles),READ_ONCE(TRIMUI_EFFECT_MEMBER(page,3912,cycles)));
 return saved_count;
}


static ssize_t store_effect_f2(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	register unsigned long page asm("x19");
	register unsigned long base asm("x20");
	register struct trimui_led_effect *argument asm("x14");
	register size_t saved_count asm("x21") = count;
	register int parser_base asm("w1") = 0;
	base = (unsigned long)&trimui_front_member_root; asm volatile("" : "+r"(base));
	kstrtoint(buffer, parser_base, (int *)&FRONT_FIELD(base, type));
	if ((unsigned int)FRONT_FIELD(base, type) > 7)
		FRONT_FIELD(base, type) = 0;
	page = (unsigned long)&trimui_front_member_root; asm volatile("" : "+r"(page));
	argument = (struct trimui_led_effect *)(page + 40);
	asm volatile("" : "+r"(argument));
	{
		register struct trimui_led_effect *call asm("x0") = argument;
		asm volatile("" : "+r"(call));
		gen_effect(call, FRONT_FIELD(page, type), FRONT_FIELD(page, target_rgb),
			   FRONT_FIELD(page, current_rgb));
	}
	WRITE_ONCE(FRONT_FIELD(page, index), 0);
	WRITE_ONCE(FRONT_FIELD(page, active_cycles), READ_ONCE(FRONT_FIELD(page, cycles)));
	return saved_count;

}

static ssize_t store_effect_lr(struct class *class, struct class_attribute *attr,
			       const char *buffer, size_t count)
{
	register unsigned long parse_base asm("x20");
	register unsigned long left_page asm("x19");
	register unsigned long right_page asm("x18");
	register struct trimui_led_effect *left_argument asm("x15");
	register struct trimui_led_effect *right_argument asm("x14");
	register struct trimui_led_effect *left_call asm("x0");
	register size_t saved_count asm("x21") = count;
	register int parser_base asm("w1") = 0;
	int right_cycles, left_type, left_target, left_current;
	int right_type, right_target, right_current;
	/* OEM ignores parser errors and still regenerates both effects. */
	parse_base = (unsigned long)&trimui_left_member_root; asm volatile("" : "+r"(parse_base));
	kstrtoint(buffer, parser_base, (int *)&TRIMUI_EFFECT_MEMBER(parse_base, 2952, type));
	if ((unsigned int)TRIMUI_EFFECT_MEMBER(parse_base, 2952, type) > 7)
		TRIMUI_EFFECT_MEMBER(parse_base, 2952, type) = 0;
	left_page = (unsigned long)&trimui_left_member_root; asm volatile("" : "+r"(left_page));
	right_page = (unsigned long)&trimui_right_member_root; asm volatile("" : "+r"(right_page));
	left_argument = (struct trimui_led_effect *)(left_page + 2952);
	right_argument = (struct trimui_led_effect *)(right_page + 3432);
	asm volatile("" : "+r"(left_argument), "+r"(right_argument));
	left_call = left_argument; asm volatile("" : "+r"(left_call));
	left_type = READ_ONCE(TRIMUI_EFFECT_MEMBER(left_page, 2952, type));
	left_current = READ_ONCE(TRIMUI_EFFECT_MEMBER(left_page, 2952, current_rgb));
	left_target = READ_ONCE(TRIMUI_EFFECT_MEMBER(left_page, 2952, target_rgb));
	WRITE_ONCE(TRIMUI_EFFECT_MEMBER(right_page, 3432, type), left_type);
	gen_effect(left_call, left_type, left_target, left_current);
	/* OEM retains x14/x18 across calls; the pinned generator preserves them. */
	right_type = READ_ONCE(TRIMUI_EFFECT_MEMBER(right_page, 3432, type));
	right_current = READ_ONCE(TRIMUI_EFFECT_MEMBER(right_page, 3432, current_rgb));
	right_target = READ_ONCE(TRIMUI_EFFECT_MEMBER(right_page, 3432, target_rgb));
	gen_effect(right_argument, right_type, right_target, right_current);
	WRITE_ONCE(TRIMUI_EFFECT_MEMBER(left_page, 2952, index), 0);
	WRITE_ONCE(TRIMUI_EFFECT_MEMBER(left_page, 2952, active_cycles),
		READ_ONCE(TRIMUI_EFFECT_MEMBER(left_page, 2952, cycles)));
	right_cycles = READ_ONCE(TRIMUI_EFFECT_MEMBER(right_page, 3432, cycles));
	WRITE_ONCE(TRIMUI_EFFECT_MEMBER(right_page, 3432, index), 0);
	TRIMUI_EFFECT_MEMBER(right_page, 3432, active_cycles) = right_cycles;
	return saved_count;
}
#undef TRIMUI_PAIR_LEFT
#undef TRIMUI_PAIR_RIGHT

static ssize_t store_effect_enable(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	int *value;
	size_t saved_count;


	value = &trimui_effect_enable;

	saved_count = count;

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

			sprintf(destination, "%06X ", pixels[i] & 0xffffff);
		}
		position += count;
		/* Keep the cumulative count live across the next LED-count read. */

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
	offset = (unsigned int)trimui_frames_len * 14u;
	/* Stock forms this root after the frame offset multiplication. The
	 * tied, empty constraint keeps the C array pointer in x1 at that point
	 * with the pinned compiler; it emits no handwritten instructions. */
	asm("" : "=r"(root) : "0"(trimui_anim_frames));
	pixels = root;
	append = pixels + (int)offset;
	/* Stock ignores append capacity and clears the start of the buffer,
	 * even when sscanf writes at a later append offset. TODO: validate the
	 * append and clear its actual destination in a separate fix. */
	for (i = 0; (int)i < colors; ) {
		unsigned long byte_offset = (unsigned long)i << 2;
		const char *scan = position;
		register const char *scan_format asm("x1") = format;

		position += 7;
		i++;
		/* Stock encodes the clear as STR WZR, [x2, x22]. Its two address
		 * registers add modulo 2^64. Form the address as unsigned integer
		 * arithmetic, then convert it to a pointer; the x1 format lifetime
		 * and x22 root constraint retain the OEM instruction schedule.
		 * This code-generation form is not a claim about OEM C. Stock still
		 * writes beyond the array for unchecked indices; TODO: validate the
		 * index and use a normal array pointer in a corrective change. */
		{
			register unsigned long clear_base asm("x2") = byte_offset;
			register unsigned long clear_root asm("x22") =
				(unsigned long)pixels;
			asm volatile("" : "+r"(clear_root) : "r"(scan_format));
			*(u32 *)(clear_base + clear_root) = 0;
		}
		sscanf(scan, scan_format, (u32 *)((char *)append + byte_offset));

	}
	trimui_frames_len += colors / 14;
	printk("\n anim frames:%d, pixels:%d\n", trimui_frames_len, colors);
	return count;
}

__attribute__((target("tune=cortex-a53")))
static __attribute__((no_reorder, optimize("no-move-loop-invariants"))) ssize_t show_frame_hex(struct class *class, struct class_attribute *attr, char *buffer)
{
	/* Stock advances the index in x22 but compares its low signed word.
	 * A mutable LED count can change between loop iterations. */
	register long i asm("x22") = 0;
	int count = 0;
	register struct trimui_led_config *config_root asm("x23") = &trimui_config;
	/* Hold a bounded configuration-object root across formatting calls. */
	asm("" : "+r" (config_root));
	register char *position asm("x21") = buffer;
	register u32 *pixels asm("x20") =
		/* OEM page-ADD order; 0x7cc remains a separate ADD. */ trimui_frame;
	/* Keep the OEM frame root and late format-page offset while retaining
	 * the compiler's pooled format literal. */
	/* The C literal supplies the complete pooled address. */
	/* Stock advances by the cumulative length, producing sparse output.
	 * At 34 LEDs the newline lands beyond a 4 KiB sysfs page.
	 * TODO: advance by the last write and bound output in a separate fix. */
	while ((int)i < (int)config_root->count) {
		count += 7;
		{
			const char *format;
			/* Stock puts the destination in x0 before forming the
			 * pooled format address. This constrains only argument order;
			 * the OEM sparse-output and overflow behavior remains.
			 */
			register char *destination asm("x0") = position;
			asm("" : "+r"(destination));
			format = "%06X ";
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
	int *value;
	size_t saved_count;


	value = &trimui_frames_len;

	saved_count = count;

	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_frames_fps(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	int *value;
	size_t saved_count;


	value = &trimui_frames_fps;

	saved_count = count;

	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_frames_cycles(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{


	register size_t result asm("x20");
	int parser_base = 0;

	/* Keep OEM's parser zero, BSS root, and saved-count schedule. */
	asm volatile("" : "+r"(parser_base));

	result = count;
	asm volatile("" : "+r"(result));
	/* OEM ignores parse errors and clears enable regardless.  TODO: reject
	 * invalid input only in a separate behavior-changing fix. */
	kstrtoint(buffer, parser_base, &trimui_frames_cycles);
	/* OEM materializes the counter root before clearing enable.  This C
	 * reference retains the same instruction order with the pinned compiler. */

	trimui_frames_enable = 0;
	/* Stock issues two separate 32-bit resets.  A frame worker can observe
	 * the intermediate state; TODO: synchronize only in a separate fix. */
	WRITE_ONCE(trimui_frame_index, 0);
	WRITE_ONCE(trimui_cycles_left, 0);
	return result;
}

static ssize_t store_frames_enable(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{

 size_t result;
 int parser_base = 0;
 int left;

 asm volatile("" : "+r"(parser_base));

 result = count;
 asm volatile("" : "+r"(result));
 /* Stock ignores parse errors and still resets/decrements state.
  * TODO: reject errors only in a separate behavior change. */
 kstrtoint(buffer,parser_base,&trimui_frames_enable);
 left=trimui_frames_cycles-1;
 trimui_frame_index=0;
 trimui_cycles_left=left;
 if(!trimui_frames_enable)trimui_frames_len=0;
 return result;
}

static ssize_t store_frames_override_m_enable(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	int *value;
	size_t saved_count;


	value = &trimui_mask_m;

	saved_count = count;

	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_frames_override_f1f2_enable(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	int *value;
	size_t saved_count;


	value = &trimui_mask_f;

	saved_count = count;

	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t store_frames_override_lr_enable(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	int *value;
	size_t saved_count;


	value = &trimui_mask_lr;

	saved_count = count;

	kstrtoint(buffer, 0, value);
	return saved_count;
}

static ssize_t show_motor_voltage(struct class *class, struct class_attribute *attr, char *buffer)
{
	return sprintf(buffer, "%d\n", trimui_motor_regulator ? regulator_get_voltage(trimui_motor_regulator) : 0);
}

static ssize_t store_motor_voltage(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{

	register size_t saved_count asm("x20");

	/* OEM keeps this state pointer live across kstrtoint and then re-reads
	 * voltage from the same object. Keep the pointer live without adding a
	 * memory access or changing the stock NULL/error-pointer handling.
	 * Saving count after forming the state root preserves OEM ADRP/ADD/MOV
	 * order without changing the stale-value behavior on parse failure. */

	saved_count = count; asm("" : : "r"(saved_count), "r"(&trimui_motor_voltage));
	kstrtoint(buffer, 0, &trimui_motor_voltage);
	/* Stock passes NULL/error pointers through. TODO: check regulator_get in a separate fix. */
	regulator_set_voltage(trimui_motor_regulator, trimui_motor_voltage,
		trimui_motor_voltage + 100000);
	return saved_count;
}

static ssize_t store_speaker_mute(struct class *class, struct class_attribute *attr, const char *buffer, size_t count)
{
	enum { requested_offset =
		__builtin_offsetof(struct trimui_stock_led_state, mute) -
		__builtin_offsetof(struct trimui_stock_led_state,
				   effects[4].frames[462]) };
	/* Preserve the OEM two-stage address calculation as unsigned integer
	 * arithmetic. Form a C pointer only after returning to the real mute
	 * scalar; no pointer is based before that separately declared object.
	 * This is reconstruction scaffolding, not recovered original spelling. */
	unsigned long root = (unsigned long)&trimui_mute_requested - requested_offset;
	int *requested;
	register size_t saved_count asm("x20");


	asm("" : "+r"(root));
	saved_count = count; asm("" : "+r"(saved_count) : "r"(root));
	requested = (int *)(root + requested_offset);
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
	/* Same __ATTR fields; name comes from the OEM-ordered C string pool.
	 * Preserve the OEM spelling, including "exterme".
	 */
	{
		.attr = {
			.name = trimui_effect_name_text.extreme_brightness,
			.mode = VERIFY_OCTAL_PERMISSIONS(0664),
		},
		.show = show_tg5040_max_scale,
		.store = store_tg5040_max_scale,
	},
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
_Static_assert(__builtin_offsetof(struct trimui_stock_led_state, effects[0]) ==
	       0x20, "LED F2 probe root");
_Static_assert(__builtin_offsetof(struct trimui_stock_led_state, effects[1]) ==
	       0xf40, "LED F1 probe root");
_Static_assert(__builtin_offsetof(struct trimui_stock_led_state, effects[2]) ==
	       0x1e60, "LED R probe root");
_Static_assert(__builtin_offsetof(struct trimui_stock_led_state, effects[3]) ==
	       0x2d80, "LED L probe root");

/* The shipped diagnostic names identify these initialization helpers.
 * Preserve their ignored failures and continued initialization as OEM does.
 */
static inline void init_motor_voltage(void)
{
	int result;

	result = __class_register(&trimui_motor_class, &trimui_motor_key);
	if (result < 0) {
		printk(KERN_ERR "%s()%d - class_register err\n", trimui_name_cluster.motor, 3158);
		class_unregister(&trimui_motor_class);
	}
	/* OEM publishes ERR_PTR values without testing them. Preserve that value
	 * and store order; TODO: handle regulator_get failure in a separate fix.
	 */
	trimui_motor_regulator = regulator_get(NULL, "axp2202-bldo3");
}

static inline void init_speaker_mute(void)
{
	int result;

	result = __class_register(&trimui_speaker_class, &trimui_speaker_key);
	if (result < 0) {
		printk(KERN_ERR "%s()%d - class_register err\n", trimui_name_cluster.speaker, 3218);
		class_unregister(&trimui_speaker_class);
	}
}

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
	memset(&trimui_effect_l, 0, sizeof(trimui_effect_l));
	memset(&trimui_effect_r, 0, sizeof(trimui_effect_r));
	memset(&trimui_effect_f1, 0, sizeof(trimui_effect_f1));
	memset(&trimui_effect_f2, 0, sizeof(trimui_effect_f2));
	/* Preserve the OEM store order. The LED class is already visible while
	 * probe initializes these globals, so a reader can observe partial state. */
	WRITE_ONCE(trimui_effect_f2.duration, 2000);
	WRITE_ONCE(trimui_effect_m.duration, 2000);
	WRITE_ONCE(trimui_effect_f1.duration, 2000);
	WRITE_ONCE(trimui_effect_r.duration, 2000);
	WRITE_ONCE(trimui_effect_l.duration, 2000);
	WRITE_ONCE(trimui_effect_f1.cycles, 1);
	WRITE_ONCE(trimui_effect_f2.cycles, 1);
	WRITE_ONCE(trimui_effect_m.cycles, 1);
	WRITE_ONCE(trimui_effect_r.cycles, 1);
	WRITE_ONCE(trimui_effect_l.cycles, 1);
	result = gpio_request(107, "led_5v");
	/* Stock continues after GPIO and thread errors. TODO: unwind and return
	 * those errors in a separate fix, and stop the thread on remove. */
	if (result < 0)
		printk("trimui_ledc_init error! led_5v: %d\n", 107);
	gpio_direction_output(107, 1);
	task = kthread_create(trimui_ledc_run_thread, NULL, "ledc daemon");
	if (IS_ERR(task))
		printk("ledc thread error\n");
	else
		wake_up_process(task);
	result = __class_register(&trimui_led_anim_class, &trimui_anim_key);
	if (result < 0) {
		/* OEM unregisters even though registration failed. The motor and
		 * speaker failures below do the same, then continue initialization.
		 * TODO: unwind only successfully registered classes in a separate fix.
		 */
		/* Stock reuses the probe diagnostic array, including pointer identity. */
		printk(KERN_ERR "%s()%d - class_register err\n", trimui_ro_writes.sunxi_led_probe, 3329);
		class_unregister(&trimui_led_anim_class);
		return -EFAULT;
	}
	init_motor_voltage();
	init_speaker_mute();
	return 0;
}
