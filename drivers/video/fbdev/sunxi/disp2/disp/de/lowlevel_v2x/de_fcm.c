// SPDX-License-Identifier: GPL-2.0-only
/* TG3040 #860 FCM register-shadow implementation, recovered from firmware.
 * A133 advertises no FCM channels. The stock APIs are nevertheless linked in;
 * notably de_fcm_enable is an observed no-op, not a substitute implementation.
 */
#include "de_fcm.h"
#include "de_fcm_tables.h"

static u32 *fcm_dev[DE_NUM][CHN_NUM];
static struct de_reg_blocks fcm_block[DE_NUM][CHN_NUM];

int de_fcm_set_reg_base(unsigned int sel, unsigned int channel, void *base)
{
	fcm_dev[sel][channel] = base;
	return 0;
}

int de_fcm_set_init_para(unsigned int sel, unsigned int channel)
{
	u32 *reg = fcm_dev[sel][channel];
	unsigned int i, offset;
	memcpy(reg + 0x20 / 4, fcm_angle_hue, sizeof(fcm_angle_hue));
	memcpy(reg + 0xa0 / 4, fcm_angle_sat, sizeof(fcm_angle_sat));
	memcpy(reg + 0xe0 / 4, fcm_angle_lum, sizeof(fcm_angle_lum));
	for (i = 0; i < 28; i++) {
		reg[0x120 / 4 + i] = (reg[0x120 / 4 + i] & 0xe000e000) |
			(fcm_hbh[i] & 0x1fff) |
			((i == 27 ? 0 : fcm_hbh[i + 1] & 0x1fff) << 16);
		reg[0x1a0 / 4 + i] = (reg[0x1a0 / 4 + i] & 0xf800f800) |
			(fcm_sbh[i] & 0x7ff) |
			((i == 27 ? 200 : fcm_sbh[i + 1] & 0x7ff) << 16);
		reg[0x220 / 4 + i] = (reg[0x220 / 4 + i] & 0xfe00fe00) |
			(fcm_ybh[i] & 0x1ff) |
			((i == 27 ? 50 : fcm_ybh[i + 1] & 0x1ff) << 16);
	}
	for (offset = 0x300; offset <= 0x2100; offset += 0x600)
		for (i = 0; i < 0x5b0 / 4; i++)
			reg[offset / 4 + i] = 0x64646464;
	reg[0] |= 1;
	fcm_block[sel][channel].dirty = 1;
	return 0;
}

int de_fcm_init(unsigned int sel, unsigned int channel, uintptr_t reg_base)
{
	void *memory = kzalloc(0x26b0, GFP_KERNEL);
	if (!memory)
		return -1;
	fcm_dev[sel][channel] = memory;
	fcm_block[sel][channel].val = memory;
	fcm_block[sel][channel].off = reg_base + (sel + 1) * 0x100000 + 0xaa000;
	fcm_block[sel][channel].size = 0x26b0;
	fcm_block[sel][channel].dirty = 0;
	de_fcm_set_init_para(sel, channel);
	return 0;
}

int de_fcm_exit(unsigned int sel, unsigned int channel)
{
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
	u32 *reg = fcm_dev[sel][channel];
	reg[2] = (reg[2] & 0xe000e000) | (width ? (width - 1) & 0x1fff : 0) |
		((height ? (height - 1) & 0x1fff : 0) << 16);
	fcm_block[sel][channel].dirty = 1;
	return 0;
}

int de_fcm_set_window(unsigned int sel, unsigned int channel,
		      unsigned int enable, struct de_rect window)
{
	u32 *reg = fcm_dev[sel][channel];
	reg[0] = (reg[0] & ~(1 << 4)) | ((enable & 1) << 4);
	if (enable) {
		reg[3] = (reg[3] & 0xe000e000) | (window.x & 0x1fff) |
			((window.y & 0x1fff) << 16);
		reg[4] = (reg[4] & 0xe000e000) | ((window.x + window.w - 1) & 0x1fff) |
			(((window.y + window.h - 1) & 0x1fff) << 16);
	}
	fcm_block[sel][channel].dirty = 1;
	return 0;
}

int set_364(u32 *table)
{
	u8 *registers = (u8 *)fcm_dev[0][0];
	unsigned int row, column, plane, index;
	for (row = 0; row < 28; row++)
		for (column = 0; column < 13; column++) {
			index = row * 13 + column;
			for (plane = 0x300; plane <= 0xf00; plane += 0x600) {
				registers[plane + 4 * index] = table[index];
				registers[plane + 4 * index + 1] = table[index + 1];
				registers[plane + 4 * index + 2] = table[index + 13];
				registers[plane + 4 * index + 3] = table[index + 14];
			}
		}
	return 0;
}
