/*
 * libui shims for Lollipop blobs on Pie.
 */
#include <new>
#include <string>
#include <sys/types.h>
#include <unistd.h>

#include <ui/GraphicBuffer.h>
#include <ui/GraphicBufferMapper.h>

/* GraphicBufferMapper::lock(handle, int usage, Rect, void**) (camera.stock) */
extern "C" android::status_t _ZN7android19GraphicBufferMapper4lockEPK13native_handleiRKNS_4RectEPPv(
        android::GraphicBufferMapper* thiz, buffer_handle_t handle, int usage,
        const android::Rect& bounds, void** vaddr)
{
    return thiz->lock(handle, static_cast<uint32_t>(usage), bounds, vaddr);
}

/*
 * GraphicBuffer(uint32_t w, uint32_t h, PixelFormat format, uint32_t usage) (libHWComposerGC).
 * This is a constructor: the caller allocated the object and passes it as `thiz`.
 * libHWComposerGC allocates 128 bytes (Lollipop sizeof) while the Pie object is bigger; the
 * hwcomposer wrapper pads that allocation (HWCCompat.cpp) so the placement construction fits.
 */
extern "C" void _ZN7android13GraphicBufferC1Ejjij(void* thiz, uint32_t w, uint32_t h,
        android::PixelFormat format, uint32_t usage)
{
    new (thiz) android::GraphicBuffer(w, h, format, usage, std::string("<HWComposerGC>"));
}

extern "C" pid_t androidGetTid()
{
    return gettid();
}
