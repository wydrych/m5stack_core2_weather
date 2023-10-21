#pragma once

#include <M5GFX.h>
#include "fonts/all.h"
#include "lang.hpp"

#if __has_include("settings-private.hpp")
#include "settings-private.hpp"
#endif

#ifndef WIFI_SSID
#define WIFI_SSID NULL
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD NULL
#endif
#ifndef MQTT_SERVER
#define MQTT_SERVER NULL
#endif
#ifndef MQTT_PORT
#define MQTT_PORT 1883
#endif
#ifndef TZ
#define TZ UTC
#endif
#ifndef LANG
#define LANG Lang::LANG_EN
#endif

namespace settings
{
    const Lang lang = LANG;
    namespace time
    {
        const char *const ntpServer = "pool.ntp.org";
        const char *const tz = TZ;
    }
    namespace wifi
    {
        const char *const ssid = WIFI_SSID;
        const char *const password = WIFI_PASSWORD;
    }
    namespace mqtt
    {
        const char *const server = MQTT_SERVER;
        const uint16_t port = MQTT_PORT;
        const double reconnect = 3.0;
    }
    namespace colors
    {
        const uint32_t text = m5gfx::convert_to_rgb888(TFT_BLACK);
        const uint32_t background = m5gfx::convert_to_rgb888(TFT_WHITE);
        namespace icons
        {
            const uint32_t down = m5gfx::convert_to_rgb888(0x979797u);
            const uint32_t wifi = m5gfx::convert_to_rgb888(0x0000B0u);
            const uint32_t mqtt = m5gfx::convert_to_rgb888(0x660066u);
        }
    }
    namespace fonts
    {
        const m5gfx::IFont *const currentTime = &NotoSans_SemiBold731fpt8b;
    }
}
