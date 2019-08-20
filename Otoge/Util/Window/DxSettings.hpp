#pragma once
struct DxSettings
{
    static int windowWidth;
    static int windowHeight;
    static bool isFullScreen;
    static bool doVSync;
    static int antialiasingQuality;
    static int antialiasingSample;
    static bool useOriginalCursor;

    static std::string defaultFont;
    static std::string alphabetFont;
    static int fontType;
};
