#pragma once
#include "GUI.hpp"

class Label : public GUI
{
private:

public:
    enum class TextAlignment : int
    {
        left = 1 << 0,
        center = 1 << 1,
        right = 1 << 2,
        top = 1 << 3,
        middle = 1 << 4,
        bottom = 1 << 5,

    };

    friend TextAlignment operator|(TextAlignment l, TextAlignment r);
    friend TextAlignment operator&(TextAlignment l, TextAlignment r);

    TextAlignment textAlign;

    Label(const std::string& label, const ScreenData& layoutScreen, std::shared_ptr<FlexibleScaler> parentScaler = nullptr);
    ~Label();

    void GUIUpdate(float deltaTime) override;
    void Draw() override;

    void SetTextAlign(TextAlignment align);
};
