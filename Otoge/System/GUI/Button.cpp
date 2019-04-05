#include "Button.hpp"
#include "../../Util/Calculate/Screen/FontStringCalculator.hpp"
#include "../Task/TaskManager.hpp"

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
    int r, g, b;
    GetColor2(baseColor, &r, &g, &b);
    if (IsHoldMouse()) r -= 50, g -= 50, b -= 50;
    DrawBox(0, 0, screen.width, screen.height, GetColor(r, g, b), TRUE);
}

void Button::SetTextLabelInstance(std::shared_ptr<Label> textLabel)
{
    TextLabel_.reset(textLabel.get());
}

std::shared_ptr<Label> Button::GetTextLabelInstance()
{
    return TextLabel_;
}
