#pragma once

#include "forecast.hpp"

class PressureForecastPanel : public ForecastPanel
{
protected:
    std::pair<time_t, time_t> getXrange()
    {
        return std::make_pair(forecast.trpres_point.start, forecast.trpres_point.start + settings::forecast::plot::x_span);
    }

    std::vector<Series> getSeries(std::pair<time_t, time_t> xrange)
    {
        // TODO
        return std::vector<Series>();
    };
    std::pair<float, float> getYrange(std::vector<Series> &series)
    {
        // TODO
        return std::make_pair(923, 1014);
    };

public:
    PressureForecastPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth)
        : ForecastPanel(parentCanvas, w, h, depth)
    {
    }
};
