﻿#pragma once
#include "../Task/Scene.hpp"
#include "../../Util/Calculate/ScreenData.h"

#include "EventSystem/Include/EventSystem.hpp"

class GUI : public Scene
{
private:
    static int GlobalGUICount_;
    static int JapaneseFontCount_;
    static char* JapaneseFonts_;
protected:
    std::string Label_;
    bool IsChangedLabel_ = false;
    bool IsFontSizeInit_ = false;
    bool IsUseFont_ = false;
    int FontHandle_ = -1;
public:
    events::Delegate<bool, events::Event&> testEvent;
    bool adjustmentFontSize = false;
    unsigned baseColor;
    GUI(const std::string& guiName, const ScreenData& layoutScreen,
        std::shared_ptr<FlexibleScaler> parentScaler = nullptr, bool isUseFont = false);
    ~GUI();
    void SceneUpdate(float deltaTime) override;
    virtual void GUIUpdate(float deltaTime) = 0;
    void Draw() override = 0;
    void OnReCalculateScreen() override;
    void SetLabel(const std::string& label);
    std::string GetLabel() const;
    void AdjustFont();
    bool ChangeFont(const char* fontName = nullptr, int size = -1, int thickness = -1, int fontType = -1);
    bool ChangeFontSize(int size = -1);
    bool ChangeFontThickness(int thickness = -1);
    void SetFontHandle(int fontHandle);
    int GetFontHandle() const;
    int GetFontSize() const;
    void RemoveFont();
    static int GetDefaultFontDrawType();
};
