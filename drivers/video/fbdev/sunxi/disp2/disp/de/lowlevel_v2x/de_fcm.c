// SPDX-License-Identifier: GPL-2.0-only
/* TG3040 #860 FCM register-shadow implementation, recovered from firmware.
 * A133 advertises no FCM channels. The stock APIs are nevertheless linked in;
 * notably de_fcm_enable is an observed no-op, not a substitute implementation.
 */
#include "de_fcm.h"
#include "de_fcm_tables.h"

/* Volatile fields preserve stock's separate read/modify/write accesses even
 * when set_reg_base points directly at registers rather than a RAM shadow.
 */
union fcm_word {
	u32 value;
	struct { u32 low:13, reserved0:3, high:13, reserved1:3; } pair13;
	struct { u32 low:11, reserved0:5, high:11, reserved1:5; } pair11;
	struct { u32 low:9, reserved0:7, high:9, reserved1:7; } pair9;
	struct { u32 enable:1, reserved0:3, window:1, reserved1:27; } control;
};
static volatile union fcm_word *fcm_dev[DE_NUM][CHN_NUM];
static struct de_reg_blocks fcm_block[DE_NUM][CHN_NUM];

int de_fcm_set_reg_base(unsigned int sel, unsigned int channel, void *base)
{
	fcm_dev[sel][channel] = base;
	return 0;
}

int de_fcm_set_init_para(unsigned int sel, unsigned int channel)
{
	/* OEM bug: the selectors and shadow pointer are used unchecked. A bad
	 * caller can index beyond fcm_dev or dereference NULL. Preserve that
	 * behavior here; TODO: validate both before accessing the shadow buffer.
	 * The linked A133 feature query bypasses normal FCM initialization.
	 */
	volatile union fcm_word *reg = fcm_dev[sel][channel];
	int i;

	memcpy((void *)(reg + 0x20 / 4), fcm_angle_hue, sizeof(fcm_angle_hue));
	memcpy((void *)(reg + 0xa0 / 4), fcm_angle_sat, sizeof(fcm_angle_sat));
	memcpy((void *)(reg + 0xe0 / 4), fcm_angle_lum, sizeof(fcm_angle_lum));
	for (i = 0; i < 28; i++) {
		union fcm_word hue_word;
		u32 hue, sat;

		/* Stock reads the saturation table between the hue shadow read
		 * and write. Keep that order even if the unchecked shadow pointer
		 * aliases another mapped region. TODO: validate the shadow pointer
		 * in a separate behavior change.
		 */
		hue = READ_ONCE(fcm_hbh[i]);
		hue_word.value = reg[0x120 / 4 + i].value;
		sat = READ_ONCE(fcm_sbh[i]);
		hue_word.pair13.low = hue;
		reg[0x120 / 4 + i].value = hue_word.value;
		reg[0x1a0 / 4 + i].pair11.low = sat;
		reg[0x220 / 4 + i].pair9.low = fcm_ybh[i];
		if (i == 27) {
			reg[0x120 / 4 + i].pair13.high = 0;
			reg[0x1a0 / 4 + i].pair11.high = 200;
			reg[0x220 / 4 + i].pair9.high = 50;
		} else {
			reg[0x120 / 4 + i].pair13.high = fcm_hbh[i + 1];
			reg[0x1a0 / 4 + i].pair11.high = fcm_sbh[i + 1];
			reg[0x220 / 4 + i].pair9.high = fcm_ybh[i + 1];
		}
	}
	/* Stock reloads the base after the LUT updates, then fills each plane. */
	reg = fcm_dev[sel][channel];
	for (i = 0; i < 364; i++)
		reg[0x300 / 4 + i].value = 0x64646464;
	for (i = 0; i < 364; i++)
		reg[0x900 / 4 + i].value = 0x64646464;
	for (i = 0; i < 364; i++)
		reg[0xf00 / 4 + i].value = 0x64646464;
	for (i = 0; i < 364; i++)
		reg[0x1500 / 4 + i].value = 0x64646464;
	for (i = 0; i < 364; i++)
		reg[0x1b00 / 4 + i].value = 0x64646464;
	for (i = 0; i < 364; i++)
		reg[0x2100 / 4 + i].value = 0x64646464;
	reg[0].control.enable = 1;
	fcm_block[sel][channel].dirty = 1;
	return 0;
}

int set_364(u32 *table)
{
	volatile u8 *registers = (volatile u8 *)fcm_dev[0][0];
	unsigned int row, column, index;

	/* OEM bug: the final cell reads table[377] beyond a 364-word grid.
	 * TODO: validate/pad input; retain stock accesses, including aliases.
	 */
	for (row = 0; row < 28; row++)
		for (column = 0; column < 13; column++) {
			index = row * 13 + column;
			registers[0x300 + 4 * index] = table[index];
			registers[0x900 + 4 * index] = table[index];
			registers[0xf00 + 4 * index] = table[index];
			registers[0x301 + 4 * index] = table[index + 1];
			registers[0x901 + 4 * index] = table[index + 1];
			registers[0xf01 + 4 * index] = table[index + 1];
			registers[0x302 + 4 * index] = table[index + 13];
			registers[0x902 + 4 * index] = table[index + 13];
			registers[0xf02 + 4 * index] = table[index + 13];
			registers[0x303 + 4 * index] = table[index + 14];
			registers[0x903 + 4 * index] = table[index + 14];
			registers[0xf03 + 4 * index] = table[index + 14];
		}
	return 0;
}

int de_fcm_init(unsigned int sel, unsigned int channel, uintptr_t reg_base)
{
	uintptr_t base = reg_base + (sel + 1) * 0x100000 + 0xaa000;
	void *memory = kzalloc(0x26b0, GFP_KERNEL);

	if (!memory) {
		DISP_STOCK_WRN(190,
			"malloc vep fcm[%d][%d] memory fail! size=0x%x\n",
			sel, channel, 0x26b0);
		return -1;
	}
	fcm_block[sel][channel].val = memory;
	fcm_block[sel][channel].off = base;
	fcm_block[sel][channel].size = 0x26b0;
	fcm_block[sel][channel].dirty = 0;
	de_fcm_set_reg_base(sel, channel, memory);
	de_fcm_set_init_para(sel, channel);
	return 0;
}

int de_fcm_exit(unsigned int sel, unsigned int channel)
{
	/* TODO: stock leaves the freed shadow pointer and descriptor intact. */
	kfree(fcm_block[sel][channel].val);
	return 0;
}

int de_fcm_update_regs(unsigned int sel, unsigned int channel)
{
	struct de_reg_blocks *block = &fcm_block[sel][channel];
	if (block->dirty == 1) {
		memcpy((void *)block->off, block->val, block->size);
		block->dirty = 0;
	}
	return 0;
}

int de_fcm_enable(unsigned int sel, unsigned int channel, unsigned int enable)
{
	return 0;
}

int de_fcm_set_size(unsigned int sel, unsigned int channel,
		    unsigned int width, unsigned int height)
{
	fcm_dev[sel][channel][2].pair13.low = width ? width - 1 : 0;
	fcm_dev[sel][channel][2].pair13.high = height ? height - 1 : 0;
	fcm_block[sel][channel].dirty = 1;
	return 0;
}

int de_fcm_set_window(unsigned int sel, unsigned int channel,
		      unsigned int enable, struct de_rect window)
{
	volatile union fcm_word *reg = fcm_dev[sel][channel];
	reg[0].control.window = enable;
	/* Stock tests the full argument although only bit zero is stored above. */
	if (enable) {
		reg[3].pair13.low = window.x;
		reg[3].pair13.high = window.y;
		reg[4].pair13.low = window.x + window.w - 1;
		reg[4].pair13.high = window.y + window.h - 1;
	}
	fcm_block[sel][channel].dirty = 1;
	return 0;
}
