#include "SettingScene.hpp"
#include "../../../../Util/Setting/SettingManager.h"
#include "../../../../System/Config.h"
#include "../../../../System/Input/KeyboardManager.hpp"
#include "../../../../System/Input/MouseManager.hpp"
#include "../../../../System/Task/TaskManager.hpp"
#include "../../../../System/GUI/Button.hpp"
#include "../../../../System/GUI/Label.hpp"
#include "../../../../Util/Calculate/Animation/Easing.hpp"

SettingScene::SettingScene() : Scene("SettingScene", 40.f)
{
    CloseButton_ = std::make_shared<Button>("< Close", ScreenData(0.f, 0.f, 20.f, 5.f), DefaultScaler_);
    CloseButton_->baseColor = GetColor(255, 255, 255);
    CloseButton_->animationColor = GetColor(33, 33, 33);
    CloseButton_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    CloseButton_->GetTextLabelInstance()->ChangeFontThickness(1);
    CloseButton_->SetTransparent(100.f);
    AddChildTask(std::static_pointer_cast<Task>(CloseButton_));

    std::shared_ptr<Label> l_TitleLabel = std::make_shared<Label>(u8"設定", ScreenData(20.0f, 0.0f, 60.f, CloseButton_->GetScreenHeight()), DefaultScaler_);
    l_TitleLabel->SetTextAlign(Label::TextAlignment::center | Label::TextAlignment::middle);
    l_TitleLabel->baseColor = GetColor(117, 117, 117);
    l_TitleLabel->AdjustmentFontSize_ = false;
    l_TitleLabel->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    l_TitleLabel->ChangeFontThickness(1);
    AddChildTask(std::static_pointer_cast<Task>(l_TitleLabel));
}


SettingScene::~SettingScene()
{
}

void SettingScene::SceneFadeIn(float deltaTime)
{
    float totalTime = 0.5f;
    Easing::EaseFunction ease = Easing::OutExp;

    SetTransparent(ease(timerCount, totalTime, 100.f, 0.f));
    SetPositionX(ease(timerCount, totalTime, 0.f, -20.f));

    if (timerCount > totalTime)
    {
        IsFadingIn_ = false;
    }
}

void SettingScene::SceneFadeOut(float deltaTime)
{
    float totalTime = 0.5f;
    Easing::EaseFunction ease = Easing::OutQuart;

    SetTransparent(ease(timerCount, totalTime, 0.f, 100.f));
    SetPositionX(ease(timerCount, totalTime, -20.f, 0.f));

    if (timerCount > totalTime)
    {
        IsFadingOut_ = false;
        Terminate();
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
    ScreenData l_TitleBar;
    l_TitleBar.posX = 0.f;
    l_TitleBar.posY = 0.f;
    l_TitleBar.width = 100.f;
    l_TitleBar.height = CloseButton_->GetScreenHeight();
    ScreenData l_ContentField;
    l_ContentField.posX = 0.f;
    l_ContentField.posY = l_TitleBar.posY + l_TitleBar.height;
    l_ContentField.width = 100.f;
    l_ContentField.height = 100.f;

    ScreenData l_FixedTitlebar = DefaultScaler_->Calculate(&l_TitleBar);
    ScreenData l_FixedContentField = DefaultScaler_->Calculate(&l_ContentField);
    DrawBox(l_FixedTitlebar.posX, l_FixedTitlebar.posY, l_FixedTitlebar.width, l_FixedTitlebar.height, GetColor(255, 255, 255), TRUE);
    DrawBox(l_FixedContentField.posX, l_FixedContentField.posY, l_FixedContentField.width, l_FixedContentField.height, GetColor(127, 127, 127), TRUE);
}