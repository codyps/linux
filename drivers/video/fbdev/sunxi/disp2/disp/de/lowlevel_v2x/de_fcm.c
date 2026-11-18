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
/* Partial Allwinner v2x hue-bank view. The holes are real register space;
 * they are not scratch storage. The arrays match offsets 0x120/0x1a0/0x220.
 */
struct fcm_hue_registers {
	u8 reserved0[0x120];
	union fcm_word hbh_hue[28];
	u8 reserved1[0x10];
	union fcm_word sbh_hue[28];
	u8 reserved2[0x10];
	union fcm_word vbh_hue[28];
};
_Static_assert(__builtin_offsetof(struct fcm_hue_registers, hbh_hue) == 0x120,
	       "FCM H hue register offset");
_Static_assert(__builtin_offsetof(struct fcm_hue_registers, sbh_hue) == 0x1a0,
	       "FCM S hue register offset");
_Static_assert(__builtin_offsetof(struct fcm_hue_registers, vbh_hue) == 0x220,
	       "FCM V hue register offset");
/* The three saturation planes used by set_364 are 364 four-byte cells each.
 * The 0x50-byte gaps are register space, not available scratch storage.
 */
struct fcm_byte_word { u8 b[4]; };
struct fcm_grid_registers {
	u8 reserved0[0x300];
	struct fcm_byte_word hbh_sat[364];
	u8 reserved1[0x50];
	struct fcm_byte_word sbh_sat[364];
	u8 reserved2[0x50];
	struct fcm_byte_word vbh_sat[364];
};
_Static_assert(sizeof(struct fcm_byte_word) == 4,
	       "FCM saturation cell stride");
_Static_assert(__builtin_offsetof(struct fcm_grid_registers, hbh_sat) == 0x300,
	       "FCM H saturation register offset");
_Static_assert(__builtin_offsetof(struct fcm_grid_registers, sbh_sat) == 0x900,
	       "FCM S saturation register offset");
_Static_assert(__builtin_offsetof(struct fcm_grid_registers, vbh_sat) == 0xf00,
	       "FCM V saturation register offset");
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
	volatile struct fcm_hue_registers *bank;
	long i;
	unsigned int offset;
	unsigned int next_byte;

	memcpy((void *)(reg + 0x20 / 4), fcm_angle_hue, sizeof(fcm_angle_hue));
	memcpy((void *)(reg + 0xa0 / 4), fcm_angle_sat, sizeof(fcm_angle_sat));
	memcpy((void *)(reg + 0xe0 / 4), fcm_angle_lum, sizeof(fcm_angle_lum));
	bank = (volatile struct fcm_hue_registers *)reg;
	for (i = 0, next_byte = 4; i < 28; i++, next_byte += 4) {
		union fcm_word hue_word;
		u32 hue, sat;

		/* Stock reads the saturation table between the hue shadow read
		 * and write. Keep that order even if the unchecked shadow pointer
		 * aliases another mapped region. TODO: validate the shadow pointer
		 * in a separate behavior change.
		 */
		hue = READ_ONCE(fcm_hbh[i]);
		hue_word.value = bank->hbh_hue[i].value;
		sat = READ_ONCE(fcm_sbh[i]);
		hue_word.pair13.low = hue;
		bank->hbh_hue[i].value = hue_word.value;
		bank->sbh_hue[i].pair11.low = sat;
		/* Preserve the OEM read/write schedule for shadow/table aliases. */
		bank->vbh_hue[i].pair9.low = READ_ONCE(fcm_ybh[i]);
		if (i == 27) {
			bank->hbh_hue[i].pair13.high = 0;
			bank->sbh_hue[i].pair11.high = 200;
			bank->vbh_hue[i].pair9.high = 50;
		} else {
			/* OEM reads the H shadow before its next-row table value.
			 * Preserve that order when the unchecked shadow aliases a table.
			 */
			volatile union fcm_word *dst_h = bank->hbh_hue + i;
			union fcm_word current_h;
			current_h.value = dst_h->value;
			u32 next_h = READ_ONCE(
				*(const u32 *)((const char *)fcm_hbh + next_byte));
			current_h.pair13.high = next_h;
			dst_h->value = current_h.value;
			bank->sbh_hue[i].pair11.high = READ_ONCE(fcm_sbh[i + 1]);
			bank->vbh_hue[i].pair9.high =
				*(const u32 *)((const char *)fcm_ybh + next_byte);
		}
		/* OEM carries the separate next-row byte offset through the LUT
		 * body. This read-only constraint has no memory or runtime effect.
		 */
		asm volatile("" : : "r"(next_byte));
	}
	/* Stock reloads the base after the LUT updates, then fills each plane. */
	reg = fcm_dev[sel][channel];
	for (offset = 0; offset < 0x5b0; offset += 4) {
		*(volatile u32 *)((volatile u8 *)reg + 0x300 + offset) = 0x64646464;
		asm volatile("" : "+r"(offset));
	}
	for (i = 0; i < 364; i++)
		reg[0x900 / 4 + i].value = 0x64646464;
	for (offset = 0; offset < 0x5b0; offset += 4) {
		asm volatile("" : : "r"(offset));
		*(volatile u32 *)((volatile u8 *)reg + 0xf00 + offset) = 0x64646464;
	}
	for (i = 0; i < 364; i++)
		reg[0x1500 / 4 + i].value = 0x64646464;
	for (offset = 0; offset < 0x5b0; offset += 4) {
		asm volatile("" : : "r"(offset));
		*(volatile u32 *)((volatile u8 *)reg + 0x1b00 + offset) = 0x64646464;
	}
	for (offset = 0; offset < 0x5b0; offset += 4) {
		/* Keep the byte cursor live for the stock fill-loop shape. */
		asm volatile("" : : "r"(offset));
		*(volatile u32 *)((volatile u8 *)reg + 0x2100 + offset) = 0x64646464;
	}
	reg[0].control.enable = 1;
	fcm_block[sel][channel].dirty = 1;
	return 0;
}

int set_364(u32 *table)
{
	volatile struct fcm_grid_registers *bank =
		(volatile struct fcm_grid_registers *)fcm_dev[0][0];
	unsigned int row, column, index;

	/* OEM bug: the final cell reads table[377] beyond a 364-word grid.
	 * TODO: validate/pad input; retain stock accesses, including aliases.
	 */
	for (row = 0; row < 28; row++)
		for (column = 0; column < 13; column++) {
			/* OEM keeps separate row/column values for source indexing.
			 * They are bounded here, even though the final source word is
			 * still table[377] and can fault. Do not fold the expressions
			 * into index: that changes the emitted access schedule.
			 */
			int m = row, n = column;
			index = row * 13 + column;
			bank->hbh_sat[index].b[0] = table[m * 13 + n];
			bank->sbh_sat[index].b[0] = table[m * 13 + n];
			bank->vbh_sat[index].b[0] = table[m * 13 + n];
			bank->hbh_sat[index].b[1] = table[m * 13 + n + 1];
			bank->sbh_sat[index].b[1] = table[m * 13 + n + 1];
			bank->vbh_sat[index].b[1] = table[m * 13 + n + 1];
			bank->hbh_sat[index].b[2] = table[(m + 1) * 13 + n];
			bank->sbh_sat[index].b[2] = table[(m + 1) * 13 + n];
			bank->vbh_sat[index].b[2] = table[(m + 1) * 13 + n];
			bank->hbh_sat[index].b[3] = table[(m + 1) * 13 + n + 1];
			bank->sbh_sat[index].b[3] = table[(m + 1) * 13 + n + 1];
			bank->vbh_sat[index].b[3] = table[(m + 1) * 13 + n + 1];
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
