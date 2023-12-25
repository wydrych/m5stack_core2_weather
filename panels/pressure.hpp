#pragma once

#include <cmath>
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
        std::unique_ptr<Series> trpres_series(new LineSeries(forecast.trpres_point, 0.01, xrange, true, settings::colors::plot::trpres));
        series.push_back(std::move(trpres_series));
    }

    yrange_t getSoftYrange() const
    {
        // see https://en.wikipedia.org/wiki/Barometric_formula

        // reference pressure (Pa)
        const float Pb = 101325;
        // reference temperature (K)
        const float Tb = forecast.airtmp_point.points[0] + 273.15;
        // height at which pressure is calculated (m)
        const float h = settings::forecast::alt;
        // height of reference level b (m)
        const float hb = 0;
        // temperature lapse rate (K/m)
        const float Lb = 0.0065; // hb < 11000
        // gravitational acceleration (m/s^2)
        const float g0 = 9.80665;
        // molar mass of Earth's air (kg/mol)
        const float M = 0.0289644;
        // universal gas constant (J/(mol*K))
        const float R = 8.3144598;

        const float P = Pb * pow(1 - (h - hb) * Lb / Tb, g0 * M / (R * Lb));

        return {P / 100, P / 100};
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
