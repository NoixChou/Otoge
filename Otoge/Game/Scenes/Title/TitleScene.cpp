#include "TitleScene.h"
#include "../../../Util/Setting/SettingManager.h"
#include "../../../System/Config.h"
#include "../../../System/Input/KeyboardManager.hpp"
#include "../../../System/Input/MouseManager.hpp"
#include "../../../System/Task/TaskManager.hpp"
#include "../../../System/GUI/Button.hpp"
#include "../../../System/GUI/Label.hpp"


TitleScene::TitleScene() : Scene("TitleScene")
{
    std::shared_ptr<Button> testButton = std::make_shared<Button>("–ß‚é", ScreenData(0.f, 0.f, 10.f, 7.f), DefaultScaler_);
    testButton->baseColor = GetColor(255, 255, 255);
    testButton->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    testButton->GetTextLabelInstance()->ChangeFontSize(DefaultScaler_->CalculateHeight(2.f));
    AddChildTask(std::static_pointer_cast<Task>(testButton));

    std::shared_ptr<Label> testLabel = std::make_shared<Label>("TinyLabel", ScreenData(11.f, 0.f, 6.f, 1.5f), DefaultScaler_);
    testLabel->baseColor = GetColor(255, 255, 255);
    AddChildTask(std::static_pointer_cast<Task>(testLabel));

    std::shared_ptr<Label> testLabel4 = std::make_shared<Label>("SmallLabel", ScreenData(11.f, 1.5f, 8.f, 2.4f), DefaultScaler_);
    testLabel4->baseColor = GetColor(255, 255, 255);
    AddChildTask(std::static_pointer_cast<Task>(testLabel4));

    std::shared_ptr<Label> testLabel2 = std::make_shared<Label>("MediumLabel", ScreenData(11.f, 3.9f, 12.f, 3.0f), DefaultScaler_);
    testLabel2->baseColor = GetColor(255, 255, 255);
    AddChildTask(std::static_pointer_cast<Task>(testLabel2));

    std::shared_ptr<Label> testLabel3 = std::make_shared<Label>("LargeLabel", ScreenData(11.f, 6.9f, 18.f, 6.0f), DefaultScaler_);
    testLabel3->baseColor = GetColor(255, 255, 255);
    AddChildTask(std::static_pointer_cast<Task>(testLabel3));
    
}


TitleScene::~TitleScene()
{
}

void TitleScene::SceneUpdate(float deltaTime)
{
    /*if (KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_LEFT))
    {
        moveX = -1.f;
    }
    if (KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_RIGHT))
    {
        moveX = 1.f;
    }
    if(KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_UP))
    {
        moveY = -1.f;
    }
    if (KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_DOWN))
    {
        moveY = 1.f;
    }

    moveX *= 100.f;
    moveY *= 100.f;

    drawX += moveX * deltaTime;
    drawY += moveY * deltaTime;

    if(MouseManager::GetInstance()->IsMovedMouse())
    {
        drawX = MouseManager::GetInstance()->GetMouseXf();
        drawY = MouseManager::GetInstance()->GetMouseYf();
    }*/
}

void TitleScene::Draw()
{
    ScreenData d;
    d.posX = 50.f;
    d.posY = 0.f;
    d.width = 50.f;
    d.height = 50.f;
    ScreenData fixed = DefaultScaler_->Calculate(&d);
    DrawBox(fixed.posX, fixed.posY, fixed.posX + fixed.width, fixed.posY + fixed.height, GetColor(255, 255, 255), FALSE);
}
