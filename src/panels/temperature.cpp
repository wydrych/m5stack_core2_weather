#include "temperature.hpp"
#include "../settings.hpp"

ForecastPanel::xrange_t TemperatureForecastPanel::getXrange() const
{
    return {forecast.airtmp_point.start, forecast.airtmp_point.start + settings.forecast.plot.x_span};
}

ForecastPanel::yrange_t TemperatureForecastPanel::getSoftYrange() const
{
    return {0, 0};
}

ForecastPanel::yrange_t TemperatureForecastPanel::getHardYrange() const
{
    return {NAN, NAN};
}

char *TemperatureForecastPanel::getUnitLabel() const
{
    return "°C";
}

void TemperatureForecastPanel::fillSeries(ForecastPanel::xrange_t xrange, std::vector<std::unique_ptr<ForecastPanel::Series>> &series)
{
    std::unique_ptr<Series> airtmp_min_max_series(new AreaSeries(true, forecast.airtmp_min, forecast.airtmp_max, xrange, settings.colors.plot.airtmp_min_max));
    series.push_back(std::move(airtmp_min_max_series));

    std::unique_ptr<Series> grdtmp_min_max_series(new BarSeries(forecast.grdtmp_min, forecast.grdtmp_max, xrange, false, settings.colors.plot.grdtmp_min_max));
    series.push_back(std::move(grdtmp_min_max_series));

    std::unique_ptr<Series> wchill_series(new LineSeries(forecast.wchill_point, xrange, true, settings.colors.plot.wchill));
    series.push_back(std::move(wchill_series));

    std::unique_ptr<Series> airtmp_series(new LineSeries(forecast.airtmp_point, xrange, true, settings.colors.plot.airtmp));
    series.push_back(std::move(airtmp_series));
};

TemperatureForecastPanel::TemperatureForecastPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth)
    : ForecastPanel(parentCanvas, w, h, depth) {}
