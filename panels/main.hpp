#pragma once

#include "panel.hpp"

class MainPanel : public Panel
{
    unsigned long reading_millis;
    float reading_temperature;
    float reading_humidity;
    float reading_pressure;
    unsigned long last_drawn_reading_millis;

    void draw_reading()
    {
        const int buflen = 16;
        char buf_temp[buflen], buf_hum[buflen], buf_press[buflen];

        if (reading_millis)
        {
            sprintf(buf_temp, "%.1f°C", round(reading_temperature * 10) / 10 + 0.0);
            sprintf(buf_hum, "%.0f%%", reading_humidity);
            sprintf(buf_press, "%.0f hPa", reading_pressure);
        }
        else
        {
            canvas->setTextSize(3, 1);
            strcpy(buf_temp, "-");
            strcpy(buf_hum, "");
            strcpy(buf_press, "");
        }

        canvas->setTextDatum(m5gfx::textdatum::baseline_center);
        canvas->drawString(buf_temp,
                           canvas->width() / 2, 116,
                           settings::fonts::temperature);

        canvas->setTextDatum(m5gfx::textdatum::baseline_left);
        canvas->drawString(buf_hum,
                           14, canvas->height() - 14,
                           settings::fonts::humidity);

        canvas->setTextDatum(m5gfx::textdatum::baseline_right);
        canvas->drawString(buf_press,
                           canvas->width() - 14, canvas->height() - 14,
                           settings::fonts::pressure);

        canvas->setTextSize(1, 1);
    }

public:
    MainPanel(M5Canvas *canvas) : Panel(canvas)
    {
        reading_millis = 0;
        last_drawn_reading_millis = -1;
    }

    void setReading(float reading_temperature, float reading_humidity, float reading_pressure)
    {
        if (isnanf(reading_temperature) || isnanf(reading_humidity) || isnanf(reading_pressure))
            return;
        this->reading_temperature = reading_temperature;
        this->reading_humidity = reading_humidity;
        this->reading_pressure = reading_pressure;
        reading_millis = millis();
    }

    virtual bool redraw()
    {
        unsigned long now = millis();

        if (reading_millis != 0 && now - reading_millis > 3600 * 1000)
            reading_millis = 0;

        if (last_drawn_reading_millis == reading_millis)
            return false;

        last_drawn_reading_millis = reading_millis;

        canvas->clear();
        draw_reading();

        return true;
    }
};