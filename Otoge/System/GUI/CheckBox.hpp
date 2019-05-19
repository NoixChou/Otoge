#pragma once
#include "GUI.hpp"
#include "Label.hpp"
#include "Button.hpp"

class CheckBox : public GUI
{
private:
    std::shared_ptr<Button> CheckButton_;
    std::shared_ptr<Label> TextLabel_;
    bool IsOldChecked_ = false;
    bool IsChecked_ = false;
public:
    unsigned textColor;
    unsigned animationColor;
    unsigned mouseOverColor;
    CheckBox(const std::string& label, const ScreenData& layoutScreen,
             std::shared_ptr<FlexibleScaler> parentScaler = nullptr);
    ~CheckBox();
    void GUIUpdate(float deltaTime) override;
    void Draw() override;
    void SetChecked(bool check);
    bool IsChecked();
    bool IsChanged();
    std::shared_ptr<Label> GetTextLabelInstance();
};
