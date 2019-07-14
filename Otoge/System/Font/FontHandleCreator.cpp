#include "FontHandleCreator.hpp"
#include "../Config.h"
#include "../../Util/Setting/SettingManager.h"

int FontHandleCreator::Create(int size, int thickness, const TCHAR* fontName)
{
    return CreateFontToHandle(fontName, size, thickness, SettingManager::GetGlobal()->Get<int>(game_config::SETTINGS_FONT_DRAWTYPE).get());
}

int FontHandleCreator::Create(int size, int thickness, FontType fontType)
{
    std::string l_FontName;

    if(fontType == FontType::normal)
    {
        l_FontName = SettingManager::GetGlobal()->Get<std::string>(game_config::SETTINGS_FONT_NAME).get();
    }else
    {
        l_FontName = SettingManager::GetGlobal()->Get<std::string>(game_config::SETTINGS_ALPHABET_FONT_NAME).get();
    }

    return Create(size, thickness, l_FontName.c_str());
}
