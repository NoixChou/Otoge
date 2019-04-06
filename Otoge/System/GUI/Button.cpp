#include "Button.hpp"
#include "../../Util/Calculate/Screen/FontStringCalculator.hpp"
#include "../Task/TaskManager.hpp"
#include "../Input/MouseManager.hpp"
#include "../../Game/Scenes/Title/Ball.hpp"

Button::Button(const std::string& label, const ScreenData& layoutScreen, std::shared_ptr<FlexibleScaler> parentScaler) : GUI(label + "\"<Button>\"", layoutScreen, parentScaler)
{
    Label_ = label;
    textColor = GetColor(50, 50, 50);
	baseColor = GetColor(240, 240, 240);

    TextLabel_ = std::make_shared<Label>(Label_, ScreenData(0.f, 0.f, 100.f, 100.f), DefaultScaler_);
    TextLabel_->baseColor = textColor;
	
    TextLabel_->AdjustmentFontSize_ = AdjustmentFontSize_;
    TextLabel_->textAlign = Label::TextAlignment::center | Label::TextAlignment::middle;
    AddChildTask(std::static_pointer_cast<Task>(TextLabel_));
	Logger_->Debug("button defScaler: w:" + std::to_string(DefaultScaler_->GetScreenWidth()) + ", h:" + std::to_string(DefaultScaler_->GetScreenHeight()));

}

Button::~Button()
{

}

void Button::GUIUpdate(float deltaTime)
{
    TextLabel_->SetLabel(Label_);
    TextLabel_->baseColor = textColor;
}

void Button::Draw()
{
    DrawBox(0, 0, screen.width, screen.height, baseColor, TRUE);

	if (IsHoldMouse() && timerCount > 0.2f)
	{
		
	}

	if(IsDownMouse())
	{
		AddChildTask(std::static_pointer_cast<Task>(
			std::make_shared<ButtonPushedAnimate>(MouseManager::GetInstance()->GetMouseRateX(DefaultScaler_), MouseManager::GetInstance()->GetMouseRateY(DefaultScaler_), baseColor, 35.f, DefaultScaler_)
		));
		timerCount = 0.f;
	}
}

void Button::SetTextLabelInstance(std::shared_ptr<Label> textLabel)
{
    TextLabel_.reset(textLabel.get());
}

std::shared_ptr<Label> Button::GetTextLabelInstance()
{
    return TextLabel_;
}


/* ボタン押下時のアニメーション */
ButtonPushedAnimate::ButtonPushedAnimate(float x, float y, unsigned color, float size, std::shared_ptr<FlexibleScaler> parentScaler) : DrawableTask("ButtonPushedAnimation", x, y, 0.f, parentScaler)
{
	this->color = color;
	Size_ = size;
	SetTransparent(50.f);
	hasLifespan = true;
	lifespan = 0.5f;
}

ButtonPushedAnimate::~ButtonPushedAnimate()
{

}

void ButtonPushedAnimate::PreUpdate(float deltaTime)
{
	Size_ += 500.0f * deltaTime;
	SetTransparent(GetTransparent() - (100.f * deltaTime));
}

void ButtonPushedAnimate::Draw()
{
	ScreenData circle;
	circle.posX = position.x;
	circle.posY = position.y;
	circle.width = Size_;
	circle.height = Size_;

	ScreenData fixed = ParentScaler_->Calculate(&circle);
	DrawCircle(fixed.posX, fixed.posY, fixed.width + fixed.height / 2.0f, color, TRUE);
}
