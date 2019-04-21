#include "SlideBar.hpp"
#include "../Input/KeyboardManager.hpp"
#include "../Input/MouseManager.hpp"
#include "../../Util/Calculate/Animation/Easing.hpp"

SlideBar::SlideBar(const std::string& label, const ScreenData& layoutScreen, std::shared_ptr<FlexibleScaler> parentScaler) : GUI(label + "\"<SlideBar>\"", layoutScreen, parentScaler)
{
    Label_ = label;
    barColor = GetColor(88, 88, 88);
    baseColor = GetColor(230, 230, 230);
}

SlideBar::~SlideBar()
{
}

void SlideBar::GUIUpdate(float deltaTime)
{
    OldSlideValue_ = SlideValue_;

    float l_TotalTime = 0.4f;
    float l_BarLen = GetRawScreenWidth() - DefaultScaler_->CalculatePositionX(ballSize);
    float l_CurrentPosition = DefaultScaler_->CalculatePositionX((ballSize / 2.f)) + ((l_BarLen * (GetValueRate() / 100.f)));

    BallPosition_ = Easing::OutExp(timerCount, l_TotalTime, l_CurrentPosition, BeforeChangePosition_);


    if (IsOnMouse())
    {
        float l_MoveSpeed = (MaxValue_ / 12.f);
        if (KeyboardManager::GetInstance()->IsDownKey(KEY_INPUT_LEFT))
        {
            SlideValue_ -= l_MoveSpeed;
        }
        if (KeyboardManager::GetInstance()->IsDownKey(KEY_INPUT_RIGHT))
        {
            SlideValue_ += l_MoveSpeed;
        }
        //SlideValue_ += MouseManager::GetInstance()->GetMouseWheelAccel() * l_MoveSpeed;
    }

    if(IsHoldMouse())
    {
        SlideValue_ = (MaxValue_) * ( (( MouseManager::GetInstance()->GetMouseRateX(DefaultScaler_) - DefaultScaler_->CalculatePositionRateX(DefaultScaler_->GetDiffX()) ) / (100.f - ballSize)) - ((ballSize / 2.0f) / 100.f));
    }

    if (SlideValue_ > MaxValue_) SlideValue_ = MaxValue_;
    if (SlideValue_ < MinValue_) SlideValue_ = MinValue_;

    //Logger_->Debug("SlideValue: " + std::to_string(SlideValue_) + ", Old: " + std::to_string(OldSlideValue_));

    if (OldSlideValue_ != SlideValue_)
    {
        BeforeChangePosition_ = BallPosition_;
        timerCount = 0.f;
        HasChanged_ = true;
    }
    else
        HasChanged_ = false;

    if(timerCount > l_TotalTime)
    {
        timerCount = l_TotalTime;
        BallPosition_ = l_CurrentPosition;
    }
}

void SlideBar::Draw()
{
    DrawLine(
        static_cast<int>(floor(DefaultScaler_->CalculatePositionX(ballSize / 2.f))),
        static_cast<int>(floor(GetRawScreenHeight() / 2.f)),
        static_cast<int>(floor(GetRawScreenWidth() - DefaultScaler_->CalculatePositionX((ballSize / 2.f)))),
        static_cast<int>(floor(GetRawScreenHeight() / 2.f)),
        barColor, static_cast<int>(floor(DefaultScaler_->CalculateHeight(4.0f))));
    DrawCircle(
        static_cast<int>(floor(DefaultScaler_->CalculatePositionX((ballSize / 2.f)))),
        static_cast<int>(floor(GetRawScreenHeight() / 2.f)),
        static_cast<int>(floor(DefaultScaler_->CalculateHeight(8.0f))),
        barColor, TRUE);
    DrawCircle(
        static_cast<int>(floor(GetRawScreenWidth() - DefaultScaler_->CalculatePositionX((ballSize / 2.f)))),
        static_cast<int>(floor(GetRawScreenHeight() / 2.f)),
        static_cast<int>(floor(DefaultScaler_->CalculateHeight(8.0f))),
        barColor, TRUE);

    DrawCircle(
        static_cast<int>(floor(BallPosition_)),
        static_cast<int>(floor(GetRawScreenHeight() / 2.0f)),
        static_cast<int>(floor(DefaultScaler_->CalculateHeight(ballSize * 2.f))),
        baseColor, TRUE
    );
    DrawCircle(
        static_cast<int>(floor(BallPosition_)),
        static_cast<int>(floor(GetRawScreenHeight() / 2.0f)),
        static_cast<int>(floor(DefaultScaler_->CalculateHeight(ballSize * 2.f))),
        barColor, FALSE
    );
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
