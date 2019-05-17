#include "TitleScene.hpp"
#include "../../../Util/Setting/SettingManager.h"
#include "../../../System/Config.h"
#include "../../../System/Input/KeyboardManager.hpp"
#include "../../../System/Input/MouseManager.hpp"
#include "../../../System/Task/TaskManager.hpp"
#include "../../../System/GUI/Button.hpp"
#include "../../../System/GUI/Label.hpp"
#include "../../../Util/Calculate/Animation/Easing.hpp"
#include "Setting/SettingScene.hpp"
#include "../../../System/GlobalMethod.hpp"
#include "../../../Util/Visual/Color.hpp"
#include "../Play/Game3DTest.hpp"
#include "../../../System/GUI/RoundedButton.hpp"
std::weak_ptr<Scene> TitleScene::SettingScene_;

TitleScene::TitleScene() : Scene("TitleScene")
{
    // メニュー開閉ボタン
    auto l_MenuOpener = std::make_shared<Button>("おなまえ", ScreenData(40.f, 40.f, 20.f, 20.f, true), DefaultScaler_);
    MenuOpener_ = l_MenuOpener;
    MenuOpener_.lock()->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    MenuOpener_.lock()->baseColor = color_preset::BLUE;
    MenuOpener_.lock()->GetTextLabelInstance()->ChangeFontThickness(1);
    MenuOpener_.lock()->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    MenuOpener_.lock()->SetPriority(10.f);
    AddChildTask(std::static_pointer_cast<Task>(MenuOpener_.lock()));

    //メニュー項目ボタン
    auto l_GroupScreen = ScreenData(20.f, 43.f, 60.f, 14.f);
    auto l_MenuGroup = std::make_shared<Scene>("MenuGroup", l_GroupScreen, DefaultScaler_);
    MenuGroup_ = l_MenuGroup;
    MenuGroup_.lock()->SetTransparent(0.f);
    MenuGroup_.lock()->SetPriority(0.f);
    AddChildTask(std::static_pointer_cast<Task>(MenuGroup_.lock()));
    MenuGroup_.lock()->SetEnable(false);

    auto l_MenuPlay = std::make_shared<Button>("Play", ScreenData(0.f, 0.f, 100.f / 3.f, 100.f), MenuGroup_.lock()->GetDefaultScaler());
    MenuPlay_ = l_MenuPlay;
    MenuPlay_.lock()->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    MenuPlay_.lock()->baseColor = color_preset::DEEP_ORANGE;
    MenuPlay_.lock()->animationColor = color_preset::DEEP_ORANGE;
    MenuPlay_.lock()->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    MenuPlay_.lock()->GetTextLabelInstance()->ChangeFontThickness(9);
    MenuPlay_.lock()->SetTransparent(100.f);
    MenuGroup_.lock()->AddChildTask(std::static_pointer_cast<Task>(MenuPlay_.lock()));

    auto l_MenuOption = std::make_shared<Button>("Option", ScreenData(MenuPlay_.lock()->GetScreenWidth(), 0.f, 100.f / 3.f, MenuPlay_.lock()->GetScreenHeight()), MenuGroup_.lock()->GetDefaultScaler());
    MenuOption_ = l_MenuOption;
    MenuOption_.lock()->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    MenuOption_.lock()->baseColor = color_preset::BLUE_GREY;
    MenuOption_.lock()->animationColor = color_preset::DARK_BLUE_GREY;
    MenuOption_.lock()->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    MenuOption_.lock()->GetTextLabelInstance()->ChangeFontThickness(2);
    MenuOption_.lock()->SetTransparent(100.f);
    MenuGroup_.lock()->AddChildTask(std::static_pointer_cast<Task>(MenuOption_.lock()));

    auto l_MenuClose = std::make_shared<Button>("Exit", ScreenData(MenuPlay_.lock()->GetScreenWidth() + MenuOption_.lock()->GetScreenWidth(), 0.f, 100.f / 3.f, MenuOption_.lock()->GetScreenHeight()), MenuGroup_.lock()->GetDefaultScaler());
    MenuClose_ = l_MenuClose;
    MenuClose_.lock()->baseColor = color_preset::DARK_GREY;
    MenuClose_.lock()->animationColor = GetColor(33, 33, 33);
    MenuClose_.lock()->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    MenuClose_.lock()->textColor = color_preset::GREY;
    MenuClose_.lock()->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    MenuClose_.lock()->GetTextLabelInstance()->ChangeFontThickness(2);
    MenuClose_.lock()->SetTransparent(100.f);
    MenuGroup_.lock()->AddChildTask(std::static_pointer_cast<Task>(MenuClose_.lock()));

    if (SettingScene_.expired())
    {
        auto l_SettingScene = std::make_shared<SettingScene>();
        SettingScene_ = l_SettingScene;
        SettingScene_.lock()->SetPositionX(-SettingScene_.lock()->GetScreenWidth());
        SettingScene_.lock()->SetPriority(10.f);
        SettingScene_.lock()->SetEnable(false);
        TaskManager::GetInstance()->AddTask(SettingScene_.lock());
    }
}


TitleScene::~TitleScene()
{
    
}

void TitleScene::SceneFadeIn(float deltaTime)
{
    float totalTime = 0.5f;
    Easing::EaseFunction ease = Easing::OutExp;

    SetTransparent(engine::CastToFloat(ease(timerCount, totalTime, 100.f, 0.f)));
    SetPositionX(engine::CastToFloat(ease(timerCount, totalTime, 0.f, -20.f)));

    if (timerCount > totalTime)
    {
        IsFadingIn_ = false;
        SetTransparent(100.f);
        SetPositionX(0.f);
    }
}

void TitleScene::SceneUpdate(float deltaTime)
{
    if(MenuOpener_.lock()->IsClickedMouse())
    {
        timerCount = 0.0f;
		isOpened = !isOpened;
		isMoving = true;
	}
    
    if(isMoving)
    {
        if (MenuPlay_.lock()->GetRawPositionX() + MenuGroup_.lock()->GetRawPositionX() < MenuOpener_.lock()->GetRawPositionX())
            MenuPlay_.lock()->SetTransparent(0.f);
        else
            MenuPlay_.lock()->SetTransparent(100.f);

        if (MenuOption_.lock()->GetRawPositionX() + MenuGroup_.lock()->GetRawPositionX() < MenuOpener_.lock()->GetRawPositionX())
            MenuOption_.lock()->SetTransparent(0.f);
        else
            MenuOption_.lock()->SetTransparent(100.f);

        if (MenuClose_.lock()->GetRawPositionX() + MenuGroup_.lock()->GetRawPositionX() < MenuOpener_.lock()->GetRawPositionX())
            MenuClose_.lock()->SetTransparent(0.f);
        else
            MenuClose_.lock()->SetTransparent(100.f);

        float totalTime = 0.5f;
        Easing::EaseFunction ease = Easing::OutQuint;

        if(isOpened)
        {
            MenuOpener_.lock()->SetPositionX(engine::CastToFloat(ease(timerCount, totalTime, 20.f, 40.f)));
            MenuGroup_.lock()->SetPositionX(engine::CastToFloat(ease(timerCount, totalTime, 40.f, 20.f)));
            MenuGroup_.lock()->SetTransparent(engine::CastToFloat(ease(timerCount, totalTime, 99.f, 0.f)));
        }else
        {
            MenuOpener_.lock()->SetPositionX(engine::CastToFloat(ease(timerCount, totalTime, 40.f, 20.f)));
            MenuGroup_.lock()->SetPositionX(engine::CastToFloat(ease(timerCount, totalTime, 20.f, 40.f)));
            MenuGroup_.lock()->SetTransparent(engine::CastToFloat(ease(timerCount, totalTime, 0.f, 99.f)));

        }

        if(timerCount > totalTime)
        {
            isMoving = false;
            MenuGroup_.lock()->SetEnable(isOpened);
        }
    }

    if(MenuPlay_.lock()->IsClickedMouse())
    {
        TaskManager::GetInstance()->AddTask(std::make_shared<Game3DTest>());
        Terminate();
    }
    if(MenuClose_.lock()->IsClickedMouse())
    {
        TaskManager::GetInstance()->GameExit();
    }
    if(MenuOption_.lock()->IsClickedMouse())
    {
        SettingScene_.lock()->StartFadeIn();
    }

    if (SettingScene_.lock()->IsFadingIn())
    {
        SetTransparent(100.f - (SettingScene_.lock()->GetTransparent() / 2.f));
    }
    else if (SettingScene_.lock()->IsFadingOut())
    {
        SetTransparent(100.f - (SettingScene_.lock()->GetTransparent() / 2.f));
    }
}

void TitleScene::Draw()
{
    ScreenData d;
    d.posX = 0.f;
    d.posY = 0.f;
    d.width = 100.f;
    d.height = 100.f;
    ScreenData fixed = DefaultScaler_->Calculate(d);
    
    DrawBox(engine::CastToInt(fixed.posX),
            engine::CastToInt(fixed.posY),
            engine::CastToInt(fixed.width),
            engine::CastToInt(fixed.height), color_preset::BLACK, TRUE);
    
}