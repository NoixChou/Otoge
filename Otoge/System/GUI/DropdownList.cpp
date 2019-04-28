#include "DropdownList.hpp"
#include "../GlobalMethod.hpp"
#include "../Input/MouseManager.hpp"
#include "../../Util/Calculate/Animation/Easing.hpp"
#include "../Task/TaskManager.hpp"

DropdownList::DropdownList(const std::string& label, const ScreenData& layoutScreen, int maxItem, std::shared_ptr<FlexibleScaler> parentScaler) : GUI(label, layoutScreen, parentScaler)
{
    Label_ = label;
    baseColor = GetColor(220, 220, 220);
    animationColor = GetColor(127, 127, 127);
    textColor = GetColor(50, 50, 50);


    SelectedLabel_ = std::make_shared<Label>("", ScreenData(0.f, 0.f, 90.f, 100.f), DefaultScaler_);
    SelectedLabel_->baseColor = textColor;
    SelectedLabel_->AdjustmentFontSize_ = false;
    SelectedLabel_->ChangeFontSize(engine::CastToInt(SelectedLabel_->GetDefaultScaler()->CalculateHeight(70.f)));
    SelectedLabel_->ChangeFontThickness(6);
    SelectedLabel_->SetTextAlign(Label::TextAlignment::left | Label::TextAlignment::middle);
    AddChildTask(std::static_pointer_cast<Task>(SelectedLabel_));

    TriangleLabel_ = std::make_shared<Label>("▼", ScreenData(90.f, 0.f, 10.f, 100.f), DefaultScaler_);
    TriangleLabel_->baseColor = baseColor;
    TriangleLabel_->AdjustmentFontSize_ = false;
    TriangleLabel_->ChangeFontSize(engine::CastToInt(TriangleLabel_->GetDefaultScaler()->CalculateWidth(100.f)));
    TriangleLabel_->SetTextAlign(Label::TextAlignment::center | Label::TextAlignment::middle);
    AddChildTask(std::static_pointer_cast<Task>(TriangleLabel_));

    Panel_ = std::make_shared<Scene>(label + "<panel>", layoutScreen, ParentScaler_);
    Panel_->SetDrawFunction([&]
        {
            auto fixed = Panel_->GetDefaultScaler()->Calculate(ScreenData(0.f, 0.f, 100.f, 100.f));
            DrawBox(engine::CastToInt(fixed.posX), engine::CastToInt(fixed.posY), engine::CastToInt(fixed.posX + fixed.width), engine::CastToInt(fixed.posY + fixed.height), baseColor, TRUE);
        });
    Panel_->SetAlphaBlendMode(DX_BLENDMODE_ALPHA);
    Panel_->SetEnable(false);
    Panel_->SetVisible(false);
    Panel_->SetTransparent(0.f);
    Panel_->SetPriority(70.f);
    SetMaxItem(maxItem);
}

DropdownList::~DropdownList()
{
}

void DropdownList::GUIUpdate(float deltaTime)
{
    BeforeSelectedItem_ = SelectedItem_;
    //Panel_->SetPositionX(GetDefaultScaler()->CalculatePositionRateX(GetDefaultScaler()->GetDiffX()));
    //Panel_->SetPositionY(GetDefaultScaler()->CalculatePositionRateY(GetDefaultScaler()->GetDiffY()) + PanelOffsetY_);

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

    if(IsClickedMouse())
    {
        AddPanel();

        timerCount = 0.f;
        IsListOpening_ = true;
        IsListOpened_ = !IsListOpened_;

        if(IsListOpened_)
        {
            Panel_->SetVisible(true);
            TriangleLabel_->SetLabel("▲");
        }else
        {
            //Panel_->SetScreenHeight(ParentScaler_->CalculatePositionRateY(GetRawScreenHeight() / (ItemCount_)));
            Panel_->SetEnable(false);
            TriangleLabel_->SetLabel("▼");
        }
    }

    if(IsListOpening_)
    {
        Easing::EaseFunction l_EaseOpen = Easing::OutBounce;
        Easing::EaseFunction l_EaseClose = Easing::OutExp;
        float l_TotalTime = 0.5f;

        if (IsListOpened_)
        {
            Panel_->SetTransparent(l_EaseOpen(timerCount, l_TotalTime, 100.f, 0.f));
            Panel_->SetPositionY(l_EaseOpen(timerCount, l_TotalTime, GetPositionY() + GetScreenHeight(), GetPositionY()));
            Panel_->SetScreenHeight(l_EaseOpen(timerCount, l_TotalTime, PanelHeight_, 0.f));
        }else
        {
            Panel_->SetTransparent(l_EaseClose(timerCount, l_TotalTime, 0.f, 100.f));
            Panel_->SetPositionY(l_EaseClose(timerCount, l_TotalTime, GetPositionY(), GetPositionY() + GetScreenHeight()));
            Panel_->SetScreenHeight(l_EaseClose(timerCount, l_TotalTime, 0.f, PanelHeight_));
        }
        if (timerCount > l_TotalTime)
        {
            timerCount = l_TotalTime;
            IsListOpening_ = false;

            if (!IsListOpened_)
                Panel_->SetVisible(false);
            else
                Panel_->SetEnable(true);
        }

        Panel_->RefreshDrawBuffer();
    }

    int l_ItemCount = 1;

    for(auto l_Item : Items_)
    {
        if (l_Item.expired()) break;
        if(l_Item.lock()->IsClickedMouse())
        {
            SelectedItem_ = l_ItemCount;
            timerCount = 0.f;
            IsListOpening_ = true;
            IsListOpened_ = false;
        }

        l_ItemCount++;
    }

    SelectedLabel_->SetLabel(Items_[SelectedItem_ - 1].lock()->GetLabel());
    
    if(MouseManager::GetInstance()->IsDownButton(MOUSE_INPUT_LEFT) && (!Panel_->IsOnMouse() && !IsOnMouse()) && IsListOpened_)
    {
        timerCount = 0.f;
        IsListOpening_ = true;
        IsListOpened_ = false;
    }

    Panel_->SetPositionY(engine::LimitRange(Panel_->GetScreenHeight(), GetPositionY() + GetScreenHeight(), GetPositionY()));
    Panel_->SetScreenHeight(engine::LimitRange(Panel_->GetScreenHeight(), PanelHeight_, 0.f));
}

void DropdownList::Draw()
{
    ScreenData fixed = DefaultScaler_->Calculate(ScreenData(2.f, 98.f, 98.f, 1.f));
    SetDrawBlendMode(AlphaBlendMode_, engine::CastToInt(((100.f - Panel_->GetTransparent()) / 100.f) * 255.f));
    DrawLine(fixed.posX, fixed.posY, fixed.posX + fixed.width, fixed.posY, baseColor);
}

void DropdownList::AddPanel()
{
    if (!IsPanelAdded_)
    {
        auto parentScene = std::dynamic_pointer_cast<Scene>(parentTask.lock());
        if (parentScene)
        {
            parentScene->AddChildTask(std::static_pointer_cast<Task>(Panel_));
        }
        else
        {
            TaskManager::GetInstance()->AddTask(std::static_pointer_cast<Task>(Panel_));
        }
        IsPanelAdded_ = true;
    }
}

void DropdownList::SetSelectedItemNum(int num)
{
    if (num > ItemCount_ || num < 0) return;
    SelectedItem_ = num;
}

int DropdownList::GetSelectedItemNum() const
{
    return SelectedItem_;
}

bool DropdownList::IsChangedSelect() const
{
    return SelectedItem_ != BeforeSelectedItem_;
}

void DropdownList::SetMaxItem(int num)
{
    ItemCount_ = num;

    Items_.resize(num);
    ItemNames_.resize(num);

    PanelHeight_ = ParentScaler_->CalculatePositionRateY(GetRawScreenHeight() * (ItemCount_ + 1));
    Panel_->SetScreenHeight(PanelHeight_);
    Panel_->RefreshChildren();
}

void DropdownList::AddItem(int num, const std::shared_ptr<Button>& item)
{
    AddPanel();
    //item->SetPositionY(item->GetDefaultScaler()->CalculatePositionRateY(DefaultScaler_->CalculateHeight(100.f)) * ItemCount_);
    //item->SetScreenHeight(item->GetDefaultScaler()->CalculatePositionRateY(DefaultScaler_->CalculateHeight(100.f)));

    Items_[num] = item;
    ItemNames_[num] = item->GetLabel();

    item->SetPositionY((100.f / ItemCount_) * num);
    item->SetScreenHeight(100.f / ItemCount_);

    Panel_->AddChildTask(std::static_pointer_cast<Task>(item));
}

void DropdownList::AddItem(int num, const std::string& label)
{
    AddPanel();
    auto btn = std::make_shared<Button>(label, ScreenData(0.f, 0.f, 100.f, 100.f), Panel_->GetDefaultScaler());
    btn->isDrawBase = false;
    btn->textColor = textColor;
    btn->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    btn->GetTextLabelInstance()->ChangeFontSize(Panel_->GetDefaultScaler()->CalculateHeight(60.f));
    btn->GetTextLabelInstance()->ChangeFontThickness(2);
    btn->animationColor = GetColor(230, 230, 230);
    AddItem(num, btn);
}

std::shared_ptr<Scene> DropdownList::GetPanelInstance()
{
    return Panel_;
}
