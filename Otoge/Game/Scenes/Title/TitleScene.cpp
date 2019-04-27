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

TitleScene::TitleScene() : Scene("TitleScene")
{
    // メニュー開閉ボタン
    MenuOpener_ = std::make_shared<Button>("おなまえ", ScreenData(40.f, 40.f, 20.f, 20.f, true), DefaultScaler_);
    MenuOpener_->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    MenuOpener_->baseColor = GetColor(179, 229, 252);
    MenuOpener_->animationColor = GetColor(3, 169, 244);
    MenuOpener_->GetTextLabelInstance()->ChangeFontThickness(1);
    MenuOpener_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    MenuOpener_->SetPriority(10.f);
    AddChildTask(std::static_pointer_cast<Task>(MenuOpener_));

    //メニュー項目ボタン
    auto l_GroupScreen = ScreenData(20.f, 43.f, 60.f, 14.f);
    MenuGroup_ = std::make_shared<Scene>("MenuGroup", l_GroupScreen, DefaultScaler_);
    MenuGroup_->SetTransparent(0.f);
    MenuGroup_->SetPriority(0.f);
    AddChildTask(std::static_pointer_cast<Task>(MenuGroup_));

    MenuPlay_ = std::make_shared<Button>("Play", ScreenData(0.f, 0.f, 100.f / 3.f, 100.f), MenuGroup_->GetDefaultScaler());
    MenuPlay_->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    MenuPlay_->baseColor = GetColor(240, 98, 146);
    MenuPlay_->animationColor = GetColor(233, 30, 99);
    MenuPlay_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    MenuPlay_->GetTextLabelInstance()->ChangeFontThickness(9);
    MenuPlay_->SetTransparent(100.f);
    MenuGroup_->AddChildTask(std::static_pointer_cast<Task>(MenuPlay_));

    MenuOption_ = std::make_shared<Button>("Option", ScreenData(MenuPlay_->GetScreenWidth(), 0.f, 100.f / 3.f, 100.f), MenuGroup_->GetDefaultScaler());
    MenuOption_->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    MenuOption_->baseColor = GetColor(120, 144, 156);
    MenuOption_->animationColor = GetColor(38, 50, 56);
    MenuOption_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    MenuOption_->GetTextLabelInstance()->ChangeFontThickness(1);
    MenuOption_->SetTransparent(100.f);
    MenuGroup_->AddChildTask(std::static_pointer_cast<Task>(MenuOption_));

    MenuClose_ = std::make_shared<Button>("Exit", ScreenData(MenuPlay_->GetScreenWidth() + MenuOption_->GetScreenWidth(), 0.f, 100.f / 3.f, 100.f), MenuGroup_->GetDefaultScaler());
    MenuClose_->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    MenuClose_->baseColor = GetColor(117, 117, 117);
    MenuClose_->animationColor = GetColor(33, 33, 33);
    MenuClose_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    MenuClose_->GetTextLabelInstance()->ChangeFontThickness(1);
    MenuClose_->SetTransparent(100.f);
    MenuGroup_->AddChildTask(std::static_pointer_cast<Task>(MenuClose_));
}


TitleScene::~TitleScene()
{
}

void TitleScene::SceneFadeIn(float deltaTime)
{
    float totalTime = 0.5f;
    Easing::EaseFunction ease = Easing::OutExp;

    SetTransparent(ease(timerCount, totalTime, 100.f, 0.f));
    SetPositionX(ease(timerCount, totalTime, 0.f, -20.f));

    if (timerCount > totalTime)
    {
        IsFadingIn_ = false;
        SetTransparent(100.f);
        SetPositionX(0.f);
    }
}

void TitleScene::SceneUpdate(float deltaTime)
{
	static bool isOpened = false;
	static bool isMoving = false;

    if(MenuOpener_->IsClickedMouse())
    {
        timerCount = 0.0f;
		isOpened = !isOpened;
		isMoving = true;
	}
    
    if(isMoving)
    {
        float menuMove = 8.f / timerCount * deltaTime;
        //MenuGroup_->SetTransparent(((MenuGroup_->GetPositionX()-20.f) / 20.f) * 100.f);
        /*MenuPlay_->SetTransparent(((MenuGroup_->GetPositionX() - 20.f) / 20.f) * 100.f);
        MenuOption_->SetTransparent(((MenuGroup_->GetPositionX() - 22.f) / 20.f) * 100.f);
        MenuClose_->SetTransparent(((MenuGroup_->GetPositionX() - 24.f) / 20.f) * 100.f);*/
        if (MenuPlay_->GetRawPositionX() + MenuGroup_->GetRawPositionX() < MenuOpener_->GetRawPositionX())
            MenuPlay_->SetTransparent(0.f);
        else
            MenuPlay_->SetTransparent(100.f);

        if (MenuOption_->GetRawPositionX() + MenuGroup_->GetRawPositionX() < MenuOpener_->GetRawPositionX())
            MenuOption_->SetTransparent(0.f);
        else
            MenuOption_->SetTransparent(100.f);

        if (MenuClose_->GetRawPositionX() + MenuGroup_->GetRawPositionX() < MenuOpener_->GetRawPositionX())
            MenuClose_->SetTransparent(0.f);
        else
            MenuClose_->SetTransparent(100.f);

        float totalTime = 0.5f;
        Easing::EaseFunction ease = Easing::OutQuint;

        if(isOpened)
        {
            MenuOpener_->SetPositionX(ease(timerCount, totalTime, 20.f, 40.f));
            MenuGroup_->SetPositionX(ease(timerCount, totalTime, 40.f, 20.f));
            MenuGroup_->SetTransparent(ease(timerCount, totalTime, 99.f, 0.f));
        }else
        {
            MenuOpener_->SetPositionX(ease(timerCount, totalTime, 40.f, 20.f));
            MenuGroup_->SetPositionX(ease(timerCount, totalTime, 20.f, 40.f));
            MenuGroup_->SetTransparent(ease(timerCount, totalTime, 0.f, 99.f));

        }

        if(timerCount > totalTime)
        {
            isMoving = false;
        }
    }

    if(MenuClose_->IsClickedMouse())
    {
        TaskManager::GetInstance()->GameExit();
    }
    if(MenuOption_->IsClickedMouse())
    {
        if (SettingScene_.expired())
        {
            auto setting = std::make_shared<SettingScene>();
            setting->SetPriority(10.f);
            SettingScene_ = setting;
            TaskManager::GetInstance()->AddTask(setting);
            timerCount = 0.f;
        }
    }

    if (!SettingScene_.expired())
    {
        auto l_SettingScene = SettingScene_.lock();
        if(l_SettingScene->IsFadingIn())
        {
            SetTransparent(100.f - (l_SettingScene->GetTransparent() / 2.f));
            if (timerCount >= 0.5f)
            {
                SetEnable(false);
            }
            //SetTransparent(Easing::OutExp(timerCount, 0.5f, 50.f, 100.f));
        }else if(l_SettingScene->IsFadingOut())
        {
            SetTransparent(100.f - (l_SettingScene->GetTransparent() / 2.f));
            if (timerCount >= 0.5f)
            {
                SetEnable(true);
            }
            //SetTransparent(Easing::OutExp(timerCount, 0.5f, 100.f, 50.f));
        }
        if (timerCount > 0.5f)
        {
            timerCount = 0.f;
        }
    }
    
	if (IsChangedScreen())
	{
		ReCalculateScreen();
		RefreshDrawBuffer();
	}
}

void TitleScene::Draw()
{
    ScreenData d;
    d.posX = 0.f;
    d.posY = 0.f;
    d.width = 100.f;
    d.height = 100.f;
    ScreenData fixed = DefaultScaler_->Calculate(&d);
    DrawBox(fixed.posX, fixed.posY, fixed.width, fixed.height, GetColor(117, 117, 117), TRUE);
}
