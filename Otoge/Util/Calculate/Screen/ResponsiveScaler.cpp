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


ScreenData FlexibleScaler::Calculate(const ScreenData *dataOfPercent)
{
    ScreenData result;

    result.posX = ScreenWidth_ * (dataOfPercent->posX / 100.f) * Scale_;
    result.posY = ScreenHeight_ * (dataOfPercent->posY / 100.f) * Scale_;

    if(lockTop && !lockBottom)
    {
        result.height = ScreenHeight_ * (dataOfPercent->height / 100.f) * Scale_;
    }
    if(!lockTop && lockBottom)
    {
        result.height = ScreenHeight_ * (dataOfPercent->height / 100.f) * Scale_;
        result.posY -= ScreenHeight_ * (dataOfPercent->height / 100.f) * Scale_;
    }

    if(lockLeft && !lockRight)
    {
        result.width = ScreenWidth_ * (dataOfPercent->width / 100.f) * Scale_;
    }
    if(!lockLeft && lockRight)
    {
        result.width = ScreenWidth_ * (dataOfPercent->width / 100.f) * Scale_;
        result.posX -= ScreenWidth_ * (dataOfPercent->width / 100.f) * Scale_;
    }

    return result;
}
