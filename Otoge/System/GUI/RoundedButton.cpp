#include "RoundedButton.hpp"
#include "../GlobalMethod.hpp"
#include "../../Util/Calculate/Animation/Easing.hpp"

template<class T>
RoundedButton<T>::RoundedButton(const std::string& label, const ScreenData& layoutScreen, const std::shared_ptr<FlexibleScaler>& parentScaler) : T(label, layoutScreen, parentScaler)
{
}

template<class T>
void RoundedButton<T>::Draw()
{
    if (T::isDrawBase)
    {
        DrawRoundRect(engine::CastToInt(T::RawSizeX_), engine::CastToInt(T::RawSizeY_), engine::CastToInt(T::GetRawScreenWidth() - T::RawSizeX_), engine::CastToInt(T::GetRawScreenHeight() - T::RawSizeY_), engine::CastToInt(T::GetRawScreenWidth()) / 18, engine::CastToInt(T::GetRawScreenWidth()) / 18, T::baseColor, TRUE);
    }
}
