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
    // GEANT Vereniging, GEANT OV RSA CA 4
    "-----BEGIN CERTIFICATE-----\n"
    "MIIG5TCCBM2gAwIBAgIRANpDvROb0li7TdYcrMTz2+AwDQYJKoZIhvcNAQEMBQAw\n"
    "gYgxCzAJBgNVBAYTAlVTMRMwEQYDVQQIEwpOZXcgSmVyc2V5MRQwEgYDVQQHEwtK\n"
    "ZXJzZXkgQ2l0eTEeMBwGA1UEChMVVGhlIFVTRVJUUlVTVCBOZXR3b3JrMS4wLAYD\n"
    "VQQDEyVVU0VSVHJ1c3QgUlNBIENlcnRpZmljYXRpb24gQXV0aG9yaXR5MB4XDTIw\n"
    "MDIxODAwMDAwMFoXDTMzMDUwMTIzNTk1OVowRDELMAkGA1UEBhMCTkwxGTAXBgNV\n"
    "BAoTEEdFQU5UIFZlcmVuaWdpbmcxGjAYBgNVBAMTEUdFQU5UIE9WIFJTQSBDQSA0\n"
    "MIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEApYhi1aEiPsg9ZKRMAw9Q\n"
    "r8Mthsr6R20VSfFeh7TgwtLQi6RSRLOh4or4EMG/1th8lijv7xnBMVZkTysFiPmT\n"
    "PiLOfvz+QwO1NwjvgY+Jrs7fSoVA/TQkXzcxu4Tl3WHi+qJmKLJVu/JOuHud6mOp\n"
    "LWkIbhODSzOxANJ24IGPx9h4OXDyy6/342eE6UPXCtJ8AzeumTG6Dfv5KVx24lCF\n"
    "TGUzHUB+j+g0lSKg/Sf1OzgCajJV9enmZ/84ydh48wPp6vbWf1H0O3Rd3LhpMSVn\n"
    "TqFTLKZSbQeLcx/l9DOKZfBCC9ghWxsgTqW9gQ7v3T3aIfSaVC9rnwVxO0VjmDdP\n"
    "FNbdoxnh0zYwf45nV1QQgpRwZJ93yWedhp4ch1a6Ajwqs+wv4mZzmBSjovtV0mKw\n"
    "d+CQbSToalEUP4QeJq4Udz5WNmNMI4OYP6cgrnlJ50aa0DZPlJqrKQPGL69KQQz1\n"
    "2WgxvhCuVU70y6ZWAPopBa1ykbsttpLxADZre5cH573lIuLHdjx7NjpYIXRx2+QJ\n"
    "URnX2qx37eZIxYXz8ggM+wXH6RDbU3V2o5DP67hXPHSAbA+p0orjAocpk2osxHKo\n"
    "NSE3LCjNx8WVdxnXvuQ28tKdaK69knfm3bB7xpdfsNNTPH9ElcjscWZxpeZ5Iij8\n"
    "lyrCG1z0vSWtSBsgSnUyG/sCAwEAAaOCAYswggGHMB8GA1UdIwQYMBaAFFN5v1qq\n"
    "K0rPVIDh2JvAnfKyA2bLMB0GA1UdDgQWBBRvHTVJEGwy+lmgnryK6B+VvnF6DDAO\n"
    "BgNVHQ8BAf8EBAMCAYYwEgYDVR0TAQH/BAgwBgEB/wIBADAdBgNVHSUEFjAUBggr\n"
    "BgEFBQcDAQYIKwYBBQUHAwIwOAYDVR0gBDEwLzAtBgRVHSAAMCUwIwYIKwYBBQUH\n"
    "AgEWF2h0dHBzOi8vc2VjdGlnby5jb20vQ1BTMFAGA1UdHwRJMEcwRaBDoEGGP2h0\n"
    "dHA6Ly9jcmwudXNlcnRydXN0LmNvbS9VU0VSVHJ1c3RSU0FDZXJ0aWZpY2F0aW9u\n"
    "QXV0aG9yaXR5LmNybDB2BggrBgEFBQcBAQRqMGgwPwYIKwYBBQUHMAKGM2h0dHA6\n"
    "Ly9jcnQudXNlcnRydXN0LmNvbS9VU0VSVHJ1c3RSU0FBZGRUcnVzdENBLmNydDAl\n"
    "BggrBgEFBQcwAYYZaHR0cDovL29jc3AudXNlcnRydXN0LmNvbTANBgkqhkiG9w0B\n"
    "AQwFAAOCAgEAUtlC3e0xj/1BMfPhdQhUXeLjb0xp8UE28kzWE5xDzGKbfGgnrT2R\n"
    "lw5gLIx+/cNVrad//+MrpTppMlxq59AsXYZW3xRasrvkjGfNR3vt/1RAl8iI31lG\n"
    "hIg6dfIX5N4esLkrQeN8HiyHKH6khm4966IkVVtnxz5CgUPqEYn4eQ+4eeESrWBh\n"
    "AqXaiv7HRvpsdwLYekAhnrlGpioZ/CJIT2PTTxf+GHM6cuUnNqdUzfvrQgA8kt1/\n"
    "ASXx2od/M+c8nlJqrGz29lrJveJOSEMX0c/ts02WhsfMhkYa6XujUZLmvR1Eq08r\n"
    "48/EZ4l+t5L4wt0DV8VaPbsEBF1EOFpz/YS2H6mSwcFaNJbnYqqJHIvm3PLJHkFm\n"
    "EoLXRVrQXdCT+3wgBfgU6heCV5CYBz/YkrdWES7tiiT8sVUDqXmVlTsbiRNiyLs2\n"
    "bmEWWFUl76jViIJog5fongEqN3jLIGTG/mXrJT1UyymIcobnIGrbwwRVz/mpFQo0\n"
    "vBYIi1k2ThVh0Dx88BbF9YiP84dd8Fkn5wbE6FxXYJ287qfRTgmhePecPc73Yrzt\n"
    "apdRcsKVGkOpaTIJP/l+lAHRLZxk/dUtyN95G++bOSQqnOCpVPabUGl2E/OEyFrp\n"
    "Ipwgu2L/WJclvd6g+ZA/iWkLSMcpnFb+uX6QBqvD6+RNxul1FaB5iHY=\n"
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
