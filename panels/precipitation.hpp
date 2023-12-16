#pragma once

#include "forecast.hpp"

class PrecipitationForecastPanel : public ForecastPanel
{
protected:
    xrange_t getXrange()
    {
        return {forecast.pcpttl_aver.start, forecast.pcpttl_aver.start + settings::forecast::plot::x_span};
    }

    std::vector<Series> getSeries(xrange_t xrange)
    {
        // TODO
        return std::vector<Series>();
    };
    yrange_t getYrange(std::vector<Series> &series)
    {
        // TODO
        return {0, 6};
    };

public:
    PrecipitationForecastPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth)
        : ForecastPanel(parentCanvas, w, h, depth)
    {
    }
};
