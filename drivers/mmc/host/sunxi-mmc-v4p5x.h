/*
* Sunxi SD/MMC host driver
*
* Copyright (C) 2015 AllWinnertech Ltd.
* Author: lixiang <lixiang@allwinnertech>
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License version 2 as
* published by the Free Software Foundation.
*
* This program is distributed "as is" WITHOUT ANY WARRANTY of any
* kind, whether express or implied; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#ifndef __SUNXI_MMC_V4P5X_H__
#define __SUNXI_MMC_V4P5X_H__

/* Stock sunxi_mmc_clk_set_rate_for_sdmmc_v4p5x compares bus_width to
 * literal 8, although MMC_BUS_WIDTH_8 is 3. Preserve the emitted clock
 * selection; TODO: correct the enum comparison in a separate behavior fix.
 */

void sunxi_mmc_init_priv_v4p5x(struct sunxi_mmc_host *host,
			       struct platform_device *pdev, int phy_index);
void sunxi_mmc_init_priv_v4p6x(struct sunxi_mmc_host *host,
			       struct platform_device *pdev, int phy_index);
#endif
