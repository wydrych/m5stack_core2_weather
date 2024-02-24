#pragma once

#include "panel.hpp"
#include "../forecast.hpp"

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
        Series(bool below_grid);

        virtual ~Series() = default;

        inline bool isBelowGrid() const
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

        void plotPoint(size_t i, M5Canvas *canvas, int32_t x0, int32_t y0, time_t x0val, float y0val, float xscale, float yscale) const;
        void plotLine(size_t i, size_t j, M5Canvas *canvas, int32_t x0, int32_t y0, time_t x0val, float y0val, float xscale, float yscale) const;

    public:
        LineSeries(forecast_entry_t<float> const &entry, xrange_t xrange, bool thick, uint32_t color);
        LineSeries(bool below_grid, forecast_entry_t<float> const &entry, xrange_t xrange, bool thick, uint32_t color);
        LineSeries(forecast_entry_t<float> const &entry, float scale, xrange_t xrange, bool thick, uint32_t color);
        LineSeries(bool below_grid, forecast_entry_t<float> const &entry, float scale, xrange_t xrange, bool thick, uint32_t color);

        ~LineSeries() = default;

        void plot(M5Canvas *canvas, int32_t x0, int32_t y0, time_t x0val, float y0val, float xscale, float yscale) const;
        float getMin() const;
        float getMax() const;
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

        BiSeries(bool below_grid, forecast_entry_t<float> const &entry, float scale, xrange_t xrange);
        BiSeries(bool below_grid, forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, float scale, xrange_t xrange);

    public:
        virtual ~BiSeries() = default;

        float getMin() const;
        float getMax() const;
    };

    class BarSeries : public BiSeries
    {
    private:
        time_t thickness;
        uint32_t color; // used unless colorf == nullptr
        uint32_t (*colorf)(time_t const &);

    public:
        BarSeries(forecast_entry_t<float> const &entry, xrange_t xrange, bool thick, uint32_t (*colorf)(time_t const &));
        BarSeries(bool below_grid, forecast_entry_t<float> const &entry, xrange_t xrange, bool thick, uint32_t (*colorf)(time_t const &));
        BarSeries(forecast_entry_t<float> const &entry, xrange_t xrange, bool thick, uint32_t color, uint32_t (*colorf)(time_t const &) = nullptr);
        BarSeries(bool below_grid, forecast_entry_t<float> const &entry, xrange_t xrange, bool thick, uint32_t color, uint32_t (*colorf)(time_t const &) = nullptr);
        BarSeries(forecast_entry_t<float> const &entry, float scale, xrange_t xrange, bool thick, uint32_t (*colorf)(time_t const &));
        BarSeries(bool below_grid, forecast_entry_t<float> const &entry, float scale, xrange_t xrange, bool thick, uint32_t (*colorf)(time_t const &));
        BarSeries(forecast_entry_t<float> const &entry, float scale, xrange_t xrange, bool thick, uint32_t color, uint32_t (*colorf)(time_t const &) = nullptr);
        BarSeries(bool below_grid, forecast_entry_t<float> const &entry, float scale, xrange_t xrange, bool thick, uint32_t color, uint32_t (*colorf)(time_t const &) = nullptr);
        BarSeries(forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, xrange_t xrange, bool thick, uint32_t (*colorf)(time_t const &));
        BarSeries(bool below_grid, forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, xrange_t xrange, bool thick, uint32_t (*colorf)(time_t const &));
        BarSeries(forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, xrange_t xrange, bool thick, uint32_t color, uint32_t (*colorf)(time_t const &) = nullptr);
        BarSeries(bool below_grid, forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, xrange_t xrange, bool thick, uint32_t color, uint32_t (*colorf)(time_t const &) = nullptr);
        BarSeries(forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, float scale, xrange_t xrange, bool thick, uint32_t color, uint32_t (*colorf)(time_t const &) = nullptr);
        BarSeries(bool below_grid, forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, float scale, xrange_t xrange, bool thick, uint32_t color, uint32_t (*colorf)(time_t const &) = nullptr);

        ~BarSeries() = default;

        void plot(M5Canvas *canvas, int32_t x0, int32_t y0, time_t x0val, float y0val, float xscale, float yscale) const;
    };

    class AreaSeries : public BiSeries
    {
    private:
        uint32_t color;

    public:
        AreaSeries(forecast_entry_t<float> const &entry, xrange_t xrange, uint32_t color);
        AreaSeries(bool below_grid, forecast_entry_t<float> const &entry, xrange_t xrange, uint32_t color);
        AreaSeries(forecast_entry_t<float> const &entry, float scale, xrange_t xrange, uint32_t color);
        AreaSeries(bool below_grid, forecast_entry_t<float> const &entry, float scale, xrange_t xrange, uint32_t color);
        AreaSeries(forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, xrange_t xrange, uint32_t color);
        AreaSeries(bool below_grid, forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, xrange_t xrange, uint32_t color);
        AreaSeries(forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, float scale, xrange_t xrange, uint32_t color);
        AreaSeries(bool below_grid, forecast_entry_t<float> const &from_entry, forecast_entry_t<float> const &to_entry, float scale, xrange_t xrange, uint32_t color);

        ~AreaSeries() = default;

        void plot(M5Canvas *canvas, int32_t x0, int32_t y0, time_t x0val, float y0val, float xscale, float yscale) const;
    };

private:
    time_t last_drawn_forecast_timestamp;

    static xsteps_t getXsteps(xrange_t xrange);
    yrange_t getYrange(std::vector<std::unique_ptr<Series>> const &series) const;
    static float getYstep(yrange_t yrange);
    static ysteps_t getYsteps(yrange_t yrange);
    void plotNights(xrange_t xrange, int32_t x0, float xscale);
    void plotGrid(xsteps_t &xsteps, ysteps_t &ysteps, int32_t x0, int32_t y0, time_t x0val, float y0val, float xscale, float yscale);

protected:
    virtual xrange_t getXrange() const = 0;
    virtual yrange_t getSoftYrange() const = 0;
    virtual yrange_t getHardYrange() const = 0;
    virtual char *getUnitLabel() const = 0;
    virtual void fillSeries(xrange_t xrange, std::vector<std::unique_ptr<Series>> &series) = 0;

    ForecastPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth);

public:
    bool redraw();

    Panel *touch(int16_t x, int16_t y);
};