#pragma once

#include <M5GFX.h>
#include "fonts/all.h"

#if __has_include("settings-private.hpp")
#include "settings-private.hpp"
#endif

#ifndef WIFI_SSID
#define WIFI_SSID NULL
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD NULL
#endif

namespace settings
{
    namespace wifi
    {
        const char *const ssid = WIFI_SSID;
        const char *const password = WIFI_PASSWORD;
    }
    namespace colors
    {
        const uint32_t text = m5gfx::convert_to_rgb888(TFT_BLACK);
        const uint32_t background = m5gfx::convert_to_rgb888(TFT_WHITE);
        namespace icons
        {
            const uint32_t wifiUp = m5gfx::convert_to_rgb888(0x0000B0u);
            const uint32_t wifiDown = m5gfx::convert_to_rgb888(0x979797u);
        }
    }
    namespace fonts
    {
        const m5gfx::IFont *const currentTime = &NotoSans_SemiBold731fpt8b;
    }
}
