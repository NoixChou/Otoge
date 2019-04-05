#pragma once
#include "../../System/Task/Task.h"
#include "../../Util/Calculate/ScreenData.h"
#include "../../Util/Calculate/Screen/FlexibleScaler.hpp"

class Scene : public Task
{
private:
    int SceneBuffer_ = -1;
    ScreenData PrevScreen_;
    bool HasChangedScreen_ = false;

    bool IsVisible_ = true;
    float Transparency_ = 100.f;

    // デバッグ
    bool isDrawFrame;
protected:
    bool isAutoScaling = true;
    /*float CurrentParentWidth_ = 0.f, CurrentParentHeight_ = 0.f;*/
    std::shared_ptr<FlexibleScaler> ParentScaler_;
    std::shared_ptr<FlexibleScaler> DefaultScaler_;
    std::shared_ptr<Scene> PrevScene_;
    ScreenData PreLayoutScreen_;

public:
    ScreenData screen;

    Scene(const std::string &sceneName, float sceneWidth = -1.f, float sceneHeight = -1.f, float sceneX = 0.f, float sceneY = 0.f, std::shared_ptr<FlexibleScaler> parentScaler = nullptr, std::shared_ptr<Scene> prevScene = nullptr);
    Scene(const std::string &sceneName, const ScreenData& screen, std::shared_ptr<FlexibleScaler> parentScaler = nullptr, std::shared_ptr<Scene> prevScene = nullptr);
    ~Scene();

    void Update(float deltaTime) override;

    virtual void SceneUpdate(float deltaTime) = 0;
    virtual void Draw() = 0;

    bool RefreshDrawBuffer();

    void SetVisible(bool visible);
    void SetTransparent(float transparent);
    float GetTransparent();

    bool IsVisible() const;
    bool IsChangedScreen();
    bool IsOnMouse() const;
    bool IsDownMouse() const;
    bool IsHoldMouse() const;
    bool IsClickedMouse();
};
