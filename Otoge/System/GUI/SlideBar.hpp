#pragma once
#include "GUI.hpp"
#include "../Task/DrawableTask.hpp"

class SlideBar : public GUI
{
private:
    float BeforeChangePosition_ = 0.f;
    float BallPosition_ = 0.f;
    float OldSlideValue_ = 0.f;
    float SlideValue_ = 0.f;
    float MinValue_ = 0.f;
    float MaxValue_ = 100.f;
    bool HasChanged_ = false;
public:
    float ballSize = 12.f;
    unsigned barColor;
    unsigned animationColor;
    SlideBar(const std::string& label, const ScreenData& layoutScreen,
             std::shared_ptr<FlexibleScaler> parentScaler = nullptr);
    ~SlideBar();
    void GUIUpdate(float deltaTime) override;
    void Draw() override;
    float GetValue() const;
    float GetValueRate() const;
    float GetMinValue() const;
    float GetMaxValue() const;
    void SetValue(float value);
    void SetMinValue(float min);
    void SetMaxValue(float max);
    bool IsChanged();
};
