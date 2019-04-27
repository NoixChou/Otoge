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

SettingScene::SettingScene() : Scene("SettingScene", 40.f, 100.f)
{
    TitleBar_ = std::make_shared<Scene>("titlebar", ScreenData(0.f, 0.f, 100.f, 5.f), DefaultScaler_);
    TitleBar_->SetDrawFunction([&]
        {
            ScreenData l_FixedTitlebar = DefaultScaler_->Calculate(0.f, 0.f, 100.f, 100.f);
            DrawBox(static_cast<int>(l_FixedTitlebar.posX), static_cast<int>(l_FixedTitlebar.posY), static_cast<int>(l_FixedTitlebar.width), static_cast<int>(l_FixedTitlebar.height), GetColor(255, 255, 255), TRUE);
        });
    TitleBar_->SetPriority(20.f);
    AddChildTask(std::static_pointer_cast<Task>(TitleBar_));

    CloseButton_ = std::make_shared<Button>("< Close", ScreenData(0.f, 0.f, 20.f, 100.f), TitleBar_->GetDefaultScaler());
    CloseButton_->baseColor = GetColor(255, 255, 255);
    CloseButton_->animationColor = GetColor(33, 33, 33);
    CloseButton_->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    CloseButton_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(2.f)));
    CloseButton_->GetTextLabelInstance()->ChangeFontThickness(1);
    CloseButton_->SetPriority(5.f);
    TitleBar_->AddChildTask(std::static_pointer_cast<Task>(CloseButton_));

    auto l_TitleLabel = std::make_shared<Label>("設定", ScreenData(0.0f, 0.0f, 100.f, CloseButton_->GetScreenHeight()), TitleBar_->GetDefaultScaler());
    l_TitleLabel->SetTextAlign(Label::TextAlignment::center | Label::TextAlignment::middle);
    l_TitleLabel->baseColor = GetColor(117, 117, 117);
    l_TitleLabel->AdjustmentFontSize_ = false;
    l_TitleLabel->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    l_TitleLabel->ChangeFontThickness(1);
    TitleBar_->AddChildTask(std::static_pointer_cast<Task>(l_TitleLabel));


    BodyPanel_ = std::make_shared<ScrollablePanel>("bodypanel", ScreenData(0.f, 5.f, 100.f, 95.f), ScreenData(0.f, 0.f, 100.f, 190.f), DefaultScaler_);
    BodyPanel_->SetDrawFunction([&]
        {
            ScreenData l_FixedContentField = DefaultScaler_->Calculate(0.f, 0.f, 100.f, 100.f);
            DrawBox(engine::CastToInt(l_FixedContentField.posX), engine::CastToInt(l_FixedContentField.posY),
                    engine::CastToInt(l_FixedContentField.width), engine::CastToInt(l_FixedContentField.height), GetColor(240, 240, 240), TRUE);
        });
    BodyPanel_->SetPriority(0.f);
    AddChildTask(std::static_pointer_cast<Task>(BodyPanel_));

    {
        DisplaySectionLabel_ = std::make_shared<Label>("ディスプレイ", ScreenData(0.f, 0.f, 100.f, 2.5f), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
        DisplaySectionLabel_->textAlign = Label::TextAlignment::center | Label::TextAlignment::middle;
        BodyPanel_->GetPanelInstance()->AddChildTask(std::static_pointer_cast<Task>(DisplaySectionLabel_));

        {
            WindowWidthDescription_ = std::make_shared<Label>("幅:", ScreenData(0.f, DisplaySectionLabel_->GetScreenHeight(), 12.f, 2.5f), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
            WindowWidthDescription_->textAlign = Label::TextAlignment::right | Label::TextAlignment::middle;
            WindowWidthDescription_->AdjustmentFontSize_ = false;
            WindowWidthDescription_->ChangeFontSize(engine::CastToInt(WindowWidthDescription_->GetDefaultScaler()->CalculateHeight(50.f)));
            BodyPanel_->GetPanelInstance()->AddChildTask(std::static_pointer_cast<Task>(WindowWidthDescription_));

            WindowWidthSlider_ = std::make_shared<SlideBar>("windowWidth", ScreenData(WindowWidthDescription_->GetScreenWidth(), DisplaySectionLabel_->GetScreenHeight(), 100.f - WindowWidthDescription_->GetScreenWidth(), WindowWidthDescription_->GetScreenHeight()), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
            BodyPanel_->GetPanelInstance()->AddChildTask(std::static_pointer_cast<Task>(WindowWidthSlider_));
        }
        {
            WindowHeightDescription_ = std::make_shared<Label>("高さ:", ScreenData(0.f, WindowWidthSlider_->GetPositionY() + WindowWidthSlider_->GetScreenHeight(), WindowWidthDescription_->GetScreenWidth(), WindowWidthDescription_->GetScreenHeight()), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
            WindowHeightDescription_->textAlign = Label::TextAlignment::right | Label::TextAlignment::middle;
            WindowHeightDescription_->AdjustmentFontSize_ = false;
            WindowHeightDescription_->ChangeFontSize(engine::CastToInt(WindowHeightDescription_->GetDefaultScaler()->CalculateHeight(50.f)));
            BodyPanel_->GetPanelInstance()->AddChildTask(std::static_pointer_cast<Task>(WindowHeightDescription_));

            WindowHeightSlider_ = std::make_shared<SlideBar>("windowHeight", ScreenData(WindowWidthSlider_->GetPositionX(), WindowHeightDescription_->GetPositionY(), WindowWidthSlider_->GetScreenWidth(), WindowHeightDescription_->GetScreenHeight()), BodyPanel_->GetPanelInstance()->GetDefaultScaler());
            BodyPanel_->GetPanelInstance()->AddChildTask(std::static_pointer_cast<Task>(WindowHeightSlider_));
        }
    }
}


SettingScene::~SettingScene()
{
}

void SettingScene::OnStartedFadeIn()
{
    TaskManager::GetInstance()->SetModalTask(weak_from_this());
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
}

void SettingScene::Draw()
{

}