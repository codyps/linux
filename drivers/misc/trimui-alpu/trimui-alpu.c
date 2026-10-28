// SPDX-License-Identifier: GPL-2.0-only
/* TG3040 #860 board authentication and serial-number interface. */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/unaligned.h>
#include "arelink.h"

u8 gTrimuiSerialNumber[5];
EXPORT_SYMBOL_GPL(gTrimuiSerialNumber);
static char serial_text[32];

void faaes128_decode(const u8 *input, u8 *output)
{
	arelink_req_enc_dec(input, output, 1);
}

char *get_trimui_serialno(void)
{
	memset(serial_text, 0, sizeof(serial_text));
	sprintf(serial_text, "TG3040%02X%08X", gTrimuiSerialNumber[4],
		get_unaligned_le32(gTrimuiSerialNumber) + 0x3c14dc00U);
	return serial_text;
}

void arelink_main(void)
{
	u8 challenge[16], input[32], decoded[32], encrypted[32];
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
	/* #860 clears the self-test buffers only after authentication. */
	memset(input, 0, sizeof(input));
	memset(decoded, 0, sizeof(decoded));
	memset(encrypted, 0, sizeof(encrypted));
	strcpy(input, "TEST 123456789");
	/* Preserve the stock 32-bit NULL store when the cipher API fails.
	 * TODO: report an initialization error instead in a separate fix.
	 */
	if (arelink_req_enc_dec(input, encrypted, 0))
		*(volatile u32 *)0 = 1;
	if (arelink_req_enc_dec(encrypted, decoded, 1))
		*(volatile u32 *)0 = 1;
}

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
			printk("ret:%d\n", result);
	}
}

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
			printk("ret:%d\n", result);
	}
}

static int __init alpu_init(void)
{
	arelink_main();
	return 0;
}
device_initcall(alpu_init);

/* #860 retains a RET between matched I2C and PHY exit-body windows.
 * Original ownership/name/registration is unknown. Keep this compatible
 * return-only slot and its register effects; __used retains built-in code.
 */
static void __exit __used alpu_stock_exit(void)
{
}
MODULE_LICENSE("GPL");
