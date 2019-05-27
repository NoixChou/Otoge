#include "Label.hpp"
#include "../Config.h"
#include "../../Util/Calculate/Screen/FontStringCalculator.hpp"
#include "../GlobalMethod.hpp"
#include "../../Util/Visual/Color.hpp"

Label::Label(const std::string& label, const ScreenData& layoutScreen,
             std::shared_ptr<FlexibleScaler> parentScaler) : GUI(label + "\"<Label>\"", layoutScreen, parentScaler,
                                                                 true)
{
    baseColor = color_preset::DARK_GREY;
    Label_ = label;
    textAlign = TextAlignment::left | TextAlignment::top;
}

Label::~Label()
{
}

void Label::GUIUpdate(float deltaTime)
{
    if(IsChangedSize() || IsChangedLabel_)
    {
        //Logger_->Debug("Ascent: " + std::to_string(GetFontAscentToHandle(FontHandle_)));
        if((textAlign & TextAlignment::left) == TextAlignment::left) TextX_ = 0.f;
        else if((textAlign & TextAlignment::center) == TextAlignment::center) TextX_ = GetRawScreenWidth() / 2.f -
            FontStringCalculator::GetStringCenterHorizontal(FontHandle_, Label_);
        else if((textAlign & TextAlignment::right) == TextAlignment::right) TextX_ = GetRawScreenWidth() -
            FontStringCalculator::GetStringWidth(FontHandle_, Label_);
        if((textAlign & TextAlignment::top) == TextAlignment::top) TextY_ = 0.f;
        else if((textAlign & TextAlignment::middle) == TextAlignment::middle) TextY_ = GetRawScreenHeight() / 2.f -
            FontStringCalculator::GetStringCenterVertical(FontHandle_);
        else if((textAlign & TextAlignment::bottom) == TextAlignment::bottom) TextY_ = GetRawScreenHeight() -
            FontStringCalculator::GetStringHeight(FontHandle_);
    }
}

void Label::Draw()
{
    DrawStringFToHandle(TextX_, TextY_, Label_.c_str(), baseColor, FontHandle_);
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
