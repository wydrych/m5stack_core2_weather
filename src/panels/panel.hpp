#pragma once

#include <M5GFX.h>

class PanelBase
{
protected:
    PanelBase(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth);

public:
    M5Canvas *const canvas;
    virtual bool redraw() = 0;
};

class Panel : public PanelBase
{
protected:
    Panel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth);

public:
    virtual Panel *touch(int16_t x, int16_t y) = 0;
};
