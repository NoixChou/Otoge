#pragma once
#include "GUI.hpp"
#include "Label.hpp"

class Button : public GUI
{
private:
    std::shared_ptr<Label> TextLabel_;

public:
    unsigned textColor;

    Button(const std::string& label, const ScreenData& layoutScreen, std::shared_ptr<FlexibleScaler> parentScaler = nullptr);
    ~Button();

    void GUIUpdate(float deltaTime) override;
    void Draw() override;

    void SetTextLabelInstance(std::shared_ptr<Label> textLabel);
    std::shared_ptr<Label> GetTextLabelInstance();
};
