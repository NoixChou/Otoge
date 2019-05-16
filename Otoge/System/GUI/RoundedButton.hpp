#pragma once
#include "Button.hpp"

class RoundedButton : public Button
{
public:
    RoundedButton(const std::string& label, const ScreenData& layoutScreen, const std::shared_ptr<FlexibleScaler>& parentScaler);

    void Draw() override;
};
