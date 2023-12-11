#pragma once

#include "panel.hpp"
#include "main.hpp"

class ForecastPanel : public Panel
{
private:
    time_t last_drawn_forecast_timestamp;

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
            sprintf(buf, settings::forecast::plot::y_step_format, y);
            ysteps.push_back(std::make_pair(y, String(buf)));
        }
        return ysteps;
    }

protected:
    class Series
    {
    protected:
        virtual void plot(int32_t x0, int32_t y0, float xscale, float yscale) = 0;
    };

    class LineSeries : public Series
    {
    private:
        std::vector<std::pair<time_t, float>> points;
        uint32_t color;

    protected:
        void plot(int32_t x0, int32_t y0, float xscale, float yscale)
        {
            // TODO
        }
    };

    virtual std::vector<Series> getSeries(time_t xmin, time_t ymax) = 0;
    virtual std::pair<float, float> getYrange(std::vector<Series> &series) = 0;

    ForecastPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth)
        : Panel(parentCanvas, w, h, depth)
    {
        last_drawn_forecast_timestamp = 0;
        canvas->clear();
    }

public:
    bool redraw()
    {
        if (last_drawn_forecast_timestamp == forecast_timestamp)
            return false;

        time_t now;
        time(&now);

        if (forecast_timestamp == 0 ||
            now > forecast_timestamp ||
            now < forecast_timestamp + settings::forecast::expiry)
        {
            canvas->clear();
            last_drawn_forecast_timestamp = forecast_timestamp;
            return true;
        }

        time_t xmin = forecast_timestamp;
        time_t xmax = forecast_timestamp + settings::forecast::plot::x_span;
        std::vector<Series> series = getSeries(xmin, xmax);
        std::pair<float, float> yrange = getYrange(series);
        std::vector<std::pair<float, String>> ysteps = getYsteps(yrange);

        canvas->clear();

        last_drawn_forecast_timestamp = forecast_timestamp;
        return true;
    }

    Panel *touch(int16_t x, int16_t y)
    {
        // return to main panel
        return nullptr;
    }
};