#pragma once

#include <sunset.h>

#include "panel.hpp"
#include "main.hpp"
#include "../forecast.h"

inline float unless_nan(const float &(*f)(const float &, const float &), const float &a, const float &b)
{
    if (isnanf(a))
        return b;
    if (isnanf(b))
        return a;
    return f(a, b);
}

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

    class Series
    {
    private:
        bool below_grid;

    public:
        Series(bool below_grid)
            : below_grid(below_grid) {}

        virtual ~Series() = default;

        bool isBelowGrid() const
        {
            return below_grid;
        }

        virtual void plot(M5Canvas *canvas, int32_t x0, int32_t y0, time_t x0val, float y0val, float xscale, float yscale) const = 0;
        virtual float getMin() const = 0;
        virtual float getMax() const = 0;
    };

    class LineSeries : public Series
    {
    private:
        struct point_t
        {
            time_t t;
            float v;
        };

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
            : LineSeries(false, entry, xrange, thick, color) {}

        LineSeries(bool below_grid, forecast_entry_t<float> const &entry, xrange_t xrange, bool thick, uint32_t color)
            : LineSeries(below_grid, entry, 1, xrange, thick, color) {}

        LineSeries(forecast_entry_t<float> const &entry, float scale, xrange_t xrange, bool thick, uint32_t color)
            : LineSeries(false, entry, scale, xrange, thick, color) {}

        LineSeries(bool below_grid, forecast_entry_t<float> const &entry, float scale, xrange_t xrange, bool thick, uint32_t color)
            : Series(below_grid), points(), thick(thick), color(color)
        {
            points.reserve((xrange.to - xrange.from) / entry.interval + 1);
            for (size_t i = 0; i < entry.points.size(); i++)
            {
                time_t t = entry.start + i * entry.interval;
                if (t < xrange.from)
                    continue;
                if (t > xrange.to)
                    break;
                points.push_back({t, entry.points[i] * scale});
            }
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

    class BiSeries : public Series
    {
    protected:
        struct point_t
        {
            time_t t;
            float from;
            float to;
        };

        std::vector<point_t> points;

        BiSeries(bool below_grid, forecast_entry_t<float> const &entry, float scale, xrange_t xrange)
            : BiSeries(below_grid, {entry.start, entry.interval, std::vector<float>(entry.points.size())}, entry, scale, xrange) {}

        BiSeries(bool below_grid, forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, float scale, xrange_t xrange)
            : Series(below_grid), points()
        {
            if (from_entry.start != to_entry.start ||
                from_entry.interval != to_entry.interval ||
                from_entry.points.size() != to_entry.points.size())
            {
                M5_LOGE("contents of from_entry and to_entry do not match");
                return;
            }

            points.reserve((xrange.to - xrange.from) / to_entry.interval + 1);
            for (size_t i = 0; i < to_entry.points.size(); i++)
            {
                time_t t = to_entry.start + i * to_entry.interval;
                if (t < xrange.from)
                    continue;
                if (t > xrange.to)
                    break;
                points.push_back({t,
                                  from_entry.points[i] * scale,
                                  to_entry.points[i] * scale});
            }
        }

    public:
        virtual ~BiSeries() = default;

        float getMin() const
        {
            if (points.size() == 0)
                return NAN;
            float min = points[0].from;
            for (size_t i = 1; i < points.size(); i++)
            {
                if (points[i].from < min)
                    min = points[i].from;
            }
            return min;
        }

        float getMax() const
        {
            if (points.size() == 0)
                return NAN;
            float max = points[0].to;
            for (size_t i = 1; i < points.size(); i++)
            {
                if (points[i].to > max)
                    max = points[i].to;
            }
            return max;
        }
    };

    class BarSeries : public BiSeries
    {
    private:
        time_t thickness;
        uint32_t color; // used unless colorf == nullptr
        uint32_t (*colorf)(time_t const &);

    public:
        BarSeries(forecast_entry_t<float> const &entry, xrange_t xrange, bool thick, uint32_t (*colorf)(time_t const &))
            : BarSeries(false, entry, xrange, thick, colorf) {}

        BarSeries(bool below_grid, forecast_entry_t<float> const &entry, xrange_t xrange, bool thick, uint32_t (*colorf)(time_t const &))
            : BarSeries(below_grid, entry, xrange, thick, 0, colorf) {}

        BarSeries(forecast_entry_t<float> const &entry, xrange_t xrange, bool thick, uint32_t color, uint32_t (*colorf)(time_t const &) = nullptr)
            : BarSeries(false, entry, xrange, thick, color, colorf) {}

        BarSeries(bool below_grid, forecast_entry_t<float> const &entry, xrange_t xrange, bool thick, uint32_t color, uint32_t (*colorf)(time_t const &) = nullptr)
            : BarSeries(below_grid, entry, 1, xrange, thick, color, colorf) {}

        BarSeries(forecast_entry_t<float> const &entry, float scale, xrange_t xrange, bool thick, uint32_t (*colorf)(time_t const &))
            : BarSeries(false, entry, scale, xrange, thick, colorf) {}

        BarSeries(bool below_grid, forecast_entry_t<float> const &entry, float scale, xrange_t xrange, bool thick, uint32_t (*colorf)(time_t const &))
            : BarSeries(below_grid, entry, scale, xrange, thick, 0, colorf) {}

        BarSeries(forecast_entry_t<float> const &entry, float scale, xrange_t xrange, bool thick, uint32_t color, uint32_t (*colorf)(time_t const &) = nullptr)
            : BarSeries(false, entry, scale, xrange, thick, color, colorf) {}

        BarSeries(bool below_grid, forecast_entry_t<float> const &entry, float scale, xrange_t xrange, bool thick, uint32_t color, uint32_t (*colorf)(time_t const &) = nullptr)
            : BiSeries(below_grid, entry, scale, xrange), thickness(thick ? entry.interval : 0), color(color), colorf(colorf) {}

        BarSeries(forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, xrange_t xrange, bool thick, uint32_t (*colorf)(time_t const &))
            : BarSeries(false, from_entry, to_entry, xrange, thick, colorf) {}

        BarSeries(bool below_grid, forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, xrange_t xrange, bool thick, uint32_t (*colorf)(time_t const &))
            : BarSeries(below_grid, from_entry, to_entry, xrange, thick, 0, colorf) {}

        BarSeries(forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, xrange_t xrange, bool thick, uint32_t color, uint32_t (*colorf)(time_t const &) = nullptr)
            : BarSeries(false, from_entry, to_entry, xrange, thick, color, colorf) {}

        BarSeries(bool below_grid, forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, xrange_t xrange, bool thick, uint32_t color, uint32_t (*colorf)(time_t const &) = nullptr)
            : BarSeries(below_grid, from_entry, to_entry, 1, xrange, thick, color, colorf) {}

        BarSeries(forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, float scale, xrange_t xrange, bool thick, uint32_t color, uint32_t (*colorf)(time_t const &) = nullptr)
            : BarSeries(false, from_entry, to_entry, scale, xrange, thick, color, colorf) {}

        BarSeries(bool below_grid, forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, float scale, xrange_t xrange, bool thick, uint32_t color, uint32_t (*colorf)(time_t const &) = nullptr)
            : BiSeries(below_grid, from_entry, to_entry, scale, xrange), thickness(thick ? to_entry.interval : 0), color(color), colorf(colorf) {}

        ~BarSeries() = default;

        void plot(M5Canvas *canvas, int32_t x0, int32_t y0, time_t x0val, float y0val, float xscale, float yscale) const
        {
            if (points.size() == 0)
                return;
            for (size_t i = 0; i < points.size(); i++)
            {
                canvas->setColor(colorf ? colorf(points[i].t) : color);
                int32_t y1 = lround(y0 + yscale * (points[i].from - y0val));
                int32_t y2 = lround(y0 + yscale * (points[i].to - y0val));
                if (thickness)
                {
                    int32_t x1 = lround(x0 + xscale * (points[i].t - thickness / 2 - x0val));
                    int32_t x2 = lround(x0 + xscale * (points[i].t + thickness / 2 - x0val));
                    canvas->fillRect(x1, y2, x2 - x1, y1 - y2 + 1);
                }
                else
                {
                    int32_t x = lround(x0 + xscale * (points[i].t - x0val));
                    canvas->drawLine(x, y1, x, y2);
                }
            }
        }
    };

    class AreaSeries : public BiSeries
    {
    private:
        uint32_t color;

    public:
        AreaSeries(forecast_entry_t<float> const &entry, xrange_t xrange, uint32_t color)
            : AreaSeries(false, entry, xrange, color) {}

        AreaSeries(bool below_grid, forecast_entry_t<float> const &entry, xrange_t xrange, uint32_t color)
            : AreaSeries(below_grid, entry, 1, xrange, color) {}

        AreaSeries(forecast_entry_t<float> const &entry, float scale, xrange_t xrange, uint32_t color)
            : AreaSeries(false, entry, scale, xrange, color) {}

        AreaSeries(bool below_grid, forecast_entry_t<float> const &entry, float scale, xrange_t xrange, uint32_t color)
            : BiSeries(below_grid, entry, scale, xrange), color(color) {}

        AreaSeries(forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, xrange_t xrange, uint32_t color)
            : AreaSeries(false, from_entry, to_entry, xrange, color) {}

        AreaSeries(bool below_grid, forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, xrange_t xrange, uint32_t color)
            : AreaSeries(below_grid, from_entry, to_entry, 1, xrange, color) {}

        AreaSeries(forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, float scale, xrange_t xrange, uint32_t color)
            : AreaSeries(false, from_entry, to_entry, scale, xrange, color) {}

        AreaSeries(bool below_grid, forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, float scale, xrange_t xrange, uint32_t color)
            : BiSeries(below_grid, from_entry, to_entry, scale, xrange), color(color) {}

        ~AreaSeries() = default;

        void plot(M5Canvas *canvas, int32_t x0, int32_t y0, time_t x0val, float y0val, float xscale, float yscale) const
        {
            if (points.size() <= 1)
                return;

            canvas->setColor(color);
            for (size_t i = 1; i < points.size(); i++)
            {
                int32_t x1 = lround(x0 + xscale * (points[i - 1].t - x0val));
                int32_t y11 = lround(y0 + yscale * (points[i - 1].from - y0val));
                int32_t y12 = lround(y0 + yscale * (points[i - 1].to - y0val));
                int32_t x2 = lround(x0 + xscale * (points[i].t - x0val));
                int32_t y21 = lround(y0 + yscale * (points[i].from - y0val));
                int32_t y22 = lround(y0 + yscale * (points[i].to - y0val));
                canvas->fillTriangle(x1, y11, x2, y21, x2, y22);
                canvas->fillTriangle(x1, y11, x1, y12, x2, y22);
            }
        }
    };

private:
    time_t last_drawn_forecast_timestamp;

    static xsteps_t getXsteps(xrange_t xrange)
    {
        struct tm timeinfo;
        localtime_r(&xrange.from, &timeinfo);
        timeinfo.tm_hour = timeinfo.tm_sec = timeinfo.tm_min = 0;
        time_t start_of_today = mktime(&timeinfo);
        xsteps_t xsteps;
        for (time_t x = start_of_today; x <= xrange.to; x += settings::forecast::plot::x_step)
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

    yrange_t getYrange(std::vector<std::unique_ptr<Series>> const &series) const
    {
        yrange_t soft = getSoftYrange();
        yrange_t hard = getHardYrange();
        bool has_hard_min = !isnanf(hard.from);
        bool has_hard_max = !isnanf(hard.to);

        if (has_hard_min && has_hard_max)
            return hard;

        float data_min = std::numeric_limits<float>::infinity();
        float data_max = -std::numeric_limits<float>::infinity();
        for (std::unique_ptr<Series> const &s : series)
        {
            if (!has_hard_min)
            {
                float smin = s->getMin();
                if (smin < data_min)
                    data_min = smin;
            }
            if (!has_hard_max)
            {
                float smax = s->getMax();
                if (smax > data_max)
                    data_max = smax;
            }
        }

        const float M = 0.1;

        if (has_hard_min)
            return {hard.from,
                    std::max(hard.from + 1 + M,
                             unless_nan(std::max, soft.to, data_max + M * (data_max - hard.from) / (1 - M)))};
        if (has_hard_max)
            return {std::min(hard.to - 1 - M,
                             unless_nan(std::min, soft.from, data_min - M * (hard.to - data_min) / (1 - M))),
                    hard.to};

        float margin = std::max(
            M * (data_max - unless_nan(std::min, soft.from, data_min - M * (data_max - data_min) / (1 - 2 * M))) / (1 - M),
            M * (unless_nan(std::max, soft.to, data_max + M * (data_max - data_min) / (1 - 2 * M)) - data_min) / (1 - M));

        float min = unless_nan(std::min, soft.from, data_min - margin);
        float max = unless_nan(std::max, soft.to, data_max + margin);

        if (max - min < 2 * (1 + M))
            return {(min + max) / 2 - 1 - M, (min + max) / 2 + 1 + M};

        return {min, max};
    };

    static float getYstep(yrange_t yrange)
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

    static ysteps_t getYsteps(yrange_t yrange)
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

    void plotNights(xrange_t xrange, int32_t x0, float xscale)
    {
        struct tm timeinfo;
        gmtime_r(&xrange.from, &timeinfo);
        float tzoffset = (xrange.from - mktime(&timeinfo)) / 3600.0;

        SunSet sun;
        sun.setPosition(settings::forecast::lat, settings::forecast::lon, tzoffset);

        localtime_r(&xrange.from, &timeinfo);
        timeinfo.tm_hour = timeinfo.tm_sec = timeinfo.tm_min = 0;
        time_t start_of_today = mktime(&timeinfo);

        for (time_t t = start_of_today - 24 * 3600; t < xrange.to; /* noop */)
        {
            localtime_r(&t, &timeinfo);
            sun.setCurrentDate(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
            time_t sunset = t + lround(sun.calcSunset() * 60);
            M5_LOGD("%d-%d-%d sunset: %d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, sunset);

            t += 24 * 3600;
            localtime_r(&t, &timeinfo);
            sun.setCurrentDate(timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
            time_t sunrise = t + lround(sun.calcSunrise() * 60);
            M5_LOGD("%d-%d-%d sunrise: %d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday, sunrise);

            int32_t sunset_x = lround(x0 + xscale * (sunset - xrange.from));
            int32_t sunrise_x = lround(x0 + xscale * (sunrise - xrange.from));
            canvas->setColor(settings::colors::plot::night);
            canvas->fillRect(sunset_x, 0, sunrise_x - sunset_x, canvas->height());
        }
    }

    void plotGrid(xsteps_t &xsteps, ysteps_t &ysteps, int32_t x0, int32_t y0, time_t x0val, float y0val, float xscale, float yscale)
    {
        canvas->setColor(settings::colors::plot::grid);
        for (xstep_t const &xstep : xsteps)
        {
            int32_t x = lround(x0 + xscale * (xstep.value - x0val));
            for (int32_t y = y0; y > 0; y -= settings::forecast::plot::grid_pattern[0] + settings::forecast::plot::grid_pattern[1])
                canvas->drawFastVLine(x, y, -settings::forecast::plot::grid_pattern[0]);
        }
        for (ystep_t const &ystep : ysteps)
        {
            int32_t y = lround(y0 + yscale * (ystep.value - y0val));
            if (ystep.value == 0)
                canvas->drawFastHLine(x0, y, canvas->width() - x0);
            else
                for (int32_t x = x0 + 1; x < canvas->width(); x += settings::forecast::plot::grid_pattern[0] + settings::forecast::plot::grid_pattern[1])
                    canvas->drawFastHLine(x, y, settings::forecast::plot::grid_pattern[0]);
        }
    }

protected:
    virtual xrange_t getXrange() const = 0;
    virtual yrange_t getSoftYrange() const = 0;
    virtual yrange_t getHardYrange() const = 0;
    virtual char *getUnitLabel() const = 0;
    virtual void fillSeries(xrange_t xrange, std::vector<std::unique_ptr<Series>> &series) = 0;

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

        char *unit_label = getUnitLabel();
        int32_t x0 = settings::forecast::plot::margin + (unit_label ? settings::forecast::plot::label_height + 4 : 0) + ylabel_width + 3;
        int32_t x1 = canvas->width() - settings::forecast::plot::margin - 1;
        int32_t y0 = canvas->height() - 1 - (settings::forecast::plot::margin + settings::forecast::plot::label_height + 4);
        int32_t y1 = settings::forecast::plot::margin;

        float xscale = static_cast<float>(x1 - x0) / static_cast<float>(xrange.to - xrange.from);
        float yscale = static_cast<float>(y1 - y0) / static_cast<float>(yrange.to - yrange.from);

        canvas->clear();
        canvas->setColor(settings::colors::plot::border);

        canvas->setTextDatum(m5gfx::textdatum::baseline_center);

        if (unit_label)
        {
            canvas->setRotation(3);
            canvas->drawString(unit_label, canvas->width() - 1 - (y0 + y1) / 2, settings::forecast::plot::label_height + settings::forecast::plot::margin);
            canvas->setRotation(0);
        }

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

        plotNights(xrange, x0, xscale);

        for (std::unique_ptr<Series> const &s : series)
            if (s->isBelowGrid())
                s->plot(canvas, x0, y0, xrange.from, yrange.from, xscale, yscale);

        plotGrid(xsteps, ysteps, x0, y0, xrange.from, yrange.from, xscale, yscale);

        for (std::unique_ptr<Series> const &s : series)
            if (!s->isBelowGrid())
                s->plot(canvas, x0, y0, xrange.from, yrange.from, xscale, yscale);

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