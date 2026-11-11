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

#ifndef MRVL_GCU_COPY_H_
#define MRVL_GCU_COPY_H_

#include <stddef.h>
#include <stdint.h>

namespace android {

// OMX parameter index (vendor range) of ACodec's request to OMXNodeInstance to copy the decoder
// output with the 2D engine. Handled by OMXNodeInstance, never sent to the component.
enum { kMrvlIndexGpuCopyOutput = 0x7F4D4701 };

/*
 * Copies memory with the Vivante 2D engine (libgcu, loaded at run time) on PXA1908.
 *
 * Marvell HW video decoder output buffers are uncached: the CPU reads them at ~70 MB/s (~45 ms per
 * 1080p frame), the 2D engine at full speed.
 *
 * Cache rules (galcore, arm64 3.14): mapping user memory for the GPU only *cleans* the CPU cache,
 * nothing invalidates it afterwards. So:
 *  - the GPU may READ any memory (the CPU's dirty lines are written back first);
 *  - the GPU may WRITE only memory the CPU does not read afterwards (gralloc buffers, or shared
 *    memory whose reader copies it with the GPU again), otherwise the CPU can see stale lines.
 *
 * A buffer is passed by physical address when /proc/self/pagemap proves it contiguous, otherwise by
 * virtual address and the GPU driver maps its pages. On any failure the caller copies with the CPU.
 * Disable with persist.mrvl.video.gcu_copy.off=1.
 */
class MrvlGcuCopy {
public:
    MrvlGcuCopy();
    ~MrvlGcuCopy();

    // libgcu loads and a 2D context can be created in this process (result cached).
    static bool available();

    // Queue a copy of `rows` rows of `rowBytes` bytes. Returns false (nothing queued) when the 2D
    // path can't be used for these buffers.
    bool copyPlane(const void *src, size_t srcStride, void *dst, size_t dstStride,
            size_t rowBytes, size_t rows);

    // Copy `len` bytes: whole 4 KB rows with the 2D engine, the tail with the CPU. Waits for the
    // copy. Returns false (nothing copied) when the 2D path can't be used.
    bool copyLinear(const void *src, void *dst, size_t len);

    // Wait for queued copies to complete.
    void finish();

private:
    bool init();
    static bool physContiguous(const void *addr, size_t len, uint32_t *phys);
    static void *createSurface(void *context, const void *addr, size_t len, size_t stride,
            size_t rows);

    enum { kMaxSurfaces = 8 };

    void *mContext;
    bool mInitTried;
    bool mPending;
    void *mSurfaces[kMaxSurfaces];
    int mNumSurfaces;
};

}  // namespace android

#endif  // MRVL_GCU_COPY_H_
