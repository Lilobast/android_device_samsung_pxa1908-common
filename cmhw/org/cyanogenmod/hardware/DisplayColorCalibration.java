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

package org.cyanogenmod.hardware;

import android.os.SystemProperties;

/*
 * LiveDisplay colour calibration through the hardware composer.
 *
 * The PXA1908 HWC (hwcomposer.mrvl) reads the per-channel gains from sys.livedisplay.rgb and
 * applies them in its 2D-engine composition, so the tint needs no GPU composition (the default
 * implementation uses SurfaceFlinger's colour matrix, which turns the HWC off).
 */
public class DisplayColorCalibration {

    private static final String PROP = "sys.livedisplay.rgb";

    private static final int MIN = 255;
    private static final int MAX = 32768;
    private static final String DEFAULT = MAX + " " + MAX + " " + MAX;

    public static boolean isSupported() {
        return true;
    }

    public static int getMaxValue() {
        return MAX;
    }

    public static int getMinValue() {
        return MIN;
    }

    public static int getDefValue() {
        return getMaxValue();
    }

    public static String getCurColors() {
        return SystemProperties.get(PROP, DEFAULT);
    }

    public static boolean setColors(String colors) {
        String[] adj = colors == null ? null : colors.trim().split("\\s+");
        if (adj == null || adj.length != 3) {
            return false;
        }
        StringBuilder sb = new StringBuilder();
        for (int i = 0; i < 3; i++) {
            int v;
            try {
                v = Integer.parseInt(adj[i]);
            } catch (NumberFormatException e) {
                return false;
            }
            v = Math.max(MIN, Math.min(MAX, v));
            if (i > 0) {
                sb.append(' ');
            }
            sb.append(v);
        }
        SystemProperties.set(PROP, sb.toString());
        return true;
    }
}
