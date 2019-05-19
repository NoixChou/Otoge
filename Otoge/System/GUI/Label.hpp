#pragma once
#include "GUI.hpp"

class Label : public GUI
{
private:
    float TextX_ = 0.f, TextY_ = 0.f;
public:
    enum class TextAlignment : int
    {
        left = 1 << 0,
        center = 1 << 1,    // 水平の中心
        right = 1 << 2,
        top = 1 << 3,
        middle = 1 << 4,    // 垂直の中心
        bottom = 1 << 5,
    };

    friend TextAlignment operator|(TextAlignment l, TextAlignment r);
    friend TextAlignment operator&(TextAlignment l, TextAlignment r);
    TextAlignment textAlign;
    Label(const std::string& label, const ScreenData& layoutScreen,
          std::shared_ptr<FlexibleScaler> parentScaler = nullptr);
    ~Label();
    void GUIUpdate(float deltaTime) override;
    void Draw() override;
    void SetTextAlign(TextAlignment align);
};
