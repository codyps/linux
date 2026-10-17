/*************************************************************************/ /*!
@File
@Title          System Configuration
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@Description    System Configuration functions
@License        Dual MIT/GPLv2

The contents of this file are subject to the MIT license as set out below.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

Alternatively, the contents of this file may be used under the terms of
the GNU General Public License Version 2 ("GPL") in which case the provisions
of GPL are applicable instead of those above.

If you wish to allow use of your version of this file only under the terms of
GPL, and not to allow others to use your version of this file under the terms
of the MIT license, indicate your decision by deleting the provisions above
and replace them with the notice and other provisions required by GPL as set
out in the file called "GPL-COPYING" included in this distribution. If you do
not delete the provisions above, a recipient may use your version of this file
under the terms of either the MIT license or GPL.

This License is also included in this distribution in the file called
"MIT-COPYING".

EXCEPT AS OTHERWISE STATED IN A NEGOTIATED AGREEMENT: (A) THE SOFTWARE IS
PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT; AND (B) IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/ /**************************************************************************/

/* TG3040 system configuration reconstructed from the stock 1.19 module. */

#include "pvrsrv_device.h"
#include "syscommon.h"
#include "allocmem.h"
#include "sysinfo.h"
#include "sysconfig.h"
#include "physheap.h"
#include "vz_vmm_pvz.h"
#include "interrupt_support.h"

#if defined(__linux__)
#include <linux/clk.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#endif

#include "platform.h"

/* Stock also retains this static config object for the platform back-pointer. */
static PVRSRV_DEVICE_CONFIG gsDevices[1];

/*
	CPU to Device physical address translation
*/
static
void UMAPhysHeapCpuPAddrToDevPAddr(IMG_HANDLE hPrivData,
								   IMG_UINT32 ui32NumOfAddr,
								   IMG_DEV_PHYADDR *psDevPAddr,
								   IMG_CPU_PHYADDR *psCpuPAddr)
{
	PVR_UNREFERENCED_PARAMETER(hPrivData);

	/* Optimise common case */
	psDevPAddr[0].uiAddr = psCpuPAddr[0].uiAddr;
	if (ui32NumOfAddr > 1)
	{
		IMG_UINT32 ui32Idx;
		for (ui32Idx = 1; ui32Idx < ui32NumOfAddr; ++ui32Idx)
		{
			psDevPAddr[ui32Idx].uiAddr = psCpuPAddr[ui32Idx].uiAddr;
		}
	}
}

/*
	Device to CPU physical address translation
*/
static
void UMAPhysHeapDevPAddrToCpuPAddr(IMG_HANDLE hPrivData,
								   IMG_UINT32 ui32NumOfAddr,
								   IMG_CPU_PHYADDR *psCpuPAddr,
								   IMG_DEV_PHYADDR *psDevPAddr)
{
	PVR_UNREFERENCED_PARAMETER(hPrivData);

	/* Optimise common case */
	psCpuPAddr[0].uiAddr = psDevPAddr[0].uiAddr;
	if (ui32NumOfAddr > 1)
	{
		IMG_UINT32 ui32Idx;
		for (ui32Idx = 1; ui32Idx < ui32NumOfAddr; ++ui32Idx)
		{
			psCpuPAddr[ui32Idx].uiAddr = psDevPAddr[ui32Idx].uiAddr;
		}
	}
}

static void SysDevFeatureDepInit(PVRSRV_DEVICE_CONFIG *psDevConfig, IMG_UINT64 ui64Features)
{
#if defined(SUPPORT_AXI_ACE_TEST)
	if (ui64Features & RGX_FEATURE_AXI_ACELITE_BIT_MASK)
	{
		psDevConfig->eCacheSnoopingMode = PVRSRV_DEVICE_SNOOP_CPU_ONLY;
	}
	else
#endif
	{
		psDevConfig->eCacheSnoopingMode = PVRSRV_DEVICE_SNOOP_NONE;
	}
}

static PHYS_HEAP_FUNCTIONS gsPhysHeapFuncs = {
	.pfnCpuPAddrToDevPAddr = UMAPhysHeapCpuPAddrToDevPAddr,
	.pfnDevPAddrToCpuPAddr = UMAPhysHeapDevPAddrToCpuPAddr,
};

PVRSRV_ERROR SysDevInit(void *pvOSDevice, PVRSRV_DEVICE_CONFIG **ppsDevConfig)
{
	struct device *dev = pvOSDevice;
	struct sunxi_platform *sunxi_data;
	PVRSRV_DEVICE_CONFIG *psDevConfig;
	RGX_DATA *psRGXData;
	RGX_TIMING_INFORMATION *psRGXTimingInfo;
	PHYS_HEAP_CONFIG *pasPhysHeaps;

	if (sunxi_platform_init(dev))
		return PVRSRV_ERROR_INIT_FAILURE;
	sunxi_data = dev->platform_data;
	dma_set_mask(pvOSDevice, DMA_BIT_MASK(48));

	psDevConfig = OSAllocZMem(sizeof(*psDevConfig) + sizeof(*psRGXData) +
				 sizeof(*psRGXTimingInfo));
	if (!psDevConfig)
		return PVRSRV_ERROR_OUT_OF_MEMORY;
	psRGXData = (RGX_DATA *)((IMG_CHAR *)psDevConfig + sizeof(*psDevConfig));
	psRGXTimingInfo = (RGX_TIMING_INFORMATION *)((IMG_CHAR *)psRGXData + sizeof(*psRGXData));

	pasPhysHeaps = OSAllocZMem(2 * sizeof(*pasPhysHeaps));
	if (!pasPhysHeaps) {
		OSFreeMem(psDevConfig);
		return PVRSRV_ERROR_OUT_OF_MEMORY;
	}
	pasPhysHeaps[0].eType = PHYS_HEAP_TYPE_UMA;
	pasPhysHeaps[0].pszPDumpMemspaceName = "SYSMEM";
	pasPhysHeaps[0].psMemFuncs = &gsPhysHeapFuncs;
	pasPhysHeaps[0].ui32UsageFlags = PHYS_HEAP_USAGE_GPU_LOCAL;
	pasPhysHeaps[1].eType = PHYS_HEAP_TYPE_UMA;
	pasPhysHeaps[1].pszPDumpMemspaceName = "SYSMEM_FW";
	pasPhysHeaps[1].psMemFuncs = &gsPhysHeapFuncs;
	pasPhysHeaps[1].ui32UsageFlags = PHYS_HEAP_USAGE_FW_SHARED;

	psRGXTimingInfo->ui32CoreClockSpeed = clk_get_rate(sunxi_data->clks.core);
	psRGXTimingInfo->bEnableActivePM = IMG_TRUE;
	psRGXTimingInfo->bEnableRDPowIsland = IMG_TRUE;
	psRGXTimingInfo->ui32ActivePMLatencyms = SYS_RGX_ACTIVE_POWER_LATENCY_MS;
	psRGXData->psRGXTimingInfo = psRGXTimingInfo;

	psDevConfig->pvOSDevice = pvOSDevice;
	psDevConfig->pszName = "sunxi";
	psDevConfig->pszVersion = NULL;
	psDevConfig->pfnSysDevFeatureDepInit = SysDevFeatureDepInit;
	psDevConfig->sRegsCpuPBase.uiAddr = sunxi_data->reg_base;
	psDevConfig->ui32RegsSize = sunxi_data->reg_size;
	psDevConfig->ui32IRQ = sunxi_data->irq_num;
	psDevConfig->pfnPrePowerState = sunxiPrePowerState;
	psDevConfig->pfnPostPowerState = sunxiPostPowerState;
	psDevConfig->hDevData = psRGXData;
	psDevConfig->hSysData = sunxi_data;
	psDevConfig->eDefaultHeap = PVRSRV_PHYS_HEAP_GPU_LOCAL;
	psDevConfig->pasPhysHeaps = pasPhysHeaps;
	psDevConfig->ui32PhysHeapCount = 2;
	psDevConfig->pfnClockFreqGet = NULL;
	*ppsDevConfig = psDevConfig;
	sunxi_data->config = &gsDevices[0];
	return PVRSRV_OK;
}

void SysDevDeInit(PVRSRV_DEVICE_CONFIG *psDevConfig)
{
	PHYS_HEAP_CONFIG *pasPhysHeaps;

	sunxi_platform_term();
	pasPhysHeaps = psDevConfig->pasPhysHeaps;
	OSFreeMem(pasPhysHeaps);
	OSFreeMem(psDevConfig);
}

PVRSRV_ERROR SysInstallDeviceLISR(IMG_HANDLE hSysData,
								  IMG_UINT32 ui32IRQ,
								  const IMG_CHAR *pszName,
								  PFN_LISR pfnLISR,
								  void *pvData,
								  IMG_HANDLE *phLISRData)
{
	PVR_UNREFERENCED_PARAMETER(hSysData);
	return OSInstallSystemLISR(phLISRData, ui32IRQ, pszName, pfnLISR, pvData,
								SYS_IRQ_FLAG_TRIGGER_DEFAULT);
}

PVRSRV_ERROR SysUninstallDeviceLISR(IMG_HANDLE hLISRData)
{
	return OSUninstallSystemLISR(hLISRData);
}

PVRSRV_ERROR SysDebugInfo(PVRSRV_DEVICE_CONFIG *psDevConfig,
				DUMPDEBUG_PRINTF_FUNC *pfnDumpDebugPrintf,
				void *pvDumpDebugFile)
{
	PVR_UNREFERENCED_PARAMETER(psDevConfig);
	PVR_UNREFERENCED_PARAMETER(pfnDumpDebugPrintf);
	PVR_UNREFERENCED_PARAMETER(pvDumpDebugFile);
	return PVRSRV_OK;
}

/******************************************************************************
 End of file (sysconfig.c)
******************************************************************************/
