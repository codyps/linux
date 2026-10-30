/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef TRIMUI_EFFECT_NAME_TEXT_H
#define TRIMUI_EFFECT_NAME_TEXT_H

/* Complete NUL-terminated effect names in stock order. */
struct trimui_effect_name_text {
	char linear[7];
	char breath[7];
	char sniff[6];
	char static_effect[7];
	char blink1[7];
	char blink2[7];
	char blink3[7];
};

extern const struct trimui_effect_name_text trimui_effect_name_text;

#endif
