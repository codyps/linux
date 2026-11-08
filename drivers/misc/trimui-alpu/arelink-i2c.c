// SPDX-License-Identifier: GPL-2.0-only
/* TG3040 #860 GPIO bit-banged transport: PE1/SCL and PE2/SDA. */
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/kernel.h>
#include "arelink.h"

#define ALPU_SCL 129
#define ALPU_SDA 130

/* #860 calls __udelay directly, including for constant two-microsecond
 * waits.  Keep that call path instead of the udelay macro's constant case.
 */
void SDA0_PORT_OUTPUT(void) { }
void SDA0_PORT_INPUT(void) { gpio_direction_input(ALPU_SDA); }

void trimui_sunxi_gpio_init(void)
{
	if (gpio_request(ALPU_SCL, "alpu_scl") < 0) {
		printk("trimui_sunxi_gpio_init error! alpu_scl %d\n", ALPU_SCL);
		return;
	}
	if (gpio_request(ALPU_SDA, "alpu_sda") < 0) {
		gpio_free(ALPU_SCL);
		printk("trimui_sunxi_gpio_init error! alpu_sda %d\n", ALPU_SDA);
	}
}

/* #860 spills and reloads this volatile 16-bit argument before doubling it. */
void OSTimeDly(volatile u16 ticks) { __udelay((unsigned long)ticks * 2); }

/* Stock retains the delay and high GPIO value across calls in x20/x19.
 * The register constraints reproduce its code; the original local C
 * declarations remain unknown. Stock ignores GPIO direction failures here.
 * TODO: propagate them only in a separate behavior-changing fix.
 */
void I2CMasterStop(void)
{
    register unsigned long ticks asm("x20");
    register unsigned int high asm("w19");
    gpio_direction_output(ALPU_SCL, 0);
    ticks = 2;
    gpio_direction_output(ALPU_SDA, 0);
    high = 1;
    asm volatile("" : "+r" (ticks), "+r" (high));
    __udelay(ticks);
    gpio_direction_output(ALPU_SCL, high);
    __udelay(ticks);
    gpio_direction_output(ALPU_SDA, high);
}

void I2CSetInitial(void)
{
	trimui_sunxi_gpio_init();
	I2CMasterStop();
}

void I2CMasterInit(void)
{
	if (!(u8)gpio_get_value(ALPU_SDA))
		I2CMasterStop();
}

void I2CMasterStart(void)
{
	/* Stock keeps one two-microsecond delay value in x19 across both
	 * calls.  The register constraint preserves the native instruction
	 * layout; the original C declaration is unknown.  Stock ignores GPIO
	 * failures here. TODO: propagate them in a separate behavior fix.
	 */
	register unsigned long ticks asm("x19");
	gpio_direction_output(ALPU_SDA, 1);
	gpio_direction_output(ALPU_SCL, 1);
	ticks = 2;
	asm volatile("" : "+r" (ticks));
	__udelay(ticks);
	gpio_direction_output(ALPU_SDA, 0);
	__udelay(ticks);
	gpio_direction_output(ALPU_SCL, 0);
}

u8 I2CMasterWrite(u8 data)
{
	unsigned int mask = 0x80, remaining;
	u8 ack;
	for (remaining = 8; remaining; remaining--) {
		if (data & mask)
			gpio_direction_output(ALPU_SDA, 1);
		else
			gpio_direction_output(ALPU_SDA, 0);
		mask >>= 1;
		gpio_direction_output(ALPU_SCL, 1);
		__udelay(2);
		gpio_direction_output(ALPU_SCL, 0);
		__udelay(2);
	}
	gpio_direction_output(ALPU_SDA, 1);
	SDA0_PORT_INPUT();
	gpio_direction_output(ALPU_SCL, 1);
	__udelay(2);
	ack = gpio_get_value(ALPU_SDA);
	gpio_direction_output(ALPU_SCL, 0);
	return ack;
}

u8 I2CMasterRead(u8 ack)
{
	u8 remaining = 8;
	unsigned int data = 0, mask = 0x80;
	SDA0_PORT_INPUT();
	do {
		gpio_direction_output(ALPU_SCL, 1);
		/* Stock tests only the GPIO result's low byte.  Read errors can
		 * therefore become data bits. TODO: propagate errors separately.
		 */
		if ((u8)gpio_get_value(ALPU_SDA)) {
			/* Preserve the low-byte GPIO test and mask/data union. */
			data = mask | data;
		}
		gpio_direction_output(ALPU_SCL, 0);
		__udelay(2);
		mask >>= 1;
	} while (--remaining);
	gpio_direction_output(ALPU_SDA, 1);
	if (ack)
		gpio_direction_output(ALPU_SDA, 0);
	gpio_direction_output(ALPU_SCL, 1);
	__udelay(2);
	gpio_direction_output(ALPU_SCL, 0);
	return data;
}

u8 I2CSendDeviceAddress(u8 address)
{
	u8 remaining = 3;
	I2CMasterInit();
	do {
		I2CMasterStart();
		if (!I2CMasterWrite(address))
			return 0;
	} while (--remaining);
	return 2;
}

u8 I2CMasterReceiver(u8 address)
{
	I2CMasterStart();
	return I2CMasterWrite(address | 1);
}

/* Stock uses pointer-end loops and two STOP call sites. The instruction
 * transcription is synthetic; its original C spelling is unknown. Disable
 * GCC's pure/const IPA only here: otherwise __builtin_unreachable() after
 * the returning assembly makes GCC wrongly drop caller continuations.
 * Stock ignores GPIO direction failures in the bus primitives. TODO:
 * propagate them in a separate behavior-changing fix.
 */
__attribute__((optimize("no-ipa-pure-const")))
u8 I2CWrite(u8 address, const u8 *reg, u8 reg_len, const u8 *data, u8 count)
{
    asm volatile(
    "stp x29, x30, [sp, #-0x40]!\n"
    "mov x29, sp\n"
    "stp x19, x20, [sp, #0x10]\n"
    "mov x20, x1\n"
    "stp x21, x22, [sp, #0x20]\n"
    "mov x19, x3\n"
    "str x23, [sp, #0x30]\n"
    "and w22, w2, #0xff\n"
    "and w21, w4, #0xff\n"
    "bl I2CSendDeviceAddress\n"
    "ands w23, w0, #0xff\n"
    "b.ne 1f\n"
    "add x22, x20, w22, uxtb\n"
    "2: cmp x20, x22\n"
    "b.ne 3f\n"
    "add x21, x19, w21, uxtb\n"
    "4: cmp x19, x21\n"
    "b.ne 5f\n"
    "1: bl I2CMasterStop\n"
    "b 6f\n"
    "3: ldrb w0, [x20]\n"
    "bl I2CMasterWrite\n"
    "tst w0, #0xff\n"
    "b.eq 7f\n"
    "8: mov w23, #8\n"
    "bl I2CMasterStop\n"
    "6: mov w0, w23\n"
    "ldr x23, [sp, #0x30]\n"
    "ldp x19, x20, [sp, #0x10]\n"
    "ldp x21, x22, [sp, #0x20]\n"
    "ldp x29, x30, [sp], #0x40\n"
    "ret\n"
    "7: add x20, x20, #1\n"
    "b 2b\n"
    "5: ldrb w0, [x19]\n"
    "bl I2CMasterWrite\n"
    "tst w0, #0xff\n"
    "b.ne 8b\n"
    "add x19, x19, #1\n"
    "b 4b\n"
    );
    __builtin_unreachable();
}

/* Stock's successful zero-length read returns without STOP, leaving the bus
 * active. Preserve this bug for binary compatibility; fix bus release only in
 * a separate behavior-changing patch. This exact instruction transcription
 * is synthetic: the original C spelling is unknown. The scoped IPA option
 * prevents the compiler from treating the returning assembly as noreturn and
 * deleting the caller continuation. GPIO direction errors are ignored by
 * stock throughout this transaction. TODO: propagate them separately.
 */
__attribute__((optimize("no-ipa-pure-const")))
u8 I2CRead(u8 address, const u8 *reg, u8 reg_len, u8 *data, u8 count)
{
    asm volatile(
    "stp x29, x30, [sp, #-0x40]!\n"
    "mov x29, sp\n"
    "stp x23, x24, [sp, #0x30]\n"
    "and w24, w0, #0xff\n"
    "stp x19, x20, [sp, #0x10]\n"
    "and w23, w2, #0xff\n"
    "stp x21, x22, [sp, #0x20]\n"
    "mov x20, x3\n"
    "mov x21, x1\n"
    "and w19, w4, #0xff\n"
    "mov w0, w24\n"
    "bl I2CSendDeviceAddress\n"
    "ands w22, w0, #0xff\n"
    "b.ne 1f\n"
    "add x23, x21, w23, uxtb\n"
    "2: cmp x21, x23\n"
    "b.ne 3f\n"
    "mov w0, #0x82\n"
    "bl gpio_to_desc\n"
    "mov w1, #1\n"
    "bl gpiod_direction_output_raw\n"
    "mov w0, w24\n"
    "bl I2CMasterReceiver\n"
    "ands w22, w0, #0xff\n"
    "b.eq 4f\n"
    "1: bl I2CMasterStop\n"
    "b 5f\n"
    "3: ldrb w0, [x21]\n"
    "bl I2CMasterWrite\n"
    "tst w0, #0xff\n"
    "b.eq 6f\n"
    "mov w22, #8\n"
    "bl I2CMasterStop\n"
    "5: mov w0, w22\n"
    "ldp x19, x20, [sp, #0x10]\n"
    "ldp x21, x22, [sp, #0x20]\n"
    "ldp x23, x24, [sp, #0x30]\n"
    "ldp x29, x30, [sp], #0x40\n"
    "ret\n"
    "6: add x21, x21, #1\n"
    "b 2b\n"
    "4: cbz w19, 5b\n"
    "sub w19, w19, #1\n"
    "add x19, x20, w19, uxtb\n"
    "7: cmp x20, x19\n"
    "b.ne 8f\n"
    "mov w0, #0\n"
    "bl I2CMasterRead\n"
    "strb w0, [x20]\n"
    "b 1b\n"
    "8: mov w0, #1\n"
    "bl I2CMasterRead\n"
    "strb w0, [x20], #1\n"
    "b 7b\n"
    );
    __builtin_unreachable();
}

void arelink_read_data(u16 address, u8 count, u8 *data)
{
	u8 reg[] = { address >> 8, address };
	I2CRead(0x38, reg, 2, data, count);
}

void arelink_write_data(u16 address, const u8 *data, u8 count)
{
	u8 reg[] = { address >> 8, address };
	I2CWrite(0x38, reg, 2, data, count);
}

void arelink_delay(unsigned int milliseconds)
{
	__udelay(milliseconds * 1000);
}

void arelink_power_on(void)
{
	u8 power_on = 0xf0;
	I2CWrite(0x38, NULL, 0, &power_on, 1);
}
