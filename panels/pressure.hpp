#pragma once

#include "forecast.hpp"

class PressureForecastPanel : public ForecastPanel
{
protected:
    xrange_t getXrange() const
    {
        return {forecast.trpres_point.start, forecast.trpres_point.start + settings::forecast::plot::x_span};
    }

    void fillSeries(xrange_t xrange, std::vector<std::unique_ptr<Series>> &series)
    {
        // TODO
    }

    yrange_t getSoftYrange() const
    {
        return {1013.25, 1013.25};
    }

    yrange_t getHardYrange() const
    {
        return {NAN, NAN};
    }

public:
    PressureForecastPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth)
        : ForecastPanel(parentCanvas, w, h, depth)
    {
    }
};
