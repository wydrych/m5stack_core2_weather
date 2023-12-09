#pragma once

#include <M5GFX.h>
#include "fonts/NotoSans_SemiBold731fpt8b.hpp"
#include "fonts/NotoSans_SemiBold20pt8b.hpp"
#include "fonts/NotoSans_SemiBold40pt8b.hpp"
#include "lang.hpp"
#include "ca.hpp"

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
#ifndef MQTT_TOPIC
#define MQTT_TOPIC NULL
#endif
#ifndef FORECAST_LAT
#define FORECAST_LAT NAN
#endif
#ifndef FORECAST_LON
#define FORECAST_LON NAN
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
    const int32_t header_height = 22;
    const uint32_t watchdog_timer = 60;
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
        const char *const topic = MQTT_TOPIC;
    }
    namespace forecast
    {
        const char *const base_url = "https://devmgramapi.meteo.pl/meteorograms/";
        const char *const available = "available";
        const char *const model = "um4_60";
        const unsigned long retry = 10;
        const unsigned long refresh = 10 * 60;
        const double lat = FORECAST_LAT;
        const double lon = FORECAST_LON;
        const unsigned long expiry = 24 * 3600;
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
            const uint32_t meteo = m5gfx::convert_to_rgb888(0x0a328cu);
        }
    }
    namespace fonts
    {
        const m5gfx::IFont *const currentTime = &NotoSans_SemiBold731fpt8b;
        const m5gfx::IFont *const temperature = &NotoSans_SemiBold40pt8b;
        const m5gfx::IFont *const humidity = &NotoSans_SemiBold20pt8b;
        const m5gfx::IFont *const pressure = &NotoSans_SemiBold20pt8b;
    }
}
