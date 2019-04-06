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

    float GetScreenWidth();
    float GetScreenHeight();
    float GetScale();
    void SetScreenWidth(float width);
    void SetScreenHeight(float height);
    void SetScale(float scale);


	// スクリーンベース座標から割合に変換
	float CalculatePositionRateX(float rawX);
	float CalculatePositionRateY(float rawY);

	// 割合から実際の画面サイズに基づく座標に調整
    float CalculatePositionX(float px);
    float CalculatePositionY(float py);
    float CalculateWidth(float width);
    float CalculateHeight(float height);
    ScreenData Calculate(const ScreenData *dataOfPercent);
    ScreenData Calculate(float px, float py, float width, float height);
};
