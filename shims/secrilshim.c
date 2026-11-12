/*
 * libsec-ril (Lollipop) calls pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE)
 * on a stack attribute it never initialised (it imports no pthread_mutexattr_init).
 * Old bionic overwrote the whole attribute with the type. Pie keeps the other bits,
 * here stack garbage (process-shared / priority inheritance), so pthread_mutex_init()
 * fails, the list mutex stays a plain one, and the RIL init thread deadlocks when it
 * locks it a second time (List::StartIteration -> GetHeadPosition): the modem never boots.
 * libsec-ril uses no other mutexattr setter, so writing only the type is exactly the old
 * behaviour. Loaded for libsec-ril only (TARGET_LD_SHIM_LIBS).
 */
#include <errno.h>
#include <pthread.h>

int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type)
{
    if (attr == NULL || type < PTHREAD_MUTEX_NORMAL || type > PTHREAD_MUTEX_ERRORCHECK)
        return EINVAL;
    *attr = type;
    return 0;
}
