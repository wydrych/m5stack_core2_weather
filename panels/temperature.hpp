#pragma once

#include "forecast.hpp"

class TemperatureForecastPanel : public ForecastPanel
{
protected:
    xrange_t getXrange()
    {
        return {forecast.airtmp_point.start, forecast.airtmp_point.start + settings::forecast::plot::x_span};
    }

    void fillSeries(xrange_t xrange, std::vector<std::unique_ptr<Series>> &series)
    {
        series.reserve(2);

        std::unique_ptr<Series> wchill_series(new LineSeries(forecast.wchill_point, xrange, settings::colors::plot::wchill));
        series.push_back(std::move(wchill_series));

        std::unique_ptr<Series> airtmp_series(new LineSeries(forecast.airtmp_point, xrange, settings::colors::plot::airtmp));
        series.push_back(std::move(airtmp_series));
    };

    yrange_t getYrange(std::vector<std::unique_ptr<Series>> const &series)
    {
        float min = 0;
        float max = 0;
        for (std::unique_ptr<Series> const &s : series)
        {
            float smin = s->getMin();
            float smax = s->getMax();
            if (smin < min)
                min = smin;
            if (smax > max)
                max = smax;
        }
        min = min * 1.1;
        max = max * 1.1;
        if (min > 0 || min == 0 && max >= 1)
            return {0, max};
        if (max < 0 || max == 0 && min <= -1)
            return {min, 0};
        if (max < 1)
            max = 1;
        if (min > -1)
            min = -1;
        return {min, max};
    };

public:
    TemperatureForecastPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth)
        : ForecastPanel(parentCanvas, w, h, depth)
    {
    }
};
