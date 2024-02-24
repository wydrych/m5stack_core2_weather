#pragma once

#include <time.h>

#include "panel.hpp"

class HeaderPanel : public PanelBase
{
    bool wifi_status;
    bool last_drawn_wifi_status;
    bool mqtt_status;
    bool last_drawn_mqtt_status;
    bool forecast_status;
    bool last_drawn_forecast_status;
    time_t last_drawn_time;

    void draw_icons();
    void draw_time(time_t now);

public:
    HeaderPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth);

    inline void setWifiStatus(bool wifi_status)
    {
        this->wifi_status = wifi_status;
    }

    inline void setMqttStatus(bool mqtt_status)
    {
        this->mqtt_status = mqtt_status;
    }

    bool redraw();
};
