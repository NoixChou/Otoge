#include "TitleScene.hpp"
#include "../../../Util/Setting/SettingManager.h"
#include "../../../System/Config.h"
#include "../../../System/Input/KeyboardManager.hpp"
#include "../../../System/Input/MouseManager.hpp"
#include "../../../System/Task/TaskManager.hpp"
#include "../../../System/GUI/Button.hpp"
#include "../../../System/GUI/Label.hpp"
#include "Ball.hpp"


TitleScene::TitleScene() : Scene("TitleScene")
{
    std::shared_ptr<Button> testButton = std::make_shared<Button>("Button", ScreenData(0.f, 0.f, 10.f, 7.f), DefaultScaler_);
    testButton->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    testButton->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(2.f)));
    AddChildTask(std::static_pointer_cast<Task>(testButton));

    // メニュー開閉ボタン
	MenuOpener_ = std::make_shared<Button>("Menu", ScreenData(40.f, 40.f, 20.f, 20.f), DefaultScaler_);
	MenuOpener_->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    MenuOpener_->baseColor = GetColor(179, 229, 252);
    MenuOpener_->animationColor = GetColor(3, 169, 244);
	MenuOpener_->GetTextLabelInstance()->ChangeFontThickness(1);
    MenuOpener_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
	AddChildTask(std::static_pointer_cast<Task>(MenuOpener_));

    //メニュー項目ボタン
    auto l_GroupScreen = ScreenData(50.f, 50.f, 50.f, 14.f);
    MenuGroup_ = std::make_shared<Scene>("MenuGroup", l_GroupScreen, std::make_shared<FlexibleScaler>(static_cast<float>(SettingManager::GetGlobal()->Get<int>(SETTINGS_RES_WIDTH).get()), static_cast<float>(SettingManager::GetGlobal()->Get<int>(SETTINGS_RES_HEIGHT).get())));
    TaskManager::GetInstance()->AddTask(std::static_pointer_cast<Task>(MenuGroup_));

    MenuPlay_ = std::make_shared<Button>("Play", ScreenData(10.f, 0.f, 30.f, 100.f), MenuGroup_->GetDefaultScaler());
    MenuPlay_->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    MenuPlay_->baseColor = GetColor(240, 98, 146);
    MenuPlay_->animationColor = GetColor(233, 30, 99);
    MenuPlay_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    MenuPlay_->GetTextLabelInstance()->ChangeFontThickness(3);
    MenuGroup_->AddChildTask(std::static_pointer_cast<Task>(MenuPlay_));

    MenuOption_ = std::make_shared<Button>("Option", ScreenData(40.f, 0.f, 30.f, 100.f), MenuGroup_->GetDefaultScaler());
    MenuOption_->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    MenuOption_->baseColor = GetColor(120, 144, 156);
    MenuOption_->animationColor = GetColor(38, 50, 56);
    MenuOption_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    MenuOption_->GetTextLabelInstance()->ChangeFontThickness(9);
    MenuGroup_->AddChildTask(std::static_pointer_cast<Task>(MenuOption_));

    MenuClose_ = std::make_shared<Button>("Exit", ScreenData(70.f, 0.f, 30.f, 100.f), MenuGroup_->GetDefaultScaler());
    MenuClose_->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    MenuClose_->baseColor = GetColor(117, 117, 117);
    MenuClose_->animationColor = GetColor(33, 33, 33);
    MenuClose_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    MenuClose_->GetTextLabelInstance()->ChangeFontThickness(6);
    MenuGroup_->AddChildTask(std::static_pointer_cast<Task>(MenuClose_));


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
	float moveSpeed = 50.f * deltaTime;

	if (KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_LEFT))
	{
		Screen_.width -= moveSpeed;
	}
	if (KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_RIGHT))
	{
		Screen_.width += moveSpeed;
	}
	if (KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_UP))
	{
		Screen_.height -= moveSpeed;
	}
	if (KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_DOWN))
	{
		Screen_.height += moveSpeed;
	}

    //MenuOpener_->SetPositionX(MouseManager::GetInstance()->GetMouseRateX(DefaultScaler_));
    //MenuOpener_->SetPositionY(MouseManager::GetInstance()->GetMouseRateY(DefaultScaler_));

    
	static bool isOpened = false;
	static bool isMoving = false;
    if(MenuOpener_->IsClickedMouse())
    {
        timerCount = 0.02f;
		isOpened = !isOpened;
		isMoving = true;
	}

    if(isMoving)
    {
        if(isOpened)
        {
			MenuOpener_->SetPositionX(MenuOpener_->GetPositionX() - (timerCount * 200.f * deltaTime));
        }else
        {
			MenuOpener_->SetPositionX(MenuOpener_->GetPositionX() + (timerCount * 200.f * deltaTime));
        }

        if (MenuOpener_->GetPositionX() < 20.f)
        {
            timerCount = 0.02f;
            isMoving = false;
            MenuOpener_->SetPositionX(20.f);
        }

        if (MenuOpener_->GetPositionX() > 40.f)
        {
            timerCount = 0.02f;
            isMoving = false;
            MenuOpener_->SetPositionX(40.f);
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
    //DrawBox(fixed.posX, fixed.posY, fixed.width, fixed.height, GetColor(0, 0, 0), TRUE);
}