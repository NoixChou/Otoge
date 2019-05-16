#include "FlexibleScaler.hpp"
#include "../../Setting/SettingManager.h"
#include "../../../System/Config.h"
#include "../../../System/GlobalMethod.hpp"
#include "../../Window/DxSettings.hpp"
std::shared_ptr<FlexibleScaler> FlexibleScaler::GlobalInstance_ = nullptr;
std::vector<FlexibleScaler*> FlexibleScaler::Scalers_;

FlexibleScaler::FlexibleScaler(float screenWidth, float screenHeight, float scale)
{
    Logger::LowLevelLog("Scaler created, w: " + std::to_string(screenWidth) + ", h:" + std::to_string(screenHeight),
                        "DEBUG");
    ScreenWidth_ = screenWidth;
    ScreenHeight_ = screenHeight;
    Scale_ = scale;
    Scalers_.push_back(this);
}

FlexibleScaler::~FlexibleScaler()
{
    auto result = std::find(Scalers_.begin(), Scalers_.end(), this);
    if(result == Scalers_.end())
    {
        Logger::LowLevelLog("Broken scaler list!!!", "CRITICAL");
    }
    else
    {
        Scalers_.erase(result);
    }
}

void FlexibleScaler::ApplyWindowSizeChanges()
{
    GlobalInstance_->SetScreenWidth(DxSettings::windowWidth);
    GlobalInstance_->SetScreenHeight(DxSettings::windowHeight);
    Logger::LowLevelLog("Scalers Count: " + std::to_string(Scalers_.size()), "Flex Scaler");
    for(auto s : Scalers_)
    {
        Logger::LowLevelLog("Reset " + std::to_string(s->GetScreenWidth()), "Flex       Scaler");
        s->SetScreenWidth(s->CalculateWidth(GlobalInstance_->CalculatePositionRateX(s->GetScreenWidth())));
        s->SetScreenHeight(s->CalculateHeight(GlobalInstance_->CalculatePositionRateY(s->GetScreenHeight())));
    }
}

std::shared_ptr<FlexibleScaler> FlexibleScaler::GetWindowBasedInstance()
{
    Logger::LowLevelLog("WindowBased!", "INFO FlexScaler");
    return GlobalInstance_;
}

void FlexibleScaler::CreateWindowBasedInstance()
{
    if(!GlobalInstance_)
    {
        GlobalInstance_.reset(new FlexibleScaler(engine::CastToFloat(DxSettings::windowWidth),
                                                 engine::CastToFloat(DxSettings::windowHeight), 1.f));
    }
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

float FlexibleScaler::CalculatePositionRateX(float rawX) const
{
    return rawX / (ScreenWidth_) * 100.f * Scale_;
}

float FlexibleScaler::CalculatePositionRateY(float rawY) const
{
    return rawY / (ScreenHeight_) * 100.f * Scale_;
}

float FlexibleScaler::CalculatePositionX(float px) const
{
    return ScreenWidth_ * (px / 100.f) * Scale_;
}

float FlexibleScaler::CalculatePositionY(float py) const
{
    return ScreenHeight_ * (py / 100.f) * Scale_;
}

float FlexibleScaler::CalculateWidth(float width) const
{
    return ScreenWidth_ * (width / 100.f) * Scale_;
}

float FlexibleScaler::CalculateHeight(float height) const
{
    return ScreenHeight_ * (height / 100.f) * Scale_;
}

ScreenData FlexibleScaler::Calculate(const ScreenData& dataOfPercent) const
{
    ScreenData result;
    result.lockAspectRate = dataOfPercent.lockAspectRate;
    result.posX = CalculatePositionX(dataOfPercent.posX);
    result.posY = CalculatePositionY(dataOfPercent.posY);
    if(lockTop && !lockBottom)
    {
        result.height = CalculateHeight(dataOfPercent.height);
    }
    if(!lockTop && lockBottom)
    {
        result.height = CalculateHeight(dataOfPercent.height);
        result.posY -= result.height;
    }
    if(lockLeft && !lockRight)
    {
        result.width = CalculateWidth(dataOfPercent.width);
    }
    if(!lockLeft && lockRight)
    {
        result.width = CalculateWidth(dataOfPercent.width);
        result.posX -= result.width;
    }
    return result;
}

ScreenData FlexibleScaler::Calculate(float px, float py, float width, float height) const
{
    ScreenData data(px, py, width, height);
    return Calculate(data);
}
