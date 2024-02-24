#pragma once

#include <stdint.h>

struct icon_t
{
    const uint8_t width;
    const uint8_t height;
    const uint8_t *const data;
};

class Icons
{
public:
    const icon_t wifi;
    const icon_t mqtt;
    const icon_t meteo;
    Icons();
};

extern Icons icons;
