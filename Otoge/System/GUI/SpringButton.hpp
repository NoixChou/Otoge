#pragma once
#include "Button.hpp"

class SpringButton : public Button
{
public:
    float hoverAnimationAmount = 4.f;

    SpringButton(const std::string& label, const ScreenData& layoutScreen, const std::shared_ptr<FlexibleScaler>& parentScaler);

    void GUIUpdate(float deltaTime) override;
};
