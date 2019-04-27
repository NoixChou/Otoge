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
    MenuGroup_->SetEnable(false);

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

    SettingScene_ = std::make_shared<SettingScene>();
    SettingScene_->SetPositionX(-SettingScene_->GetScreenWidth());
    SettingScene_->SetPriority(10.f);
    SettingScene_->SetEnable(false);
    TaskManager::GetInstance()->AddTask(SettingScene_);

    StartFadeIn();
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
            MenuOpener_->SetPositionX(engine::CastToFloat(ease(timerCount, totalTime, 20.f, 40.f)));
            MenuGroup_->SetPositionX(engine::CastToFloat(ease(timerCount, totalTime, 40.f, 20.f)));
            MenuGroup_->SetTransparent(engine::CastToFloat(ease(timerCount, totalTime, 99.f, 0.f)));
        }else
        {
            MenuOpener_->SetPositionX(engine::CastToFloat(ease(timerCount, totalTime, 40.f, 20.f)));
            MenuGroup_->SetPositionX(engine::CastToFloat(ease(timerCount, totalTime, 20.f, 40.f)));
            MenuGroup_->SetTransparent(engine::CastToFloat(ease(timerCount, totalTime, 0.f, 99.f)));

        }

        if(timerCount > totalTime)
        {
            isMoving = false;
            MenuGroup_->SetEnable(isOpened);
        }
    }

    if(MenuClose_->IsClickedMouse())
    {
        TaskManager::GetInstance()->GameExit();
    }
    if(MenuOption_->IsClickedMouse())
    {
        SettingScene_->StartFadeIn();
    }

    if (SettingScene_->IsFadingIn())
    {
        SetTransparent(100.f - (SettingScene_->GetTransparent() / 2.f));
    }
    else if (SettingScene_->IsFadingOut())
    {
        SetTransparent(100.f - (SettingScene_->GetTransparent() / 2.f));
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
            engine::CastToInt(fixed.height), GetColor(117, 117, 117), TRUE);
}