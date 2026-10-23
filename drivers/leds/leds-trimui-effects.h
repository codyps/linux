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

static void gen_effect(struct trimui_led_effect *effect, int type,
		       u32 target, u32 previous)
{
	int n, ramp, i, count, r, g, b, from_r, from_g, from_b;
	int to_r = (target >> 16) & 255, to_g = (target >> 8) & 255;
	int to_b = target & 255;
	if (effect->duration < 200)
		effect->duration = 200;
	/* Stock leaves linear/static effects unbounded and can overwrite the
	 * following state after frames[959]. TODO: validate those durations in
	 * a separate fix; preserve the shipped writes during reconstruction.
	 */
	if (type == 1) {
		n = effect->duration / 16;
		from_r = (previous >> 16) & 255;
		from_g = (previous >> 8) & 255;
		from_b = previous & 255;
		r = n * from_r; g = n * from_g; b = n * from_b;
		for (i = 0; i < n; i++) {
			effect->frames[i] = trimui_rgb(r / n, g / n, b / n);
			r += to_r - from_r; g += to_g - from_g; b += to_b - from_b;
		}
	} else if (type == 2) {
		if (effect->duration < 500)
			effect->duration = 500;
		if (effect->duration > 8000)
			effect->duration = 8000;
		n = effect->duration / 16;
		ramp = effect->duration / 48;
		from_r = (target >> 20) & 15;
		from_g = (target >> 12) & 15;
		from_b = (target >> 4) & 15;
		for (i = 0; i < n; i++)
			effect->frames[i] = target;
		r = ramp * from_r; g = ramp * from_g; b = ramp * from_b;
		for (i = 0; i < ramp; i++) {
			u32 rgb = trimui_rgb(r / ramp, g / ramp, b / ramp);
			effect->frames[i] = rgb;
			effect->frames[n - 1 - i] = rgb;
			r += to_r - from_r; g += to_g - from_g; b += to_b - from_b;
		}
	} else if (type == 3) {
		/* OEM has an independent type-3 clamp, fill and ramp path. The
		 * same two loops retain its initial writes and mirrored ramp order.
		 */
		if (effect->duration < 500)
			effect->duration = 500;
		if (effect->duration > 8000)
			effect->duration = 8000;
		n = effect->duration / 16;
		ramp = effect->duration / 64;
		from_r = (target >> 20) & 15;
		from_g = (target >> 12) & 15;
		from_b = (target >> 4) & 15;
		for (i = 0; i < n; i++)
			effect->frames[i] = trimui_rgb(from_r, from_g, from_b);
		r = ramp * from_r; g = ramp * from_g; b = ramp * from_b;
		for (i = 0; i < ramp; i++) {
			u32 rgb = trimui_rgb(r / ramp, g / ramp, b / ramp);
			effect->frames[i] = rgb;
			effect->frames[2 * ramp - 1 - i] = rgb;
			r += to_r - from_r; g += to_g - from_g; b += to_b - from_b;
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
		count = (type - 4) * 10;
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

static u32 step_one_frame(struct trimui_led_effect *effect)
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

static int is_effect_actived(struct trimui_led_effect *effect)
{
	return (unsigned int)(effect->type - 1) < 7 && effect->active_cycles;
}
