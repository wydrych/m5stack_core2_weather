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
            step.value = y + 0.0;
            snprintf(step.label, step.label_size, settings::forecast::plot::y_step_format, step.value);
            ysteps.push_back(step);
        }
        return ysteps;
    }

    void plotGrid(xsteps_t &xsteps, ysteps_t &ysteps, int32_t x0, int32_t y0, time_t x0val, float y0val, float xscale, float yscale) const
    {
        canvas->setColor(settings::colors::plot::grid);
        for (xstep_t const &xstep : xsteps)
        {
            int32_t x = lround(x0 + xscale * (xstep.value - x0val));
            for (int32_t y = y0; y > 0; y -= settings::forecast::plot::grid_pattern[0] + settings::forecast::plot::grid_pattern[1])
            {
                canvas->drawFastVLine(x, y, -settings::forecast::plot::grid_pattern[0]);
            }
        }
        for (ystep_t const &ystep : ysteps)
        {
            int32_t y = lround(y0 + yscale * (ystep.value - y0val));
            if (ystep.value == 0)
                canvas->drawFastHLine(x0, y, canvas->width() - x0);
            else
                for (int32_t x = x0 + 1; x < canvas->width(); x += settings::forecast::plot::grid_pattern[0] + settings::forecast::plot::grid_pattern[1])
                {
                    canvas->drawFastHLine(x, y, settings::forecast::plot::grid_pattern[0]);
                }
        }
    }

protected:
    class Series
    {
    public:
        virtual ~Series() = default;

        virtual void plot(M5Canvas *canvas, int32_t x0, int32_t y0, time_t x0val, float y0val, float xscale, float yscale) const = 0;
        virtual float getMin() const = 0;
        virtual float getMax() const = 0;
    };

    class LineSeries : public Series
    {
    private:
        std::vector<point_t> points;
        bool thick;
        uint32_t color;

        void plotPoint(size_t i, M5Canvas *canvas, int32_t x0, int32_t y0, time_t x0val, float y0val, float xscale, float yscale) const
        {
            int32_t x = lround(x0 + xscale * (points[i].t - x0val));
            int32_t y = lround(y0 + yscale * (points[i].v - y0val));
            if (thick)
                canvas->fillRect(x - 1, y - 1, 3, 3);
            else
                canvas->drawPixel(x, y);
        }
        void plotLine(size_t i, size_t j, M5Canvas *canvas, int32_t x0, int32_t y0, time_t x0val, float y0val, float xscale, float yscale) const
        {
            int32_t xi = lround(x0 + xscale * (points[i].t - x0val));
            int32_t yi = lround(y0 + yscale * (points[i].v - y0val));
            int32_t xj = lround(x0 + xscale * (points[j].t - x0val));
            int32_t yj = lround(y0 + yscale * (points[j].v - y0val));
            canvas->drawLine(xi, yi, xj, yj);
            if (thick)
                if (abs(yj - yi) > abs(xj - xi)) // steep
                {
                    canvas->drawLine(xi - 1, yi, xj - 1, yj);
                    canvas->drawLine(xi + 1, yi, xj + 1, yj);
                }
                else
                {
                    canvas->drawLine(xi, yi - 1, xj, yj - 1);
                    canvas->drawLine(xi, yi + 1, xj, yj + 1);
                }
        }

    public:
        LineSeries(forecast_entry_t<float> const &entry, xrange_t xrange, bool thick, uint32_t color)
        {
            points.reserve((xrange.to - xrange.from) / entry.interval + 1);
            for (size_t i = 0; i < entry.points.size(); i++)
            {
                time_t t = entry.start + i * entry.interval;
                if (t < xrange.from)
                    continue;
                if (t > xrange.to)
                    break;
                points.push_back({t, entry.points[i]});
            }
            this->thick = thick;
            this->color = color;
        }

        ~LineSeries() = default;

        void plot(M5Canvas *canvas, int32_t x0, int32_t y0, time_t x0val, float y0val, float xscale, float yscale) const
        {
            if (points.size() == 0)
                return;
            canvas->setColor(color);
            plotPoint(0, canvas, x0, y0, x0val, y0val, xscale, yscale);
            for (size_t i = 1; i < points.size(); i++)
            {
                plotLine(i - 1, i, canvas, x0, y0, x0val, y0val, xscale, yscale);
                plotPoint(i, canvas, x0, y0, x0val, y0val, xscale, yscale);
            }
        }

        float getMin() const
        {
            if (points.size() == 0)
                return NAN;
            float min = points[0].v;
            for (size_t i = 1; i < points.size(); i++)
            {
                if (points[i].v < min)
                    min = points[i].v;
            }
            return min;
        }

        float getMax() const
        {
            if (points.size() == 0)
                return NAN;
            float max = points[0].v;
            for (size_t i = 1; i < points.size(); i++)
            {
                if (points[i].v > max)
                    max = points[i].v;
            }
            return max;
        }
    };

    virtual xrange_t getXrange() = 0;
    virtual void fillSeries(xrange_t xrange, std::vector<std::unique_ptr<Series>> &series) = 0;
    virtual yrange_t getYrange(std::vector<std::unique_ptr<Series>> const &series) = 0;

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

        std::vector<std::unique_ptr<Series>> series;
        xrange_t xrange = getXrange();
        xsteps_t xsteps = getXsteps(xrange);
        fillSeries(xrange, series);
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

        canvas->setTextDatum(m5gfx::textdatum::baseline_center);
        for (xstep_t xstep : xsteps)
        {
            int32_t x = lround(x0 + xscale * (xstep.value - xrange.from));
            canvas->drawString(xstep.label, x, y0 + 4 + settings::forecast::plot::label_height);
            canvas->drawLine(x, y0, x, y0 + 2);
        }

        canvas->setTextDatum(m5gfx::textdatum::baseline_right);
        for (ystep_t ystep : ysteps)
        {
            int32_t y = lround(y0 + yscale * (ystep.value - yrange.from));
            canvas->drawString(ystep.label, x0 - 3, y + settings::forecast::plot::label_height / 2 - 1);
            canvas->drawLine(x0 - 2, y, x0, y);
        }

        canvas->drawRect(x0, y1, x1 - x0 + 1, y0 - y1 + 1);

        canvas->setClipRect(x0 + 1, y1 + 1, x1 - x0 - 1, y0 - y1 - 1);

        plotGrid(xsteps, ysteps, x0, y0, xrange.from, yrange.from, xscale, yscale);

        for (std::unique_ptr<Series> const &s : series)
        {
            s->plot(canvas, x0, y0, xrange.from, yrange.from, xscale, yscale);
        }

        canvas->clearClipRect();

        last_drawn_forecast_timestamp = forecast.timestamp;
        return true;
    }

    Panel *touch(int16_t x, int16_t y)
    {
        // return to main panel
        return nullptr;
    }
};