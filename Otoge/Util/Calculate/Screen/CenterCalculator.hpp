#pragma once
#include "../ScreenData.h"

class FontStringCalculator
{
private:
    /*
    static std::shared_ptr<FontStringCalculator> Instance_;
    FontStringCalculator();
    */

public:
    /*
    static std::shared_ptr<FontStringCalculator> GetInstance();
    static void CreateInstance();
    static void DestroyInstance();
    */

    static float GetFontStringCenterHorizontal(int fontHandle, const std::string &string);
    static float GetFontStringCenterVertical(int fontHandle);

    static VECTOR GetFontCenterInBox(int fontHandle, const std::string &string, const ScreenData& screenBox);
};
