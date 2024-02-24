#include "panel.hpp"
#include "../settings.hpp"

PanelBase::PanelBase(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth)
    : canvas(new M5Canvas(parentCanvas))
{
    canvas->setColorDepth(depth);
    canvas->setBaseColor(settings.colors.background);
    canvas->setTextColor(settings.colors.text, settings.colors.background);
    canvas->createSprite(w, h);
}

Panel::Panel(M5Canvas *parentCanvas, int32_t w, int32_t h, lgfx::v1::color_depth_t depth)
    : PanelBase(parentCanvas, w, h, depth) {}
