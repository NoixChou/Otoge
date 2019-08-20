#include "DropdownList.hpp"
#include "../GlobalMethod.hpp"
#include "../Input/MouseManager.hpp"
#include "../../Util/Calculate/Animation/Easing.hpp"
#include "../Task/TaskManager.hpp"
#include "../../Util/Visual/Color.hpp"
#include "../../Util/Calculate/Screen/FontStringCalculator.hpp"
#include "../GUI/SpringButton.hpp"

template<typename V>
DropdownList<V>::BaseItem::BaseItem(std::string text, float textSize, float thickness) :
    text(std::move(text)),
    textSize(textSize),
    textThickness(thickness)
{
}

template<typename V>
DropdownList<V>::Separator::Separator(std::string text, float textSize, float thickness) :
    BaseItem(text, textSize, thickness)
{
	this->backColor = color_preset::GREY;
    this->textColor = color_preset::BLACK;
	this->doDrawBack = true;
}

template<typename V>
DropdownList<V>::SimpleItem::SimpleItem(std::string text, V value, float textSize, float thickness) :
    BaseItem(text, textSize, thickness),
    value(value)
{
	this->backColor = color_preset::DARK_GREY;
	this->doDrawBack = true;
}

template<typename V>
DropdownList<V>::DropdownList(const std::string& label, const ScreenData& layoutScreen, int maxItem, std::shared_ptr<FlexibleScaler> parentScaler) : GUI(label, layoutScreen, parentScaler)
{
    Label_ = label;
    baseColor = color_preset::GREY;
    animationColor = color_preset::DARK_GREY;
    textColor = color_preset::BLACK;
    listColor = color_preset::LIGHT_GREY;

    SelectedLabel_ = std::make_shared<Label>("", ScreenData(0.f, 0.f, 90.f, 100.f), DefaultScaler_);
    SelectedLabel_->baseColor = textColor;
    SelectedLabel_->adjustmentFontSize = true;
    //SelectedLabel_->ChangeFontSize(engine::CastToInt(SelectedLabel_->GetDefaultScaler()->CalculateHeight(70.f)));
    SelectedLabel_->ChangeFontThickness(4);
    SelectedLabel_->SetTextAlign(Label::TextAlignment::left | Label::TextAlignment::middle);
    AddChildTask(std::static_pointer_cast<Task>(SelectedLabel_));

    TriangleLabel_ = std::make_shared<Label>("▼", ScreenData(90.f, 0.f, 10.f, 100.f), DefaultScaler_);
    TriangleLabel_->baseColor = baseColor;
    TriangleLabel_->adjustmentFontSize = false;
    TriangleLabel_->ChangeFontSize(engine::CastToInt(TriangleLabel_->GetDefaultScaler()->CalculateWidth(100.f)));
    TriangleLabel_->SetTextAlign(Label::TextAlignment::center | Label::TextAlignment::middle);
    AddChildTask(std::static_pointer_cast<Task>(TriangleLabel_));

    Panel_ = std::make_shared<Scene>(label + "<panel>", layoutScreen, ParentScaler_);
    Panel_->SetDrawFunction([=]
    {
        auto fixed = Panel_->GetDefaultScaler()->Calculate(ScreenData(0.f, 0.f, 100.f, 100.f));
        //DrawBox(engine::CastToInt(fixed.posX), engine::CastToInt(fixed.posY),
        //        engine::CastToInt(fixed.posX + fixed.width), engine::CastToInt(fixed.posY + fixed.height), listColor,
        //        TRUE);
    });
    //Panel_->SetAlphaBlendMode(DX_BLENDMODE_PMA_ALPHA);
    Panel_->SetEnable(false);
    //Panel_->SetVisible(false);
    Panel_->SetTransparent(0.f);
    Panel_->SetPriority(70.f);
    SetMaxItem(maxItem);
}

template<typename V>
DropdownList<V>::~DropdownList()
{
}

template<typename V>
void DropdownList<V>::GUIUpdate(float deltaTime)
{
    //Panel_->SetPositionX(GetDefaultScaler()->CalculatePositionRateX(GetDefaultScaler()->GetDiffX()));
    //Panel_->SetPositionY(GetDefaultScaler()->CalculatePositionRateY(GetDefaultScaler()->GetDiffY()) + PanelOffsetY_);

    if(SelectedItem_ > engine::CastToInt(Items_.size())) SelectedItem_ = Items_.size() - 1;

    if(IsDownMouse())
    {
        AddChildTask(std::static_pointer_cast<Task>(
            std::make_shared<ButtonPushedAnimate>(
                DefaultScaler_->CalculatePositionRateX(MouseManager::GetInstance()->GetMouseXf() - GetRawPositionX() - ParentScaler_->GetDiffX()),
                DefaultScaler_->CalculatePositionRateY(MouseManager::GetInstance()->GetMouseYf() - GetRawPositionY() - ParentScaler_->GetDiffY()),
                animationColor, 35.f, DefaultScaler_)
            ));
    }

    if(IsListOpening_)
    {
        Easing::EaseFunction l_EaseOpen = Easing::OutBounce;
        Easing::EaseFunction l_EaseClose = Easing::OutExp;
        float l_TotalTime = 0.5f;

        if (IsListOpened_)
        {
            Panel_->SetTransparent(Easing::OutExp(timerCount, l_TotalTime, 100., 0.));
            Panel_->SetPositionY(l_EaseOpen(timerCount, l_TotalTime, GetPositionY() + GetScreenHeight(), GetPositionY()));
            //Panel_->SetPositionY(GetPositionY() + GetScreenHeight());
            Panel_->SetScreenHeight(l_EaseOpen(timerCount, l_TotalTime, PanelHeight_, 0.));
        }else
        {
            Panel_->SetTransparent(l_EaseClose(timerCount, l_TotalTime, 0., 100.));
            Panel_->SetPositionY(l_EaseClose(timerCount, l_TotalTime, GetPositionY(), GetPositionY() + GetScreenHeight()));
            //Panel_->SetPositionY(GetPositionY());
            Panel_->SetScreenHeight(l_EaseClose(timerCount, l_TotalTime, 0., PanelHeight_));
        }
        if (timerCount > l_TotalTime)
        {
            timerCount = 0.f;
            IsListOpening_ = false;

            if (IsListOpened_)
            {
                Panel_->SetEnable(true);
                Panel_->SetTransparent(100.f);
                Panel_->SetScreenHeight(PanelHeight_);
            }
            else
            {
                Panel_->SetEnable(false);
                Panel_->SetTransparent(0.f);
                Panel_->SetScreenHeight(0.f);
            }
        }

        Panel_->RefreshDrawBuffer();
    }

    BeforeSelectedItem_ = SelectedItem_;

    int l_ItemCount = 0;
    for(auto l_Item : Items_)
    {
        if(l_Item.expired()) break;
        if(l_Item.lock()->IsClickedMouse() && l_Item.lock()->IsEnable())
        {
            SelectedItem_ = l_ItemCount;
            CloseList();
        }
        l_ItemCount++;
    }


    if (IsClickedMouse())
    {
        AddPanel();
        InvertOpening();
        Logger_->Warn("ボタンクリックで切り替え");
    }
    else
    {
        if (MouseManager::GetInstance()->IsDownButton(MOUSE_INPUT_LEFT) && (!Panel_->IsOnMouse() && !IsOnMouse()) && IsListOpened_)
        {
            Logger_->Warn("範囲外クリックで閉じる");
            CloseList();
        }
    }

    if (IsChangedSelect())
    {
        UpdateSelected();
    }

    Panel_->SetTransparent(engine::LimitRange(Panel_->GetTransparent(), 0.f, 100.f));
    Panel_->SetPositionY(engine::LimitRange(Panel_->GetScreenHeight(), GetPositionY() + GetScreenHeight(), GetPositionY()));
    Panel_->SetScreenHeight(engine::LimitRange(Panel_->GetScreenHeight(), PanelHeight_, 0.f));
}

template <class V>
void DropdownList<V>::Draw()
{
    ScreenData fixed = DefaultScaler_->Calculate(ScreenData(2.f, 97.f, 98.f, 1.f));
    SetDrawBlendMode(AlphaBlendMode_, engine::CastToInt(((100.f - Panel_->GetTransparent()) / 100.f) * 255.f));
    DrawLine(engine::CastToInt(fixed.posX), engine::CastToInt(fixed.posY), engine::CastToInt(fixed.posX + fixed.width), engine::CastToInt(fixed.posY), baseColor);
}

template <class V>
void DropdownList<V>::AddPanel()
{
    if (!IsPanelAdded_)
    {
        auto l_ParentScene = std::dynamic_pointer_cast<Scene>(parentTask.lock());
        if (l_ParentScene)
        {
            l_ParentScene->AddChildTask(std::static_pointer_cast<Task>(Panel_));
        }
        else
        {
            TaskManager::GetInstance()->AddTask(std::static_pointer_cast<Task>(Panel_));
        }
        IsPanelAdded_ = true;
    }
}

template <class V>
void DropdownList<V>::ResetAnimation()
{
    timerCount = 0.f;
    IsListOpening_ = true;
}

template <class V>
void DropdownList<V>::InvertOpening()
{
    if (IsListOpened_)
        CloseList();
    else
        OpenList();
}

template <class V>
void DropdownList<V>::OpenList()
{
    ResetAnimation();

    IsListOpened_ = true;
    Panel_->SetEnable(true);
    TriangleLabel_->SetLabel("▲");
}

template <class V>
void DropdownList<V>::CloseList()
{
    ResetAnimation();

    IsListOpened_ = false;
    Panel_->SetEnable(false);
    TriangleLabel_->SetLabel("▼");
}

template< typename V >
bool DropdownList<V>::IsOpenList() const
{
    return IsListOpened_;
}

template <class V>
void DropdownList<V>::SetSelectedItemNum(int num, bool doChangeFlag)
{
    if (num > ItemCount_ || num < 0) return;
    SelectedItem_ = num;
    if (!doChangeFlag)
        BeforeSelectedItem_ = num;

    UpdateSelected();
}

template <class V>
int DropdownList<V>::GetSelectedItemNum() const
{
    if (SelectedItem_ > Items_.size()) return Items_.size() - 1;
    return SelectedItem_;
}

template <class V>
std::shared_ptr<typename DropdownList<V>::BaseItem> DropdownList<V>::GetSelectedItem() const
{
    if (SelectedItem_ > Items_.size()) return ItemData_[Items_.size() - 1];
    return ItemData_[SelectedItem_];
}

template <class V>
std::optional<V> DropdownList<V>::GetSelectedItemValue() const
{
    int l_Index;
    if (SelectedItem_ > Items_.size())
    {
        l_Index = engine::CastToInt(Items_.size()) - 1;
    }
    else
    {
        l_Index = SelectedItem_;
    }

    auto l_Item = std::static_pointer_cast<SimpleItem>(ItemData_[l_Index]);
    return l_Item->value;
}

template <class V>
bool DropdownList<V>::IsChangedSelect() const
{
    return (SelectedItem_ != BeforeSelectedItem_) && IsEnable();
}

template <class V>
void DropdownList<V>::UpdateSelected()
{
    if(typeid(ItemData_[SelectedItem_]) == typeid(std::shared_ptr<Separator>))
    {
        while (typeid(ItemData_[SelectedItem_]) == typeid(std::shared_ptr<Separator>))
        {
            if ((SelectedItem_ + 1) > ItemData_.size() || Items_[SelectedItem_ + 1].expired()) break;
            SelectedItem_++;
        }
    }
    if (Items_[SelectedItem_].expired()) return;
    SelectedLabel_->SetLabel(Items_[SelectedItem_].lock()->GetLabel());
    SelectedLabel_->AdjustFont();
}

template <class V>
void DropdownList<V>::SetMaxItem(int num)
{
    ItemCount_ = num;

    Items_.resize(num);
    ItemData_.resize(num);

    PanelHeight_ = ParentScaler_->CalculatePositionRateY(GetRawScreenHeight() * (ItemCount_ + 1));
    Panel_->SetScreenHeight(PanelHeight_);
    Panel_->RefreshChildren();
}

template <class V>
void DropdownList<V>::AddItem(int num, const std::shared_ptr<Button>& item) // private
{
    AddPanel();
    item->SetPositionY(item->GetDefaultScaler()->CalculatePositionRateY(DefaultScaler_->CalculateHeight(100.f)) * ItemCount_);
    item->SetScreenHeight(item->GetDefaultScaler()->CalculatePositionRateY(DefaultScaler_->CalculateHeight(100.f)));

    Items_[num] = item;

    item->SetPositionY((100.f / ItemCount_) * num);
    item->SetScreenHeight(100.f / ItemCount_);
    item->SetPriority(71.f);

    item->ReCalculateScreen();
    item->GetTextLabelInstance()->ReCalculateScreen();
    item->RefreshDrawBuffer();
    item->GetTextLabelInstance()->RefreshDrawBuffer();

    Panel_->AddChildTask(std::static_pointer_cast<Task>(item));

    //item->ReCalculateScreen();
    //item->GetTextLabelInstance()->ReCalculateScreen();
    //UpdateSelected();
}

template <class V>
void DropdownList<V>::AddBaseItem(int num, std::shared_ptr<BaseItem> item)
{
    AddPanel();
    std::shared_ptr<Button> l_Btn;
    
    if (std::shared_ptr<Separator> l_Sep = std::dynamic_pointer_cast<Separator>(item))
        l_Btn = std::make_shared<Button>(item->text, ScreenData(0.f, 0.f, 100.f, -1.f), Panel_->GetDefaultScaler());
    else
        l_Btn = std::make_shared<SpringButton>(item->text, ScreenData(0.f, 0.f, 100.f, -1.f), Panel_->GetDefaultScaler());

    l_Btn->isDrawBase = item->doDrawBack;
    l_Btn->textColor = item->textColor;
    l_Btn->baseColor = item->backColor;
    l_Btn->GetTextLabelInstance()->adjustmentFontSize = false;
    l_Btn->GetTextLabelInstance()->ChangeFontSize(engine::CastToInt(Panel_->GetDefaultScaler()->CalculateHeight(item->textSize)));
    l_Btn->GetTextLabelInstance()->ChangeFontThickness(engine::CastToInt(Panel_->GetDefaultScaler()->CalculateHeight(item->textThickness)));
    l_Btn->GetTextLabelInstance()->SetTextAlign(item->Align());
    l_Btn->animationColor = color_preset::DARK_GREY;
    l_Btn->SetEnable(item->IsEnabledOnInit());
    l_Btn->SetTransparent(100.f);
    l_Btn->SetVisible(true);
    ItemData_[num] = item;
    AddItem(num, l_Btn);
}

template <class V>
void DropdownList<V>::AddSeparator(int num, const std::string& label)
{
    AddPanel();
    auto l_Item = std::make_shared<Separator>(label);
    Logger_->Info("Separator");
    AddBaseItem(num, l_Item);
}

template <class V>
void DropdownList<V>::AddSimpleItem(int num, const std::string& label, V value)
{
    AddPanel();
    auto l_Item = std::make_shared<SimpleItem>(label, value);
    Logger_->Info("SimpleItem");
    AddBaseItem(num, l_Item);
}

template <class V>
std::shared_ptr<Scene> DropdownList<V>::GetPanelInstance()
{
    return Panel_;
}
