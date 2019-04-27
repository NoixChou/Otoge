#pragma once
#include "../../../../System/Task/Scene.hpp"
#include "../../../../Util/Calculate/Screen/FlexibleScaler.hpp"
#include "../../../../System/GUI/Button.hpp"
#include "../../../../System/GUI/ScrollablePanel.hpp"
#include "../../../../System/GUI/SlideBar.hpp"

class SettingScene :
    public Scene
{
private:
    std::shared_ptr<Scene> TitleBar_;
	std::shared_ptr<Button> CloseButton_;

    std::shared_ptr<ScrollablePanel> BodyPanel_;

    std::shared_ptr<Label> DisplaySectionLabel_;
    std::shared_ptr<Label> WindowWidthDescription_;
    std::shared_ptr<Label> WindowHeightDescription_;
    std::shared_ptr<SlideBar> WindowWidthSlider_;
    std::shared_ptr<SlideBar> WindowHeightSlider_;
    std::shared_ptr<Button> WindowFullSizeButton_;

public:
    SettingScene();
    ~SettingScene();

    void OnStartedFadeIn() override;
    void SceneFadeIn(float deltaTime) override;
    void SceneFadeOut(float deltaTime) override;

    void SceneUpdate(float deltaTime) override;
    void Draw() override;
};

