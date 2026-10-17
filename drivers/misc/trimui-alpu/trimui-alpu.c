// SPDX-License-Identifier: GPL-2.0-only
/* TG3040 #860 board authentication and serial-number interface. */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/unaligned.h>
#include "arelink.h"

u8 gTrimuiSerialNumber[5];
EXPORT_SYMBOL(gTrimuiSerialNumber);
static char serial_text[32];

char *get_trimui_serialno(void)
{
	memset(serial_text, 0, sizeof(serial_text));
	sprintf(serial_text, "TG3040%02X%08X", gTrimuiSerialNumber[4],
		get_unaligned_le32(gTrimuiSerialNumber) + 0x3c14dc00U);
	return serial_text;
}
EXPORT_SYMBOL(get_trimui_serialno);

void faaes128_decode(const u8 *input, u8 *output)
{
	arelink_req_enc_dec(input, output, 1);
}
EXPORT_SYMBOL(faaes128_decode);

void are_enc_path256(const u8 *input, u8 *output)
{
	unsigned int i;
	u8 result;
	if (!input || !output)
		return;
	memset(output, 0, 256);
	for (i = 0; i < 256; i += 16) {
		result = arelink_req_enc_dec(input + i, output + i, 0);
		if (result)
			pr_info("ret:%d\n", result);
	}
}
EXPORT_SYMBOL(are_enc_path256);

void are_dec_path256(const u8 *input, u8 *output)
{
	unsigned int i;
	u8 result;
	if (!input || !output)
		return;
	memset(output, 0, 256);
	for (i = 0; i < 256; i += 16) {
		result = arelink_req_enc_dec(input + i, output + i, 1);
		if (result)
			pr_info("ret:%d\n", result);
	}
}
EXPORT_SYMBOL(are_dec_path256);

void arelink_main(void)
{
	u8 challenge[16], input[32] = { 0 }, decoded[32] = { 0 };
	u8 encrypted[32] = { 0 };
	unsigned int i;
	I2CSetInitial();
	arelink_power_on();
	arelink_delay(2);
	arelink_init(gTrimuiSerialNumber);
	pr_info("serial: %02X%02X%02X%02X%02X\n", gTrimuiSerialNumber[4],
		gTrimuiSerialNumber[3], gTrimuiSerialNumber[2], gTrimuiSerialNumber[1], gTrimuiSerialNumber[0]);
	for (i = 0; i < 16; i++)
		challenge[i] = arelink_random();
	arelink_authentication(2, 0, challenge);
	strcpy(input, "TEST 123456789");
	/* Preserve the stock fatal error path when the cipher API fails. */
	if (arelink_req_enc_dec(input, encrypted, 0))
		*(volatile u8 *)0 = 1;
	if (arelink_req_enc_dec(encrypted, decoded, 1))
		*(volatile u8 *)0 = 1;
}

static int __init alpu_init(void)
{
	arelink_main();
	return 0;
}
device_initcall(alpu_init);
MODULE_LICENSE("GPL");
