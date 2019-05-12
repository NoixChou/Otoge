#include "GUI.hpp"
#include "../../Util/Calculate/ScreenData.h"
#include "../Config.h"
#include "../../Util/Calculate/Screen/FontStringCalculator.hpp"
#include "../../Util/Setting/SettingManager.h"
#include "../GlobalMethod.hpp"
#include "../../Util/Visual/Color.hpp"
#include "../../Util/Window/DxSettings.hpp"
#include "../../Util/Encoding/CharacterType.hpp"
#include "../../Util/Encoding/EncodingConverter.h"
int GUI::GlobalGUICount_ = 0;
int GUI::JapaneseFontCount_ = 0;
char* GUI::JapaneseFonts_ = nullptr;

GUI::GUI(const std::string& guiName, const ScreenData& layoutScreen, std::shared_ptr<FlexibleScaler> parentScaler, bool isUseFont) : Scene(guiName, layoutScreen, parentScaler)
{
    baseColor = color_preset::WHITE;
    IsUseFont_ = isUseFont;

    if (IsUseFont_)
    {
        if(JapaneseFonts_ == nullptr)
        {
            JapaneseFontCount_ = EnumFontName(nullptr, 0, FALSE);
            JapaneseFonts_ = static_cast<char*>(std::calloc(JapaneseFontCount_, 64));
            EnumFontName(JapaneseFonts_, JapaneseFontCount_);
        }
    }
    GlobalGUICount_++;
    Logger_->Debug("GUI初期化 完了");
}

GUI::~GUI()
{
    RemoveFont();

    GlobalGUICount_--;

    if(JapaneseFonts_ != nullptr && GlobalGUICount_ <= 0)
    {
        free(JapaneseFonts_);
        JapaneseFonts_ = nullptr;
        JapaneseFontCount_ = 0;
    }
}

void GUI::SceneUpdate(float deltaTime)
{
    if(IsUseFont_ && FontHandle_ == -1 && !Label_.empty())
    {
        if (character::HasDoubleByteString(Label_))
            ChangeFont(SettingManager::GetGlobal()->Get<std::string>(game_config::SETTINGS_FONT_NAME).get().c_str(), -1, 1, -1);
        else
            ChangeFont(SettingManager::GetGlobal()->Get<std::string>(game_config::SETTINGS_ALPHABET_FONT_NAME).get().c_str(), -1, 1, -1);

        if (AdjustmentFontSize_)
            AdjustFont();
    }

    if((IsChangedSize()) && AdjustmentFontSize_)
    {
        AdjustFont();
    }

    GUIUpdate(deltaTime);
    IsChangedLabel_ = false;
}

void GUI::OnReCalculateScreen()
{
}

void GUI::SetLabel(const std::string& label)
{
    IsChangedLabel_ = Label_ != label;
    Label_ = label;
}

std::string GUI::GetLabel()
{
    return Label_;
}

void GUI::AdjustFont()
{
    //if (((FontStringCalculator::GetStringWidth(FontHandle_, Label_) > GetRawScreenWidth()) &&
    //    (FontStringCalculator::GetStringHeight(FontHandle_) > GetRawScreenHeight())) ||
    //    (engine::CompareTolerance(FontStringCalculator::GetStringWidth(FontHandle_, Label_), GetRawScreenWidth(), 0.01f) != 0))
    {

        ChangeFontSize(engine::CastToInt(GetRawScreenHeight()));
        if(FontStringCalculator::GetStringWidth(FontHandle_, Label_) > GetRawScreenWidth())
        {
            ChangeFontSize(engine::CastToInt(GetRawScreenWidth()) / static_cast<int>(Label_.length()));
        }
        /*while (FontStringCalculator::GetStringWidth(FontHandle_, Label_) > GetRawScreenWidth())
        {
            if (GetFontSize() <= 1) break;
            ChangeFontSize(GetFontSize() - 1);
            //ChangeFontSize(engine::CastToInt(GetRawScreenWidth()) / static_cast<int>(Label_.length()));
        }*/
    }
}

bool GUI::ChangeFont(const char* fontName, int size, int thickness, int fontType)
{
    if(FontHandle_ != -1)
    {
        if (fontName != nullptr && size != -1 && thickness != -1)
        {
            TCHAR l_fontName[512];
            int l_size;
            int l_thickness;
            if (GetFontStateToHandle(l_fontName, &l_size, &l_thickness, FontHandle_))
            {
                if (fontName == l_fontName && size == l_size && thickness == l_thickness)
                    return true;
            }
        }
    }

    RemoveFont();

    if (IsUseFont_)
    {
        if (fontType == -1) fontType = GetDefaultFontDrawType();

        if (FontHandle_ == -1)
        {
            std::string l_Fontstr = SettingManager::GetGlobal()->Get<std::string>(game_config::SETTINGS_FONT_NAME).get();
            if (fontName == nullptr)
            {
                fontName = l_Fontstr.c_str();

                if (!character::HasDoubleByteString(Label_))
                {
                    l_Fontstr = SettingManager::GetGlobal()->Get<std::string>(game_config::SETTINGS_ALPHABET_FONT_NAME).get();
                    fontName = l_Fontstr.c_str();
                    Logger_->Debug("英字フォントに切り替えます: " + l_Fontstr);
                }
            }
            FontHandle_ = CreateFontToHandle(fontName, size, thickness, fontType);
        }
        else
        {
            Logger_->Error("既にフォントが生成されていて、削除に失敗");
        }

        if (FontHandle_ == -1)
        {
            Logger_->Error("フォントの生成に失敗。デフォルトフォントを使用");
        }
    }

    return FontHandle_ != -1;
}

bool GUI::ChangeFontSize(int size)
{
    TCHAR fontName[512];
    int thickness;
    int fontType;

    if (GetFontStateToHandle(fontName, nullptr, &thickness, FontHandle_, &fontType) != -1)
    {
        return ChangeFont(fontName, size, thickness, fontType);
    }

    return ChangeFont(nullptr, size, -1, -1);
}

bool GUI::ChangeFontThickness(int thickness)
{
    TCHAR fontName[512];
    int size;
    int fontType;
    if (GetFontStateToHandle(fontName, &size, nullptr, FontHandle_, &fontType) != -1)
    {
        return ChangeFont(fontName, size, thickness, fontType);
    }

    return ChangeFont(nullptr, -1, thickness, -1);
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

int GUI::GetDefaultFontDrawType()
{
    return SettingManager::GetGlobal()->Get<int>(game_config::SETTINGS_FONT_DRAWTYPE).get();
}
