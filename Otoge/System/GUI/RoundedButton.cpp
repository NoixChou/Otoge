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
        DrawRoundRect(engine::CastToInt(RawSizeX_), engine::CastToInt(RawSizeY_), engine::CastToInt(GetRawScreenWidth() - RawSizeX_), engine::CastToInt(GetRawScreenHeight() - RawSizeY_), engine::CastToInt(GetRawScreenWidth()) / 20, engine::CastToInt(GetRawScreenWidth()) / 20, baseColor, TRUE);
    }
}
