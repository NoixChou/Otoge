#include "GUI.hpp"
#include "../../Util/Calculate/ScreenData.h"
#include "../Config.h"
#include "../../Util/Calculate/Screen/FontStringCalculator.hpp"
#include "../../Util/Setting/SettingManager.h"

GUI::GUI(const std::string& guiName, const ScreenData& layoutScreen, std::shared_ptr<FlexibleScaler> parentScaler) : Scene(guiName, layoutScreen, parentScaler)
{
    Logger_->Debug("GUI初期化 完了");
    ChangeFont(SettingManager::GetGlobal()->Get<std::string>(SETTINGS_FONT_NAME).get().c_str(), -1, 1, SettingManager::GetGlobal()->Get<int>(SETTINGS_FONT_DRAWTYPE).get());
}

GUI::~GUI()
{
    RemoveFont();
}

void GUI::SceneUpdate(float deltaTime)
{
    if(IsChangedScreen() && AdjustmentFontSize_)
    {
        ChangeFontSize(static_cast<int>(floor(GetRawScreenHeight())));
        if (FontStringCalculator::GetStringWidth(FontHandle_, Label_) > GetRawScreenWidth()) ChangeFontSize(static_cast<int>(floor(GetRawScreenWidth() * 1.5f)) / static_cast<int>(Label_.length()));
    }

    GUIUpdate(deltaTime);
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
        Logger_->Error("既にフォントが生成されていて、削除に失敗");
    }

    if(FontHandle_ == -1)
    {
        Logger_->Error("フォントの生成に失敗。デフォルトフォントを使用");
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

int GUI::GetFontHandle() const
{
    return FontHandle_;
}

int GUI::GetFontSize() const
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
