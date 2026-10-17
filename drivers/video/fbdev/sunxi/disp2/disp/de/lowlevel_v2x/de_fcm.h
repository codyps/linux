/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef TRIMUI_DE_FCM_H
#define TRIMUI_DE_FCM_H
#include "de_rtmx.h"
int de_fcm_set_reg_base(unsigned int sel, unsigned int channel, void *base);
int de_fcm_set_init_para(unsigned int sel, unsigned int channel);
int de_fcm_init(unsigned int sel, unsigned int channel, uintptr_t reg_base);
int de_fcm_exit(unsigned int sel, unsigned int channel);
int de_fcm_update_regs(unsigned int sel, unsigned int channel);
int de_fcm_enable(unsigned int sel, unsigned int channel, unsigned int enable);
int de_fcm_set_size(unsigned int sel, unsigned int channel, unsigned int width, unsigned int height);
int de_fcm_set_window(unsigned int sel, unsigned int channel, unsigned int enable, struct de_rect window);
int set_364(u32 *table);
#endif
