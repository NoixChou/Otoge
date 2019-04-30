#pragma once
#include "GUI.hpp"
#include "Label.hpp"
#include "../Task/DrawableTask.hpp"

class Button : public GUI
{
private:
    std::shared_ptr<Label> TextLabel_;

public:
    bool isDrawBase = true;

    unsigned textColor;
	unsigned animationColor;
    unsigned mouseOverColor;

    Button(const std::string& label, const ScreenData& layoutScreen, std::shared_ptr<FlexibleScaler> parentScaler = nullptr);
    ~Button();

    void GUIUpdate(float deltaTime) override;
    void Draw() override;

    void SetTextLabelInstance(std::shared_ptr<Label> textLabel);
    std::shared_ptr<Label> GetTextLabelInstance();
};

class ButtonPushedAnimate : public DrawableTask
{
private:
	float Size_;
public:
	unsigned color;

	ButtonPushedAnimate(float x, float y, unsigned color, float size = 1.f, std::shared_ptr<FlexibleScaler> parentScaler = nullptr);
	~ButtonPushedAnimate();

	void PreUpdate(float deltaTime) override;
	void Draw() override;
};