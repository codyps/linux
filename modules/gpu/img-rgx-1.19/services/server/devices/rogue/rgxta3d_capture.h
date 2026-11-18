/*************************************************************************/ /*!
@File
@Title          RGX TA/3D routines
@Copyright      Copyright (c) Imagination Technologies Ltd. All Rights Reserved
@Description    RGX TA/3D routines
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
/* for the offsetof macro */
#ifndef RGXTA3D_CAPTURE_H
#define RGXTA3D_CAPTURE_H

/* Optional capture implementation and hooks for rgxta3d.c. */
#if defined(PF_ODYSSEY_CAPTURE)
#include <crypto/hash.h>
#include <linux/atomic.h>
#include <linux/moduleparam.h>
#include <linux/mutex.h>

#define ODYSSEY_CAPTURE_MAX_RECORD (64U * 1024U)
#define ODYSSEY_CAPTURE_BOOT_BUDGET (1024U * 1024U)
/* The Rogue Services VHeap table occupies one device page. */
#define ODYSSEY_VHEAP_TABLE_BYTES (4U * 1024U)
extern PVRSRV_ERROR DevmemIntDiagAcquirePMR(IMG_DEV_VIRTADDR sDevVAddr,
		IMG_DEVMEM_SIZE_T uiSize, PMR **ppsPMR,
		IMG_DEVMEM_OFFSET_T *puiPMROffset, const IMG_CHAR **ppszReason);
bool g_bOdysseyCapture;
module_param_named(odyssey_capture, g_bOdysseyCapture, bool, 0600);
MODULE_PARM_DESC(odyssey_capture, "Enable bounded ODYSSEY render-state capture");
static atomic64_t g_ui64OdysseyId = ATOMIC64_INIT(0);
static atomic64_t g_ui64OdysseyBytes = ATOMIC64_INIT(0);

/*
 * One render contributes up to RGXMKIF_NUM_RTDATAS records of each capture
 * kind (region, VCE, and geom command), for up to
 * 3 * RGXMKIF_NUM_RTDATAS records.
 */
#define ODYSSEY_CAPTURE_RECORD_CAP (3U * RGXMKIF_NUM_RTDATAS)
#define ODYSSEY_CAPTURE_META_MAX 512U
static DEFINE_MUTEX(g_sOdysseyCaptureLock);
static IMG_CHAR *g_apszOdysseyRecords[ODYSSEY_CAPTURE_RECORD_CAP];
static IMG_UINT32 g_ui32OdysseyRecordCount;
static IMG_UINT32 g_ui32OdysseyDropped;

static void _OdysseyBufferRecord(IMG_CHAR *record)
{
	IMG_CHAR *old = NULL;

	mutex_lock(&g_sOdysseyCaptureLock);
	if (g_ui32OdysseyRecordCount == ODYSSEY_CAPTURE_RECORD_CAP)
	{
		old = g_apszOdysseyRecords[0];
		PVR_DPF((PVR_DBG_WARNING,
			"PF-ODYSSEY: capture ring overflow — evicting oldest record (cap=%u); increase ODYSSEY_CAPTURE_RECORD_CAP",
			(unsigned)ODYSSEY_CAPTURE_RECORD_CAP));
		memmove(&g_apszOdysseyRecords[0], &g_apszOdysseyRecords[1],
			(ODYSSEY_CAPTURE_RECORD_CAP - 1U) * sizeof(record));
		g_ui32OdysseyRecordCount--;
		g_ui32OdysseyDropped++;
	}
	g_apszOdysseyRecords[g_ui32OdysseyRecordCount++] = record;
	mutex_unlock(&g_sOdysseyCaptureLock);
	if (old)
		OSFreeMem(old);
}

static int _OdysseyCaptureDumpSet(const char *val, const struct kernel_param *kp)
{
	unsigned long flags = 0;
	IMG_UINT32 i;

	PVR_UNREFERENCED_PARAMETER(val);
	PVR_UNREFERENCED_PARAMETER(kp);
	mutex_lock(&g_sOdysseyCaptureLock);
	PVRSRVReleasePrintfLock(&flags);
	if (g_ui32OdysseyDropped)
		PVRSRVReleasePrintfLocked("PF-CAPTURE transport-overflow dropped=%u",
			g_ui32OdysseyDropped);
	for (i = 0; i < g_ui32OdysseyRecordCount; i++)
	{
		IMG_CHAR *line = g_apszOdysseyRecords[i];
		IMG_CHAR *next;

		while (line && *line)
		{
			next = strchr(line, '\n');
			if (next)
				*next++ = '\0';
			PVRSRVReleasePrintfLocked("%s", line);
			line = next;
		}
	}
	PVRSRVReleasePrintfUnlock(flags);
	for (i = 0; i < g_ui32OdysseyRecordCount; i++)
	{
		OSFreeMem(g_apszOdysseyRecords[i]);
		g_apszOdysseyRecords[i] = NULL;
	}
	g_ui32OdysseyRecordCount = 0;
	g_ui32OdysseyDropped = 0;
	mutex_unlock(&g_sOdysseyCaptureLock);
	return 0;
}

static const struct kernel_param_ops g_sOdysseyCaptureDumpOps = {
	.set = _OdysseyCaptureDumpSet,
};
module_param_cb(odyssey_capture_dump, &g_sOdysseyCaptureDumpOps, NULL, 0200);
MODULE_PARM_DESC(odyssey_capture_dump, "Write to drain buffered ODYSSEY captures");

static IMG_BOOL _OdysseyBase64Encode(const IMG_UINT8 *src, IMG_UINT32 src_len,
		IMG_CHAR *dst, IMG_UINT32 dst_size, IMG_UINT32 *encoded_len)
{
	static const IMG_CHAR alphabet[] =
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	IMG_UINT32 i, o = 0;
	IMG_UINT32 needed = ((src_len + 2U) / 3U) * 4U;

	if (dst_size <= needed)
		return IMG_FALSE;

	for (i = 0; i < src_len; i += 3U)
	{
		IMG_UINT32 remaining = src_len - i;
		IMG_UINT32 value = (IMG_UINT32)src[i] << 16;

		if (remaining > 1U)
			value |= (IMG_UINT32)src[i + 1U] << 8;
		if (remaining > 2U)
			value |= src[i + 2U];
		dst[o++] = alphabet[(value >> 18) & 0x3fU];
		dst[o++] = alphabet[(value >> 12) & 0x3fU];
		dst[o++] = remaining > 1U ? alphabet[(value >> 6) & 0x3fU] : '=';
		dst[o++] = remaining > 2U ? alphabet[value & 0x3fU] : '=';
	}
	dst[o] = '\0';
	*encoded_len = o;
	return IMG_TRUE;
}

static void _OdysseyError(IMG_UINT64 id, IMG_UINT32 rt, IMG_DEV_VIRTADDR va,
		IMG_UINT32 bytes, IMG_UINT32 t1, IMG_UINT32 t2, IMG_UINT32 screen,
		IMG_UINT32 isp, const IMG_CHAR *reason)
{
	IMG_CHAR *record = OSAllocMem(ODYSSEY_CAPTURE_META_MAX);

	if (!record)
	{
		mutex_lock(&g_sOdysseyCaptureLock);
		g_ui32OdysseyDropped++;
		mutex_unlock(&g_sOdysseyCaptureLock);
		return;
	}
	OSSNPrintf(record, ODYSSEY_CAPTURE_META_MAX,
		"<<<PF-CAPTURE v=1 kind=odyssey-rgn id=%llu rt=%u devva=0x%llx bytes=%u te_mtile1=0x%x te_mtile2=0x%x te_screen=0x%x isp_mtile_size=0x%x status=error reason=%s>>>\n"
		"<<<PF-CAPTURE-END id=%llu sha256=e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855>>>",
		(unsigned long long)id, rt, (unsigned long long)va.uiAddr, bytes,
		t1, t2, screen, isp, reason, (unsigned long long)id);
	_OdysseyBufferRecord(record);
}

static void _OdysseyDump(PMR *pmr, IMG_DEVMEM_OFFSET_T off, IMG_UINT64 id,
		IMG_UINT32 rt, IMG_DEV_VIRTADDR va, IMG_UINT32 bytes,
		IMG_UINT32 t1, IMG_UINT32 t2, IMG_UINT32 screen, IMG_UINT32 isp)
{
	PVRSRV_ERROR err;
	void *addr, *snapshot;
	size_t mapped;
	IMG_HANDLE priv;
	struct crypto_shash *tfm;
	struct shash_desc *desc;
	IMG_UINT8 digest[32];
	IMG_CHAR hex[65], *b64, *record, *cursor;
	IMG_UINT32 i, encoded;

	err = PMRAcquireKernelMappingData(pmr, off, bytes, &addr, &mapped, &priv);
	if (err != PVRSRV_OK || mapped < bytes) { _OdysseyError(id, rt, va, bytes, t1, t2, screen, isp, "non-kernel-mappable"); return; }
	snapshot = OSAllocMem(bytes);
	if (!snapshot) { PVR_DPF((PVR_DBG_WARNING, "PF-ODYSSEY: skip record id=%llu rt=%u (snapshot alloc failed)", (unsigned long long)id, rt)); PMRReleaseKernelMappingData(pmr, priv); return; }
	OSDeviceMemCopy(snapshot, addr, bytes);
	tfm = crypto_alloc_shash("sha256", 0, 0);
	if (IS_ERR(tfm)) { OSFreeMem(snapshot); PMRReleaseKernelMappingData(pmr, priv); _OdysseyError(id, rt, va, bytes, t1, t2, screen, isp, "sha256-unavailable"); return; }
	desc = OSAllocMem(sizeof(*desc) + crypto_shash_descsize(tfm));
	encoded = ((bytes + 2U) / 3U) * 4U;
	b64 = OSAllocMem(encoded + 1U);
	if (!desc || !b64) { if (desc) OSFreeMem(desc); if (b64) OSFreeMem(b64); crypto_free_shash(tfm); OSFreeMem(snapshot); PMRReleaseKernelMappingData(pmr, priv); _OdysseyError(id, rt, va, bytes, t1, t2, screen, isp, "out-of-memory"); return; }
	desc->tfm = tfm;
	err = crypto_shash_digest(desc, snapshot, bytes, digest) ? PVRSRV_ERROR_INVALID_PARAMS : PVRSRV_OK;
	if (!_OdysseyBase64Encode(snapshot, bytes, b64, encoded + 1U, &encoded))
		err = PVRSRV_ERROR_INVALID_PARAMS;
	PMRReleaseKernelMappingData(pmr, priv);
	OSFreeMem(snapshot);
	if (err != PVRSRV_OK) { OSFreeMem(b64); OSFreeMem(desc); crypto_free_shash(tfm); _OdysseyError(id, rt, va, bytes, t1, t2, screen, isp, "sha256-failed"); return; }
	for (i = 0; i < 32; i++) OSSNPrintf(&hex[i * 2], 3, "%02x", digest[i]);
	record = OSAllocMem(encoded + (encoded / 76U) + ODYSSEY_CAPTURE_META_MAX);
	if (!record) { OSFreeMem(b64); OSFreeMem(desc); crypto_free_shash(tfm); _OdysseyError(id, rt, va, bytes, t1, t2, screen, isp, "out-of-memory"); return; }
	cursor = record;
	cursor += OSSNPrintf(cursor, ODYSSEY_CAPTURE_META_MAX,
		"<<<PF-CAPTURE v=1 kind=odyssey-rgn id=%llu rt=%u devva=0x%llx bytes=%u te_mtile1=0x%x te_mtile2=0x%x te_screen=0x%x isp_mtile_size=0x%x>>>\n",
		(unsigned long long)id, rt, (unsigned long long)va.uiAddr, bytes, t1, t2, screen, isp);
	for (i = 0; i < encoded; i += 76)
		cursor += OSSNPrintf(cursor, 78, "%.*s\n", (int)MIN(76U, encoded - i), b64 + i);
	OSSNPrintf(cursor, 128, "<<<PF-CAPTURE-END id=%llu sha256=%s>>>",
		(unsigned long long)id, hex);
	_OdysseyBufferRecord(record);
	OSFreeMem(b64); OSFreeMem(desc); crypto_free_shash(tfm);
}

static void _OdysseyDumpGeomCmd(IMG_UINT64 ui64Id, const IMG_BYTE *pui8Cmd,
		IMG_UINT32 ui32Bytes)
{
	struct crypto_shash *tfm;
	struct shash_desc *desc;
	IMG_UINT8 digest[32];
	IMG_CHAR hex[65], *b64, *record, *cursor;
	IMG_UINT32 i, encoded;

	tfm = crypto_alloc_shash("sha256", 0, 0);
	if (IS_ERR(tfm))
	{
		PVR_DPF((PVR_DBG_WARNING,
			"PF-ODYSSEY: skip geom-cmd record id=%llu (sha256 unavailable)",
			(unsigned long long)ui64Id));
		return;
	}
	desc = OSAllocMem(sizeof(*desc) + crypto_shash_descsize(tfm));
	encoded = ((ui32Bytes + 2U) / 3U) * 4U;
	b64 = OSAllocMem(encoded + 1U);
	if (!desc || !b64)
	{
		if (desc) OSFreeMem(desc);
		if (b64) OSFreeMem(b64);
		crypto_free_shash(tfm);
		PVR_DPF((PVR_DBG_WARNING,
			"PF-ODYSSEY: skip geom-cmd record id=%llu (out of memory)",
			(unsigned long long)ui64Id));
		return;
	}
	desc->tfm = tfm;
	if (crypto_shash_digest(desc, pui8Cmd, ui32Bytes, digest) ||
		!_OdysseyBase64Encode(pui8Cmd, ui32Bytes, b64, encoded + 1U, &encoded))
	{
		OSFreeMem(b64);
		OSFreeMem(desc);
		crypto_free_shash(tfm);
		PVR_DPF((PVR_DBG_WARNING,
			"PF-ODYSSEY: skip geom-cmd record id=%llu (encoding failed)",
			(unsigned long long)ui64Id));
		return;
	}
	for (i = 0; i < 32; i++) OSSNPrintf(&hex[i * 2], 3, "%02x", digest[i]);
	record = OSAllocMem(encoded + (encoded / 76U) + ODYSSEY_CAPTURE_META_MAX);
	if (!record)
	{
		OSFreeMem(b64);
		OSFreeMem(desc);
		crypto_free_shash(tfm);
		PVR_DPF((PVR_DBG_WARNING,
			"PF-ODYSSEY: skip geom-cmd record id=%llu (out of memory)",
			(unsigned long long)ui64Id));
		return;
	}
	cursor = record;
	cursor += OSSNPrintf(cursor, ODYSSEY_CAPTURE_META_MAX,
		"<<<PF-CAPTURE v=1 kind=odyssey-geomcmd id=%llu bytes=%u>>>\n",
		(unsigned long long)ui64Id, ui32Bytes);
	for (i = 0; i < encoded; i += 76)
		cursor += OSSNPrintf(cursor, 78, "%.*s\n",
			(int)MIN(76U, encoded - i), b64 + i);
	OSSNPrintf(cursor, 128, "<<<PF-CAPTURE-END id=%llu sha256=%s>>>",
		(unsigned long long)ui64Id, hex);
	_OdysseyBufferRecord(record);
	OSFreeMem(b64);
	OSFreeMem(desc);
	crypto_free_shash(tfm);
}

static void _OdysseyResolveDump(IMG_UINT64 id, IMG_UINT32 rt, IMG_DEV_VIRTADDR va,
		IMG_UINT32 bytes, IMG_UINT32 t1, IMG_UINT32 t2, IMG_UINT32 screen, IMG_UINT32 isp)
{
	PMR *pmr; IMG_DEVMEM_OFFSET_T off; const IMG_CHAR *reason;
	if (bytes == 0 || bytes > ODYSSEY_CAPTURE_MAX_RECORD) { _OdysseyError(id, rt, va, bytes, t1, t2, screen, isp, "length-out-of-range"); return; }
	if (DevmemIntDiagAcquirePMR(va, bytes, &pmr, &off, &reason) != PVRSRV_OK) { _OdysseyError(id, rt, va, bytes, t1, t2, screen, isp, reason); return; }
	if (atomic64_add_return(bytes, &g_ui64OdysseyBytes) > ODYSSEY_CAPTURE_BOOT_BUDGET) { atomic64_sub(bytes, &g_ui64OdysseyBytes); PMRUnrefPMR(pmr); _OdysseyError(id, rt, va, bytes, t1, t2, screen, isp, "boot-budget-exhausted"); return; }
	_OdysseyDump(pmr, off, id, rt, va, bytes, t1, t2, screen, isp); PMRUnrefPMR(pmr);
}

static void _OdysseyDumpVCE(PVRSRV_RGXDEV_INFO *psDevInfo,
		IMG_DEV_VIRTADDR sVHeapDevVAddr, IMG_UINT64 ui64Id,
		IMG_UINT32 ui32RT)
{
	IMG_UINT32 aui32UVS[6], ui32VCE, aui32First[8] = { 0 };
	IMG_UINT64 ui64CatBaseVCE0, ui64CatBaseVCE1;
	IMG_UINT32 ui32NonZero = 0, i;
	IMG_DEVMEM_OFFSET_T uiOffset;
	const IMG_CHAR *pszReason;
	PVRSRV_ERROR eError;
	IMG_HANDLE hPriv;
	PMR *psPMR;
	void *pvAddr, *pvSnapshot;
	size_t uiMapped;

	aui32UVS[0] = OSReadHWReg32(psDevInfo->pvRegsBaseKM, RGX_CR_USC_UVS0_CHECKSUM);
	aui32UVS[1] = OSReadHWReg32(psDevInfo->pvRegsBaseKM, RGX_CR_USC_UVS1_CHECKSUM);
	aui32UVS[2] = OSReadHWReg32(psDevInfo->pvRegsBaseKM, RGX_CR_USC_UVS2_CHECKSUM);
	aui32UVS[3] = OSReadHWReg32(psDevInfo->pvRegsBaseKM, RGX_CR_USC_UVS3_CHECKSUM);
	aui32UVS[4] = OSReadHWReg32(psDevInfo->pvRegsBaseKM, RGX_CR_USC_UVS4_CHECKSUM);
	aui32UVS[5] = OSReadHWReg32(psDevInfo->pvRegsBaseKM, RGX_CR_USC_UVS5_CHECKSUM);
	ui32VCE = OSReadHWReg32(psDevInfo->pvRegsBaseKM, RGX_CR_VCE_CHECKSUM);
	ui64CatBaseVCE0 = OSReadHWReg64(psDevInfo->pvRegsBaseKM, RGX_CR_BIF_PM_CAT_BASE_VCE0);
	ui64CatBaseVCE1 = OSReadHWReg64(psDevInfo->pvRegsBaseKM, RGX_CR_BIF_PM_CAT_BASE_VCE1);

	eError = DevmemIntDiagAcquirePMR(sVHeapDevVAddr,
		ODYSSEY_VHEAP_TABLE_BYTES, &psPMR, &uiOffset, &pszReason);
	if (eError != PVRSRV_OK)
	{
		PVR_LOG(("<<<PF-CAPTURE-VCE v=1 id=%llu rt=%u vheap_devva=0x%llx vheap_bytes=%u status=error reason=%s uvs0=0x%08x uvs1=0x%08x uvs2=0x%08x uvs3=0x%08x uvs4=0x%08x uvs5=0x%08x vce=0x%08x cat_base_vce0=0x%016llx cat_base_vce1=0x%016llx>>>",
			(unsigned long long)ui64Id, ui32RT,
			(unsigned long long)sVHeapDevVAddr.uiAddr,
			ODYSSEY_VHEAP_TABLE_BYTES, pszReason,
			aui32UVS[0], aui32UVS[1], aui32UVS[2], aui32UVS[3],
			aui32UVS[4], aui32UVS[5], ui32VCE,
			(unsigned long long)ui64CatBaseVCE0,
			(unsigned long long)ui64CatBaseVCE1));
		return;
	}

	eError = PMRAcquireKernelMappingData(psPMR, uiOffset,
		ODYSSEY_VHEAP_TABLE_BYTES, &pvAddr, &uiMapped, &hPriv);
	if (eError != PVRSRV_OK || uiMapped < ODYSSEY_VHEAP_TABLE_BYTES)
	{
		if (eError == PVRSRV_OK)
			PMRReleaseKernelMappingData(psPMR, hPriv);
		PMRUnrefPMR(psPMR);
		PVR_LOG(("<<<PF-CAPTURE-VCE v=1 id=%llu rt=%u vheap_devva=0x%llx vheap_bytes=%u status=error reason=non-kernel-mappable uvs0=0x%08x uvs1=0x%08x uvs2=0x%08x uvs3=0x%08x uvs4=0x%08x uvs5=0x%08x vce=0x%08x cat_base_vce0=0x%016llx cat_base_vce1=0x%016llx>>>",
			(unsigned long long)ui64Id, ui32RT,
			(unsigned long long)sVHeapDevVAddr.uiAddr,
			ODYSSEY_VHEAP_TABLE_BYTES,
			aui32UVS[0], aui32UVS[1], aui32UVS[2], aui32UVS[3],
			aui32UVS[4], aui32UVS[5], ui32VCE,
			(unsigned long long)ui64CatBaseVCE0,
			(unsigned long long)ui64CatBaseVCE1));
		return;
	}

	pvSnapshot = OSAllocMem(ODYSSEY_VHEAP_TABLE_BYTES);
	if (!pvSnapshot)
	{
		PMRReleaseKernelMappingData(psPMR, hPriv);
		PMRUnrefPMR(psPMR);
		PVR_LOG(("<<<PF-CAPTURE-VCE v=1 id=%llu rt=%u vheap_devva=0x%llx vheap_bytes=%u status=error reason=out-of-memory uvs0=0x%08x uvs1=0x%08x uvs2=0x%08x uvs3=0x%08x uvs4=0x%08x uvs5=0x%08x vce=0x%08x cat_base_vce0=0x%016llx cat_base_vce1=0x%016llx>>>",
			(unsigned long long)ui64Id, ui32RT,
			(unsigned long long)sVHeapDevVAddr.uiAddr,
			ODYSSEY_VHEAP_TABLE_BYTES,
			aui32UVS[0], aui32UVS[1], aui32UVS[2], aui32UVS[3],
			aui32UVS[4], aui32UVS[5], ui32VCE,
			(unsigned long long)ui64CatBaseVCE0,
			(unsigned long long)ui64CatBaseVCE1));
		return;
	}
	OSDeviceMemCopy(pvSnapshot, pvAddr, ODYSSEY_VHEAP_TABLE_BYTES);
	PMRReleaseKernelMappingData(psPMR, hPriv);
	PMRUnrefPMR(psPMR);

	for (i = 0; i < ODYSSEY_VHEAP_TABLE_BYTES / sizeof(IMG_UINT32); i++)
	{
		IMG_UINT32 ui32Word = ((IMG_UINT32 *)pvSnapshot)[i];
		if (ui32Word != 0)
			ui32NonZero++;
		if (i < ARRAY_SIZE(aui32First))
			aui32First[i] = ui32Word;
	}
	OSFreeMem(pvSnapshot);

	PVR_LOG(("<<<PF-CAPTURE-VCE v=1 id=%llu rt=%u vheap_devva=0x%llx vheap_bytes=%u vheap_nonzero_dwords=%u vheap_first8=%08x,%08x,%08x,%08x,%08x,%08x,%08x,%08x uvs0=0x%08x uvs1=0x%08x uvs2=0x%08x uvs3=0x%08x uvs4=0x%08x uvs5=0x%08x vce=0x%08x cat_base_vce0=0x%016llx cat_base_vce1=0x%016llx>>>",
		(unsigned long long)ui64Id, ui32RT,
		(unsigned long long)sVHeapDevVAddr.uiAddr,
		ODYSSEY_VHEAP_TABLE_BYTES, ui32NonZero,
		aui32First[0], aui32First[1], aui32First[2], aui32First[3],
		aui32First[4], aui32First[5], aui32First[6], aui32First[7],
		aui32UVS[0], aui32UVS[1], aui32UVS[2], aui32UVS[3],
		aui32UVS[4], aui32UVS[5], ui32VCE,
		(unsigned long long)ui64CatBaseVCE0,
		(unsigned long long)ui64CatBaseVCE1));
}

void RGXOdysseyCaptureCreate(CONNECTION_DATA *c, const IMG_DEV_VIRTADDR *vas,
		IMG_UINT32 bytes, IMG_UINT32 t1, IMG_UINT32 t2, IMG_UINT32 screen,
		IMG_UINT32 isp, IMG_UINT64 *id)
{
	IMG_UINT32 i; PVR_UNREFERENCED_PARAMETER(c); *id = 0;
	if (!g_bOdysseyCapture) return;
	*id = atomic64_inc_return(&g_ui64OdysseyId);
	for (i = 0; i < RGXMKIF_NUM_RTDATAS; i++) _OdysseyResolveDump(*id, i, vas[i], bytes, t1, t2, screen, isp);
}

void RGXOdysseyCaptureRetain(RGX_KM_HW_RT_DATASET **sets, const IMG_DEV_VIRTADDR *vas,
		IMG_UINT32 bytes, IMG_UINT32 t1, IMG_UINT32 t2, IMG_UINT32 screen,
		IMG_UINT32 isp, IMG_UINT64 id)
{
	IMG_UINT32 i; if (!id || bytes == 0 || bytes > ODYSSEY_CAPTURE_MAX_RECORD) return;
	for (i = 0; i < RGXMKIF_NUM_RTDATAS; i++) { PMR *p; IMG_DEVMEM_OFFSET_T o; const IMG_CHAR *r; RGX_KM_HW_RT_DATASET *s = sets[i];
		s->ui64OdysseyCaptureId=id; s->ui32OdysseyRT=i; s->sOdysseyDevVAddr=vas[i]; s->ui32OdysseyRgnHeaderSize=bytes; s->ui32OdysseyTEMTILE1=t1; s->ui32OdysseyTEMTILE2=t2; s->ui32OdysseyTEScreen=screen; s->ui32OdysseyISPMtileSize=isp;
		if (DevmemIntDiagAcquirePMR(vas[i], bytes, &p, &o, &r) == PVRSRV_OK) { s->psOdysseyPMR=p; s->uiOdysseyPMROffset=o; }
	}
}

static void OdysseyCaptureReleaseDataSet(RGX_KM_HW_RT_DATASET *psKMHWRTDataSet)
{
	if (psKMHWRTDataSet->psOdysseyPMR)
	{
		PMRUnrefPMR(psKMHWRTDataSet->psOdysseyPMR);
		psKMHWRTDataSet->psOdysseyPMR = NULL;
	}
}

static void OdysseyCaptureFirstKick(RGX_KM_HW_RT_DATASET *psKMHWRTDataSet,
        PVRSRV_DEVICE_NODE * const *ppsDeviceNode, const IMG_BYTE *pui8TADMCmd,
        IMG_UINT32 ui32TACmdSize)
{
	if (g_bOdysseyCapture && psKMHWRTDataSet &&
		!psKMHWRTDataSet->bOdysseyFirstKickCaptured &&
		psKMHWRTDataSet->ui64OdysseyCaptureId)
	{
		IMG_UINT64 ui64KickId = atomic64_inc_return(&g_ui64OdysseyId);

		psKMHWRTDataSet->bOdysseyFirstKickCaptured = IMG_TRUE;
		if (psKMHWRTDataSet->psOdysseyPMR)
		{
			IMG_UINT32 ui32Bytes = psKMHWRTDataSet->ui32OdysseyRgnHeaderSize;
			if (ui32Bytes == 0 || ui32Bytes > ODYSSEY_CAPTURE_MAX_RECORD)
			{
				_OdysseyError(ui64KickId, psKMHWRTDataSet->ui32OdysseyRT,
					psKMHWRTDataSet->sOdysseyDevVAddr, ui32Bytes,
					psKMHWRTDataSet->ui32OdysseyTEMTILE1,
					psKMHWRTDataSet->ui32OdysseyTEMTILE2,
					psKMHWRTDataSet->ui32OdysseyTEScreen,
					psKMHWRTDataSet->ui32OdysseyISPMtileSize,
					"length-out-of-range");
			}
			else if (atomic64_add_return(ui32Bytes, &g_ui64OdysseyBytes) <= ODYSSEY_CAPTURE_BOOT_BUDGET)
				_OdysseyDump(psKMHWRTDataSet->psOdysseyPMR,
				psKMHWRTDataSet->uiOdysseyPMROffset, ui64KickId,
				psKMHWRTDataSet->ui32OdysseyRT,
				psKMHWRTDataSet->sOdysseyDevVAddr,
				psKMHWRTDataSet->ui32OdysseyRgnHeaderSize,
				psKMHWRTDataSet->ui32OdysseyTEMTILE1,
				psKMHWRTDataSet->ui32OdysseyTEMTILE2,
				psKMHWRTDataSet->ui32OdysseyTEScreen,
				psKMHWRTDataSet->ui32OdysseyISPMtileSize);
			else
			{
				atomic64_sub(ui32Bytes, &g_ui64OdysseyBytes);
				_OdysseyError(ui64KickId, psKMHWRTDataSet->ui32OdysseyRT,
					psKMHWRTDataSet->sOdysseyDevVAddr, ui32Bytes,
					psKMHWRTDataSet->ui32OdysseyTEMTILE1,
					psKMHWRTDataSet->ui32OdysseyTEMTILE2,
					psKMHWRTDataSet->ui32OdysseyTEScreen,
					psKMHWRTDataSet->ui32OdysseyISPMtileSize,
					"boot-budget-exhausted");
			}
		}
		_OdysseyDumpVCE((*ppsDeviceNode)->pvDevice,
			psKMHWRTDataSet->sOdysseyVHeapDevVAddr, ui64KickId,
			psKMHWRTDataSet->ui32OdysseyRT);
		_OdysseyDumpGeomCmd(ui64KickId, pui8TADMCmd, ui32TACmdSize);
	}
}
#define ODYSSEY_DATASET_ADDRESS(dataset, address) ((dataset)->sOdysseyVHeapDevVAddr = (address));
#define ODYSSEY_DATASET_RELEASE(dataset) OdysseyCaptureReleaseDataSet(dataset);
#define ODYSSEY_FIRST_KICK(dataset, context, command, size) OdysseyCaptureFirstKick(dataset, &(context)->psDeviceNode, command, size);
#else
#define ODYSSEY_DATASET_ADDRESS(dataset, address)
#define ODYSSEY_DATASET_RELEASE(dataset)
#define ODYSSEY_FIRST_KICK(dataset, context, command, size)
#endif

#endif
