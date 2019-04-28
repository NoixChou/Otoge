#pragma once
#include "../../../../System/Task/Scene.hpp"
#include "../../../../Util/Calculate/Screen/FlexibleScaler.hpp"
#include "../../../../System/GUI/Button.hpp"
#include "../../../../System/GUI/ScrollablePanel.hpp"
#include "../../../../System/GUI/SlideBar.hpp"
#include "../../../../System/GUI/DropdownList.hpp"

class SettingScene :
    public Scene
{
private:
    std::shared_ptr<Scene> TitleBar_;
	std::shared_ptr<Button> CloseButton_;

    std::shared_ptr<ScrollablePanel> BodyPanel_;

    std::shared_ptr<Label> DisplaySectionLabel_;
    std::shared_ptr<Label> WindowSizeDescription_;
    std::shared_ptr<DropdownList> WindowSizeList_;

    std::vector<std::vector<std::string>> AllowWindowSizes_;

public:
    SettingScene();
    ~SettingScene();

    void OnStartedFadeIn() override;
    void SceneFadeIn(float deltaTime) override;
    void SceneFadeOut(float deltaTime) override;

    void SceneUpdate(float deltaTime) override;
    void Draw() override;
};

