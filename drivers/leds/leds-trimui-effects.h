/* SPDX-License-Identifier: GPL-2.0-only */
/* Effect engine reconstructed from TG3040 #860 gen_effect/step_one_frame. */
#ifdef TRIMUI_LED_HOST_TEST
#include <stdint.h>
typedef uint32_t u32;
#define READ_ONCE(value) (*(volatile const __typeof__(value) *)&(value))
#define WRITE_ONCE(value, assigned) \
	(*(volatile __typeof__(value) *)&(value) = (assigned))
#else
#include <linux/types.h>
#endif

struct trimui_led_effect {
	int type, duration, index, length, active_cycles, cycles;
	u32 current_rgb, target_rgb;
	u32 frames[960];
};

#include "leds-trimui-ro-pool.h"

static u32 trimui_rgb(int r, int g, int b)
{
	return ((r & 255) << 16) | ((g & 255) << 8) | (b & 255);
}

/* Keep these three helpers in OEM order. GCC otherwise moves the small
 * predicate to the front of .text and pushes the generator to the tail. */
static __attribute__((no_reorder)) void gen_effect(struct trimui_led_effect *effect, int type,
		       u32 target, u32 previous)
{
	int n, ramp, i, count, r, g, b, from_r, from_g, from_b;
	int to_r, to_g, to_b;
	/* OEM clamps before validating type. An unsupported type can therefore
	 * change duration even though no frames or metadata are published.
	 */
	if (effect->duration < 200)
		effect->duration = 200;
	/* Stock leaves linear/static effects unbounded and can overwrite the
	 * following state after frames[959]. TODO: validate those durations in
	 * a separate fix; preserve the shipped writes during reconstruction.
	 */
	if (type == 1) {
		/* Stock keeps the source channels in w5/w6/w3 and the target
		 * channels in w9/w8/w2 while forming the ramp deltas. These C
		 * register roles reproduce its arithmetic schedule without adding
		 * executable assembly.
		 */
		register int from_r asm("w5"), from_g asm("w6"), from_b asm("w3");
		register int to_r asm("w9"), to_g asm("w8"), to_b asm("w2");
		n = effect->duration / 16;
		from_r = (previous >> 16) & 255;
		from_g = (previous >> 8) & 255;
		from_b = previous & 255;
		/* OEM extracts all three previous channels before touching target.
		 * Keep that register schedule without changing memory observations.
		 */
		asm volatile("" : "+r"(from_r), "+r"(from_g), "+r"(from_b));
		to_r = (target >> 16) & 255;
		to_g = (target >> 8) & 255;
		to_b = target & 255;
		/* OEM materializes target blue before starting the accumulators. */
		asm volatile("" : "+r"(to_b));
		int delta_r, delta_g, delta_b;
		delta_r = to_r - from_r;
		delta_g = to_g - from_g;
		delta_b = to_b - from_b;
		r = n * from_r; g = n * from_g; b = n * from_b;
		for (i = 0; i < n; i++) {
			effect->frames[i] = trimui_rgb(r / n, g / n, b / n);
			r += delta_r; g += delta_g; b += delta_b;
		}
	} else if (type == 2) {
		u32 start_rgb;
		int delta_r, delta_g, delta_b;

		if (effect->duration < 500)
			effect->duration = 500;
		if (effect->duration > 8000)
			effect->duration = 8000;
		n = effect->duration / 16;
		ramp = effect->duration / 48;
		/* OEM finishes the type-2 ramp division before packing the start color. */
		asm volatile("" : : "r"(ramp));
		for (i = 0; i < n; i++)
			effect->frames[i] = target;
		/* OEM forms the four-bit-per-channel ramp start color after
		 * the initial fill. Keep this ordering: the unchecked fill can
		 * overwrite following state before these channels are formed.
		 */
		start_rgb = trimui_rgb((target >> 20) & 15,
				(target >> 12) & 15, (target >> 4) & 15);
		from_r = (start_rgb >> 16) & 255;
		from_g = (start_rgb >> 8) & 255;
		from_b = start_rgb & 255;
		/* OEM computes the target channels after the initial fill. */
		to_r = (target >> 16) & 255;
		to_g = (target >> 8) & 255;
		to_b = target & 255;
		/* OEM computes channel differences before the ramp accumulators. */
		delta_r = to_r - from_r;
		delta_g = to_g - from_g;
		delta_b = to_b - from_b;
		r = ramp * from_r; g = ramp * from_g; b = ramp * from_b;
		for (i = 0; i < ramp; i++) {
			u32 rgb = trimui_rgb(r / ramp, g / ramp, b / ramp);
			effect->frames[i] = rgb;
			effect->frames[n - 1 - i] = rgb;
			r += delta_r; g += delta_g; b += delta_b;
		}
		/* Stock's type-2 path publishes these fields and returns here. */
		WRITE_ONCE(effect->length, n);
		WRITE_ONCE(effect->index, 0);
		WRITE_ONCE(effect->active_cycles, 0);
		return;
	} else if (type == 3) {
		u32 start_rgb;
		/* Match OEM's red accumulator source in w5 during type-3 ramp. */
		register int start_r asm("w5");
		/* OEM extracts the packed green nibble into w6 before the ramp.
		 * This is a C register role; it adds no executable assembly.
		 */
		register int start_g asm("w6");
		/* OEM leaves x14/x15 intact across this generator path. Keep
		 * the target channels in its w10/w11/w2 register schedule; the
		 * paired-effect caller historically retains pointers in x14.
		 */
		register int to_r asm("w10"), to_g asm("w11"), to_b asm("w2");

		/* OEM has an independent type-3 clamp, fill and ramp path. The
		 * same two loops retain its initial writes and mirrored ramp order.
		 */
		if (effect->duration < 500)
			effect->duration = 500;
		if (effect->duration > 8000)
			effect->duration = 8000;
		n = effect->duration / 16;
		ramp = effect->duration / 64;
		start_rgb = trimui_rgb((target >> 20) & 15,
				(target >> 12) & 15, (target >> 4) & 15);
		/* Keep OEM's type-3 packed start color materialized here. */
		asm volatile("" : "+r"(start_rgb));
		for (i = 0; i < n; i++)
			effect->frames[i] = start_rgb;
		start_r = (start_rgb >> 16) & 255;
		start_g = (start_rgb >> 8) & 15;
		from_b = start_rgb & 255;
		to_r = (target >> 16) & 255;
		to_g = (target >> 8) & 255;
		to_b = target & 255;
		/* OEM keeps these deltas across the mirrored ramp. The frame
		 * stores below retain its unchecked duration/frame bound bug.
		 */
		int delta_r, delta_g, delta_b;
		delta_r = to_r - start_r;
		delta_g = to_g - start_g;
		delta_b = to_b - from_b;
		r = ramp * start_r; g = ramp * start_g; b = ramp * from_b;
		for (i = 0; i < ramp; i++) {
			u32 rgb = trimui_rgb(r / ramp, g / ramp, b / ramp);
			effect->frames[i] = rgb;
			effect->frames[2 * ramp - 1 - i] = rgb;
			r += delta_r; g += delta_g; b += delta_b;
		}
	} else if (type == 4) {
		n = effect->duration / 16;
		for (i = 0; i < n; i++)
			effect->frames[i] = target;
	} else if (type >= 5 && type <= 7) {
		if (effect->duration > 8000)
			effect->duration = 8000;
		if (type == 7 && effect->duration < 500)
			effect->duration = 500;
		/* Stock rereads duration for blink2 after the earlier clamp; blink3
		 * uses its prior value. Keep the asymmetric observation order.
		 */
		if (type == 6 && READ_ONCE(effect->duration) < 350)
			effect->duration = 350;
		n = effect->duration / 16;
		for (i = 0; i < n; i++)
			effect->frames[i] = 0;
		/* Stock selects the three fixed blink-table extents through
		 * distinct type branches, in descending type order.
		 */
		if (type == 7)
			count = 30;
		else if (type == 6)
			count = 20;
		else
			count = 10;
		/* OEM always writes the fixed pattern extent after the zero fill.
		 * If duration changes before the count reread, this can write
		 * beyond the newly published length. Preserve that ordering.
		 */
		for (i = 0; i < count; i++)
			if (trimui_blink_pattern[i])
				effect->frames[i] = target;
	} else {
		return;
	}
	/* Stock publishes length before resetting the cursor and cycle count.
	 * Keep those distinct writes observable in that order.
	 */
	WRITE_ONCE(effect->length, n);
	WRITE_ONCE(effect->index, 0);
	WRITE_ONCE(effect->active_cycles, 0);
}

/* GCC otherwise makes and discards a partial clone here. Suppress that
 * compiler-only clone to retain the OEM clone numbering of the neighboring
 * power, output-mode, and RGB helpers; the emitted frame body is unchanged.
 * The original source-level cause of the OEM clone order is unknown.
 */
static __attribute__((no_reorder, noclone)) u32 step_one_frame(struct trimui_led_effect *effect)
{
	/* Stock loads index and length as one register pair, then reads the
	 * frame before publishing cursor and color. READ_ONCE on these inputs
	 * splits the paired load in this compiler, changing race/fault order.
	 * OEM never bounds the cursor; TODO: validate it in a separate fix.
	 */
	int index = effect->index;
	int length = effect->length;
	u32 color = effect->frames[index];
	WRITE_ONCE(effect->index, index + 1);
	WRITE_ONCE(effect->current_rgb, color);
	if (index + 1 >= length) {
		effect->index = 0;
		if (effect->active_cycles > 0)
			effect->active_cycles--;
		else if (!effect->active_cycles)
			effect->type = 0;
	}
	return color;
}

static __attribute__((no_reorder)) int is_effect_actived(struct trimui_led_effect *effect)
{
	return (unsigned int)(effect->type - 1) < 7 && effect->active_cycles;
}
