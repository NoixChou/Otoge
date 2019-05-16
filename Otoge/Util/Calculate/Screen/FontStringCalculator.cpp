#include "FontStringCalculator.hpp"
#include "../../../System/GlobalMethod.hpp"

/* このクラスをインスタンス化する必要性がないためコメントアウト
std::shared_ptr<FontStringCalculator> FontStringCalculator::Instance_ = nullptr;

FontStringCalculator::FontStringCalculator()
{

}

std::shared_ptr<FontStringCalculator> FontStringCalculator::GetInstance()
{
    return Instance_;
}

void FontStringCalculator::CreateInstance()
{
    if (!Instance_)
    {
        Instance_.reset(new FontStringCalculator);
    }
}

void FontStringCalculator::DestroyInstance()
{
    Instance_.reset();
}
*/
float FontStringCalculator::GetStringWidth(int fontHandle, const std::string& string)
{
    const int l_Length = static_cast<int>(string.length());
    if(fontHandle != -1) return engine::CastToFloat(GetDrawStringWidthToHandle(string.c_str(), l_Length, fontHandle));
    return engine::CastToFloat(GetDrawStringWidth(string.c_str(), l_Length));
}

float FontStringCalculator::GetStringHeight(int fontHandle)
{
    if(fontHandle != -1) return engine::CastToFloat(GetFontLineSpaceToHandle(fontHandle));
    return engine::CastToFloat(GetFontLineSpace());
}

float FontStringCalculator::GetStringCenterHorizontal(int fontHandle, const std::string& string)
{
    return GetStringWidth(fontHandle, string) / 2.0f;
}

float FontStringCalculator::GetStringCenterVertical(int fontHandle)
{
    return GetStringHeight(fontHandle) / 2.0f;
}

VECTOR FontStringCalculator::GetStringCenterInBox(int fontHandle, const std::string& string,
                                                  const ScreenData& screenBox)
{
    VECTOR result;
    result.x = (screenBox.width / 2) - GetStringCenterHorizontal(fontHandle, string);
    result.y = (screenBox.height / 2) - GetStringCenterVertical(fontHandle);
    result.z = 0.f;
    return result;
}
