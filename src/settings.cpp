#include <stdint.h>

#include "settings.hpp"
#include "fonts/NotoSans_SemiBold731fpt8b.hpp"
#include "fonts/NotoSans_SemiBold20pt8b.hpp"
#include "fonts/NotoSans_SemiBold40pt8b.hpp"
#include "fonts/NotoSans_Condensed549fpt8b.hpp"

#if __has_include("../settings-private.hpp")
#include "../settings-private.hpp"
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
#ifndef MQTT_TOPIC_STATUS
#define MQTT_TOPIC_STATUS "m5stack_core2_weather/status"
#endif
#ifndef MQTT_USER
#define MQTT_USER NULL
#endif
#ifndef MQTT_PASSWORD
#define MQTT_PASSWORD NULL
#endif
#ifndef MQTT_CLIENT_NAME_PREFIX
#define MQTT_CLIENT_NAME_PREFIX "M5Stack Core2"
#endif
#ifndef FORECAST_LAT
#define FORECAST_LAT NAN
#endif
#ifndef FORECAST_LON
#define FORECAST_LON NAN
#endif
#ifndef ALTITUDE
#define ALTITUDE 0
#endif
#ifndef TZ
#define TZ "UTC"
#endif
#ifndef LANG
#define LANG LANG_EN
#endif

Settings::Settings()
    : lang(LANG),
      header_height(22),
      watchdog_timer(60),
      panel_timeout(30) {}

Settings::Time::Time()
    : ntpServer("pool.ntp.org"),
      tz(TZ) {}

Settings::Wifi::Wifi()
    : ssid(WIFI_SSID),
      password(WIFI_PASSWORD) {}

Settings::Mqtt::Mqtt()
    : server(MQTT_SERVER),
      port(MQTT_PORT),
      user(MQTT_USER),
      password(MQTT_PASSWORD),
      client_name(getClientName(MQTT_CLIENT_NAME_PREFIX)),
      topic(MQTT_TOPIC),
      status_interval(60),
      status_topic_name(MQTT_TOPIC_STATUS),
      reconnect(3) {}

char *Settings::Mqtt::getClientName(const char *prefix) const
{
    uint8_t mac[6];
    char *name;
    esp_read_mac(mac, ESP_MAC_WIFI_STA);
    asprintf(&name, "%s %02X:%02X:%02X:%02X:%02X:%02X",
             prefix, mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    return name;
}

const char *const forecast_root_ca PROGMEM =
    // HARICA TLS RSA Root CA 2021
    "-----BEGIN CERTIFICATE-----\n"
    "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\n"
    "63ZTGI0RmLo=\n"
    "-----END CERTIFICATE-----\n";

Settings::Forecast::Forecast()
    : base_url("https://devmgramapi.meteo.pl/meteorograms/"),
      available("available"),
      model("um4_60"),
      root_ca(forecast_root_ca),
      retry(10),
      refresh(10 * 60),
      lat(FORECAST_LAT),
      lon(FORECAST_LON),
      alt(ALTITUDE),
      expiry(24 * 3600) {}

Settings::Forecast::Plot::Plot()
    : margin(8),
      x_span(24 * 3600),
      x_step(6 * 3600),
      x_step_format("%H:%M"),
      y_steps({1, 2, 5, 10, 20, 50, 100, 200, 500}),
      y_step_format("%.0f"),
      label_height(NotoSans_Condensed549fpt8b.glyph['M' - NotoSans_Condensed549fpt8b.first].height),
      grid_pattern({3, 3}) {}

Settings::Colors::Colors()
    : text(m5gfx::convert_to_rgb888(TFT_BLACK)),
      background(m5gfx::convert_to_rgb888(TFT_WHITE)) {}

Settings::Colors::Icons::Icons()
    : down(m5gfx::convert_to_rgb888(0x979797u)),
      wifi(m5gfx::convert_to_rgb888(0x0000B0u)),
      mqtt(m5gfx::convert_to_rgb888(0x660066u)),
      meteo(m5gfx::convert_to_rgb888(0x0a328cu)) {}

Settings::Colors::Plot::Plot()
    : border(m5gfx::convert_to_rgb888(TFT_BLACK)),
      night(m5gfx::convert_to_rgb888(0xd8d8d8u)),
      grid(m5gfx::convert_to_rgb888(0x808080u)),
      airtmp(m5gfx::convert_to_rgb888(0xff0000u)),
      airtmp_min_max(m5gfx::convert_to_rgb888(0xffc0c0u)),
      wchill(m5gfx::convert_to_rgb888(0x0000ffu)),
      grdtmp_min_max(m5gfx::convert_to_rgb888(0xa52a2au)),
      pcpttl({
          m5gfx::convert_to_rgb888(0x808080u),
          m5gfx::convert_to_rgb888(0x00aa00u),
          m5gfx::convert_to_rgb888(0x1432c8u),
          m5gfx::convert_to_rgb888(0x12CBB7u),
      }),
      trpres(m5gfx::convert_to_rgb888(0x000000u))
{
}

Settings::Fonts::Fonts()
    : currentTime(&NotoSans_SemiBold731fpt8b),
      temperature(&NotoSans_SemiBold40pt8b),
      humidity(&NotoSans_SemiBold20pt8b),
      pressure(&NotoSans_SemiBold20pt8b),
      plot(&NotoSans_Condensed549fpt8b) {}

Settings settings;
