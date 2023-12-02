#pragma once

#include <time.h>

#include "panel.hpp"
#include "../icons/icons.hpp"
#include "../lang.hpp"
#include "../settings.hpp"

class HeaderPanel : public Panel
{
    bool wifi_status;
    bool last_drawn_wifi_status;
    bool mqtt_status;
    bool last_drawn_mqtt_status;
    time_t last_drawn_time;

    void draw_icons()
    {
        canvas->pushGrayscaleImage(
            2, 2, icons::wifi::width, icons::wifi::height,
            icons::wifi::data,
            m5gfx::grayscale_8bit,
            wifi_status ? settings::colors::icons::wifi : settings::colors::icons::down,
            settings::colors::background);

        canvas->pushGrayscaleImage(
            24, 2, icons::mqtt::width, icons::mqtt::height,
            icons::mqtt::data,
            m5gfx::grayscale_8bit,
            mqtt_status ? settings::colors::icons::mqtt : settings::colors::icons::down,
            settings::colors::background);
    }

    void draw_time(time_t now)
    {
        struct tm timeinfo;
        const int buflen = 64;
        char buf[buflen];

        localtime_r(&now, &timeinfo);
        if (timeinfo.tm_year < 120)
        {
            return;
        }
        const char *weekday = weekdays_short[timeinfo.tm_wday][settings::lang];
        strcpy(buf, weekday);
        strftime(buf + strlen(weekday),
                 buflen - strlen(weekday),
                 time_format[settings::lang],
                 &timeinfo);
        canvas->setTextDatum(m5gfx::textdatum::baseline_right);
        canvas->drawString(buf, canvas->width() - 2, 17, settings::fonts::currentTime);
    }

public:
    HeaderPanel(M5Canvas *canvas) : Panel(canvas)
    {
        wifi_status = last_drawn_wifi_status = false;
        mqtt_status = last_drawn_mqtt_status = false;
        last_drawn_time = -1;
    }

    void setWifiStatus(bool wifi_status)
    {
        this->wifi_status = wifi_status;
    }

    void setMqttStatus(bool mqtt_status)
    {
        this->mqtt_status = mqtt_status;
    }

    virtual bool redraw()
    {
        time_t now;
        time(&now);
        if (last_drawn_wifi_status == wifi_status &&
            last_drawn_mqtt_status == mqtt_status &&
            last_drawn_time == now)
            return false;

        last_drawn_wifi_status = wifi_status;
        last_drawn_mqtt_status = mqtt_status;
        last_drawn_time = now;

        canvas->clear();
        draw_icons();
        draw_time(now);

        return true;
    }
};
