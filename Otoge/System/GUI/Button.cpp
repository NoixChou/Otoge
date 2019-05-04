#include "Button.hpp"
#include "../../Util/Calculate/Screen/FontStringCalculator.hpp"
#include "../Task/TaskManager.hpp"
#include "../Input/MouseManager.hpp"
#include "../GlobalMethod.hpp"
#include "../../Util/Visual/Color.hpp"

Button::Button(const std::string& label, const ScreenData& layoutScreen, std::shared_ptr<FlexibleScaler> parentScaler) : GUI(label + "\"<Button>\"", layoutScreen, parentScaler)
{
    Label_ = label;
    textColor = color_preset::BLACK;
	baseColor = color_preset::WHITE_GREY;
	animationColor = color_preset::GREY;
    //mouseOverColor = color_preset::LIGHT_GREY;

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
        timerCount = 0.f;
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
    if(isDrawBase)
        DrawBox(0, 0, engine::CastToInt(GetRawScreenWidth()), engine::CastToInt(GetRawScreenHeight()), baseColor, TRUE);

    if (IsHoldMouse() && timerCount > 0.3f)
    {
        SetDrawBlendMode(AlphaBlendMode_, 127);
        DrawBox(0, 0, engine::CastToInt(GetRawScreenWidth()), engine::CastToInt(GetRawScreenHeight()), animationColor, TRUE);
    }
    if (IsOnMouse())
    {
        //Logger_->Debug("btn Draw");
        SetDrawBlendMode(AlphaBlendMode_, 20);
        DrawBox(0, 0, engine::CastToInt(GetRawScreenWidth()), engine::CastToInt(GetRawScreenHeight()), color_preset::BLACK, TRUE);
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
	HasLifespan_ = true;
	Lifespan_ = 0.5f;
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
	ScreenData l_Circle;
	l_Circle.posX = position.x;
	l_Circle.posY = position.y;
	l_Circle.width = Size_;
	l_Circle.height = Size_;
    const auto l_Fixed = ParentScaler_->Calculate(l_Circle);
    SetDrawBlendMode(DX_BLENDMODE_PMA_INVSRC, 255);
    DrawCircle(engine::CastToInt(l_Fixed.posX), engine::CastToInt(l_Fixed.posY), engine::CastToInt(l_Fixed.width + l_Fixed.height / 2.0f), GetColor(255,255,255) , TRUE);
}
