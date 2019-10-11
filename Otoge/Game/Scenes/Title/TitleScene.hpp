#pragma once
#include "EventSystem/Include/EventSystem.hpp"
#include "../../../System/Task/Scene.hpp"
#include "../../../Util/Calculate/Screen/FlexibleScaler.hpp"
#include "../../../System/GUI/Button.hpp"

class TitleScene : public Scene, events::ManagedEventListener
{
private:
    bool IsOpened_ = false;
    bool IsMoving_ = false;

    std::shared_ptr<Button> MenuOpener_;
    std::shared_ptr<Scene> MenuGroup_;
    std::shared_ptr<Button> MenuPlay_;
    std::shared_ptr<Button> MenuOption_;
    std::shared_ptr<Button> MenuClose_;

public:
    TitleScene();
    ~TitleScene();

    static std::shared_ptr<Scene> SettingScene_;

    void SceneFadeIn(float deltaTime) override;
    void OnStoppedFadeIn() override;

    void OnStartedFadeOut() override;
    void SceneFadeOut(float deltaTime) override;
    void OnStoppedFadeOut() override;

    void SceneUpdate(float deltaTime) override;
    void Draw() override;

    bool TestHandler(events::Event& e) const;
};
