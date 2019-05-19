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
    static float GetStringWidth(int fontHandle, const std::string& string);
    static float GetStringHeight(int fontHandle);
    static float GetStringCenterHorizontal(int fontHandle, const std::string& string);
    static float GetStringCenterVertical(int fontHandle);
    static VECTOR GetStringCenterInBox(int fontHandle, const std::string& string, const ScreenData& screenBox);
};
