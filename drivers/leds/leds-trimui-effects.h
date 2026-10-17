/* SPDX-License-Identifier: GPL-2.0-only */
/* Effect engine reconstructed from TG3040 #860 gen_effect/step_one_frame. */
#ifdef TRIMUI_LED_HOST_TEST
#include <stdint.h>
typedef uint32_t u32;
#else
#include <linux/types.h>
#endif

struct trimui_led_effect {
	int type, duration, index, length, active_cycles, cycles;
	u32 current_rgb, target_rgb;
	u32 frames[960];
};

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
	} else if (type == 2 || type == 3) {
		if (effect->duration < 500)
			effect->duration = 500;
		if (effect->duration > 8000)
			effect->duration = 8000;
		n = effect->duration / 16;
		ramp = effect->duration / (type == 2 ? 48 : 64);
		from_r = (target >> 20) & 15;
		from_g = (target >> 12) & 15;
		from_b = (target >> 4) & 15;
		for (i = 0; i < n; i++)
			effect->frames[i] = type == 2 ? target :
				trimui_rgb(from_r, from_g, from_b);
		r = ramp * from_r; g = ramp * from_g; b = ramp * from_b;
		for (i = 0; i < ramp; i++) {
			u32 rgb = trimui_rgb(r / ramp, g / ramp, b / ramp);
			effect->frames[i] = rgb;
			effect->frames[(type == 2 ? n : 2 * ramp) - 1 - i] = rgb;
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
		if (type == 6 && effect->duration < 350)
			effect->duration = 350;
		n = effect->duration / 16;
		for (i = 0; i < n; i++)
			effect->frames[i] = 0;
		count = (type - 4) * 10;
		for (i = 0; i < count; i++)
			if (i % 10 < 5)
				effect->frames[i] = target;
	} else {
		return;
	}
	effect->length = n;
	effect->index = 0;
	effect->active_cycles = 0;
}

static u32 step_one_frame(struct trimui_led_effect *effect)
{
	u32 color = effect->frames[effect->index++];
	effect->current_rgb = color;
	if (effect->index >= effect->length) {
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
