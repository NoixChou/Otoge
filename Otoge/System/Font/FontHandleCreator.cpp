#include "FontHandleCreator.hpp"
#include "../Config.h"
#include "../../Util/Setting/SettingManager.h"
#include "../../Util/Window/DxSettings.hpp"

int FontHandleCreator::Create(int size, int thickness, const TCHAR* fontName)
{
    int l_PrevAsyncStat = GetUseASyncLoadFlag();

    SetUseASyncLoadFlag(FALSE);
    int l_Handle = CreateFontToHandle(fontName, size, thickness, DxSettings::fontType);
    SetUseASyncLoadFlag(l_PrevAsyncStat);

    return l_Handle;
}

int FontHandleCreator::Create(int size, int thickness, FontType fontType)
{
    std::string l_FontName;

    if(fontType == FontType::normal)
    {
        l_FontName = DxSettings::defaultFont;
    }else
    {
        l_FontName = DxSettings::alphabetFont;
    }

    return Create(size, thickness, l_FontName.c_str());
}
