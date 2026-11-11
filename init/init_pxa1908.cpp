/*
 * Author: Nemirtingas
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>

#include <android-base/logging.h>
#include <android-base/properties.h>
#include <android-base/stringprintf.h>

#include "vendor_init.h"
#include "property_service.h"

#include "init_pxa1908.h"

using android::base::GetProperty;
using android::base::StringPrintf;
using android::init::property_set;


__attribute__ ((weak))
void init_target_properties()
{
}

void property_override(char const prop[], char const value[])
{
    prop_info *pi;
    size_t len = strlen(value);

    if (len >= PROP_VALUE_MAX) {
        LOG(ERROR) << "property_override: value of " << prop << " too long, not set";
        return;
    }

    pi = (prop_info*) __system_property_find(prop);
    if (pi) {
        // Pie stores ro.* values of PROP_VALUE_MAX chars or more out of line ("long" property).
        // __system_property_update() would write over the entry and keep the long flag, so later
        // reads follow a garbage offset (init crashed in strlen expanding ${ro.build.fingerprint}).
        if (GetProperty(prop, "").size() >= PROP_VALUE_MAX) {
            LOG(ERROR) << "property_override: " << prop << " is a long property, not overridden";
            return;
        }
        __system_property_update(pi, value, len);
    } else {
        __system_property_add(prop, strlen(prop), value, len);
    }
}

static int read_file2(const char *fname, char *data, int max_size)
{
    int fd, rc;

    if (max_size < 1)
        return 0;

    fd = open(fname, O_RDONLY);
    if (fd < 0) {
        LOG(ERROR) << StringPrintf("failed to open '%s'", fname);
        return 0;
    }

    rc = read(fd, data, max_size - 1);
    if ((rc > 0) && (rc < max_size))
        data[rc] = '\0';
    else
        data[0] = '\0';
    close(fd);

    return 1;
}

static void init_alarm_boot_properties()
{
    char const *alarm_file = "/proc/sys/kernel/boot_reason";
    char buf[64];

    if (read_file2(alarm_file, buf, sizeof(buf))) {
        /*
         * Setup ro.alarm_boot value to true when it is RTC triggered boot up
         * For existing PMIC chips, the following mapping applies
         * for the value of boot_reason:
         *
         * 0 -> unknown
         * 1 -> hard reset
         * 2 -> sudden momentary power loss (SMPL)
         * 3 -> real time clock (RTC)
         * 4 -> DC charger inserted
         * 5 -> USB charger insertd
         * 6 -> PON1 pin toggled (for secondary PMICs)
         * 7 -> CBLPWR_N pin toggled (for external power supply)
         * 8 -> KPDPWR_N pin toggled (power key pressed)
         */
        if (buf[0] == '3')
            property_set("ro.alarm_boot", "true");
        else
            property_set("ro.alarm_boot", "false");
    }
}

void vendor_load_properties()
{
    init_target_properties();
    init_alarm_boot_properties();
}
