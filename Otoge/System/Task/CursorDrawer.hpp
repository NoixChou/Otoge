#pragma once
#include "Scene.hpp"

class CursorDrawer : public Scene
{
public:
    CursorDrawer();
    ~CursorDrawer();
    void SceneUpdate(float deltaTime) override;
    void Draw() override;
};
