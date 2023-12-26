#pragma once

#include "forecast.hpp"

class TemperatureForecastPanel : public ForecastPanel
{
protected:
    xrange_t getXrange() const
    {
        return {forecast.airtmp_point.start, forecast.airtmp_point.start + settings::forecast::plot::x_span};
    }

    yrange_t getSoftYrange() const
    {
        return {0, 0};
    }

    yrange_t getHardYrange() const
    {
        return {NAN, NAN};
    }

    void fillSeries(xrange_t xrange, std::vector<std::unique_ptr<Series>> &series)
    {
        std::unique_ptr<Series> airtmp_min_max_series(new AreaSeries(forecast.airtmp_min, forecast.airtmp_max, xrange, settings::colors::plot::airtmp_min_max));
        series.push_back(std::move(airtmp_min_max_series));

        std::unique_ptr<Series> grdtmp_min_max_series(new BarSeries(forecast.grdtmp_min, forecast.grdtmp_max, xrange, false, settings::colors::plot::grdtmp_min_max));
        series.push_back(std::move(grdtmp_min_max_series));

        std::unique_ptr<Series> wchill_series(new LineSeries(forecast.wchill_point, xrange, true, settings::colors::plot::wchill));
        series.push_back(std::move(wchill_series));

        std::unique_ptr<Series> airtmp_series(new LineSeries(forecast.airtmp_point, xrange, true, settings::colors::plot::airtmp));
        series.push_back(std::move(airtmp_series));
    };

public:
    TemperatureForecastPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth)
        : ForecastPanel(parentCanvas, w, h, depth)
    {
    }
};
