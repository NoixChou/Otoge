#include "ScrollablePanel.hpp"
#include "../Input/MouseManager.hpp"
#include "../../Util/Calculate/Animation/Easing.hpp"

ScrollablePanel::ScrollablePanel(const std::string& label, const ScreenData& layoutScreen, const ScreenData& panelScreen, std::shared_ptr<FlexibleScaler> parentScaler) : GUI(label + "<ScrollablePanel>", layoutScreen, parentScaler)
{
    Label_ = label;
    baseColor = GetColor(230, 230, 230);

    Panel_ = std::make_shared<Scene>(label + "<panel>", panelScreen, DefaultScaler_);
    AddChildTask(std::static_pointer_cast<Task>(Panel_));
}

ScrollablePanel::~ScrollablePanel()
{

}

void ScrollablePanel::GUIUpdate(float deltaTime)
{
    float l_TotalTime = 0.5f;

    if(IsOnMouse())
    {
        float mouseVel = MouseManager::GetInstance()->GetMouseWheelAccel();
        if (mouseVel != 0.f)
        {
            BeforeChangePosition_ = ScrollPosition_;
            timerCount = 0.f;
        }
        ScrollPosition_ += mouseVel * 12.f;
        ReCalculateScreen();
        RefreshDrawBuffer();
    }

    Panel_->SetPositionY(Easing::OutExp(timerCount, l_TotalTime, ScrollPosition_, BeforeChangePosition_));

    if(ScrollPosition_ < -GetScreenHeight())
    {
        BeforeChangePosition_ = ScrollPosition_;
        ScrollPosition_  = -GetScreenHeight();
    }
    if(ScrollPosition_ > 0.f)
    {
        BeforeChangePosition_ = ScrollPosition_;
        ScrollPosition_ = 0.f;
    }

    if(timerCount > l_TotalTime)
    {
        timerCount = l_TotalTime;
    }
}

void ScrollablePanel::Draw()
{
    ScreenData d = DefaultScaler_->Calculate(0.f, ScrollPosition_ / Panel_->GetScreenHeight(), 2.f, 5.f);
}

std::shared_ptr<Scene> ScrollablePanel::GetPanelInstance()
{
    return Panel_;
}
