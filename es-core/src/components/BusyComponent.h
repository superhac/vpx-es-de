//  SPDX-License-Identifier: MIT
//
//  ES-DE Frontend
//  BusyComponent.h
//
//  Animated busy indicator.
//

#ifndef ES_CORE_COMPONENTS_BUSY_COMPONENT_H
#define ES_CORE_COMPONENTS_BUSY_COMPONENT_H

#include "GuiComponent.h"
#include "components/ComponentGrid.h"
#include "components/NinePatchComponent.h"
#include "components/TextComponent.h"

class AnimatedImageComponent;
class TextComponent;

class BusyComponent : public GuiComponent
{
public:
    BusyComponent();

    void setText(const std::string& text) { mText->setText(text, true); }
    void onSizeChanged() override;

    void reset(); // Reset to frame 0.

private:
    NinePatchComponent mBackground;
    ComponentGrid mGrid;

    std::shared_ptr<AnimatedImageComponent> mAnimation;
    std::shared_ptr<TextComponent> mText;
};

#endif // ES_CORE_COMPONENTS_BUSY_COMPONENT_H
