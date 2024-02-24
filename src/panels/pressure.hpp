#pragma once

#include "forecast.hpp"

class PressureForecastPanel : public ForecastPanel
{
protected:
    xrange_t getXrange() const;
    void fillSeries(xrange_t xrange, std::vector<std::unique_ptr<Series>> &series);
    yrange_t getSoftYrange() const;
    yrange_t getHardYrange() const;
    char *getUnitLabel() const;

public:
    PressureForecastPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth);
};
