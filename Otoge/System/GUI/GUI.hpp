#pragma once
#include "../Task/Scene.hpp"
#include "../../Util/Calculate/ScreenData.h"

class GUI : public Scene
{
private:

protected:
    std::string Label_;
    int FontHandle_ = -1;

public:
    bool AdjustmentFontSize_ = true;
    unsigned baseColor;

    GUI(const std::string& guiName, const ScreenData& layoutScreen, std::shared_ptr<FlexibleScaler> parentScaler = nullptr);
    ~GUI();

    void SceneUpdate(float deltaTime) override;
    virtual void GUIUpdate(float deltaTime) = 0;
    virtual void Draw() = 0;

    void SetLabel(const std::string& label);

    bool ChangeFont(const char *fontName = nullptr, int size = -1, int thickness = -1, int fontType = -1);
    bool ChangeFontSize(int size = -1);
    bool ChangeFontThickness(int thickness = -1);
    const int GetFontHandle();
    int GetFontSize();
    void RemoveFont();
};
