#include "FlexibleScaler.hpp"
#include "../../Setting/SettingManager.h"
#include "../../../System/Config.h"

std::shared_ptr<FlexibleScaler> FlexibleScaler::GlobalInstance_ = nullptr;

FlexibleScaler::FlexibleScaler(float screenWidth, float screenHeight, float scale)
{
    ScreenWidth_ = screenWidth;
    ScreenHeight_ = screenHeight;
    Scale_ = scale;
}

FlexibleScaler::~FlexibleScaler()
{
}

std::shared_ptr<FlexibleScaler> FlexibleScaler::GetWindowBasedInstance()
{
    return GlobalInstance_;
}

void FlexibleScaler::CreateWindowBasedInstance()
{
    if (!GlobalInstance_)
    {
        GlobalInstance_.reset(new FlexibleScaler(SettingManager::GetGlobal()->Get<int>(SETTINGS_RES_WIDTH).get(), SettingManager::GetGlobal()->Get<int>(SETTINGS_RES_HEIGHT).get(), 1.f));
    }
}

void FlexibleScaler::DestroyWindowBasedInstance()
{
    GlobalInstance_.reset();
}

float FlexibleScaler::GetScreenWidth()
{
    return ScreenWidth_;
}

float FlexibleScaler::GetScreenHeight()
{
    return ScreenHeight_;
}

float FlexibleScaler::GetScale()
{
    return Scale_;
}

void FlexibleScaler::SetScreenWidth(float width)
{
    ScreenWidth_ = width;
}

void FlexibleScaler::SetScreenHeight(float height)
{
    ScreenHeight_ = height;
}

void FlexibleScaler::SetScale(float scale)
{
    Scale_ = scale;
}

float FlexibleScaler::CalculatePositionRateX(float rawX)
{
	return rawX / ScreenWidth_ * 100.f * Scale_;
}

float FlexibleScaler::CalculatePositionRateY(float rawY)
{
	return rawY / ScreenHeight_ * 100.f * Scale_;
}

float FlexibleScaler::CalculatePositionX(float px)
{
    return ScreenWidth_ * (px / 100.f) * Scale_;
}

float FlexibleScaler::CalculatePositionY(float py)
{
    return ScreenHeight_ * (py / 100.f) * Scale_;
}

float FlexibleScaler::CalculateWidth(float width)
{
    return ScreenWidth_ * (width / 100.f) * Scale_;
}

float FlexibleScaler::CalculateHeight(float height)
{
    return ScreenHeight_ * (height / 100.f) * Scale_;
}

ScreenData FlexibleScaler::Calculate(const ScreenData *dataOfPercent)
{
    //Logger::LowLevelLog("Calc:\nx" + std::to_string(dataOfPercent->posX) + ",\ny:" + std::to_string(dataOfPercent->posY) + ",\nwidth:" + std::to_string(dataOfPercent->width) + ",\nheight:" + std::to_string(dataOfPercent->height), "DEBUG<flexscaler>");
    ScreenData result;

    result.posX = CalculatePositionX(dataOfPercent->posX);
    result.posY = CalculatePositionY(dataOfPercent->posY);

    if(lockTop && !lockBottom)
    {
        result.height = CalculateHeight(dataOfPercent->height);
    }
    if(!lockTop && lockBottom)
    {
        result.height = CalculateHeight(dataOfPercent->height);
        result.posY -= result.height;
    }

    if(lockLeft && !lockRight)
    {
        result.width = CalculateWidth(dataOfPercent->width);
    }
    if(!lockLeft && lockRight)
    {
        result.width = CalculateWidth(dataOfPercent->width);
        result.posX -= result.width;
    }

    return result;
}

ScreenData FlexibleScaler::Calculate(float px, float py, float width, float height)
{
    ScreenData data(px, py, width, height);
    return Calculate(&data);
}
