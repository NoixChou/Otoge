#pragma once
#include "../../../System/Task/Scene.hpp"
#include "../../../Util/Calculate/Screen/FlexibleScaler.hpp"

class TitleScene :
    public Scene
{
private:

public:
    TitleScene();
    ~TitleScene();

    void SceneUpdate(float deltaTime) override;
    void Draw() override;
};

