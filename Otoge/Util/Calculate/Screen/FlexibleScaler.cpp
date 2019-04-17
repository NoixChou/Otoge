#include "FlexibleScaler.hpp"
#include "../../Setting/SettingManager.h"
#include "../../../System/Config.h"

std::shared_ptr<FlexibleScaler> FlexibleScaler::GlobalInstance_ = nullptr;

FlexibleScaler::FlexibleScaler(float screenWidth, float screenHeight, float scale, float offsetX, float offsetY)
{
    OffsetX_ = offsetX;
    OffsetY_ = offsetY;
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
        GlobalInstance_.reset(new FlexibleScaler(static_cast<float>(SettingManager::GetGlobal()->Get<int>(SETTINGS_RES_WIDTH).get()), static_cast<float>(SettingManager::GetGlobal()->Get<int>(SETTINGS_RES_HEIGHT).get()), 1.f));
    }
}

void FlexibleScaler::DestroyWindowBasedInstance()
{
    GlobalInstance_.reset();
}

float FlexibleScaler::GetOffsetX() const
{
    return OffsetX_;
}

float FlexibleScaler::GetOffsetY() const
{
    return OffsetY_;
}

float FlexibleScaler::GetScreenWidth() const
{
    return ScreenWidth_;
}

float FlexibleScaler::GetScreenHeight() const
{
    return ScreenHeight_;
}

float FlexibleScaler::GetScale() const
{
    return Scale_;
}

void FlexibleScaler::SetOffsetX(float offsetX)
{
    OffsetX_ = offsetX;
}

void FlexibleScaler::SetOffsetY(float offsetY)
{
    OffsetY_ = offsetY;
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

float FlexibleScaler::CalculatePositionRateX(float rawX) const
{
	return rawX / (ScreenWidth_ - OffsetX_) * 100.f * Scale_;
}

float FlexibleScaler::CalculatePositionRateY(float rawY) const
{
	return rawY / (ScreenHeight_ - OffsetY_) * 100.f * Scale_;
}

float FlexibleScaler::CalculatePositionX(float px) const
{
    return OffsetX_ + ScreenWidth_ * (px / 100.f) * Scale_;
}

float FlexibleScaler::CalculatePositionY(float py) const
{
    return OffsetY_ + ScreenHeight_ * (py / 100.f) * Scale_;
}

float FlexibleScaler::CalculateWidth(float width) const
{
    return OffsetX_ + ScreenWidth_ * (width / 100.f) * Scale_;
}

float FlexibleScaler::CalculateHeight(float height) const
{
    return OffsetY_ + ScreenHeight_ * (height / 100.f) * Scale_;
}

ScreenData FlexibleScaler::Calculate(const ScreenData *dataOfPercent) const
{
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

ScreenData FlexibleScaler::Calculate(float px, float py, float width, float height) const
{
    ScreenData data(px, py, width, height);
    return Calculate(&data);
}
