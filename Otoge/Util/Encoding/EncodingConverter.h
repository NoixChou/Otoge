#pragma once

// **************************************************************************
//      Source code from https://theolizer.com/cpp-school1/cpp-school1-18/
// **************************************************************************
namespace encoding
{
    // ***************************************************************************
    //      convert from UFT-8 to UTF-16
    // ***************************************************************************
    inline std::wstring ConvertUtf8ToUtf16(char const* iString)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> l_Converter;
        return l_Converter.from_bytes(iString);
    }

    inline std::wstring ConvertUtf8ToUtf16(std::string const& iString)
    {
        return ConvertUtf8ToUtf16(iString.c_str());
    }

    // ***************************************************************************
    //      convert from UFT-16 to UTF-8
    // ***************************************************************************
    inline std::string ConvertUtf16ToUtf8(wchar_t const* iString)
    {
        std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> l_Converter;
        return l_Converter.to_bytes(iString);
    }

    inline std::string ConvertUtf16ToUtf8(std::wstring const& iString)
    {
        return ConvertUtf16ToUtf8(iString.c_str());
    }

    inline std::string ConvertUtf16ToUtf8(char16_t const* iString)
    {
        return ConvertUtf16ToUtf8(reinterpret_cast<wchar_t const*>(iString));
    }

    inline std::string ConvertUtf16ToUtf8(std::u16string const& iString)
    {
        return ConvertUtf16ToUtf8(iString.c_str());
    }

    // ***************************************************************************
    //      convert from UFT-16 to Shift-JIS
    // ***************************************************************************
    inline std::string ConvertUtf16ToSJIS(wchar_t const* iString)
    {
        const int l_Len = WideCharToMultiByte(CP_ACP, 0, iString, -1, nullptr, 0, nullptr, nullptr);
        std::string aShiftJis;
        aShiftJis.resize(l_Len);
        WideCharToMultiByte(CP_ACP, 0, iString, -1, &*aShiftJis.begin(), l_Len, nullptr, nullptr);
        aShiftJis.resize(l_Len - 1);
        return aShiftJis;
    }

    inline std::string ConvertUtf16ToSJIS(std::wstring const& iString)
    {
        return ConvertUtf16ToSJIS(iString.c_str());
    }

    inline std::string ConvertUtf16ToSJIS(char16_t const* iString)
    {
        return ConvertUtf16ToSJIS(reinterpret_cast<wchar_t const*>(iString));
    }

    inline std::string ConvertUtf16ToSJIS(std::u16string const& iString)
    {
        return ConvertUtf16ToSJIS(iString.c_str());
    }

    /* ================= Original Code ================= */

    // ***************************************************************************
    //      convert from UFT-8 to Shift-JIS
    // ***************************************************************************
    inline std::string ConvertUtf8ToSJIS(char const* iString)
    {
        return ConvertUtf16ToSJIS(ConvertUtf8ToUtf16(iString));
    }

    inline std::string ConvertUtf8ToSJIS(std::string const& iString)
    {
        return ConvertUtf8ToSJIS(iString.c_str());
    }
}
