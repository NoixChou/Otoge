#include "SpringButton.hpp"
#include "../../Util/Calculate/Animation/Easing.hpp"

SpringButton::SpringButton(const std::string& label, const ScreenData& layoutScreen, const std::shared_ptr<FlexibleScaler>& parentScaler) : Button(label, layoutScreen, parentScaler)
{
}

void SpringButton::GUIUpdate(float deltaTime)
{
    Button::GUIUpdate(deltaTime);

    const float l_TotalTime = 0.4f;
    if (IsBeginOnMouse() || IsEndOnMouse() || IsDownMouse())
    {
        timerCount = 0.f;
        MovingSize_ = true;
    }

    if (MovingSize_)
    {
        if (IsOnMouse())
        {
            RawSizeX_ = Easing::OutExp(timerCount, l_TotalTime, 0.0, DefaultScaler_->CalculateWidth(hoverAnimationAmount));
            RawSizeY_ = Easing::OutExp(timerCount, l_TotalTime, 0.0, DefaultScaler_->CalculateHeight(hoverAnimationAmount));
        }
        if (!IsOnMouse() || IsHoldMouse())
        {
            RawSizeX_ = Easing::OutBounce(timerCount, l_TotalTime, DefaultScaler_->CalculateWidth(hoverAnimationAmount), 0.0);
            RawSizeY_ = Easing::OutBounce(timerCount, l_TotalTime, DefaultScaler_->CalculateHeight(hoverAnimationAmount), 0.0);
        }

        if (l_TotalTime <= timerCount)
        {
            timerCount = l_TotalTime;
            MovingSize_ = false;
        }
        GetTextLabelInstance()->SetPositionX(DefaultScaler_->CalculatePositionRateX(RawSizeX_));
        GetTextLabelInstance()->SetPositionY(DefaultScaler_->CalculatePositionRateY(RawSizeY_));
        GetTextLabelInstance()->SetScreenWidth(100.f - DefaultScaler_->CalculatePositionRateX(RawSizeX_) * 2.f);
        GetTextLabelInstance()->SetScreenHeight(100.f - DefaultScaler_->CalculatePositionRateY(RawSizeY_) * 2.f);
    }
    else
    {
        if (!IsOnMouse() || IsHoldMouse())
        {
            RawSizeX_ = DefaultScaler_->CalculateWidth(hoverAnimationAmount);
            RawSizeY_ = DefaultScaler_->CalculateHeight(hoverAnimationAmount);
        }

        if (IsOnMouse() || !IsRawEnable())
        {
            RawSizeX_ = 0.f;
            RawSizeY_ = 0.f;
        }
    }
}