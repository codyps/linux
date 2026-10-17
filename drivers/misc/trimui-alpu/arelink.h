/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef TRIMUI_ARELINK_H
#define TRIMUI_ARELINK_H
#ifdef ARELINK_HOST_TEST
#include <stdint.h>
#include <string.h>
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
#else
#include <linux/types.h>
#include <linux/string.h>
#endif
void arelink_read_data(u16 address, u8 count, u8 *data);
void arelink_write_data(u16 address, const u8 *data, u8 count);
void arelink_delay(unsigned int milliseconds);
void arelink_power_on(void);
void I2CSetInitial(void);
u8 arelink_random(void);
void arelink_srand(u32 seed);
u8 arelink_init(u8 *serial);
u8 arelink_set_aes_key_size(u8 size);
u8 arelink_req_enc_dec(const u8 *input, u8 *output, u8 decrypt);
u8 arelink_req_sw_enc_dec(const u8 *input, u8 *output, u8 decrypt);
u8 arelink_req_cypher(const u8 *input, u8 *output);
u8 arelink_bypass_mode(const u8 *input, u8 *output);
u8 arelink_authentication(u8 key_size, u32 seed, const u8 *challenge);
u8 get_lib_version(u8 *output, u8 *length);
u8 arelink_eeprom_write(u32 password, u8 page, const u8 *data, u8 encrypted);
u8 arelink_eeprom_read(u32 password, u8 page, u8 *data, u8 encrypted);
u8 arelink_eeprom_pwchg(u8 page, u32 old_password, u32 new_password);
#endif
