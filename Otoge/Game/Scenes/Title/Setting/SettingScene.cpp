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

SettingScene::SettingScene() : Scene("SettingScene", 40.f, 100.f)
{
    TitleBar_ = std::make_shared<Scene>("titlebar", ScreenData(0.f, 0.f, 100.f, 5.f), DefaultScaler_);
    TitleBar_->SetDrawFunction([&]
        {
            ScreenData l_FixedTitlebar = DefaultScaler_->Calculate(0.f, 0.f, 100.f, 100.f);
            DrawBox(l_FixedTitlebar.posX, l_FixedTitlebar.posY, l_FixedTitlebar.width, l_FixedTitlebar.height, GetColor(255, 255, 255), TRUE);
        });

    AddChildTask(std::static_pointer_cast<Task>(TitleBar_));

    CloseButton_ = std::make_shared<Button>("< Close", ScreenData(0.f, 0.f, 20.f, 100.f), TitleBar_->GetDefaultScaler());
    CloseButton_->baseColor = GetColor(255, 255, 255);
    CloseButton_->animationColor = GetColor(33, 33, 33);
    CloseButton_->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    CloseButton_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(2.f)));
    CloseButton_->GetTextLabelInstance()->ChangeFontThickness(1);
    TitleBar_->AddChildTask(std::static_pointer_cast<Task>(CloseButton_));

    auto l_TitleLabel = std::make_shared<Label>("設定", ScreenData(20.0f, 0.0f, 60.f, CloseButton_->GetScreenHeight()), TitleBar_->GetDefaultScaler());
    l_TitleLabel->SetTextAlign(Label::TextAlignment::center | Label::TextAlignment::middle);
    l_TitleLabel->baseColor = GetColor(117, 117, 117);
    l_TitleLabel->AdjustmentFontSize_ = false;
    l_TitleLabel->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    l_TitleLabel->ChangeFontThickness(1);
    TitleBar_->AddChildTask(std::static_pointer_cast<Task>(l_TitleLabel));


    BodyPanel_ = std::make_shared<ScrollablePanel>("bodypanel", ScreenData(0.f, 5.f, 100.f, 95.f), ScreenData(0.f, 0.f, 100.f, 150.f), DefaultScaler_);
    BodyPanel_->SetDrawFunction([&]
        {
            ScreenData l_FixedContentField = DefaultScaler_->Calculate(0.f, 0.f, 100.f, 100.f);
            DrawBox(l_FixedContentField.posX, l_FixedContentField.posY, l_FixedContentField.width, l_FixedContentField.height, GetColor(240, 240, 240), TRUE);
        });

    AddChildTask(std::static_pointer_cast<Task>(BodyPanel_));

    auto testSlide = std::make_shared<SlideBar>("test", ScreenData(30.f, 0.f, 70.f, 5.f), BodyPanel_->GetDefaultScaler());
    BodyPanel_->GetPanelInstance()->AddChildTask(std::static_pointer_cast<Task>(testSlide));

    auto testLabel = std::make_shared<Label>("マウス感度:", ScreenData(0.f, 0.f, 30.f, 5.f), BodyPanel_->GetDefaultScaler());
    testLabel->textAlign = Label::TextAlignment::center | Label::TextAlignment::middle;
    BodyPanel_->GetPanelInstance()->AddChildTask(std::static_pointer_cast<Task>(testLabel));
}


SettingScene::~SettingScene()
{
}

void SettingScene::SceneFadeIn(float deltaTime)
{
    float totalTime = 0.5f;
    Easing::EaseFunction ease = Easing::OutExp;

    SetTransparent(ease(timerCount, totalTime, 100.f, 0.f));
    SetPositionX(ease(timerCount, totalTime, 0.f, -GetScreenWidth()));

    if (timerCount > totalTime)
    {
        SetTransparent(100.f);
        SetPositionX(0.f);
        IsFadingIn_ = false;
        SetEnable(true);
    }
}

void SettingScene::SceneFadeOut(float deltaTime)
{
    float totalTime = 0.3f;
    Easing::EaseFunction ease = Easing::InExp;

    SetTransparent(ease(timerCount, totalTime, 0.f, 100.f));
    SetPositionX(ease(timerCount, totalTime, -GetScreenWidth(), 0.f));

    if (timerCount > totalTime)
    {
        IsFadingOut_ = false;
        SetEnable(false);
    }
}

void SettingScene::SceneUpdate(float deltaTime)
{
    if(CloseButton_->IsClickedMouse() || (!IsOnMouse() && MouseManager::GetInstance()->IsDownButton(MOUSE_INPUT_LEFT) && IsEnable()))
    {
        SetEnable(false);
        StartFadeOut();
    }
}

void SettingScene::Draw()
{

}