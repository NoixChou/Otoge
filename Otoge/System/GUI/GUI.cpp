#include "GUI.hpp"
#include "../../Util/Calculate/ScreenData.h"
#include "../Config.h"
#include "../../Util/Calculate/Screen/FontStringCalculator.hpp"
#include "../../Util/Setting/SettingManager.h"
#include "../GlobalMethod.hpp"
#include "../../Util/Visual/Color.hpp"
#include "../../Util/Window/DxSettings.hpp"

GUI::GUI(const std::string& guiName, const ScreenData& layoutScreen, std::shared_ptr<FlexibleScaler> parentScaler) : Scene(guiName, layoutScreen, parentScaler)
{
    baseColor = color_preset::WHITE;
    ChangeFont(SettingManager::GetGlobal()->Get<std::string>(game_config::SETTINGS_FONT_NAME).get().c_str(), -1, 1, SettingManager::GetGlobal()->Get<int>(game_config::SETTINGS_FONT_DRAWTYPE).get());
    Logger_->Debug("GUI初期化 完了");
}

GUI::~GUI()
{
    RemoveFont();
}

void GUI::SceneUpdate(float deltaTime)
{
    if(IsChangedSize() && AdjustmentFontSize_)
    {
        ChangeFontSize(engine::CastToInt(GetRawScreenHeight()));
        if (FontStringCalculator::GetStringWidth(FontHandle_, Label_) > GetRawScreenWidth()) ChangeFontSize(engine::CastToInt(GetRawScreenWidth()) / static_cast<int>(Label_.length()));
    }

    GUIUpdate(deltaTime);
    IsChangedLabel_ = false;
}

void GUI::SetLabel(const std::string& label)
{
    IsChangedLabel_ = true;
    Label_ = label;
}

std::string GUI::GetLabel()
{
    return Label_;
}

bool GUI::ChangeFont(const char* fontName, int size, int thickness, int fontType)
{
    RemoveFont();

    if(FontHandle_ == -1)
    {
        Logger_->Debug("thickness: " + std::to_string(thickness) + ", size: " + std::to_string(size));
        if (thickness != -1 && (thickness < 6 && size < 10)) fontType = DX_FONTTYPE_NORMAL;
        else fontType = SettingManager::GetGlobal()->Get<int>(game_config::SETTINGS_FONT_DRAWTYPE).get();
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
