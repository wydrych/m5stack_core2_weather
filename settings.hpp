#pragma once

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
        const char *ssid = WIFI_SSID;
        const char *password = WIFI_PASSWORD;
    }
    namespace colors
    {
        const m5gfx::rgb888_t background = m5gfx::convert_to_rgb888(TFT_WHITE);
        namespace icons
        {
            const m5gfx::rgb888_t wifiUp = m5gfx::convert_to_rgb888(0x0000B0u);
            const m5gfx::rgb888_t wifiDown = m5gfx::convert_to_rgb888(0x979797u);
        }
    }
}
