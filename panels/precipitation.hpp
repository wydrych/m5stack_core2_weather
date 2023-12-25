#pragma once

#include "forecast.hpp"

class PrecipitationForecastPanel : public ForecastPanel
{
protected:
    xrange_t getXrange() const
    {
        return {forecast.pcpttl_aver.start, forecast.pcpttl_aver.start + settings::forecast::plot::x_span};
    }

    void fillSeries(xrange_t xrange, std::vector<std::unique_ptr<Series>> &series)
    {
        std::unique_ptr<Series> pcpttl_aver_series(new BarSeries(forecast.pcpttl_aver, xrange, true, settings::colors::plot::pcpttl));
        series.push_back(std::move(pcpttl_aver_series));

        std::unique_ptr<Series> pcpttl_max_series(new BarSeries(forecast.pcpttl_max, xrange, false, settings::colors::plot::pcpttl));
        series.push_back(std::move(pcpttl_max_series));
    }

    yrange_t getSoftYrange() const
    {
        return {NAN, NAN};
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
