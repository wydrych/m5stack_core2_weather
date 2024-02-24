#pragma once

#include <vector>
#include <M5GFX.h>
#include "lang.hpp"

class Settings
{
public:
    const Lang lang;
    const int32_t header_height;
    const uint32_t watchdog_timer;
    const unsigned long panel_timeout;

    class Time
    {
    public:
        const char *const ntpServer;
        const char *const tz;
        Time();
    };
    const Time time;

    class Wifi
    {
    public:
        const char *const ssid;
        const char *const password;
        Wifi();
    };
    const Wifi wifi;

    class Mqtt
    {
    private:
        char *getClientName(const char *prefix) const;

    public:
        const char *const server;
        const uint16_t port;
        const char *const user;
        const char *const password;
        const char *const client_name;
        const char *const topic;
        const unsigned int reconnect;
        Mqtt();
    };
    const Mqtt mqtt;

    class Forecast
    {
    public:
        const char *const base_url;
        const char *const available;
        const char *const model;
        const char *const root_ca;
        const unsigned long retry;
        const unsigned long refresh;
        const float lat;
        const float lon;
        const float alt;
        const time_t expiry;

        class Plot
        {
        public:
            const int32_t margin;
            const time_t x_span;
            const time_t x_step;
            const char *const x_step_format;
            const std::vector<float> y_steps;
            const char *const y_step_format;
            const int32_t label_height;
            const int32_t grid_pattern[2];
            Plot();
        };
        const Plot plot;

        Forecast();
    };
    const Forecast forecast;

    class Colors
    {
    public:
        const uint32_t text;
        const uint32_t background;

        class Icons
        {
        public:
            const uint32_t down;
            const uint32_t wifi;
            const uint32_t mqtt;
            const uint32_t meteo;
            Icons();
        };
        const Icons icons;

        class Plot
        {
        public:
            const uint32_t border;
            const uint32_t night;
            const uint32_t grid;
            const uint32_t airtmp;
            const uint32_t airtmp_min_max;
            const uint32_t wchill;
            const uint32_t grdtmp_min_max;
            const std::vector<uint32_t> pcpttl;
            const uint32_t trpres;
            Plot();
        };
        const Plot plot;

        Colors();
    };
    const Colors colors;

    class Fonts
    {
    public:
        const m5gfx::IFont *const currentTime;
        const m5gfx::IFont *const temperature;
        const m5gfx::IFont *const humidity;
        const m5gfx::IFont *const pressure;
        const m5gfx::IFont *const plot;
        Fonts();
    };
    const Fonts fonts;

    Settings();
};

extern Settings settings;
