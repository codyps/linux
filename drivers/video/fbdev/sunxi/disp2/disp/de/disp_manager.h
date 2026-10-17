/*
 * Allwinner SoCs display driver.
 *
 * Copyright (C) 2016 Allwinner.
 *
 * This file is licensed under the terms of the GNU General Public
 * License version 2.  This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */

#ifndef __DISP_MANAGER_H__
#define __DISP_MANAGER_H__

#include "disp_private.h"

s32 disp_init_mgr(struct disp_bsp_init_para *para);

#if defined(CONFIG_SUNXI_IOMMU)
#define DE_MASTOR_ID 0
/* TG3040 #860 disp_init_mgr assigns 0 to DE0 and 1 to DE1. */
#define IOMMU_DE0_MASTOR_ID 0
#define IOMMU_DE1_MASTOR_ID 1
extern void sunxi_enable_device_iommu(unsigned int mastor_id, bool flag);
#endif

/* Stock disp_mgr_enable_iommu() dereferences the private-state result even
 * when a NULL manager makes disp_mgr_get_priv() return NULL.
 * TODO: validate this result in a separate corrective change. */

extern s32 __disp_config_transfer2inner(
	struct disp_layer_config_inner *config_inner,
	struct disp_layer_config *config);
extern s32 __disp_config2_transfer2inner(
	struct disp_layer_config_inner *config_inner,
	struct disp_layer_config2 *config);

s32 disp_mgr_set_rtwb_layer(struct disp_manager *mgr,
			    struct disp_layer_config2 *config,
			    struct disp_capture_info2 *p_cptr_info,
			    unsigned int layer_num);

#endif
