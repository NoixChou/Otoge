#pragma once
#include "GUI.hpp"
#include "Button.hpp"

class DropdownList : public GUI
{
private:
    int ItemCount_ = 0;
    int BeforeSelectedItem_ = 1;
    int SelectedItem_ = 1;
    bool IsListOpened_ = false;
    bool IsListOpening_ = false;

    bool IsPanelAdded_ = false;

    float PanelHeight_ = 0.f;

    std::shared_ptr<Scene> Panel_;
    std::shared_ptr<Label> SelectedLabel_;
    std::shared_ptr<Label> TriangleLabel_;
    
    std::vector<std::string> ItemNames_;
    std::vector<std::weak_ptr<Button>> Items_;

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
    bool IsChangedSelect() const;

    void SetMaxItem(int num);
    void AddItem(int num, const std::shared_ptr<Button>& item);
    void AddItem(int num, const std::string& label);

    std::shared_ptr<Scene> GetPanelInstance();
};
