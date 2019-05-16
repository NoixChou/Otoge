#pragma once
#include "GUI.hpp"
#include "Button.hpp"
#include "../../Util/Visual/Color.hpp"
#include "../GlobalMethod.hpp"

class DropdownList : public GUI
{
public:
    struct SimpleItem
    {
        std::string text = "";
        float textSize = 60.f;
        float textThickness = 1.f;
        Label::TextAlignment align = Label::TextAlignment::center | Label::TextAlignment::middle;
        unsigned textColor = color_preset::BLACK;
        unsigned backColor = color_preset::WHITE;
        bool doDrawBack = false;
        bool isSection = false;
        boost::any value;
        SimpleItem() = default;
        SimpleItem(std::string text, bool isSection = false, float textSize = 60.f, float thickness = 1.f);
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
    std::vector<SimpleItem> ItemData_;
    std::vector<std::weak_ptr<Button> > Items_;
    void AddItem(int num, const std::shared_ptr<Button>& item);
public:
    unsigned animationColor;
    unsigned textColor;
    unsigned listColor;
    DropdownList(const std::string& label, const ScreenData& layoutScreen, int maxItem,
                 std::shared_ptr<FlexibleScaler> parentScaler = nullptr);
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
    SimpleItem GetSelectedItem() const;
    bool IsChangedSelect() const;
    void UpdateSelected();
    void SetMaxItem(int num);
    void AddItem(int num, const SimpleItem& item);
    void AddItem(int num, const std::string& label, boost::any value = 0, bool isSection = false);
    std::shared_ptr<Scene> GetPanelInstance();
};
