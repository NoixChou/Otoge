#include "GUI.hpp"
#include "../../Util/Calculate/ScreenData.h"
#include "../Config.h"
#include "../../Util/Calculate/Screen/FontStringCalculator.hpp"

GUI::GUI(const std::string& guiName, const ScreenData& layoutScreen, std::shared_ptr<FlexibleScaler> parentScaler) : Scene(guiName, layoutScreen, parentScaler)
{
    Logger_->Debug("GUI初期化 完了");
    ChangeFont(GAME_APP_DEFAULT_FONT, -1, 1, -1);
}

GUI::~GUI()
{
    RemoveFont();
}

void GUI::SceneUpdate(float deltaTime)
{
    if(IsChangedScreen() && AdjustmentFontSize_)
    {
        ChangeFontSize(static_cast<int>(floor(this->screen.height)));
        if (FontStringCalculator::GetStringWidth(FontHandle_, Label_) > screen.width) ChangeFontSize(static_cast<int>(floor(screen.width * 1.8f)) / Label_.length());
    }

    GUIUpdate(deltaTime);
    //Draw();
}

void GUI::SetLabel(const std::string& label)
{
    Label_ = label;
}

bool GUI::ChangeFont(const char* fontName, int size, int thickness, int fontType)
{
    RemoveFont();

    if(FontHandle_ == -1)
    {
        FontHandle_ = CreateFontToHandle(fontName, size, thickness, fontType);
    }else
    {
        Logger_->Error("既にフォントが生成されていて、削除に失敗しました");
    }

    if(FontHandle_ == -1)
    {
        Logger_->Error("フォントの生成に失敗しました。デフォルトフォントを使用します。");
    }

    return FontHandle_ != -1;
}

bool GUI::ChangeFontSize(int size)
{
    if (FontHandle_ != -1) {
        TCHAR fontName[256];
        int thickness;
        int fontType;
        
        if (GetFontStateToHandle(fontName, nullptr, &thickness, FontHandle_, &fontType) != -1)
        {
            return ChangeFont(fontName, size, thickness, fontType);
        }
    }
    return false;
}

bool GUI::ChangeFontThickness(int thickness)
{
    if (FontHandle_ != -1) {
        TCHAR* fontName = nullptr;
        int size;
        int fontType;
        if (GetFontStateToHandle(fontName, &size, nullptr, FontHandle_, &fontType) != -1)
        {
            return ChangeFont(fontName, size, thickness, fontType);
        }
    }
    return false;
}

const int GUI::GetFontHandle()
{
    return FontHandle_;
}

int GUI::GetFontSize()
{
    int fontSize = 0;
    GetFontStateToHandle(nullptr, &fontSize, nullptr, FontHandle_);
    return fontSize;
}

void GUI::RemoveFont()
{
    if (FontHandle_ != -1)
    {
        DeleteFontToHandle(FontHandle_);
        FontHandle_ = -1;
    }
}
