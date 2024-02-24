#pragma once

#include <vector>

template <typename T>
struct forecast_entry_t
{
    time_t start;
    time_t interval;
    std::vector<T> points;
};

struct forecast_t
{
    time_t timestamp;
    forecast_entry_t<float> airtmp_point;
    forecast_entry_t<float> airtmp_min;
    forecast_entry_t<float> airtmp_max;
    forecast_entry_t<float> wchill_point;
    forecast_entry_t<float> grdtmp_min;
    forecast_entry_t<float> grdtmp_max;
    forecast_entry_t<float> pcpttl_aver;
    forecast_entry_t<float> pcpttl_max;
    forecast_entry_t<uint8_t> pcpttl_type_max;
    forecast_entry_t<float> trpres_point;
};

extern forecast_t forecast;

void forecast_loop();
