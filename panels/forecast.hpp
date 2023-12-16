#pragma once

#include "panel.hpp"
#include "main.hpp"
#include "../forecast.h"

class ForecastPanel : public Panel
{
private:
    template <typename T>
    struct range_t
    {
        T from;
        T to;
    };

    template <typename T>
    struct step_t
    {
        static const size_t label_size = 32 - sizeof(T);
        T value;
        char label[label_size];
    };

    typedef step_t<time_t> xstep_t;
    typedef std::vector<xstep_t> xsteps_t;
    typedef step_t<float> ystep_t;
    typedef std::vector<ystep_t> ysteps_t;

protected:
    typedef range_t<time_t> xrange_t;
    typedef range_t<float> yrange_t;

    struct point_t
    {
        time_t t;
        float v;
    };

private:
    time_t last_drawn_forecast_timestamp;

    xsteps_t getXsteps(xrange_t xrange)
    {
        struct tm timeinfo;
        localtime_r(&xrange.from, &timeinfo);
        timeinfo.tm_hour = timeinfo.tm_sec = timeinfo.tm_min = 0;
        time_t start_of_day = mktime(&timeinfo);
        xsteps_t xsteps;
        for (time_t x = start_of_day; x <= xrange.to; x += settings::forecast::plot::x_step)
        {
            if (x < xrange.from)
                continue;
            xstep_t step;
            step.value = x;
            localtime_r(&x, &timeinfo);
            strftime(step.label, step.label_size, settings::forecast::plot::x_step_format, &timeinfo);
            xsteps.push_back(step);
        }
        return xsteps;
    }

    float getYstep(yrange_t yrange)
    {
        float yspan = yrange.to - yrange.from;
        float ystepmax = yspan / 4;
        float ystep = settings::forecast::plot::y_steps[0];
        for (float s : settings::forecast::plot::y_steps)
            if (ystep <= ystepmax)
                ystep = s;
            else
                break;
        return ystep;
    }

    ysteps_t getYsteps(yrange_t yrange)
    {
        float ystep = getYstep(yrange);
        float first = ceilf(yrange.from / ystep) * ystep;
        float last = floor(yrange.to / ystep) * ystep;
        ysteps_t ysteps;
        for (float y = first; y <= last; y += ystep)
        {
            ystep_t step;
            step.value = y;
            snprintf(step.label, step.label_size, settings::forecast::plot::y_step_format, y);
            ysteps.push_back(step);
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
        std::vector<point_t> points;
        uint32_t color;

    protected:
        LineSeries(std::vector<point_t> _points, uint32_t _color)
        {
            _points = points;
            _color = color;
        }
        void plot(int32_t x0, int32_t y0, time_t x0val, float y0val, float xscale, float yscale)
        {
            // TODO
        }
    };

    virtual xrange_t getXrange() = 0;
    virtual std::vector<Series> getSeries(xrange_t xrange) = 0;
    virtual yrange_t getYrange(std::vector<Series> &series) = 0;

    std::vector<point_t> extractSeriesPoints(forecast_entry_t<float> const &forecast_entry, xrange_t xrange)
    {
        return std::vector<point_t>();
    }

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

        xrange_t xrange = getXrange();
        xsteps_t xsteps = getXsteps(xrange);
        std::vector<Series> series = getSeries(xrange);
        yrange_t yrange = getYrange(series);
        ysteps_t ysteps = getYsteps(yrange);
        int32_t ylabel_width = 0;
        for (ystep_t ystep : ysteps)
        {
            int32_t ystepwidth = canvas->textWidth(ystep.label);
            if (ylabel_width < ystepwidth)
                ylabel_width = ystepwidth;
        }

        int32_t x0 = settings::forecast::plot::margin + ylabel_width + 3;
        int32_t x1 = canvas->width() - settings::forecast::plot::margin - 1;
        int32_t y0 = canvas->height() - 1 - (settings::forecast::plot::margin + settings::forecast::plot::label_height + 4);
        int32_t y1 = settings::forecast::plot::margin;

        float xscale = static_cast<float>(x1 - x0) / static_cast<float>(xrange.to - xrange.from);
        float yscale = static_cast<float>(y1 - y0) / static_cast<float>(yrange.to - yrange.from);

        canvas->clear();
        canvas->setColor(settings::colors::plot::border);

        canvas->setTextDatum(m5gfx::textdatum::baseline_right);
        for (ystep_t ystep : ysteps)
        {
            int32_t y = lround(y0 + yscale * (ystep.value - yrange.from));
            canvas->drawString(ystep.label, x0 - 3, y + settings::forecast::plot::label_height / 2 - 1);
            canvas->drawLine(x0 - 2, y, x0, y);
        }

        canvas->setTextDatum(m5gfx::textdatum::baseline_center);
        for (xstep_t xstep : xsteps)
        {
            int32_t x = lround(x0 + xscale * (xstep.value - xrange.from));
            canvas->drawString(xstep.label, x, y0 + 4 + settings::forecast::plot::label_height);
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