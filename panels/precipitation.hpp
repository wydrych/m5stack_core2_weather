#pragma once

#include "forecast.hpp"

class PrecipitationForecastPanel : public ForecastPanel
{
protected:
    std::pair<time_t, time_t> getXrange()
    {
        return std::make_pair(forecast.pcpttl_aver.start, forecast.pcpttl_aver.start + settings::forecast::plot::x_span);
    }

    std::vector<Series> getSeries(std::pair<time_t, time_t> xrange)
    {
        // TODO
        return std::vector<Series>();
    };
    std::pair<float, float> getYrange(std::vector<Series> &series)
    {
        // TODO
        return std::make_pair(0, 6);
    };

public:
    PrecipitationForecastPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth)
        : ForecastPanel(parentCanvas, w, h, depth)
    {
    }
};
