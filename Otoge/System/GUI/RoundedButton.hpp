#pragma once
#include "Button.hpp"
#include "SpringButton.hpp"

template<class T>
class RoundedButton : public T
{
public:
    RoundedButton(const std::string& label, const ScreenData& layoutScreen, const std::shared_ptr<FlexibleScaler>& parentScaler);

    void Draw() override;
};
