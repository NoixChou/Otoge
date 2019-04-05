#pragma once
#include "../../System/Task/Scene.hpp"
#include "../../System/GUI/Label.hpp"

class DebugScene : public Scene
{
private:
    std::shared_ptr<Label> FpsLabel_;
    std::shared_ptr<Label> DeltaTimeLabel_;

public:
    DebugScene();
    ~DebugScene();

    void SceneUpdate(float deltaTime) override;
    void Draw() override;
};
