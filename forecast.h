#pragma once

template <typename T>
struct forecast_entry_t
{
    time_t start;
    time_t interval;
    std::vector<T> points;
};

struct forecast_t
{
    forecast_entry_t<float> airtmp_point;
    forecast_entry_t<float> wchill_point;
    forecast_entry_t<float> pcpttl_aver;
    forecast_entry_t<float> pcpttl_max;
    forecast_entry_t<uint8_t> pcpttl_type_max;
    forecast_entry_t<float> trpres_point;
};

extern forecast_t forecast;
extern time_t forecast_timestamp;

void on_forecast_successfull_fetch(time_t);
