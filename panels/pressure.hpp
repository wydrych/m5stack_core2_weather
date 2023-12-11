#pragma once

#include "forecast.hpp"

class PressureForecastPanel : public ForecastPanel
{
protected:
    std::vector<Series> getSeries(time_t xmin, time_t ymax)
    {
        // TODO
        return std::vector<Series>();
    };
    std::pair<float, float> getYrange(std::vector<Series> &series)
    {
        // TODO
        return std::make_pair(0, 1);
    };

public:
    PressureForecastPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth)
        : ForecastPanel(parentCanvas, w, h, depth)
    {
    }
};
