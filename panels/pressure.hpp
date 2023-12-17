#pragma once

#include "forecast.hpp"

class PressureForecastPanel : public ForecastPanel
{
protected:
    xrange_t getXrange()
    {
        return {forecast.trpres_point.start, forecast.trpres_point.start + settings::forecast::plot::x_span};
    }

    void fillSeries(xrange_t xrange, std::vector<std::unique_ptr<Series>> &series)
    {
        // TODO
    };
    yrange_t getYrange(std::vector<std::unique_ptr<Series>> const &series)
    {
        // TODO
        return {923, 1014};
    };

public:
    PressureForecastPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth)
        : ForecastPanel(parentCanvas, w, h, depth)
    {
    }
};
