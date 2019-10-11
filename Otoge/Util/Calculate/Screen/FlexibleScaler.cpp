﻿#include "FlexibleScaler.hpp"
#include "../../Setting/SettingManager.h"
#include "../../../System/Config.h"
#include "../../../System/GlobalMethod.hpp"
#include "../../Window/DxSettings.hpp"
std::shared_ptr<FlexibleScaler> FlexibleScaler::GlobalInstance_ = nullptr;
std::vector<FlexibleScaler*> FlexibleScaler::Scalers_;

FlexibleScaler::FlexibleScaler(float screenWidth, float screenHeight, float scale, float ratioX, float ratioY)
{
    Logger::LowLevelLog("Scaler created, w: " + std::to_string(screenWidth) + ", h:" + std::to_string(screenHeight) + ", x" + std::to_string(scale),
                        "DEBUG");
    ScreenWidth_ = screenWidth * scale * ratioX;
    ScreenHeight_ = screenHeight * scale * ratioY;
    Scale_ = scale;
    RatioX_ = ratioX;
    RatioY_ = ratioY;
    Scalers_.push_back(this);
}

FlexibleScaler::~FlexibleScaler()
{
    auto result = std::find(Scalers_.begin(), Scalers_.end(), this);
    if(result == Scalers_.end())
    {
        Logger::LowLevelLog("Broken scaler list!", "CRITICAL");
    }
    else
    {
        Scalers_.erase(result);
    }
}

void FlexibleScaler::ApplyWindowSizeChanges()
{
    if (!GlobalInstance_) return;

    GlobalInstance_->SetScreenWidth(engine::CastToFloat(DxSettings::windowWidth));
    GlobalInstance_->SetScreenHeight(engine::CastToFloat(DxSettings::windowHeight));
    Logger::LowLevelLog("Scalers Count: " + std::to_string(Scalers_.size()), "FlexScaler");
    for(auto s : Scalers_)
    {
        Logger::LowLevelLog("Reset " + std::to_string(s->GetScreenWidth()), "FlexScaler");
        s->SetScreenWidth(s->CalculateWidth(GlobalInstance_->CalculatePositionRateX(s->GetScreenWidth())));
        s->SetScreenHeight(s->CalculateHeight(GlobalInstance_->CalculatePositionRateY(s->GetScreenHeight())));
    }
    Logger::LowLevelLog("Completed SizeChange", "FlexScaler");
}

std::shared_ptr<FlexibleScaler> FlexibleScaler::GetWindowBasedInstance()
{
    return GlobalInstance_;
}

void FlexibleScaler::CreateWindowBasedInstance()
{
    if (GlobalInstance_) return;

    //GlobalInstance_ = std::make_shared<FlexibleScaler>(1920.f, 1080.f, 1.0f);
    GlobalInstance_ = std::make_shared<FlexibleScaler>(engine::CastToFloat(DxSettings::windowWidth), engine::CastToFloat(DxSettings::windowHeight), 1.f);
    //GlobalInstance_.reset(new FlexibleScaler(engine::CastToFloat(DxSettings::windowWidth),
    //                                        engine::CastToFloat(DxSettings::windowHeight), 1.f));
}

void FlexibleScaler::DestroyWindowBasedInstance()
{
    GlobalInstance_.reset();
}

void FlexibleScaler::AddDiffX(float dx)
{
    GlobalDiffX_ += dx;
}

void FlexibleScaler::AddDiffY(float dy)
{
    GlobalDiffY_ += dy;
}

float FlexibleScaler::GetDiffX() const
{
    return GlobalDiffX_;
}

float FlexibleScaler::GetDiffY() const
{
    return GlobalDiffY_;
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

float FlexibleScaler::GetRatioX() const
{
    return RatioX_;
}

float FlexibleScaler::GetRatioY() const
{
    return RatioY_;
}

void FlexibleScaler::SetDiffX(float offsetX)
{
    GlobalDiffX_ = offsetX;
}

void FlexibleScaler::SetDiffY(float offsetY)
{
    GlobalDiffY_ = offsetY;
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

void FlexibleScaler::SetRatioX(float ratioX)
{
    RatioX_ = ratioX;
}

void FlexibleScaler::SetRatioY(float ratioY)
{
    RatioY_ = ratioY;
}

float FlexibleScaler::CalculatePositionRateX(float rawX) const
{
    return rawX / (ScreenWidth_ * Scale_ * RatioX_) * 100.f;
}

float FlexibleScaler::CalculatePositionRateY(float rawY) const
{
    return rawY / (ScreenHeight_ * Scale_ * RatioY_) * 100.f;
}

float FlexibleScaler::CalculatePositionX(float px) const
{
    return ScreenWidth_ * Scale_ * RatioX_ * (px / 100.f);
}

float FlexibleScaler::CalculatePositionY(float py) const
{
    return ScreenHeight_ * Scale_ * RatioY_ * (py / 100.f);
}

float FlexibleScaler::CalculateWidth(float width) const
{
    return ScreenWidth_ * Scale_ * RatioX_ * (width / 100.f);
}

float FlexibleScaler::CalculateHeight(float height) const
{
    return ScreenHeight_ * Scale_ * RatioY_* (height / 100.f);
}

ScreenData FlexibleScaler::Calculate(const ScreenData& dataOfPercent) const
{
    ScreenData l_Result;
    l_Result.lockAspectRate = dataOfPercent.lockAspectRate;
    l_Result.posX = CalculatePositionX(dataOfPercent.posX);
    l_Result.posY = CalculatePositionY(dataOfPercent.posY);
    if(lockTop && !lockBottom)
    {
        l_Result.height = CalculateHeight(dataOfPercent.height);
    }
    if(!lockTop && lockBottom)
    {
        l_Result.height = CalculateHeight(dataOfPercent.height);
        l_Result.posY -= l_Result.height;
    }
    if(lockLeft && !lockRight)
    {
        l_Result.width = CalculateWidth(dataOfPercent.width);
    }
    if(!lockLeft && lockRight)
    {
        l_Result.width = CalculateWidth(dataOfPercent.width);
        l_Result.posX -= l_Result.width;
    }
    return l_Result;
}

ScreenData FlexibleScaler::Calculate(float px, float py, float width, float height) const
{
    ScreenData data(px, py, width, height);
    return Calculate(data);
}
