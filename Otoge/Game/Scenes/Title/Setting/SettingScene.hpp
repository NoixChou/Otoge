#pragma once
#include "../../../../System/Task/Scene.hpp"
#include "../../../../Util/Calculate/Screen/FlexibleScaler.hpp"
#include "../../../../System/GUI/Button.hpp"
#include "../../../../System/GUI/ScrollablePanel.hpp"
#include "../../../../System/GUI/SlideBar.hpp"
#include "../../../../System/GUI/DropdownList.hpp"
#include "../../../../System/GUI/CheckBox.hpp"

class SettingScene :
    public Scene
{
private:
    std::weak_ptr<Scene> TitleBar_;
	std::shared_ptr<Button> CloseButton_;

    std::shared_ptr<ScrollablePanel> BodyPanel_;

    std::shared_ptr<Label> DisplaySectionLabel_;
    std::shared_ptr<Label> WindowSizeDescription_;
    std::shared_ptr<DropdownList<std::pair<int, int>>> WindowSizeList_;
    std::shared_ptr<CheckBox> FullscreenCheck_;
    std::shared_ptr<CheckBox> VSyncCheck_;

    std::shared_ptr<Label> InterfaceSectionLabel_;
    //std::shared_ptr<DropdownList> JapaneseFontList_;
    //std::shared_ptr<DropdownList> AlphabetFontList_;

    std::vector<std::pair<std::string, std::pair<int, int>>> AllowWindowSizes_;

public:
    SettingScene();
    ~SettingScene();

    void OnStartedFadeIn() override;
    void OnStartedFadeOut() override;
    void SceneFadeIn(float deltaTime) override;
    void SceneFadeOut(float deltaTime) override;

    void SceneUpdate(float deltaTime) override;
    void Draw() override;
};

