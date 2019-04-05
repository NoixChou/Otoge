#include "Label.hpp"
#include "../Config.h"
#include "../../Util/Calculate/Screen/FontStringCalculator.hpp"


Label::Label(const std::string& label, const ScreenData& layoutScreen, std::shared_ptr<FlexibleScaler> parentScaler) : GUI(label + "\"<Label>\"", layoutScreen, parentScaler)
{
    Label_ = label;
    textAlign = TextAlignment::left | TextAlignment::top;
}

Label::~Label()
{

}

void Label::GUIUpdate(float deltaTime)
{

}

void Label::Draw()
{
    float textX = 0.f, textY = 0.f;

    if ((textAlign & TextAlignment::left) == TextAlignment::left)
        textX = 0.f;
    else if ((textAlign & TextAlignment::center) == TextAlignment::center)
        textX = screen.width / 2.f - FontStringCalculator::GetStringCenterHorizontal(FontHandle_, Label_);
    else if ((textAlign & TextAlignment::right) == TextAlignment::right)
        textX = screen.width - FontStringCalculator::GetStringWidth(FontHandle_, Label_);

    if ((textAlign & TextAlignment::top) == TextAlignment::top)
        textY = 0.f;
    else if ((textAlign & TextAlignment::middle) == TextAlignment::middle)
        textY = screen.height / 2.f - FontStringCalculator::GetStringCenterVertical(FontHandle_);
    else if ((textAlign & TextAlignment::bottom) == TextAlignment::bottom)
        textY = screen.height - FontStringCalculator::GetStringHeight(FontHandle_);

    DrawStringToHandle(static_cast<int>(textX), static_cast<int>(textY), Label_.c_str(), baseColor, FontHandle_);
}

void Label::SetTextAlign(TextAlignment align)
{
    textAlign = align;
}


Label::TextAlignment operator|(Label::TextAlignment l, Label::TextAlignment r)
{
    return static_cast<Label::TextAlignment>(static_cast<int>(l) | static_cast<int>(r));
}

Label::TextAlignment operator&(Label::TextAlignment l, Label::TextAlignment r)
{
    return static_cast<Label::TextAlignment>(static_cast<int>(l) & static_cast<int>(r));
}
