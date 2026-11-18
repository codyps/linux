/*************************************************************************/ /*!
@File
@Title          Device Memory Management
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@Description    Server-side component of the Device Memory Management.
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
/* our exported API */
#ifndef DEVMEM_CAPTURE_H
#define DEVMEM_CAPTURE_H

/* Optional capture additions kept out of the stock memory-manager body. */
#if defined(PF_ODYSSEY_CAPTURE)
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/overflow.h>
#define ODYSSEY_CAPTURE_MAX_RECORD (64U * 1024U)
extern bool g_bOdysseyCapture;
static LIST_HEAD(g_sOdysseyMappings);
static DEFINE_MUTEX(g_sOdysseyMappingsLock);
PVRSRV_ERROR DevmemIntDiagAcquirePMR(IMG_DEV_VIRTADDR sDevVAddr,
		IMG_DEVMEM_SIZE_T uiSize, PMR **ppsPMR,
		IMG_DEVMEM_OFFSET_T *puiPMROffset, const IMG_CHAR **ppszReason);
#define DEVMEM_CAPTURE_FIELDS \
	struct list_head sOdysseyNode; \
	IMG_PID uiOdysseyPID;
#define DEVMEM_CAPTURE_MAP \
	INIT_LIST_HEAD(&psMapping->sOdysseyNode); \
	if (g_bOdysseyCapture) \
	{ \
		/* DevmemIntMapPMR has no connection argument in this DDK. Scope the \
		 * diagnostic lookup to the calling client PID and reject ambiguous \
		 * address ranges rather than widening the production map API. */ \
		psMapping->uiOdysseyPID = OSGetCurrentClientProcessIDKM(); \
		mutex_lock(&g_sOdysseyMappingsLock); \
		list_add_tail(&psMapping->sOdysseyNode, &g_sOdysseyMappings); \
		mutex_unlock(&g_sOdysseyMappingsLock); \
	}
#define DEVMEM_CAPTURE_UNMAP \
	if (!list_empty(&psMapping->sOdysseyNode)) \
	{ \
		mutex_lock(&g_sOdysseyMappingsLock); \
		list_del_init(&psMapping->sOdysseyNode); \
		mutex_unlock(&g_sOdysseyMappingsLock); \
	}
#else
#define DEVMEM_CAPTURE_FIELDS
#define DEVMEM_CAPTURE_MAP
#define DEVMEM_CAPTURE_UNMAP
#endif

#endif
