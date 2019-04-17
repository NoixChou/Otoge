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
    //メニュー項目ボタン
    auto l_GroupScreen = ScreenData(20.f, 43.f, 50.f, 14.f);
    MenuGroup_ = std::make_shared<Scene>("MenuGroup", l_GroupScreen, DefaultScaler_);
    MenuGroup_->SetTransparent(100.f);
    AddChildTask(std::static_pointer_cast<Task>(MenuGroup_));

    MenuPlay_ = std::make_shared<Button>("Play", ScreenData(0.f, 0.f, 30.f, 100.f), MenuGroup_->GetDefaultScaler());
    MenuPlay_->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    MenuPlay_->baseColor = GetColor(240, 98, 146);
    MenuPlay_->animationColor = GetColor(233, 30, 99);
    MenuPlay_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    MenuPlay_->GetTextLabelInstance()->ChangeFontThickness(9);
    MenuPlay_->SetTransparent(0.f);
    MenuGroup_->AddChildTask(std::static_pointer_cast<Task>(MenuPlay_));

    MenuOption_ = std::make_shared<Button>("Option", ScreenData(30.f, 0.f, 30.f, 100.f), MenuGroup_->GetDefaultScaler());
    MenuOption_->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    MenuOption_->baseColor = GetColor(120, 144, 156);
    MenuOption_->animationColor = GetColor(38, 50, 56);
    MenuOption_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    MenuOption_->GetTextLabelInstance()->ChangeFontThickness(1);
    MenuOption_->SetTransparent(0.f);
    MenuGroup_->AddChildTask(std::static_pointer_cast<Task>(MenuOption_));

    MenuClose_ = std::make_shared<Button>("Exit", ScreenData(59.9f, 0.f, 30.f, 100.f), MenuGroup_->GetDefaultScaler());
    MenuClose_->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    MenuClose_->baseColor = GetColor(117, 117, 117);
    MenuClose_->animationColor = GetColor(33, 33, 33);
    MenuClose_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    MenuClose_->GetTextLabelInstance()->ChangeFontThickness(1);
    MenuClose_->SetTransparent(0.f);
    MenuGroup_->AddChildTask(std::static_pointer_cast<Task>(MenuClose_));

    // メニュー開閉ボタン
    MenuOpener_ = std::make_shared<Button>("おなまえ", ScreenData(40.f, 40.f, 20.f, 20.f), DefaultScaler_);
    MenuOpener_->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    MenuOpener_->baseColor = GetColor(179, 229, 252);
    MenuOpener_->animationColor = GetColor(3, 169, 244);
    MenuOpener_->GetTextLabelInstance()->ChangeFontThickness(1);
    MenuOpener_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(3.f)));
    AddChildTask(std::static_pointer_cast<Task>(MenuOpener_));
    
}


TitleScene::~TitleScene()
{
}

void TitleScene::SceneUpdate(float deltaTime)
{
	float moveSpeed = 1.f * deltaTime;

	if (KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_LEFT))
	{
		AddScreenWidth(-moveSpeed);
	}
	if (KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_RIGHT))
	{
		AddScreenWidth(moveSpeed);
	}
	if (KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_UP))
	{
		AddScreenHeight(-moveSpeed);
	}
	if (KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_DOWN))
	{
		AddScreenHeight(moveSpeed);
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
        float menuMove = 8.f / timerCount * deltaTime;
        //MenuGroup_->SetTransparent(((MenuGroup_->GetPositionX()-20.f) / 20.f) * 100.f);
        MenuPlay_->SetTransparent(((MenuGroup_->GetPositionX() - 20.f) / 20.f) * 100.f);
        MenuOption_->SetTransparent(((MenuGroup_->GetPositionX() - 22.f) / 20.f) * 100.f);
        MenuClose_->SetTransparent(((MenuGroup_->GetPositionX() - 24.f) / 20.f) * 100.f);

        if(isOpened)
        {
			MenuOpener_->AddPositionX(-menuMove);
            MenuGroup_->AddPositionX(menuMove);
        }else
        {
			MenuOpener_->AddPositionX(menuMove);
            MenuGroup_->AddPositionX(-menuMove);
        }

        if (MenuOpener_->GetPositionX() < 20.f)
        {
            timerCount = 0.02f;
            isMoving = false;
            MenuOpener_->SetPositionX(20.f);
        }
        if(MenuGroup_->GetPositionX() < 20.f)
        {
            MenuGroup_->SetPositionX(20.f);
        }

        if (MenuOpener_->GetPositionX() > 40.f)
        {
            timerCount = 0.02f;
            isMoving = false;
            MenuOpener_->SetPositionX(40.f);
        }
        if (MenuGroup_->GetPositionX() > 40.f)
        {
            MenuGroup_->SetPositionX(40.f);
        }
    }

    if(MenuClose_->IsClickedMouse())
    {
        TaskManager::GetInstance()->GameExit();
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