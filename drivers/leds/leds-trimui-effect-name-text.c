/* SPDX-License-Identifier: GPL-2.0-only */
#include "leds-trimui-effect-name-text.h"

/* Stock puts these seven names after all other LED diagnostics. Keep this
 * C object in a companion translation unit so its literal section is linked
 * after leds-sunxi-ordered.o. TODO: recover the original source/link order.
 */
const struct trimui_effect_name_text trimui_effect_name_text
	__attribute__((section(".rodata.str1.1"), aligned(1))) = {
	.linear = "linear",
	.breath = "breath",
	.sniff = "sniff",
	.static_effect = "static",
	.blink1 = "blink1",
	.blink2 = "blink2",
	.blink3 = "blink3",
	.extreme_brightness = "tg5040_exterme_brightness",
};
