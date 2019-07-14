#include "SettingScene.hpp"
#include "../../../../Util/Setting/SettingManager.h"
#include "../../../../System/Config.h"
#include "../../../../System/Input/KeyboardManager.hpp"
#include "../../../../System/Input/MouseManager.hpp"
#include "../../../../System/Task/TaskManager.hpp"
#include "../../../../System/GUI/Button.hpp"
#include "../../../../System/GUI/Label.hpp"
#include "../../../../Util/Calculate/Animation/Easing.hpp"
#include "../../../../System/GlobalMethod.hpp"
#include "../../../../System/GUI/DropdownList.cpp"
#include "../../../../Util/Visual/Color.hpp"
#include "../../../../System/GUI/CheckBox.hpp"
#include "../../../../Util/Window/DxSettings.hpp"
#include "../../../../System/Font/FontHandleCreator.hpp"

SettingScene::SettingScene() : Scene("SettingScene", 40.f, 100.f)
{
    const float l_HeaderFontSize = 2.5f;
    const int l_HeaderFontThickness = 4;

    const int l_HeaderFontHandle = FontHandleCreator::Create(engine::CastToInt(DefaultScaler_->CalculateHeight(l_HeaderFontSize)), l_HeaderFontThickness);
    
    const float l_ItemFontSize = 1.5f;
    const int l_ItemFontThickness = 1;

    const int l_ItemFontHandle = FontHandleCreator::Create(engine::CastToInt(DefaultScaler_->CalculateHeight(l_ItemFontSize)), l_ItemFontThickness);


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
    CloseButton_->GetTextLabelInstance()->adjustmentFontSize = false;
    CloseButton_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(2.f)));
    CloseButton_->GetTextLabelInstance()->ChangeFontThickness(1);
    CloseButton_->SetPriority(5.f);
    TitleBar_.lock()->AddChildTask(std::static_pointer_cast<Task>(CloseButton_));

    auto l_TitleLabel = std::make_shared<Label>("設定", ScreenData(0.0f, 0.0f, 100.f, CloseButton_->GetScreenHeight()), TitleBar_.lock()->GetDefaultScaler());
    l_TitleLabel->SetTextAlign(Label::TextAlignment::center | Label::TextAlignment::middle);
    l_TitleLabel->baseColor = color_preset::BLACK;
    l_TitleLabel->adjustmentFontSize = false;
    l_TitleLabel->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    l_TitleLabel->ChangeFontThickness(1);
    TitleBar_.lock()->AddChildTask(std::static_pointer_cast<Task>(l_TitleLabel));


    BodyPanel_ = std::make_shared<ScrollablePanel>("bodypanel", ScreenData(3.f, 5.f, 94.f, 95.f), ScreenData(0.f, 0.f, 100.f, 190.f), DefaultScaler_);
    BodyPanel_->SetPriority(0.f);
    AddChildTask(std::static_pointer_cast<Task>(BodyPanel_));

	{
		AllowWindowSizes_.emplace_back(std::make_pair("16:9 WIDE", std::make_pair(0, 0)));
		AllowWindowSizes_.emplace_back(std::make_pair("1920x1080", std::make_pair(1920, 1080)));
		AllowWindowSizes_.emplace_back(std::make_pair("1600x1024", std::make_pair(1600, 1024)));
		AllowWindowSizes_.emplace_back(std::make_pair("1440x900", std::make_pair(1440, 900)));
		AllowWindowSizes_.emplace_back(std::make_pair("1280x720", std::make_pair(1280, 720)));
		AllowWindowSizes_.emplace_back(std::make_pair("4:3 STD", std::make_pair(0, 0)));
		AllowWindowSizes_.emplace_back(std::make_pair("1280x1024", std::make_pair(1280, 1024)));
		AllowWindowSizes_.emplace_back(std::make_pair("800x600", std::make_pair(800, 600)));
	}

    float l_TopPosition = 0.f;

    {
        DisplaySectionLabel_ = std::make_shared<Label>("ディスプレイ", ScreenData(0.f, l_TopPosition, 100.f, 2.5f), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
        DisplaySectionLabel_->textAlign = Label::TextAlignment::center | Label::TextAlignment::middle;
        DisplaySectionLabel_->adjustmentFontSize = false;
        DisplaySectionLabel_->SetFontHandle(l_HeaderFontHandle);
        BodyPanel_->GetPanelInstance()->AddChildTask(std::static_pointer_cast<Task>(DisplaySectionLabel_));

        {
            WindowSizeDescription_ = std::make_shared<Label>("解像度:", ScreenData(0.f, l_TopPosition + DisplaySectionLabel_->GetScreenHeight(), 12.f, 1.5f), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
            WindowSizeDescription_->textAlign = Label::TextAlignment::right | Label::TextAlignment::middle;
            WindowSizeDescription_->adjustmentFontSize = false;
            WindowSizeDescription_->SetFontHandle(l_ItemFontHandle);
            BodyPanel_->GetPanelInstance()->AddChildTask(std::static_pointer_cast<Task>(WindowSizeDescription_));

            WindowSizeList_ = std::make_shared<DropdownList<std::pair<int, int>>>("WindowSizeList", ScreenData(WindowSizeDescription_->GetScreenWidth(), WindowSizeDescription_->GetPositionY(), 22.f, WindowSizeDescription_->GetScreenHeight() + 0.0f), AllowWindowSizes_.size(), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
            BodyPanel_->GetPanelInstance()->AddChildTask(std::static_pointer_cast<Task>(WindowSizeList_));

            int l_ItemCount = 0;
            for (l_ItemCount = 0; l_ItemCount < AllowWindowSizes_.size(); l_ItemCount++)
            {
                std::string l_WindowSize = AllowWindowSizes_[l_ItemCount].first;
                if (AllowWindowSizes_[l_ItemCount].second.first == 0)
                {
                    WindowSizeList_->AddSeparator(l_ItemCount, l_WindowSize);
                    continue;
                }
                WindowSizeList_->AddSimpleItem(l_ItemCount, l_WindowSize, AllowWindowSizes_[l_ItemCount].second);
            }
        }

        FullscreenCheck_ = std::make_shared<CheckBox>("フルスクリーン", ScreenData(WindowSizeDescription_->GetPositionX(), WindowSizeDescription_->GetPositionY() + WindowSizeDescription_->GetScreenHeight() + 0.5f, 25.f, 1.5f), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
        FullscreenCheck_->GetTextLabelInstance()->adjustmentFontSize = false;
        FullscreenCheck_->GetTextLabelInstance()->SetFontHandle(l_ItemFontHandle);
        BodyPanel_->GetPanelInstance()->AddChildTask(FullscreenCheck_);

        VSyncCheck_ = std::make_shared<CheckBox>("垂直同期", ScreenData(WindowSizeDescription_->GetPositionX(), FullscreenCheck_->GetPositionY() + FullscreenCheck_->GetScreenHeight() + 0.3f, FullscreenCheck_->GetScreenWidth(), 1.5f), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
        VSyncCheck_->GetTextLabelInstance()->adjustmentFontSize = false;
        VSyncCheck_->GetTextLabelInstance()->SetFontHandle(l_ItemFontHandle);
        BodyPanel_->GetPanelInstance()->AddChildTask(VSyncCheck_);
    }

    l_TopPosition = VSyncCheck_->GetPositionY() + VSyncCheck_->GetScreenHeight() + 0.6f;

    {
        InterfaceSectionLabel_ = std::make_shared<Label>("インターフェイス", ScreenData(0.f, l_TopPosition, 100.f, 2.5f), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
        InterfaceSectionLabel_->textAlign = Label::TextAlignment::center | Label::TextAlignment::middle;
        InterfaceSectionLabel_->adjustmentFontSize = false;
        InterfaceSectionLabel_->SetFontHandle(l_HeaderFontHandle);
        BodyPanel_->GetPanelInstance()->AddChildTask(std::static_pointer_cast<Task>(InterfaceSectionLabel_));

        UseSystemCursorCheck_ = std::make_shared<CheckBox>("OSのカーソルを使用する", ScreenData(WindowSizeDescription_->GetPositionX(), InterfaceSectionLabel_->GetPositionY() + InterfaceSectionLabel_->GetScreenHeight() + 0.3f, 33.f, 1.5f), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
        UseSystemCursorCheck_->GetTextLabelInstance()->adjustmentFontSize = false;
        UseSystemCursorCheck_->GetTextLabelInstance()->SetFontHandle(l_ItemFontHandle);
        BodyPanel_->GetPanelInstance()->AddChildTask(UseSystemCursorCheck_);
    }

    l_TopPosition = UseSystemCursorCheck_->GetPositionY() + UseSystemCursorCheck_->GetScreenHeight() + 0.6f;

    {
        DebugSectionLabel_ = std::make_shared<Label>("デバッグ", ScreenData(0.f, l_TopPosition, 100.f, 2.5f), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
        DebugSectionLabel_->textAlign = Label::TextAlignment::center | Label::TextAlignment::middle;
        DebugSectionLabel_->adjustmentFontSize = false;
        DebugSectionLabel_->SetFontHandle(l_HeaderFontHandle);
        BodyPanel_->GetPanelInstance()->AddChildTask(std::static_pointer_cast<Task>(DebugSectionLabel_));

        DebugSectionCaution_ = std::make_shared<Label>("⚠ 開発者向け/試験中の機能です", ScreenData(0.f, l_TopPosition + DebugSectionLabel_->GetScreenHeight(), 100.f, 2.f), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
        DebugSectionCaution_->baseColor = color_preset::ORANGE;
        DebugSectionCaution_->textAlign = Label::TextAlignment::center | Label::TextAlignment::middle;
        DebugSectionCaution_->adjustmentFontSize = false;
        DebugSectionCaution_->ChangeFontSize(engine::CastToInt(DefaultScaler_->CalculateHeight(l_HeaderFontSize)));
        DebugSectionCaution_->ChangeFontThickness(l_ItemFontThickness);
        BodyPanel_->GetPanelInstance()->AddChildTask(std::static_pointer_cast<Task>(DebugSectionCaution_));

        SceneFrameDrawCheck_ = std::make_shared<CheckBox>(game_config::SETTINGS_DEBUG_DRAW_SCENE_FRAME, ScreenData(WindowSizeDescription_->GetPositionX(), DebugSectionCaution_->GetPositionY() + DebugSectionCaution_->GetScreenHeight() + 0.3f, 47.f, 1.5f), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
        SceneFrameDrawCheck_->GetTextLabelInstance()->adjustmentFontSize = false;
        SceneFrameDrawCheck_->GetTextLabelInstance()->SetFontHandle(l_ItemFontHandle);
        BodyPanel_->GetPanelInstance()->AddChildTask(SceneFrameDrawCheck_);

        DrawablePointDrawCheck_ = std::make_shared<CheckBox>(game_config::SETTINGS_DEBUG_DRAW_DTASK_POINT, ScreenData(WindowSizeDescription_->GetPositionX(), SceneFrameDrawCheck_->GetPositionY() + SceneFrameDrawCheck_->GetScreenHeight() + 0.3f, SceneFrameDrawCheck_->GetScreenWidth(), 1.5f), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
        DrawablePointDrawCheck_->GetTextLabelInstance()->adjustmentFontSize = false;
        DrawablePointDrawCheck_->GetTextLabelInstance()->SetFontHandle(l_ItemFontHandle);
        BodyPanel_->GetPanelInstance()->AddChildTask(DrawablePointDrawCheck_);
    }

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
        WindowSizeList_->SetSelectedItemNum(engine::CastToInt(std::distance(AllowWindowSizes_.begin(), l_CurrentSize)), false);
    }

    // フルスクリーン
    auto l_IsFullscreen = SettingManager::GetGlobal()->Get<bool>(game_config::SETTINGS_FULLSCREEN);
    if (l_IsFullscreen) FullscreenCheck_->SetChecked(l_IsFullscreen.get());

    // 垂直同期
    auto l_DoVSync = SettingManager::GetGlobal()->Get<bool>(game_config::SETTINGS_VSYNC);
    if (l_DoVSync) VSyncCheck_->SetChecked(l_DoVSync.get());

    // カーソル
    auto l_UseSysCursor = SettingManager::GetGlobal()->Get<bool>(game_config::SETTINGS_MOUSE_USEORIGINAL);
    if (l_UseSysCursor) UseSystemCursorCheck_->SetChecked(!l_UseSysCursor.get());

    // デバッグ - Sceneの枠描画
    auto l_DrawSceneFrame = SettingManager::GetGlobal()->Get<bool>(game_config::SETTINGS_DEBUG_DRAW_SCENE_FRAME);
    if (l_DrawSceneFrame) SceneFrameDrawCheck_->SetChecked(l_DrawSceneFrame.get());

    // デバッグ - DrawableTaskの点描画
    auto l_DrawDrawableTask = SettingManager::GetGlobal()->Get<bool>(game_config::SETTINGS_DEBUG_DRAW_DTASK_POINT);
    if (l_DrawDrawableTask) DrawablePointDrawCheck_->SetChecked(l_DrawDrawableTask.get());
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
		int l_WindowWidth = SettingManager::GetGlobal()->Get<int>(game_config::SETTINGS_RES_WIDTH).get();
        int l_WindowHeight = SettingManager::GetGlobal()->Get<int>(game_config::SETTINGS_RES_HEIGHT).get();

        std::optional<std::pair<int, int>> l_ItemValue = WindowSizeList_->GetSelectedItemValue();
		if (!l_ItemValue.has_value()) return;
		std::tie(l_WindowWidth, l_WindowHeight) = l_ItemValue.value();

        SettingManager::GetGlobal()->Set(game_config::SETTINGS_RES_WIDTH, l_WindowWidth);
        SettingManager::GetGlobal()->Set(game_config::SETTINGS_RES_HEIGHT, l_WindowHeight);
        DxSettings::windowWidth = l_WindowWidth;
        DxSettings::windowHeight = l_WindowHeight;
        SetGraphMode(l_WindowWidth, l_WindowHeight, 32);
        FlexibleScaler::ApplyWindowSizeChanges();
        ReCalculateScreen();
    }

    if (WindowSizeList_->IsOpenList())
    {
        VSyncCheck_->SetEnable(false);
        FullscreenCheck_->SetEnable(false);
        UseSystemCursorCheck_->SetEnable(false);
        SceneFrameDrawCheck_->SetEnable(false);
        DrawablePointDrawCheck_->SetEnable(false);
        return;
    }
    /** 解像度リストが開いていなかったら **/

    VSyncCheck_->SetEnable(true);
    FullscreenCheck_->SetEnable(true);
    UseSystemCursorCheck_->SetEnable(true);
    SceneFrameDrawCheck_->SetEnable(true);
    DrawablePointDrawCheck_->SetEnable(true);

    // フルスクリーン
    if(FullscreenCheck_->IsChanged())
    {
        SettingManager::GetGlobal()->Set(game_config::SETTINGS_FULLSCREEN, FullscreenCheck_->IsChecked());
        DxSettings::isFullScreen = FullscreenCheck_->IsChecked();
        ChangeWindowMode(!DxSettings::isFullScreen);
    }

    // 垂直同期
    if (VSyncCheck_->IsChanged())
    {
        SettingManager::GetGlobal()->Set(game_config::SETTINGS_VSYNC, VSyncCheck_->IsChecked());
        DxSettings::doVSync = VSyncCheck_->IsChecked();
    }

    // カーソル
    if (UseSystemCursorCheck_->IsChanged())
    {
        SettingManager::GetGlobal()->Set(game_config::SETTINGS_MOUSE_USEORIGINAL, !UseSystemCursorCheck_->IsChecked());
        DxSettings::useOriginalCursor = UseSystemCursorCheck_->IsChecked();
    }

    // デバッグ - Sceneの枠描画
    if (SceneFrameDrawCheck_->IsChanged())
    {
        SettingManager::GetGlobal()->Set(game_config::SETTINGS_DEBUG_DRAW_SCENE_FRAME, SceneFrameDrawCheck_->IsChecked());
    }

    // デバッグ - DrawableTaskの点描画
    if (DrawablePointDrawCheck_->IsChanged())
    {
        SettingManager::GetGlobal()->Set(game_config::SETTINGS_DEBUG_DRAW_DTASK_POINT, DrawablePointDrawCheck_->IsChecked());
    }
}

void SettingScene::Draw()
{
    ScreenData l_FixedContentField = DefaultScaler_->Calculate(0.f, 0.f, 100.f, 100.f);
    //SetDrawBlendMode(AlphaBlendMode_, 127);
    DrawBox(engine::CastToInt(l_FixedContentField.posX), engine::CastToInt(l_FixedContentField.posY),
            engine::CastToInt(l_FixedContentField.width), engine::CastToInt(l_FixedContentField.height), color_preset::LIGHT_GREY, TRUE);
}
