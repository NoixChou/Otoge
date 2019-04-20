#pragma once
#include "GUI.hpp"
#include "../Task/DrawableTask.hpp"

class ScrollablePanel : public GUI
{
private:

public:
    unsigned animationColor;

    ScrollablePanel(const std::string& label, const ScreenData& layoutScreen, std::shared_ptr<FlexibleScaler> parentScaler = nullptr);
    ~ScrollablePanel();

    void GUIUpdate(float deltaTime) override;
    void Draw() override;
};