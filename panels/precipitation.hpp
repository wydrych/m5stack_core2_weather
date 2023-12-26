#pragma once

#include "forecast.hpp"

class PrecipitationForecastPanel : public ForecastPanel
{
protected:
    xrange_t getXrange() const
    {
        return {forecast.pcpttl_aver.start, forecast.pcpttl_aver.start + settings::forecast::plot::x_span};
    }

    static uint32_t getColor(time_t const &t)
    {
        uint8_t type = forecast.pcpttl_type_max.points[(t - forecast.pcpttl_type_max.start + forecast.pcpttl_type_max.interval / 2) / forecast.pcpttl_type_max.interval];
        if (type < sizeof(settings::colors::plot::pcpttl) / sizeof(settings::colors::plot::pcpttl[0]))
            return settings::colors::plot::pcpttl[type];
        else
            return settings::colors::plot::pcpttl[0];
    }

    void fillSeries(xrange_t xrange, std::vector<std::unique_ptr<Series>> &series)
    {
        std::unique_ptr<Series> pcpttl_aver_series(new BarSeries({forecast.pcpttl_aver.start - forecast.pcpttl_aver.interval / 2, forecast.pcpttl_aver.interval, forecast.pcpttl_aver.points}, xrange, true, 0, getColor));
        series.push_back(std::move(pcpttl_aver_series));

        std::unique_ptr<Series> pcpttl_max_series(new BarSeries({forecast.pcpttl_max.start - forecast.pcpttl_max.interval / 2, forecast.pcpttl_max.interval, forecast.pcpttl_max.points}, xrange, false, 0, getColor));
        series.push_back(std::move(pcpttl_max_series));
    }

    yrange_t getSoftYrange() const
    {
        return {NAN, 5};
    }

    yrange_t getHardYrange() const
    {
        return {0, NAN};
    }

public:
    PrecipitationForecastPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth)
        : ForecastPanel(parentCanvas, w, h, depth)
    {
    }
};
