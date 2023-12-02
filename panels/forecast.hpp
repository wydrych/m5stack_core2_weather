#pragma once

#include "panel.hpp"
#include "main.hpp"

class ForecastPanel : public Panel
{

public:
    ForecastPanel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth)
        : Panel(parentCanvas, w, h, depth)
    {
    }

    virtual bool redraw()
    {
        return false;
    }

    virtual Panel *touch(int16_t x, int16_t y)
    {
        // return to main panel
        return nullptr;
    }
};