#pragma once

#include <M5GFX.h>

class Panel
{
protected:
    Panel(M5Canvas *canvas) : canvas(canvas) {}

public:
    M5Canvas *const canvas;
    virtual bool redraw() = 0;
};