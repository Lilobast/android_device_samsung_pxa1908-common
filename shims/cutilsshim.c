/*
 * libcutils shim for Lollipop blobs on Pie (libGAL, libsec-ril).
 * Pie's libcutils no longer exports android_atomic_*; the header still has the
 * implementations. Defining ANDROID_ATOMIC_INLINE as a plain exported definition
 * emits real symbols, as Lollipop's libcutils/atomic.c did.
 */
#define ANDROID_ATOMIC_INLINE __attribute__((visibility("default")))
#include <cutils/atomic.h>
