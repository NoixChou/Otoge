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

GUI::GUI(const std::string& guiName, const ScreenData& layoutScreen, std::shared_ptr<FlexibleScaler> parentScaler,
         bool isUseFont) : Scene(guiName, layoutScreen, parentScaler)
{
    baseColor = color_preset::WHITE;
    IsUseFont_ = isUseFont;
    if(IsUseFont_)
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
        if(character::HasDoubleByteString(Label_))
            ChangeFont(DxSettings::defaultFont.c_str(), -1, 1, -1);
        else
            ChangeFont(DxSettings::alphabetFont.c_str(), -1, 1, -1);
        AdjustFont();
    }
    if((IsChangedSize()) && adjustmentFontSize)
    {
        AdjustFont();
    }
    if(IsClickedMouse())
    {
        events::Event e;
        e.name = "testClickEvent";
        testEvent(e);
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

std::string GUI::GetLabel() const
{
    return Label_;
}

void GUI::AdjustFont()
{
    if (!IsFontSizeInit_)
    {
        ChangeFontSize(engine::CastToInt(GetRawScreenHeight()));
        IsFontSizeInit_ = true;
    }
    //ChangeFontSize(engine::CastToInt(GetRawScreenHeight()));

    if (((FontStringCalculator::GetStringWidth(FontHandle_, Label_) > GetRawScreenWidth()) &&
        (FontStringCalculator::GetStringHeight(FontHandle_) > GetRawScreenHeight())) ||
        (engine::CompareTolerance(FontStringCalculator::GetStringWidth(FontHandle_, Label_), GetRawScreenWidth(), 0.01f) != 0))
    {
        
        while (FontStringCalculator::GetStringWidth(FontHandle_, Label_) > GetRawScreenWidth())
        {
            if (GetFontSize() <= 1) break;
            ChangeFontSize(GetFontSize() - 1);
            //ChangeFontSize(engine::CastToInt(GetRawScreenWidth()) / static_cast<int>(Label_.length()));
        }
    }
}

bool GUI::ChangeFont(const char* fontName, int size, int thickness, int fontType)
{
    if(FontHandle_ != -1)
    {
        if(fontName != nullptr && size != -1 && thickness != -1)
        {
            TCHAR l_FontName[512];
            int l_Size;
            int l_Thickness;
            if(GetFontStateToHandle(l_FontName, &l_Size, &l_Thickness, FontHandle_))
            {
                if(fontName == l_FontName && size == l_Size && thickness == l_Thickness) return true;
            }
        }
    }
    RemoveFont();
    if(IsUseFont_)
    {
        if(fontType == -1) fontType = GetDefaultFontDrawType();
        if(FontHandle_ == -1)
        {
            if (size % 2 != 0) size--;

            std::string l_Fontstr = DxSettings::defaultFont;
            if(fontName == nullptr)
            {
                fontName = l_Fontstr.c_str();
                if(!character::HasDoubleByteString(Label_))
                {
                    l_Fontstr = DxSettings::alphabetFont;
                    fontName = l_Fontstr.c_str();
                    Logger_->Debug("英字フォントに切り替えます: " + l_Fontstr);
                }
            }

            SetFontCacheUsePremulAlphaFlag(TRUE);
            FontHandle_ = CreateFontToHandle(fontName, size, thickness, fontType);
        }
        else
        {
            Logger_->Error("既にフォントが生成されていて、削除に失敗");
        }
        if(FontHandle_ == -1)
        {
            Logger_->Error("フォントの生成に失敗。デフォルトフォントを使用");
        }
    }
    return FontHandle_ != -1;
}

bool GUI::ChangeFontSize(int size)
{
    TCHAR l_FontName[512];
    int l_Thickness;
    int l_FontType;
    if(GetFontStateToHandle(l_FontName, nullptr, &l_Thickness, FontHandle_, &l_FontType) != -1)
    {
        return ChangeFont(l_FontName, size, l_Thickness, l_FontType);
    }
    return ChangeFont(nullptr, size, -1, -1);
}

bool GUI::ChangeFontThickness(int thickness)
{
    TCHAR l_FontName[512];
    int l_Size;
    int l_FontType;
    if(GetFontStateToHandle(l_FontName, &l_Size, nullptr, FontHandle_, &l_FontType) != -1)
    {
        return ChangeFont(l_FontName, l_Size, thickness, l_FontType);
    }
    return ChangeFont(nullptr, -1, thickness, -1);
}

void GUI::SetFontHandle(int fontHandle)
{
    if (CheckFontHandleValid(fontHandle) == FALSE) return;

    RemoveFont();
    IsUseFont_ = false;
    FontHandle_ = fontHandle;

    Logger_->Debug("生成済みフォントハンドルをセット");
}

int GUI::GetFontHandle() const
{
    return FontHandle_;
}

int GUI::GetFontSize() const
{
    int l_FontSize = 0;
    GetFontStateToHandle(nullptr, &l_FontSize, nullptr, FontHandle_);
    return l_FontSize;
}

void GUI::RemoveFont()
{
    if(FontHandle_ != -1)
    {
        DeleteFontToHandle(FontHandle_);
        FontHandle_ = -1;
    }
}

int GUI::GetDefaultFontDrawType()
{
    return DxSettings::fontType;
}
