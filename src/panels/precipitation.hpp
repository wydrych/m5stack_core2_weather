#pragma once

#include "forecast.hpp"

class PrecipitationForecastPanel : public ForecastPanel
{
protected:
    xrange_t getXrange() const;
    static uint32_t getColor(time_t const &t);
    void fillSeries(xrange_t xrange, std::vector<std::unique_ptr<Series>> &series);
    yrange_t getSoftYrange() const;
    yrange_t getHardYrange() const;
    char *getUnitLabel() const;

public:
    PrecipitationForecastPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth);
};
