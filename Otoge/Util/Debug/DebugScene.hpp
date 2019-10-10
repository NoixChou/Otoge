#pragma once
// #include "../Event/EventListener.hpp"
#include "../../System/Task/Scene.hpp"
#include "../../System/GUI/Label.hpp"
#include "../../System/GUI/Button.hpp"

class DebugScene : public Scene//, public EventListener
{
private:
    std::shared_ptr<Label> FpsLabel_;
    std::shared_ptr<Label> FpsDescLabel_;
    std::shared_ptr<Label> DeltaTimeLabel_;
    std::shared_ptr<Label> DeltaTimeDescLabel_;
    std::shared_ptr<Button> TestButton_;
public:
    DebugScene();
    ~DebugScene();
    void SceneUpdate(float deltaTime) override;
    void Draw() override;

    //bool HandleTestEvent(Event e);
};
