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
	animationColor = GetColor(180,180,180);

    TextLabel_ = std::make_shared<Label>(Label_, ScreenData(0.f, 0.f, 100.f, 100.f), DefaultScaler_);
    TextLabel_->baseColor = textColor;
	
    TextLabel_->AdjustmentFontSize_ = AdjustmentFontSize_;
    TextLabel_->textAlign = Label::TextAlignment::center | Label::TextAlignment::middle;
    AddChildTask(std::static_pointer_cast<Task>(TextLabel_));
}

Button::~Button()
{

}

void Button::GUIUpdate(float deltaTime)
{
    TextLabel_->SetLabel(Label_);
    TextLabel_->baseColor = textColor;

    if (IsDownMouse())
    {
        AddChildTask(std::static_pointer_cast<Task>(
            std::make_shared<ButtonPushedAnimate>(
                DefaultScaler_->CalculatePositionRateX(MouseManager::GetInstance()->GetMouseXf() - GetRawPositionX() - ParentScaler_->GetDiffX()),
                DefaultScaler_->CalculatePositionRateY(MouseManager::GetInstance()->GetMouseYf() - GetRawPositionY() - ParentScaler_->GetDiffY()),
                animationColor, 35.f, DefaultScaler_)
            ));
    }
}

void Button::Draw()
{
    DrawBox(0, 0, static_cast<int>(floor(GetRawScreenWidth())), static_cast<int>(floor(GetRawScreenHeight())), baseColor, TRUE);
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
	Size_ += Size_ * 10.f * deltaTime;
	SetTransparent(GetTransparent() - (100.f * deltaTime));
}

void ButtonPushedAnimate::Draw()
{
	ScreenData circle;
	circle.posX = position.x;
	circle.posY = position.y;
	circle.width = Size_;
	circle.height = Size_;
    const auto l_Fixed = ParentScaler_->Calculate(&circle);
	DrawCircle(static_cast<int>(floor(l_Fixed.posX)), static_cast<int>(floor(l_Fixed.posY)), static_cast<int>(floor(l_Fixed.width + l_Fixed.height / 2.0f)), color, TRUE);
}
