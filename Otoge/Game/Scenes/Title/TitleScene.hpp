#pragma once
#include "../../../System/Task/Scene.hpp"
#include "../../../Util/Calculate/Screen/FlexibleScaler.hpp"
#include "../../../System/GUI/Button.hpp"

class TitleScene : public Scene
{
private:
    bool isOpened = false;
    bool isMoving = false;

    std::weak_ptr<Button> MenuOpener_;
    std::weak_ptr<Scene> MenuGroup_;
    std::weak_ptr<Button> MenuPlay_;
    std::weak_ptr<Button> MenuOption_;
    std::weak_ptr<Button> MenuClose_;
    static std::weak_ptr<Scene> SettingScene_;
public:
    TitleScene();
    ~TitleScene();
    void SceneFadeIn(float deltaTime) override;
    void SceneUpdate(float deltaTime) override;
    void Draw() override;
};
