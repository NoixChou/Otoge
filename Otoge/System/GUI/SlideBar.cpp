#include "SlideBar.hpp"
#include "../Input/KeyboardManager.hpp"
#include "../Input/MouseManager.hpp"
#include "../../Util/Calculate/Animation/Easing.hpp"
#include "../GlobalMethod.hpp"
#include "../../Util/Visual/Color.hpp"

SlideBar::SlideBar(const std::string& label, const ScreenData& layoutScreen,
                   std::shared_ptr<FlexibleScaler> parentScaler) : GUI(label + "\"<SlideBar>\"", layoutScreen,
                                                                       parentScaler)
{
    Label_ = label;
    barColor = color_preset::DARK_GREY;
    baseColor = color_preset::LIGHT_GREY;
    animationColor = color_preset::GREY;
}

SlideBar::~SlideBar()
{
}

void SlideBar::GUIUpdate(float deltaTime)
{
    OldSlideValue_ = SlideValue_;
    float l_TotalTime = 0.4f;
    float l_BarLen = GetRawScreenWidth() - DefaultScaler_->CalculatePositionX(ballSize);
    float l_CurrentPosition = DefaultScaler_->CalculatePositionX((ballSize / 2.f)) + ((l_BarLen * (GetValueRate() /
        100.f)));
    BallPosition_ = engine::CastToFloat(Easing::OutExp(timerCount, l_TotalTime, l_CurrentPosition,
                                                       BeforeChangePosition_));
    if(IsOnMouse())
    {
        float l_MoveSpeed = (MaxValue_ / 12.f);
        if(KeyboardManager::GetInstance()->IsDownKey(KEY_INPUT_LEFT))
        {
            SlideValue_ -= l_MoveSpeed;
        }
        if(KeyboardManager::GetInstance()->IsDownKey(KEY_INPUT_RIGHT))
        {
            SlideValue_ += l_MoveSpeed;
        }
        //SlideValue_ += MouseManager::GetInstance()->GetMouseWheelAccel() * l_MoveSpeed;
    }
    bool isDown = engine::IsPointInScreen(
        MouseManager::GetInstance()->GetDownPosXf(), MouseManager::GetInstance()->GetDownPosYf(),
        ScreenData(GetRawPositionX() + ParentScaler_->GetDiffX() - DefaultScaler_->CalculatePositionX(GetOriginX()),
            GetRawPositionY() + ParentScaler_->GetDiffY() - DefaultScaler_->CalculatePositionY(GetOriginY()),
            GetRawScreenWidth(),
            GetRawScreenHeight()
        ));
    if(IsHoldMouse() && isDown)
    {
        SlideValue_ = MaxValue_ * (((MouseManager::GetInstance()->GetMouseRateX(DefaultScaler_) - GetOriginX() - DefaultScaler_->CalculatePositionRateX(DefaultScaler_->GetDiffX())) / (100.f - ballSize)) - ((ballSize / 1.75f) / 100.f));
    }
    SlideValue_ = engine::LimitRange(SlideValue_, MinValue_, MaxValue_);

    //Logger_->Debug("SlideValue: " + std::to_string(SlideValue_) + ", Old: " + std::to_string(OldSlideValue_));
    if(OldSlideValue_ != SlideValue_)
    {
        BeforeChangePosition_ = BallPosition_;
        timerCount = 0.f;
        HasChanged_ = true;
    }
    else HasChanged_ = false;
    if(timerCount > l_TotalTime)
    {
        timerCount = l_TotalTime;
        BallPosition_ = l_CurrentPosition;
    }
}

void SlideBar::Draw()
{
    DrawLineAA(DefaultScaler_->CalculatePositionX(ballSize / 2.f),
             GetRawScreenHeight() / 2.f,
             GetRawScreenWidth() - DefaultScaler_->CalculatePositionX((ballSize / 2.f)),
             GetRawScreenHeight() / 2.f, barColor,
             DefaultScaler_->CalculateHeight(4.0f));
    DrawCircleAA(DefaultScaler_->CalculatePositionX((ballSize / 2.f)),
               GetRawScreenHeight() / 2.f, DefaultScaler_->CalculateHeight(8.0f), 12,
               barColor, TRUE);
    DrawCircleAA(GetRawScreenWidth() - DefaultScaler_->CalculatePositionX((ballSize / 2.f)),
               GetRawScreenHeight() / 2.f, DefaultScaler_->CalculateHeight(8.0f), 12,
               barColor, TRUE);
    DrawCircleAA(BallPosition_, GetRawScreenHeight() / 2.0f,
               DefaultScaler_->CalculateHeight(ballSize * 2.f), 24,
               baseColor, TRUE);
    DrawCircleAA(BallPosition_, GetRawScreenHeight() / 2.0f,
               DefaultScaler_->CalculateHeight(ballSize * 2.f), 24,
               barColor, FALSE, 1.8f);
}

float SlideBar::GetValue() const
{
    return SlideValue_;
}

float SlideBar::GetValueRate() const
{
    return ((SlideValue_ + MinValue_) / (MaxValue_ - MinValue_)) * 100.f;
}

float SlideBar::GetMinValue() const
{
    return MinValue_;
}

float SlideBar::GetMaxValue() const
{
    return MaxValue_;
}

void SlideBar::SetValue(float value)
{
    SlideValue_ = value;
}

void SlideBar::SetMinValue(float min)
{
    MinValue_ = min;
}

void SlideBar::SetMaxValue(float max)
{
    MaxValue_ = max;
}

bool SlideBar::IsChanged()
{
    return HasChanged_;
}
