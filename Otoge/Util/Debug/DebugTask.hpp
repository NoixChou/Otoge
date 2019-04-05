#pragma once
#include "../../System/Task/Task.h"
#include "../../System/GUI/Label.hpp"

class DebugScene : public Task
{
private:
    std::shared_ptr<Label> FpsLabel_;
    std::shared_ptr<Label> DeltaTimeLabel_;

public:
    DebugScene();
    ~DebugScene();


    void Update(float deltaTime) override;
};
