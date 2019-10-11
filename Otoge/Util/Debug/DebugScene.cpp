﻿#include "DebugScene.hpp"
#include "../../System/Task/TaskManager.hpp"
#include "../Setting/SettingManager.h"
#include "../../System/Config.h"
#include "../../System/Input/KeyboardManager.hpp"
#include "../../System/GUI/Button.hpp"
#include "../../System/GlobalMethod.hpp"
#include "../Visual/Color.hpp"

DebugScene::DebugScene() : Scene("DebugScene")
{
    DefaultScaler_->lockTop = false;
    DefaultScaler_->lockBottom = true;
    {
        auto l_FpsPanel = std::make_shared<Scene>("FpsLabelPanel", ScreenData(1.f, 94.f, 7.f, 2.8f), DefaultScaler_);
        l_FpsPanel->SetDrawFunction([&]
        {
            ScreenData l_FpsScreen;
            l_FpsScreen.posX = 0.f;
            l_FpsScreen.posY = 0.f;
            l_FpsScreen.width = GetRawScreenWidth();
            l_FpsScreen.height = GetRawScreenHeight();
            SetDrawBlendMode(DX_BLENDMODE_PMA_ALPHA, 230);
            DrawBox(engine::CastToInt(l_FpsScreen.posX), engine::CastToInt(l_FpsScreen.posY),
                    engine::CastToInt(l_FpsScreen.posX + l_FpsScreen.width),
                    engine::CastToInt(l_FpsScreen.posY + l_FpsScreen.height), color_preset::LIGHT_BLUE, TRUE);
        });
        AddChildTask(std::static_pointer_cast<Task>(l_FpsPanel));
        FpsLabel_ = std::make_shared<Label>("--", ScreenData(0.f, 0.f, 70.f, 100.f), l_FpsPanel->GetDefaultScaler());
        FpsLabel_->baseColor = color_preset::LEAF_GREEN;
        FpsLabel_->textAlign = Label::TextAlignment::middle | Label::TextAlignment::right;
        FpsLabel_->ChangeFontThickness(8);
        FpsLabel_->adjustmentFontSize = true;
        l_FpsPanel->AddChildTask(std::static_pointer_cast<Task>(FpsLabel_));
        FpsDescLabel_ = std::make_shared<Label>("fps", ScreenData(70.f, 0.f, 30.f, 100.f),
                                                l_FpsPanel->GetDefaultScaler());
        FpsDescLabel_->baseColor = color_preset::LEAF_GREEN;
        FpsDescLabel_->textAlign = Label::TextAlignment::bottom | Label::TextAlignment::center;
        FpsDescLabel_->ChangeFontThickness(2);
        FpsDescLabel_->adjustmentFontSize = true;
        FpsDescLabel_->ChangeFontSize(static_cast<int>(l_FpsPanel->GetDefaultScaler()->CalculateHeight(70.f)));
        l_FpsPanel->AddChildTask(std::static_pointer_cast<Task>(FpsDescLabel_));
    }
    {
        auto l_DeltaTimePanel = std::make_shared<Scene>("DeltaTimeLabelPanel", ScreenData(1.f, 96.f, 7.f, 2.0f),
                                                        DefaultScaler_);
        l_DeltaTimePanel->SetDrawFunction([&]
        {
            ScreenData l_DeltaTimeScreen;
            l_DeltaTimeScreen.posX = 0.f;
            l_DeltaTimeScreen.posY = 0.f;
            l_DeltaTimeScreen.width = GetRawScreenWidth();
            l_DeltaTimeScreen.height = GetRawScreenHeight();
            SetDrawBlendMode(DX_BLENDMODE_PMA_ALPHA, 230);
            DrawBox(engine::CastToInt(l_DeltaTimeScreen.posX), engine::CastToInt(l_DeltaTimeScreen.posY),
                    engine::CastToInt(l_DeltaTimeScreen.posX + l_DeltaTimeScreen.width),
                    engine::CastToInt(l_DeltaTimeScreen.posY + l_DeltaTimeScreen.height), color_preset::AMBER_ORANGE,
                    TRUE);
        });
        AddChildTask(std::static_pointer_cast<Task>(l_DeltaTimePanel));
        DeltaTimeLabel_ = std::make_shared<Label>("--", ScreenData(0.f, 0.f, 70.f, 100.f),
                                                  l_DeltaTimePanel->GetDefaultScaler());
        DeltaTimeLabel_->baseColor = color_preset::LEMON_YELLOW;
        DeltaTimeLabel_->textAlign = Label::TextAlignment::middle | Label::TextAlignment::right;
        DeltaTimeLabel_->ChangeFontThickness(6);
        DeltaTimeLabel_->adjustmentFontSize = true;
        l_DeltaTimePanel->AddChildTask(std::static_pointer_cast<Task>(DeltaTimeLabel_));
        DeltaTimeDescLabel_ = std::make_shared<Label>(" ms", ScreenData(70.f, 0.f, 30.f, 100.f),
                                                      l_DeltaTimePanel->GetDefaultScaler());
        DeltaTimeDescLabel_->baseColor = color_preset::LEMON_YELLOW;
        DeltaTimeDescLabel_->textAlign = Label::TextAlignment::bottom | Label::TextAlignment::center;
        DeltaTimeDescLabel_->ChangeFontThickness(6);
        DeltaTimeDescLabel_->adjustmentFontSize = true;
        DeltaTimeDescLabel_->ChangeFontSize(
            static_cast<int>(l_DeltaTimePanel->GetDefaultScaler()->CalculateHeight(80.f)));
        l_DeltaTimePanel->AddChildTask(std::static_pointer_cast<Task>(DeltaTimeDescLabel_));
    }

    SetTransparent(80.f);
    SetPriority(100.f);
}

DebugScene::~DebugScene()
{
}

void DebugScene::SceneUpdate(float deltaTime)
{
    if(KeyboardManager::GetInstance()->IsDownKey(KEY_INPUT_1))
    {
        SetVisible(!IsVisible());
    }
    FpsLabel_->SetLabel(std::to_string(engine::CastToInt(TaskManager::GetInstance()->GetFrameRate())));
    if(timerCount > 0.1f) DeltaTimeLabel_->SetLabel(std::_Floating_to_string("%.2f", deltaTime * 1000.f)), timerCount =
        0.f;
}

void DebugScene::Draw()
{
    ScreenData l_DeltaTimeScreen;
    l_DeltaTimeScreen.posX = DeltaTimeLabel_->GetRawPositionX();
    l_DeltaTimeScreen.posY = DeltaTimeLabel_->GetRawPositionY();
    l_DeltaTimeScreen.width = DeltaTimeLabel_->GetRawScreenWidth();
    l_DeltaTimeScreen.height = DeltaTimeLabel_->GetRawScreenHeight();
}
