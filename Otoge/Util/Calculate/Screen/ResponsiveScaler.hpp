#pragma once
#include "../ScreenData.h"

class FlexibleScaler
{
private:
    float ScreenWidth_;
    float ScreenHeight_;
    float Scale_;

    static std::shared_ptr<FlexibleScaler> GlobalInstance_;

public:
    bool lockTop = true;
    bool lockBottom = false;
    bool lockLeft = true;
    bool lockRight = false;

    FlexibleScaler(float screenWidth, float screenHeight, float scale = 1.0f);
    ~FlexibleScaler();

    static std::shared_ptr<FlexibleScaler> GetWindowBasedInstance();
    static void CreateWindowBasedInstance();
    static void DestroyWindowBasedInstance();

    ScreenData Calculate(const ScreenData *dataOfPercent); // 百分率から実際の画面サイズに基づく割合に調整
};
