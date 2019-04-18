#pragma once
#include "../../../../System/Task/Scene.hpp"
#include "../../../../Util/Calculate/Screen/FlexibleScaler.hpp"
#include "../../../../System/GUI/Button.hpp"

class SettingScene :
    public Scene
{
private:
	std::shared_ptr<Button> CloseButton_;

public:
    SettingScene();
    ~SettingScene();

    void SceneFadeIn(float deltaTime) override;
    void SceneFadeOut(float deltaTime) override;

    void SceneUpdate(float deltaTime) override;
    void Draw() override;
};

