#include "DropdownList.hpp"
#include "../GlobalMethod.hpp"
#include "../Input/MouseManager.hpp"
#include "../../Util/Calculate/Animation/Easing.hpp"
#include "../Task/TaskManager.hpp"
#include "../../Util/Visual/Color.hpp"
#include "../../Util/Calculate/Screen/FontStringCalculator.hpp"

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
}

template<typename V>
DropdownList<V>::SimpleItem::SimpleItem(std::string text, V value, float textSize, float thickness) :
	BaseItem(text, textSize, thickness),
	value(value)
{
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
    SelectedLabel_->AdjustmentFontSize_ = true;
    //SelectedLabel_->ChangeFontSize(engine::CastToInt(SelectedLabel_->GetDefaultScaler()->CalculateHeight(70.f)));
    SelectedLabel_->ChangeFontThickness(8);
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
            DrawBox(engine::CastToInt(fixed.posX), engine::CastToInt(fixed.posY), engine::CastToInt(fixed.posX + fixed.width), engine::CastToInt(fixed.posY + fixed.height), listColor, TRUE);
        });
    Panel_->SetAlphaBlendMode(DX_BLENDMODE_ALPHA);
    Panel_->SetEnable(false);
    Panel_->SetVisible(false);
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

    if (SelectedItem_ > Items_.size()) SelectedItem_ = Items_.size() - 1;

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
        InvertOpening();
    }

    if(IsListOpening_)
    {
        Easing::EaseFunction l_EaseOpen = Easing::OutExp;
        Easing::EaseFunction l_EaseClose = Easing::OutExp;
        float l_TotalTime = 0.5f;

        if (IsListOpened_)
        {
            Panel_->SetTransparent(l_EaseOpen(timerCount, l_TotalTime, 100.f, 0.f));
            //Panel_->SetPositionY(l_EaseOpen(timerCount, l_TotalTime, GetPositionY() + GetScreenHeight(), GetPositionY()));
            Panel_->SetPositionY(GetPositionY() + GetScreenHeight());
            Panel_->SetScreenHeight(l_EaseOpen(timerCount, l_TotalTime, PanelHeight_, 0.f));
        }else
        {
            Panel_->SetTransparent(l_EaseClose(timerCount, l_TotalTime, 0.f, 100.f));
            //Panel_->SetPositionY(l_EaseClose(timerCount, l_TotalTime, GetPositionY(), GetPositionY() + GetScreenHeight()));
            Panel_->SetPositionY(GetPositionY());
            Panel_->SetScreenHeight(l_EaseClose(timerCount, l_TotalTime, 0.f, PanelHeight_));
        }
        if (timerCount > (l_TotalTime + 0.1f))
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

    BeforeSelectedItem_ = SelectedItem_;

    int l_ItemCount = 0;
    for(auto l_Item : Items_)
    {
        if (l_Item.expired()) break;

        if(l_Item.lock()->IsClickedMouse() && l_Item.lock()->IsEnable())
        {
            SelectedItem_ = l_ItemCount;
            CloseList();
        }

        l_ItemCount++;
    }
	
    if (IsChangedSelect())
    {
        UpdateSelected();
    }
    
    if(MouseManager::GetInstance()->IsDownButton(MOUSE_INPUT_LEFT) && (!Panel_->IsOnMouse() && !IsOnMouse()) && IsListOpened_ && !IsListOpening_)
    {
        CloseList();
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
    DrawLine(fixed.posX, fixed.posY, fixed.posX + fixed.width, fixed.posY, baseColor);
}

template <class V>
void DropdownList<V>::AddPanel()
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
    Panel_->SetVisible(true);
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

template <class V>
void DropdownList<V>::SetSelectedItemNum(int num)
{
    if (num > ItemCount_ || num < 0) return;
    SelectedItem_ = num;

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
	int index;
	if (SelectedItem_ > Items_.size())
	{
		index = Items_.size() - 1;
	}
	else
	{
		index = SelectedItem_;
	}

	auto item = std::static_pointer_cast<SimpleItem>(ItemData_[index]);
	return item->value;
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

    Panel_->AddChildTask(std::static_pointer_cast<Task>(item));

    UpdateSelected();
}

template <class V>
void DropdownList<V>::AddBaseItem(int num, std::shared_ptr<BaseItem> item)
{
    auto btn = std::make_shared<Button>(item->text, ScreenData(0.f, 0.f, 100.f, -1.f), Panel_->GetDefaultScaler());
    btn->isDrawBase = item->doDrawBack;
    btn->textColor = item->textColor;
    btn->baseColor = item->backColor;
    btn->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    btn->GetTextLabelInstance()->ChangeFontSize(engine::CastToInt(Panel_->GetDefaultScaler()->CalculateHeight(item->textSize)));
    btn->GetTextLabelInstance()->ChangeFontThickness(engine::CastToInt(Panel_->GetDefaultScaler()->CalculateHeight(item->textThickness)));
    btn->GetTextLabelInstance()->SetTextAlign(item->align());
    btn->animationColor = color_preset::DARK_GREY;
    btn->SetEnable(item->isEnabledOnInit());
    ItemData_[num] = item;
    AddItem(num, btn);
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
