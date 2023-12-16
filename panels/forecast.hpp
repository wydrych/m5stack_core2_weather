#pragma once

#include "panel.hpp"
#include "main.hpp"
#include "../forecast.h"

class ForecastPanel : public Panel
{
private:
    time_t last_drawn_forecast_timestamp;

    std::vector<std::pair<time_t, String>> getXsteps(std::pair<time_t, time_t> xrange)
    {
        struct tm timeinfo;
        localtime_r(&xrange.first, &timeinfo);
        timeinfo.tm_hour = timeinfo.tm_sec = timeinfo.tm_min = 0;
        time_t start_of_day = mktime(&timeinfo);
        std::vector<std::pair<time_t, String>> xsteps;
        char buf[32];
        for (time_t x = start_of_day; x <= xrange.second; x += settings::forecast::plot::x_step)
        {
            if (x < xrange.first)
                continue;
            localtime_r(&x, &timeinfo);
            strftime(buf, sizeof(buf), settings::forecast::plot::x_step_format, &timeinfo);
            xsteps.push_back(std::make_pair(x, String(buf)));
        }
        return xsteps;
    }

    float getYstep(std::pair<float, float> yrange)
    {
        float yspan = yrange.second - yrange.first;
        float ystepmax = yspan / 4;
        float ystep = settings::forecast::plot::y_steps[0];
        for (float s : settings::forecast::plot::y_steps)
            if (ystep <= ystepmax)
                ystep = s;
            else
                break;
        return ystep;
    }

    std::vector<std::pair<float, String>> getYsteps(std::pair<float, float> yrange)
    {
        float ystep = getYstep(yrange);
        float first = ceilf(yrange.first / ystep) * ystep;
        float last = floor(yrange.second / ystep) * ystep;
        std::vector<std::pair<float, String>> ysteps;
        char buf[32];
        for (float y = first; y <= last; y += ystep)
        {
            snprintf(buf, sizeof(buf), settings::forecast::plot::y_step_format, y);
            ysteps.push_back(std::make_pair(y, String(buf)));
        }
        return ysteps;
    }

protected:
    class Series
    {
    protected:
        virtual void plot(int32_t x0, int32_t y0, time_t x0val, float y0val, float xscale, float yscale) = 0;
    };

    class LineSeries : public Series
    {
    private:
        std::vector<std::pair<time_t, float>> points;
        uint32_t color;

    protected:
        void plot(int32_t x0, int32_t y0, time_t x0val, float y0val, float xscale, float yscale)
        {
            // TODO
        }
    };

    virtual std::pair<time_t, time_t> getXrange() = 0;
    virtual std::vector<Series> getSeries(std::pair<time_t, time_t> xrange) = 0;
    virtual std::pair<float, float> getYrange(std::vector<Series> &series) = 0;

    ForecastPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth)
        : Panel(parentCanvas, w, h, depth)
    {
        last_drawn_forecast_timestamp = 0;
        canvas->clear();
        canvas->setFont(settings::fonts::plot);
    }

public:
    bool redraw()
    {
        if (last_drawn_forecast_timestamp == forecast.timestamp)
            return false;

        time_t now;
        time(&now);

        if (forecast.timestamp == 0 ||
            now <= forecast.timestamp ||
            now > forecast.timestamp + settings::forecast::expiry)
        {
            canvas->clear();
            last_drawn_forecast_timestamp = 0;
            return true;
        }

        std::pair<time_t, time_t> xrange = getXrange();
        std::vector<std::pair<time_t, String>> xsteps = getXsteps(xrange);
        std::vector<Series> series = getSeries(xrange);
        std::pair<float, float> yrange = getYrange(series);
        std::vector<std::pair<float, String>> ysteps = getYsteps(yrange);
        int32_t ylabel_width = 0;
        for (std::pair<float, String> ystep : ysteps)
        {
            int32_t ystepwidth = canvas->textWidth(ystep.second);
            if (ylabel_width < ystepwidth)
                ylabel_width = ystepwidth;
        }

        int32_t x0 = settings::forecast::plot::margin + ylabel_width + 3;
        int32_t x1 = canvas->width() - settings::forecast::plot::margin - 1;
        int32_t y0 = canvas->height() - 1 - (settings::forecast::plot::margin + settings::forecast::plot::label_height + 4);
        int32_t y1 = settings::forecast::plot::margin;

        float xscale = static_cast<float>(x1 - x0) / static_cast<float>(xrange.second - xrange.first);
        float yscale = static_cast<float>(y1 - y0) / static_cast<float>(yrange.second - yrange.first);

        canvas->clear();
        canvas->setColor(settings::colors::plot::border);

        canvas->setTextDatum(m5gfx::textdatum::baseline_right);
        for (std::pair<float, String> ystep : ysteps)
        {
            int32_t y = lround(y0 + yscale * (ystep.first - yrange.first));
            canvas->drawString(ystep.second, x0 - 3, y + settings::forecast::plot::label_height / 2 - 1);
            canvas->drawLine(x0 - 2, y, x0, y);
        }

        canvas->setTextDatum(m5gfx::textdatum::baseline_center);
        for (std::pair<time_t, String> xstep : xsteps)
        {
            int32_t x = lround(x0 + xscale * (xstep.first - xrange.first));
            canvas->drawString(xstep.second, x, y0 + 4 + settings::forecast::plot::label_height);
            canvas->drawLine(x, y0, x, y0 + 2);
        }

        canvas->drawRect(x0, y1, x1 - x0 + 1, y0 - y1 + 1);



        last_drawn_forecast_timestamp = forecast.timestamp;
        return true;
    }

    Panel *touch(int16_t x, int16_t y)
    {
        // return to main panel
        return nullptr;
    }
};