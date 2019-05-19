#include "Button.hpp"
#include "../../Util/Calculate/Screen/FontStringCalculator.hpp"
#include "../Task/TaskManager.hpp"
#include "../Input/MouseManager.hpp"
#include "../GlobalMethod.hpp"
#include "../../Util/Visual/Color.hpp"
#include "../../Util/Calculate/Animation/Easing.hpp"

Button::Button(const std::string& label, const ScreenData& layoutScreen,
               std::shared_ptr<FlexibleScaler> parentScaler) : GUI(label + "\"<Button>\"", layoutScreen, parentScaler)
{
    Label_ = label;
    textColor = color_preset::BLACK;
    baseColor = color_preset::WHITE_GREY;
    animationColor = color_preset::GREY;
    //mouseOverColor = color_preset::LIGHT_GREY;
    TextLabel_ = std::make_shared<Label>(Label_, ScreenData(0.f, 0.f, 100.f, 100.f), DefaultScaler_);
    TextLabel_->baseColor = textColor;
    TextLabel_->adjustmentFontSize = adjustmentFontSize;
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

    if (CurrentTextFontSize_ == -1 && !TextLabel_->GetLabel().empty())
    {
        CurrentTextFontSize_ = TextLabel_->GetFontSize();
    }

    if(IsDownMouse())
    {
        timerCount = 0.f;
        if (TextLabel_)
        {
            TextLabel_->AddChildTask(std::static_pointer_cast<Task>(std::make_shared<ButtonPushedAnimate>(
                DefaultScaler_->
                CalculatePositionRateX(
                    MouseManager::GetInstance()->GetMouseXf() - GetRawPositionX() - ParentScaler_->GetDiffX()),
                DefaultScaler_->CalculatePositionRateY(
                    MouseManager::GetInstance()->GetMouseYf() - GetRawPositionY() - ParentScaler_->GetDiffY()),
                animationColor, 20.f, DefaultScaler_)));
        }else
        {
            AddChildTask(std::static_pointer_cast<Task>(std::make_shared<ButtonPushedAnimate>(
                DefaultScaler_->
                CalculatePositionRateX(
                    MouseManager::GetInstance()->GetMouseXf() - GetRawPositionX() - ParentScaler_->GetDiffX()),
                DefaultScaler_->CalculatePositionRateY(
                    MouseManager::GetInstance()->GetMouseYf() - GetRawPositionY() - ParentScaler_->GetDiffY()),
                animationColor, 20.f, DefaultScaler_)));
        }
    }
}

void Button::Draw()
{
    if (isDrawBase)
    {
        DrawBox(engine::CastToInt(RawSizeX_), engine::CastToInt(RawSizeY_), engine::CastToInt(GetRawScreenWidth() - RawSizeX_), engine::CastToInt(GetRawScreenHeight() - RawSizeY_), baseColor, TRUE);
    }

    if(IsOnMouse())
    {
        //Logger_->Debug("btn Draw");
        SetDrawBlendMode(AlphaBlendMode_, 20);
        DrawBox(engine::CastToInt(RawSizeX_), engine::CastToInt(RawSizeY_), engine::CastToInt(GetRawScreenWidth() - RawSizeX_), engine::CastToInt(GetRawScreenHeight() - RawSizeY_), color_preset::BLACK, TRUE);
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
ButtonPushedAnimate::ButtonPushedAnimate(float x, float y, unsigned color, float size,
                                         std::shared_ptr<FlexibleScaler> parentScaler) : DrawableTask(
    "ButtonPushedAnimation", x, y, 0.f, parentScaler)
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
    if(!parentTask.expired())
    {
        auto parentScene = std::dynamic_pointer_cast<Scene>(parentTask.lock());
        if(parentScene)
        {
            if(parentScene->IsHoldMouse())
            {
                Lifespan_ = 0.5f;
                if(Size_ < parentScene->GetRawScreenWidth() || Size_ < parentScene->GetRawScreenHeight())
                {
                    SizeAnimationProcess(deltaTime);
                }
                else
                {
                    return;
                }
            }
        }
    }
    SizeAnimationProcess(deltaTime);
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
    DrawCircleAA(
        engine::CastToInt(l_Fixed.posX), engine::CastToInt(l_Fixed.posY),
        engine::CastToInt(l_Fixed.width + l_Fixed.height / 2.0f),
        engine::CastToInt(l_Fixed.width + l_Fixed.height / 2.0f) / 2, GetColor(255, 255, 255), TRUE);
}

void ButtonPushedAnimate::SizeAnimationProcess(float deltaTime)
{
    Size_ += Size_ * 10.f * deltaTime;
}
