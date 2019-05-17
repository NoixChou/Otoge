#include "RoundedButton.hpp"
#include "../GlobalMethod.hpp"
#include "../../Util/Calculate/Animation/Easing.hpp"

RoundedButton::RoundedButton(const std::string& label, const ScreenData& layoutScreen, const std::shared_ptr<FlexibleScaler>& parentScaler) : Button(label, layoutScreen, parentScaler)
{
}

void RoundedButton::Draw()
{
    if (isDrawBase)
    {
        //static float RawSizeX_ = 0.f;
        //static float RawSizeY_ = 0.f;
        const float totalTime = 0.4f;
        if(IsBeginOnMouse() || IsEndOnMouse() || IsDownMouse())
        {
            timerCount = 0.f;
            MovingSize_ = true;
        }

        if(MovingSize_)
        {
            if (IsOnMouse())
            {
                RawSizeX_ = Easing::OutExp(timerCount, totalTime, 0.0, DefaultScaler_->CalculateWidth(2.f));
                RawSizeY_ = Easing::OutExp(timerCount, totalTime, 0.0, DefaultScaler_->CalculateHeight(2.f));
            }
            if(!IsOnMouse() || IsHoldMouse())
            {
                RawSizeX_ = Easing::OutBounce(timerCount, totalTime, DefaultScaler_->CalculateWidth(2.f), 0.0);
                RawSizeY_ = Easing::OutBounce(timerCount, totalTime, DefaultScaler_->CalculateHeight(2.f), 0.0);
            }

            if(totalTime <= timerCount)
            {
                timerCount = totalTime;
                MovingSize_ = false;
                
            }
        }

        DrawRoundRect(engine::CastToInt(RawSizeX_), engine::CastToInt(RawSizeY_), engine::CastToInt(GetRawScreenWidth() - RawSizeX_), engine::CastToInt(GetRawScreenHeight() - RawSizeY_), engine::CastToInt(GetRawScreenWidth()) / 20, engine::CastToInt(GetRawScreenWidth()) / 20, baseColor, TRUE);
    }
}
