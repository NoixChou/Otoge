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
#include "../../../../System/GUI/DropdownList.cpp"
#include "../../../../Util/Visual/Color.hpp"
#include "../../../../System/GUI/CheckBox.hpp"
#include "../../../../Util/Window/DxSettings.hpp"

SettingScene::SettingScene() : Scene("SettingScene", 40.f, 100.f)
{
    auto l_TitleBar = std::make_shared<Scene>("titlebar", ScreenData(0.f, 0.f, 100.f, 5.f), DefaultScaler_);
    TitleBar_ = l_TitleBar;
    TitleBar_.lock()->SetDrawFunction([&]
    {
        ScreenData l_FixedTitlebar = DefaultScaler_->Calculate(0.f, 0.f, 100.f, 100.f);
        DrawBox(static_cast<int>(l_FixedTitlebar.posX), static_cast<int>(l_FixedTitlebar.posY),
                static_cast<int>(l_FixedTitlebar.width), static_cast<int>(l_FixedTitlebar.height), color_preset::WHITE,
                TRUE);
    });
    TitleBar_.lock()->SetPriority(20.f);
    AddChildTask(std::static_pointer_cast<Task>(TitleBar_.lock()));

    CloseButton_ = std::make_shared<Button>("< Close", ScreenData(0.f, 0.f, 20.f, 100.f), TitleBar_.lock()->GetDefaultScaler());
    CloseButton_->isDrawBase = true;
    CloseButton_->baseColor = color_preset::WHITE;
    CloseButton_->animationColor = color_preset::DARK_GREY;
    CloseButton_->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    CloseButton_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(2.f)));
    CloseButton_->GetTextLabelInstance()->ChangeFontThickness(1);
    CloseButton_->SetPriority(5.f);
    TitleBar_.lock()->AddChildTask(std::static_pointer_cast<Task>(CloseButton_));

    auto l_TitleLabel = std::make_shared<Label>("設定", ScreenData(0.0f, 0.0f, 100.f, CloseButton_->GetScreenHeight()), TitleBar_.lock()->GetDefaultScaler());
    l_TitleLabel->SetTextAlign(Label::TextAlignment::center | Label::TextAlignment::middle);
    l_TitleLabel->baseColor = color_preset::BLACK;
    l_TitleLabel->AdjustmentFontSize_ = false;
    l_TitleLabel->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    l_TitleLabel->ChangeFontThickness(1);
    TitleBar_.lock()->AddChildTask(std::static_pointer_cast<Task>(l_TitleLabel));


    BodyPanel_ = std::make_shared<ScrollablePanel>("bodypanel", ScreenData(3.f, 5.f, 94.f, 95.f), ScreenData(0.f, 0.f, 100.f, 190.f), DefaultScaler_);
    BodyPanel_->SetPriority(0.f);
    AddChildTask(std::static_pointer_cast<Task>(BodyPanel_));

	{
		AllowWindowSizes_.push_back(std::make_pair("16:9 WIDE", std::make_pair(0, 0)));
		AllowWindowSizes_.push_back(std::make_pair("1920x1080", std::make_pair(1920, 1080)));
		AllowWindowSizes_.push_back(std::make_pair("1600x1024", std::make_pair(1600, 1024)));
		AllowWindowSizes_.push_back(std::make_pair("1440x900", std::make_pair(1440, 900)));
		AllowWindowSizes_.push_back(std::make_pair("1280x720", std::make_pair(1280, 720)));
		AllowWindowSizes_.push_back(std::make_pair("4:3 STD", std::make_pair(0, 0)));
		AllowWindowSizes_.push_back(std::make_pair("1280x1024", std::make_pair(1280, 1024)));
		AllowWindowSizes_.push_back(std::make_pair("800x600", std::make_pair(800, 600)));
	}

    {
        DisplaySectionLabel_ = std::make_shared<Label>("ディスプレイ", ScreenData(0.f, 0.f, 100.f, 2.5f), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
        DisplaySectionLabel_->textAlign = Label::TextAlignment::center | Label::TextAlignment::middle;
        DisplaySectionLabel_->AdjustmentFontSize_ = false;
        DisplaySectionLabel_->ChangeFontSize(engine::CastToInt(DisplaySectionLabel_->GetDefaultScaler()->CalculateHeight(80.f)));
        DisplaySectionLabel_->ChangeFontThickness(2);
        BodyPanel_->GetPanelInstance()->AddChildTask(std::static_pointer_cast<Task>(DisplaySectionLabel_));

        {
            WindowSizeDescription_ = std::make_shared<Label>("解像度:", ScreenData(0.f, DisplaySectionLabel_->GetScreenHeight(), 20.f, 1.5f), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
            WindowSizeDescription_->textAlign = Label::TextAlignment::left | Label::TextAlignment::bottom;
            WindowSizeDescription_->AdjustmentFontSize_ = true;
            //WindowSizeDescription_->ChangeFontSize(engine::CastToInt(WindowSizeDescription_->GetDefaultScaler()->CalculateHeight(60.f)));
            WindowSizeDescription_->ChangeFontThickness(2);
            BodyPanel_->GetPanelInstance()->AddChildTask(std::static_pointer_cast<Task>(WindowSizeDescription_));

            WindowSizeList_ = std::make_shared<DropdownList<std::pair<int, int>>>("WindowSizeList", ScreenData(WindowSizeDescription_->GetScreenWidth(), WindowSizeDescription_->GetPositionY(), 20.f, WindowSizeDescription_->GetScreenHeight() + 0.0f), AllowWindowSizes_.size(), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
            BodyPanel_->GetPanelInstance()->AddChildTask(std::static_pointer_cast<Task>(WindowSizeList_));

            int l_ItemCount = 0;
            for(l_ItemCount = 0; l_ItemCount < AllowWindowSizes_.size(); l_ItemCount++)
            {
                std::string l_WindowSize = AllowWindowSizes_[l_ItemCount].first;
                if(AllowWindowSizes_[l_ItemCount].second.first == 0)
                {
                    WindowSizeList_->AddSeparator(l_ItemCount, l_WindowSize);
                    continue;
                }
                WindowSizeList_->AddSimpleItem(l_ItemCount, l_WindowSize, AllowWindowSizes_[l_ItemCount].second);
            }
        }
    }

    FullscreenCheck_ = std::make_shared<CheckBox>("フルスクリーン", ScreenData(WindowSizeList_->GetPositionX() + WindowSizeList_->GetScreenWidth() + 1.5f, WindowSizeDescription_->GetPositionY(), 25.f, 1.5f), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
    FullscreenCheck_->GetTextLabelInstance()->ChangeFontThickness(4);
    BodyPanel_->GetPanelInstance()->AddChildTask(FullscreenCheck_);

    VSyncCheck_ = std::make_shared<CheckBox>("垂直同期", ScreenData(WindowSizeList_->GetPositionX(), WindowSizeDescription_->GetPositionY() + WindowSizeDescription_->GetScreenHeight() + 1.5f, 20.f, 1.5f), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
    BodyPanel_->GetPanelInstance()->AddChildTask(VSyncCheck_);

    StopFade();
}

SettingScene::~SettingScene()
{
}

void SettingScene::OnStartedFadeIn()
{
    TaskManager::GetInstance()->SetModalTask(weak_from_this());

    // 解像度
    auto l_CurrentSize = std::find_if(AllowWindowSizes_.begin(), AllowWindowSizes_.end(), [&](const std::pair<std::string, std::pair<int, int>>& lines)
    {
        if (lines.second.first == 0 || lines.second.second == 0)
        {
            return false;
        }
        return (lines.second.first == SettingManager::GetGlobal()->Get<int>(game_config::SETTINGS_RES_WIDTH).get()) && (lines.second.second == SettingManager::GetGlobal()->Get<int>(game_config::SETTINGS_RES_HEIGHT).get());
    });

    if(l_CurrentSize != AllowWindowSizes_.end())
    {
        WindowSizeList_->SetSelectedItemNum(engine::CastToInt(std::distance(AllowWindowSizes_.begin(), l_CurrentSize)));
    }

    // フルスクリーン
    auto l_IsFullscreen = SettingManager::GetGlobal()->Get<bool>(game_config::SETTINGS_FULLSCREEN);
    if (l_IsFullscreen) FullscreenCheck_->SetChecked(l_IsFullscreen.get());
}

void SettingScene::OnStartedFadeOut()
{
    SettingManager::GetGlobal()->Save();
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
        StartFadeOut();
    }

    if(KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_LCONTROL) && KeyboardManager::GetInstance()->IsDownKey(KEY_INPUT_O))
    {
        StartFadeIn();
    }

    // 解像度
    if(WindowSizeList_->IsChangedSelect() && IsEnable())
    {
		int WindowWidth = SettingManager::GetGlobal()->Get<int>(game_config::SETTINGS_RES_WIDTH).get();
        int WindowHeight = SettingManager::GetGlobal()->Get<int>(game_config::SETTINGS_RES_HEIGHT).get();

        std::optional<std::pair<int, int>> ItemValue = WindowSizeList_->GetSelectedItemValue();
		if (!ItemValue.has_value()) return;
		std::tie(WindowWidth, WindowHeight) = ItemValue.value();

        SettingManager::GetGlobal()->Set(game_config::SETTINGS_RES_WIDTH, WindowWidth);
        SettingManager::GetGlobal()->Set(game_config::SETTINGS_RES_HEIGHT, WindowHeight);
        DxSettings::windowWidth = WindowWidth;
        DxSettings::windowHeight = WindowHeight;
        SetGraphMode(WindowWidth, WindowHeight, 32);
        FlexibleScaler::ApplyWindowSizeChanges();
        ReCalculateScreen();
    }
    // フルスクリーン
    if(FullscreenCheck_->IsChanged())
    {
        Logger_->Error("changed");
        SettingManager::GetGlobal()->Set(game_config::SETTINGS_FULLSCREEN, FullscreenCheck_->IsChecked());
        DxSettings::isFullScreen = FullscreenCheck_->IsChecked();
        ChangeWindowMode(!DxSettings::isFullScreen);
    }
}

void SettingScene::Draw()
{
    ScreenData l_FixedContentField = DefaultScaler_->Calculate(0.f, 0.f, 100.f, 100.f);
    DrawBox(engine::CastToInt(l_FixedContentField.posX), engine::CastToInt(l_FixedContentField.posY),
        engine::CastToInt(l_FixedContentField.width), engine::CastToInt(l_FixedContentField.height), BodyPanel_->baseColor, TRUE);
}
