/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_MFD_TCS4838_H
#define __LINUX_MFD_TCS4838_H
#include <linux/regmap.h>
#define TCS4838_VSEL0 0x00
#define TCS4838_VSEL1 0x01
#define TCS4838_CTRL  0x02
#define TCS4838_PGOOD 0x05
struct tcs4838 {
	struct device *dev;
	struct regmap *regmap;
};
int tcs4838_device_init(struct tcs4838 *tcs);
int tcs4838_device_exit(struct tcs4838 *tcs);
#endif
