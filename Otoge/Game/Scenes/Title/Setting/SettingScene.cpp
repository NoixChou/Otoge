#include "SettingScene.hpp"
#include "../../../../Util/Setting/SettingManager.h"
#include "../../../../System/Config.h"
#include "../../../../System/Input/KeyboardManager.hpp"
#include "../../../../System/Input/MouseManager.hpp"
#include "../../../../System/Task/TaskManager.hpp"
#include "../../../../System/GUI/Button.hpp"
#include "../../../../System/GUI/Label.hpp"
#include "../../../../Util/Calculate/Animation/Easing.hpp"
#include "../../../../System/GUI/SlideBar.hpp"
#include "../../../../System/GlobalMethod.hpp"
#include "../../../../System/GUI/DropdownList.hpp"
#include "../../../../Util/Visual/Color.hpp"

SettingScene::SettingScene() : Scene("SettingScene", 40.f, 100.f)
{
    TitleBar_ = std::make_shared<Scene>("titlebar", ScreenData(0.f, 0.f, 100.f, 5.f), DefaultScaler_);
    TitleBar_->SetDrawFunction([&]
        {
            ScreenData l_FixedTitlebar = DefaultScaler_->Calculate(0.f, 0.f, 100.f, 100.f);
            DrawBox(static_cast<int>(l_FixedTitlebar.posX), static_cast<int>(l_FixedTitlebar.posY), static_cast<int>(l_FixedTitlebar.width), static_cast<int>(l_FixedTitlebar.height), color_preset::WHITE, TRUE);
        });
    TitleBar_->SetPriority(20.f);
    AddChildTask(std::static_pointer_cast<Task>(TitleBar_));

    CloseButton_ = std::make_shared<Button>("< Close", ScreenData(0.f, 0.f, 20.f, 100.f), TitleBar_->GetDefaultScaler());
    CloseButton_->isDrawBase = false;
    CloseButton_->baseColor = color_preset::WHITE;
    CloseButton_->animationColor = color_preset::DARK_GREY;
    CloseButton_->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    CloseButton_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(2.f)));
    CloseButton_->GetTextLabelInstance()->ChangeFontThickness(1);
    CloseButton_->SetPriority(5.f);
    TitleBar_->AddChildTask(std::static_pointer_cast<Task>(CloseButton_));

    auto l_TitleLabel = std::make_shared<Label>("設定", ScreenData(0.0f, 0.0f, 100.f, CloseButton_->GetScreenHeight()), TitleBar_->GetDefaultScaler());
    l_TitleLabel->SetTextAlign(Label::TextAlignment::center | Label::TextAlignment::middle);
    l_TitleLabel->baseColor = color_preset::BLACK;
    l_TitleLabel->AdjustmentFontSize_ = false;
    l_TitleLabel->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    l_TitleLabel->ChangeFontThickness(1);
    TitleBar_->AddChildTask(std::static_pointer_cast<Task>(l_TitleLabel));


    BodyPanel_ = std::make_shared<ScrollablePanel>("bodypanel", ScreenData(3.f, 5.f, 94.f, 95.f), ScreenData(0.f, 0.f, 100.f, 190.f), DefaultScaler_);
    BodyPanel_->SetPriority(0.f);
    AddChildTask(std::static_pointer_cast<Task>(BodyPanel_));

    {
        AllowWindowSizes_.push_back({ "1920x1080", "1920", "1080" });
        AllowWindowSizes_.push_back({ "1600x1024", "1600", "1024" });
        AllowWindowSizes_.push_back({ "1440x900", "1440", "900" });
        AllowWindowSizes_.push_back({ "1280x720", "1280", "720" });
        AllowWindowSizes_.push_back({ "1280x1024", "1280", "1024" });
        AllowWindowSizes_.push_back({ "800x600", "800", "600" });
    }

    {
        DisplaySectionLabel_ = std::make_shared<Label>("ディスプレイ", ScreenData(0.f, 0.f, 100.f, 2.5f), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
        DisplaySectionLabel_->textAlign = Label::TextAlignment::center | Label::TextAlignment::middle;
        DisplaySectionLabel_->AdjustmentFontSize_ = false;
        DisplaySectionLabel_->ChangeFontSize(DisplaySectionLabel_->GetDefaultScaler()->CalculateHeight(80.f));
        DisplaySectionLabel_->ChangeFontThickness(2);
        BodyPanel_->GetPanelInstance()->AddChildTask(std::static_pointer_cast<Task>(DisplaySectionLabel_));

        {
            WindowSizeDescription_ = std::make_shared<Label>("解像度:", ScreenData(0.f, DisplaySectionLabel_->GetScreenHeight(), 20.f, 1.5f), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
            WindowSizeDescription_->textAlign = Label::TextAlignment::left | Label::TextAlignment::bottom;
            WindowSizeDescription_->AdjustmentFontSize_ = true;
            //WindowSizeDescription_->ChangeFontSize(engine::CastToInt(WindowSizeDescription_->GetDefaultScaler()->CalculateHeight(60.f)));
            WindowSizeDescription_->ChangeFontThickness(5);
            BodyPanel_->GetPanelInstance()->AddChildTask(std::static_pointer_cast<Task>(WindowSizeDescription_));

            WindowSizeList_ = std::make_shared<DropdownList>("WindowSizeList", ScreenData(WindowSizeDescription_->GetScreenWidth(), WindowSizeDescription_->GetPositionY(), 20.f, WindowSizeDescription_->GetScreenHeight() + 0.0f), AllowWindowSizes_.size(), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
            BodyPanel_->GetPanelInstance()->AddChildTask(std::static_pointer_cast<Task>(WindowSizeList_));

            int l_ItemCount = 0;
            for(l_ItemCount = 0; l_ItemCount<AllowWindowSizes_.size(); l_ItemCount++)
            {
                std::string l_WindowSize = AllowWindowSizes_[l_ItemCount][0];
                WindowSizeList_->AddItem(l_ItemCount, l_WindowSize);
            }
        }
    }
    StopFade();
}


SettingScene::~SettingScene()
{
}

void SettingScene::OnStartedFadeIn()
{
    TaskManager::GetInstance()->SetModalTask(weak_from_this());
    auto l_CurrentSize = std::find_if(AllowWindowSizes_.begin(), AllowWindowSizes_.end(), [&](const std::vector<std::string> & lines)
    {
        return lines.at(0) == SettingManager::GetGlobal()->Get<std::string>(game_config::SETTINGS_RES_WIDTH).get() + "x" + SettingManager::GetGlobal()->Get<std::string>(game_config::SETTINGS_RES_HEIGHT).get();
    });

    if(l_CurrentSize != AllowWindowSizes_.end())
    {
        WindowSizeList_->SetSelectedItemNum(engine::CastToInt(std::distance(AllowWindowSizes_.begin(), l_CurrentSize)) + 1);
    }
}

void SettingScene::SceneFadeIn(float deltaTime)
{
    float totalTime = 0.5f;
    Easing::EaseFunction ease = Easing::OutExp;

    SetTransparent(engine::CastToFloat(ease(timerCount, totalTime, 100.f, 0.f)));
    SetPositionX(engine::CastToFloat(ease(timerCount, totalTime, 0.f, -GetScreenWidth())));

    if (timerCount > totalTime)
    {
        SetTransparent(100.f);
        SetPositionX(0.f);
        IsFadingIn_ = false;
    }
}

void SettingScene::SceneFadeOut(float deltaTime)
{
    float totalTime = 0.3f;
    Easing::EaseFunction ease = Easing::InExp;

    SetTransparent(engine::CastToFloat(ease(timerCount, totalTime, 0.f, 100.f)));
    SetPositionX(engine::CastToFloat(ease(timerCount, totalTime, -GetScreenWidth(), 0.f)));

    if (timerCount > totalTime)
    {
        IsFadingOut_ = false;
        TaskManager::GetInstance()->UnsetModalTask();
    }
}

void SettingScene::SceneUpdate(float deltaTime)
{
    if(CloseButton_->IsClickedMouse() || (!IsOnMouse() && MouseManager::GetInstance()->IsDownButton(MOUSE_INPUT_LEFT) && IsEnable()))
    {
        SettingManager::GetGlobal()->Save();
        StartFadeOut();
    }

    if(WindowSizeList_->IsChangedSelect())
    {
        int WindowWidth = stoi(AllowWindowSizes_[0][1]);
        int WindowHeight = stoi(AllowWindowSizes_[0][2]);

        auto l_CurrentSize = std::find_if(AllowWindowSizes_.begin(), AllowWindowSizes_.end(), [&](const std::vector<std::string> & lines)
            {
                return lines.at(0) == AllowWindowSizes_[WindowSizeList_->GetSelectedItemNum() - 1][0];
            });

        if (l_CurrentSize != AllowWindowSizes_.end())
        {
            int currentSizePos = engine::CastToInt(std::distance(AllowWindowSizes_.begin(), l_CurrentSize));
            WindowWidth = stoi(AllowWindowSizes_[currentSizePos][1]);
            WindowHeight = stoi(AllowWindowSizes_[currentSizePos][2]);
        }

        SettingManager::GetGlobal()->Set(game_config::SETTINGS_RES_WIDTH, WindowWidth);
        SettingManager::GetGlobal()->Set(game_config::SETTINGS_RES_HEIGHT, WindowHeight);
    }
}

void SettingScene::Draw()
{
    ScreenData l_FixedContentField = DefaultScaler_->Calculate(0.f, 0.f, 100.f, 100.f);
    DrawBox(engine::CastToInt(l_FixedContentField.posX), engine::CastToInt(l_FixedContentField.posY),
        engine::CastToInt(l_FixedContentField.width), engine::CastToInt(l_FixedContentField.height), BodyPanel_->baseColor, TRUE);
}