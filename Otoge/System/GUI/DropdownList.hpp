#pragma once
#include "GUI.hpp"
#include "Button.hpp"
#include "../../Util/Visual/Color.hpp"
#include "../GlobalMethod.hpp"

template<typename V>
class DropdownList : public GUI
{
public:
    struct BaseItem
    {
        std::string text = "";
        float textSize;
        float textThickness;
        virtual Label::TextAlignment Align() { return Label::TextAlignment::center | Label::TextAlignment::middle; }
        unsigned textColor = color_preset::BLACK;
        unsigned backColor;
        bool doDrawBack = false;
        virtual bool IsEnabledOnInit() { return true; }
        BaseItem(std::string text, float textSize, float thickness);
    };
    struct Separator :
        public BaseItem
    {
        Label::TextAlignment Align() override { return Label::TextAlignment::left | Label::TextAlignment::middle; }

        bool IsEnabledOnInit() override { return false; }
        Separator(std::string text, float textSize = 65.f, float thickness = 3.f);
    };
    struct SimpleItem :
        public BaseItem
    {
        std::optional<V> value;

        SimpleItem(std::string text, V value, float textSize = 60.f, float thickness = 1.f);
    };

private:
    int ItemCount_ = 0;
    int BeforeSelectedItem_ = 0;
    int SelectedItem_ = 0;
    bool IsListOpened_ = false;
    bool IsListOpening_ = false;
    bool IsPanelAdded_ = false;
    float PanelHeight_ = 0.f;
    std::shared_ptr<Scene> Panel_;
    std::shared_ptr<Label> SelectedLabel_;
    std::shared_ptr<Label> TriangleLabel_;

    std::vector<std::shared_ptr<BaseItem>> ItemData_;
    std::vector<std::weak_ptr<Button>> Items_;

    void AddItem(int num, const std::shared_ptr<Button>& item);
public:
    unsigned animationColor;
    unsigned textColor;
    unsigned listColor;

    DropdownList(const std::string& label, const ScreenData& layoutScreen, int maxItem, std::shared_ptr<FlexibleScaler> parentScaler = nullptr);
    ~DropdownList();

    void GUIUpdate(float deltaTime) override;
    void Draw() override;

    void AddPanel();
    void ResetAnimation();
    void InvertOpening();
    void OpenList();
    void CloseList();

    void SetSelectedItemNum(int num);
    int GetSelectedItemNum() const;
    std::shared_ptr<BaseItem> GetSelectedItem() const;
    std::optional<V> GetSelectedItemValue() const;
    bool IsChangedSelect() const;
    void UpdateSelected();

    void SetMaxItem(int num);
    void AddBaseItem(int num, std::shared_ptr<BaseItem> item);
    void AddSeparator(int num, const std::string& label);
    void AddSimpleItem(int num, const std::string& label, V value);

    std::shared_ptr<Scene> GetPanelInstance();
};
