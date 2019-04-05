#include "FontStringCalculator.hpp"

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

float FontStringCalculator::GetFontStringCenterHorizontal(const int fontHandle, const std::string& string)
{
    if(fontHandle != -1)
        return static_cast<float>(GetDrawStringWidthToHandle(string.c_str(), string.length(), fontHandle)) / 2.0f;

    return static_cast<float>(GetDrawStringWidth(string.c_str(), string.length())) / 2.0f;
}

float FontStringCalculator::GetFontStringCenterVertical(int fontHandle)
{
    if(fontHandle != -1)
        return static_cast<float>(GetFontLineSpaceToHandle(fontHandle)) / 2.0f;

    return static_cast<float>(GetFontLineSpace()) / 2.0f;
}

VECTOR FontStringCalculator::GetFontCenterInBox(int fontHandle, const std::string& string, const ScreenData& screenBox)
{
    VECTOR result;
    result.x = (screenBox.width / 2) - GetFontStringCenterHorizontal(fontHandle, string);
    result.y = (screenBox.height / 2) - GetFontStringCenterVertical(fontHandle);
    result.z = 0.f;
    return result;
}
