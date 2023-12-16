#pragma once

#include "forecast.hpp"

class TemperatureForecastPanel : public ForecastPanel
{
protected:
    xrange_t getXrange()
    {
        return {forecast.airtmp_point.start, forecast.airtmp_point.start + settings::forecast::plot::x_span};
        // return std::make_pair(forecast.airtmp_point.start, forecast.airtmp_point.start + settings::forecast::plot::x_span);
    }

    std::vector<Series> getSeries(xrange_t xrange)
    {
        // TODO
        return std::vector<Series>();
    };

    yrange_t getYrange(std::vector<Series> &series)
    {
        // TODO
        return {0, 32};
    };

public:
    TemperatureForecastPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth)
        : ForecastPanel(parentCanvas, w, h, depth)
    {
    }
};
