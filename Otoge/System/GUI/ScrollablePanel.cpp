#include "ScrollablePanel.hpp"
#include "../Input/MouseManager.hpp"
#include "../../Util/Calculate/Animation/Easing.hpp"
#include "../GlobalMethod.hpp"
#include "../../Util/Visual/Color.hpp"

ScrollablePanel::ScrollablePanel(const std::string& label, const ScreenData& layoutScreen,
                                 const ScreenData& panelScreen, std::shared_ptr<FlexibleScaler> parentScaler) : GUI(
    label + "<ScrollablePanel>", layoutScreen, parentScaler)
{
    Label_ = label;
    baseColor = color_preset::LIGHT_GREY;
    Panel_ = std::make_shared<Scene>(label + "<panel>", panelScreen, DefaultScaler_);
    AddChildTask(std::static_pointer_cast<Task>(Panel_));
}

ScrollablePanel::~ScrollablePanel()
{
}

void ScrollablePanel::GUIUpdate(float deltaTime)
{
    if (GetRawScreenHeight() < Panel_->GetRawScreenHeight())
    {
        float l_TotalTime = 0.5f;
        if (IsOnMouse())
        {
            float mouseVel = MouseManager::GetInstance()->GetMouseWheelAccel();
            if (mouseVel != 0.f)
            {
                ResetAnimation();
            }
            ScrollPosition_ += mouseVel * (Panel_->GetScreenHeight() / 25.f);
        }

        Panel_->SetPositionY(engine::CastToFloat(Easing::OutExp(timerCount, l_TotalTime, ScrollPosition_, BeforeChangePosition_)));

        const float l_ScrollMin = -GetScreenHeight() * (Panel_->GetScreenHeight() / GetScreenHeight() - 1.f) + GetPositionY();

        if (ScrollPosition_ < l_ScrollMin || ScrollPosition_ > 0.f)
        {
            ResetAnimation();
        }
        ScrollPosition_ = engine::LimitRange(ScrollPosition_, l_ScrollMin, 0.f);
        if (timerCount > l_TotalTime)
        {
            timerCount = l_TotalTime;
        }
    }
}

void ScrollablePanel::Draw()
{
}

std::shared_ptr<Scene> ScrollablePanel::GetPanelInstance() const
{
    return Panel_;
}

void ScrollablePanel::ResetAnimation()
{
    BeforeChangePosition_ = ScrollPosition_;
    timerCount = 0.f;
}
