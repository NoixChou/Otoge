#pragma once
#include "GUI.hpp"
#include "../Task/DrawableTask.hpp"

class SlideBar : public GUI
{
private:

public:
    unsigned animationColor;

    SlideBar(const std::string& label, const ScreenData& layoutScreen, std::shared_ptr<FlexibleScaler> parentScaler = nullptr);
    ~SlideBar();

    void GUIUpdate(float deltaTime) override;
    void Draw() override;
};