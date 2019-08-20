#pragma once
class FontHandleCreator
{
private:

public:
    enum FontType
    {
        normal,
        alphabet,
        number,
    };

    static int Create(int size, int thickness, const TCHAR* fontName);
    static int Create(int size, int thickness, FontType fontType = FontType::normal);
};
