/*
 * Copyright (C) 2026 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "MrvlGcuCopy"
#include <log/log.h>

#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <cutils/properties.h>
#include <gcu.h>

#include "MrvlGcuCopy.h"

namespace android {

static const size_t kPageSize = 4096;
static const uint64_t kPfnMask = (1ULL << 55) - 1;
static const uint64_t kPagePresent = 1ULL << 63;
// GCU pre-allocated surfaces want 64-byte aligned addresses.
static const uint32_t kAlign = 64;
// Largest span checked at once (a 1080p NV12 frame is ~3 MB).
static const size_t kMaxPages = 1024;
// Row size of linear copies (1024 ARGB pixels).
static const size_t kLinearRow = 4096;

// libgcu entry points, resolved at run time so the framework has no build dependency on the blob.
static struct {
    decltype(&gcuInitialize) initialize;
    decltype(&gcuCreateContext) createContext;
    decltype(&gcuDestroyContext) destroyContext;
    decltype(&gcuDestroySurface) destroySurface;
    decltype(&gcuBlit) blit;
    decltype(&gcuFinish) finish;
    decltype(&gcuGetError) getError;
    decltype(&_gcuCreatePreAllocBuffer) createPreAllocBuffer;
    bool ok;
} sGcu;
static pthread_once_t sGcuOnce = PTHREAD_ONCE_INIT;

static void loadGcu() {
    if (property_get_bool("persist.mrvl.video.gcu_copy.off", false)) {
        ALOGI("2D copy disabled by property");
        return;
    }
    void *h = dlopen("libgcu.so", RTLD_NOW | RTLD_LOCAL);
    if (h == NULL) {
        ALOGW("libgcu.so: %s", dlerror());
        return;
    }
#define SYM(field, name) \
    sGcu.field = reinterpret_cast<decltype(sGcu.field)>(dlsym(h, name)); \
    if (sGcu.field == NULL) { ALOGW("libgcu.so: no %s", name); return; }
    SYM(initialize, "gcuInitialize");
    SYM(createContext, "gcuCreateContext");
    SYM(destroyContext, "gcuDestroyContext");
    SYM(destroySurface, "gcuDestroySurface");
    SYM(blit, "gcuBlit");
    SYM(finish, "gcuFinish");
    SYM(getError, "gcuGetError");
    SYM(createPreAllocBuffer, "_gcuCreatePreAllocBuffer");
#undef SYM
    GCU_INIT_DATA initData;
    memset(&initData, 0, sizeof(initData));
    if (!sGcu.initialize(&initData)) {
        ALOGW("gcuInitialize failed");
        return;
    }
    sGcu.ok = true;
}

static bool gcuLoaded() {
    pthread_once(&sGcuOnce, loadGcu);
    return sGcu.ok;
}

bool MrvlGcuCopy::available() {
    static int sAvailable = -1;  // probed once per process
    if (sAvailable < 0) {
        MrvlGcuCopy probe;
        sAvailable = probe.init() ? 1 : 0;
    }
    return sAvailable == 1;
}

MrvlGcuCopy::MrvlGcuCopy()
    : mContext(NULL), mInitTried(false), mPending(false), mNumSurfaces(0) {
}

MrvlGcuCopy::~MrvlGcuCopy() {
    if (mContext != NULL) {
        finish();
        sGcu.destroyContext((GCUContext)mContext);
    }
}

bool MrvlGcuCopy::init() {
    if (mInitTried) {
        return mContext != NULL;
    }
    mInitTried = true;
    if (!gcuLoaded()) {
        return false;
    }
    GCU_CONTEXT_DATA contextData;
    memset(&contextData, 0, sizeof(contextData));
    mContext = sGcu.createContext(&contextData);
    if (mContext == NULL) {
        ALOGW("gcuCreateContext failed: %d", sGcu.getError());
        return false;
    }
    ALOGI("2D copy ready");
    return true;
}

bool MrvlGcuCopy::physContiguous(const void *addr, size_t len, uint32_t *phys) {
    uintptr_t va = (uintptr_t)addr;
    uintptr_t first = va / kPageSize;
    uintptr_t last = (va + len - 1) / kPageSize;
    size_t pages = last - first + 1;
    if (len == 0 || pages > kMaxPages) {
        return false;
    }
    // Buffer mappings are populated on first access: touch one byte per page so every page
    // table entry exists before reading pagemap.
    for (size_t i = 0; i < pages; i++) {
        uintptr_t p = (first + i) * kPageSize;
        if (p < va) {
            p = va;
        }
        (void)*(volatile const uint8_t *)p;
    }
    int fd = open("/proc/self/pagemap", O_RDONLY | O_CLOEXEC);
    if (fd < 0) {
        return false;
    }
    uint64_t *entries = (uint64_t *)malloc(pages * sizeof(uint64_t));
    bool ok = entries != NULL;
    if (ok) {
        ssize_t want = (ssize_t)(pages * sizeof(uint64_t));
        ok = pread(fd, entries, want, (off_t)(first * sizeof(uint64_t))) == want;
    }
    close(fd);
    uint64_t pfn0 = 0;
    for (size_t i = 0; ok && i < pages; i++) {
        if (!(entries[i] & kPagePresent)) {
            ok = false;
        } else if (i == 0) {
            pfn0 = entries[0] & kPfnMask;
            ok = pfn0 != 0;
        } else {
            ok = (entries[i] & kPfnMask) == pfn0 + i;
        }
    }
    free(entries);
    if (!ok) {
        return false;
    }
    uint64_t pa = pfn0 * kPageSize + (va % kPageSize);
    if (pa + len > 0xffffffffULL) {
        return false;
    }
    *phys = (uint32_t)pa;
    return true;
}

void *MrvlGcuCopy::createSurface(void *context, const void *addr, size_t len, size_t stride,
        size_t rows) {
    GCUContext ctx = (GCUContext)context;
    uint32_t phys;
    GCUSurface surf;
    bool physical = physContiguous(addr, len, &phys) && phys % kAlign == 0;
    if (physical) {
        // The virtual address is unused when a physical address is given (as in GcuEngine).
        surf = sGcu.createPreAllocBuffer(ctx, stride / 4, rows, GCU_FORMAT_ARGB8888,
                GCU_TRUE, (void *)0x1000, GCU_TRUE, phys);
    } else {
        surf = sGcu.createPreAllocBuffer(ctx, stride / 4, rows, GCU_FORMAT_ARGB8888,
                GCU_TRUE, (void *)addr, GCU_FALSE, 0);
    }
    static int sLogged = 0;
    if (sLogged < 4) {
        sLogged++;
        ALOGI("surface %p+%zu: %s -> %p", addr, len, physical ? "physical" : "virtual", surf);
    }
    return surf;
}

bool MrvlGcuCopy::copyPlane(const void *src, size_t srcStride, void *dst, size_t dstStride,
        size_t rowBytes, size_t rows) {
    // Copied as 32-bit pixels: every size must be a multiple of 4 bytes.
    if (rows == 0 || rowBytes == 0 || (rowBytes | srcStride | dstStride) % 4 != 0
            || rowBytes > srcStride || rowBytes > dstStride || !init()) {
        return false;
    }
    if ((uintptr_t)src % kAlign != 0 || (uintptr_t)dst % kAlign != 0) {
        return false;
    }
    size_t srcLen = srcStride * (rows - 1) + rowBytes;
    size_t dstLen = dstStride * (rows - 1) + rowBytes;

    GCUContext ctx = (GCUContext)mContext;
    GCUSurface s = createSurface(ctx, src, srcLen, srcStride, rows);
    GCUSurface d = s != NULL ? createSurface(ctx, dst, dstLen, dstStride, rows) : NULL;
    if (s == NULL || d == NULL) {
        if (s != NULL) sGcu.destroySurface(ctx, s);
        if (d != NULL) sGcu.destroySurface(ctx, d);
        ALOGW("surface creation failed: %d", sGcu.getError());
        return false;
    }
    GCU_RECT rect;
    rect.left = 0;
    rect.top = 0;
    rect.right = (GCUint)(rowBytes / 4);
    rect.bottom = (GCUint)rows;
    GCU_BLT_DATA blt;
    memset(&blt, 0, sizeof(blt));
    blt.pSrcSurface = s;
    blt.pDstSurface = d;
    blt.pSrcRect = &rect;
    blt.pDstRect = &rect;
    blt.rotation = GCU_ROTATION_0;
    sGcu.blit(ctx, &blt);
    GCUenum err = sGcu.getError();
    // Surfaces stay alive until the blit has completed (finish()).
    mSurfaces[mNumSurfaces++] = s;
    mSurfaces[mNumSurfaces++] = d;
    mPending = true;
    if (err != GCU_NO_ERROR) {
        ALOGW("gcuBlit failed: %d", err);
        finish();
        return false;
    }
    if (mNumSurfaces + 2 > kMaxSurfaces) {
        finish();
    }
    return true;
}

bool MrvlGcuCopy::copyLinear(const void *src, void *dst, size_t len) {
    size_t rows = len / kLinearRow;
    if (rows == 0) {
        return false;
    }
    bool ok = copyPlane(src, kLinearRow, dst, kLinearRow, kLinearRow, rows);
    finish();
    if (!ok) {
        return false;
    }
    size_t done = rows * kLinearRow;
    if (done < len) {
        memcpy((uint8_t *)dst + done, (const uint8_t *)src + done, len - done);
    }
    return true;
}

void MrvlGcuCopy::finish() {
    if (mContext == NULL) {
        return;
    }
    if (mPending) {
        sGcu.finish((GCUContext)mContext);
        mPending = false;
    }
    for (int i = 0; i < mNumSurfaces; i++) {
        sGcu.destroySurface((GCUContext)mContext, (GCUSurface)mSurfaces[i]);
    }
    mNumSurfaces = 0;
}

}  // namespace android
