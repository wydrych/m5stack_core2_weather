#include "header.hpp"
#include "../icons/icons.hpp"
#include "../lang.hpp"
#include "../forecast.hpp"
#include "../settings.hpp"

HeaderPanel::HeaderPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth)
    : PanelBase(parentCanvas, w, h, depth)
{
    wifi_status = last_drawn_wifi_status = false;
    mqtt_status = last_drawn_mqtt_status = false;
    forecast_status = last_drawn_forecast_status = false;
    forecast.timestamp = -1;
    last_drawn_time = -1;
}

void HeaderPanel::draw_icons()
{
    canvas->pushGrayscaleImage(
        2, 2, icons.wifi.width, icons.wifi.height,
        icons.wifi.data,
        m5gfx::grayscale_8bit,
        wifi_status ? settings.colors.icons.wifi : settings.colors.icons.down,
        settings.colors.background);

    canvas->pushGrayscaleImage(
        24, 2, icons.mqtt.width, icons.mqtt.height,
        icons.mqtt.data,
        m5gfx::grayscale_8bit,
        mqtt_status ? settings.colors.icons.mqtt : settings.colors.icons.down,
        settings.colors.background);

    canvas->pushGrayscaleImage(
        42, 2, icons.meteo.width, icons.meteo.height,
        icons.meteo.data,
        m5gfx::grayscale_8bit,
        forecast_status ? settings.colors.icons.meteo : settings.colors.icons.down,
        settings.colors.background);
}

void HeaderPanel::draw_time(time_t now)
{
    struct tm timeinfo;
    const int buflen = 64;
    char buf[buflen];

    localtime_r(&now, &timeinfo);
    if (timeinfo.tm_year < 120)
    {
        return;
    }
    const char *weekday = weekdays_short[timeinfo.tm_wday][settings.lang];
    strcpy(buf, weekday);
    strftime(buf + strlen(weekday),
             buflen - strlen(weekday),
             time_format[settings.lang],
             &timeinfo);
    canvas->setTextDatum(m5gfx::textdatum::baseline_right);
    canvas->drawString(buf, canvas->width() - 2, 17, settings.fonts.currentTime);
}

bool HeaderPanel::redraw()
{
    time_t now;
    time(&now);

    forecast_status = forecast.timestamp != 0 &&
                      now > forecast.timestamp &&
                      now <= forecast.timestamp + settings.forecast.expiry;

    if (last_drawn_wifi_status == wifi_status &&
        last_drawn_mqtt_status == mqtt_status &&
        last_drawn_forecast_status == forecast_status &&
        last_drawn_time == now)
        return false;

    last_drawn_wifi_status = wifi_status;
    last_drawn_mqtt_status = mqtt_status;
    last_drawn_forecast_status = forecast_status;
    last_drawn_time = now;

    canvas->clear();
    draw_icons();
    draw_time(now);

    return true;
}
