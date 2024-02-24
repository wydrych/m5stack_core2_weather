#pragma once

#include "panel.hpp"
#include "forecast.hpp"

class MainPanel : public Panel
{
    unsigned long reading_millis;
    float reading_temperature;
    float reading_humidity;
    float reading_pressure;
    unsigned long last_drawn_reading_millis;

    ForecastPanel *temperatureForecastPanel,
        *rainForecastPanel,
        *pressureForecastPanel;

    void draw_reading();

public:
    MainPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth);
    void setReading(float reading_temperature, float reading_humidity, float reading_pressure);
    void setForecastPanels(ForecastPanel *temperature, ForecastPanel *rain, ForecastPanel *pressure);
    bool redraw();
    Panel *touch(int16_t x, int16_t y);
};