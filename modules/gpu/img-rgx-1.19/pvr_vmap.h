/*************************************************************************/ /*!
@File           pvr_vmap.h
@Title          PowerVR vmap compatibility header
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@License        Dual MIT/GPLv2

SPDX-License-Identifier: (MIT OR GPL-2.0)
*/ /**************************************************************************/

#ifndef __PVR_VMAP_H__
#define __PVR_VMAP_H__

#include <linux/version.h>
#include <linux/vmalloc.h>

/* TG3040 uses the Linux 4.9 vm_map_ram/vm_unmap_ram mapping pair. */
static inline void *pvr_vmap(struct page **pages, unsigned int count,
			     unsigned long flags, pgprot_t prot)
{
	(void)flags;
	return vm_map_ram(pages, count, NUMA_NO_NODE, prot);
}

static inline void pvr_vunmap(const void *addr, unsigned int count,
			      pgprot_t prot)
{
	(void)prot;
	vm_unmap_ram(addr, count);
}

#endif /* __PVR_VMAP_H__ */
