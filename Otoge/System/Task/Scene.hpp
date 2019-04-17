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
	ScreenData Screen_;

    bool isAutoScaling = true;
    float CurrentParentWidth_ = 0.f, CurrentParentHeight_ = 0.f;
    std::shared_ptr<FlexibleScaler> ParentScaler_ = nullptr;
    std::shared_ptr<FlexibleScaler> DefaultScaler_ = nullptr;
    std::shared_ptr<Scene> PrevScene_ = nullptr;
    ScreenData PreLayoutScreen_ = {0.f, 0.f, 0.f, 0.f};
	bool IsCalculated_ = false;

public:
    Scene(const std::string &sceneName, float sceneWidth = -1.f, float sceneHeight = -1.f, float sceneX = 0.f, float sceneY = 0.f, std::shared_ptr<FlexibleScaler> parentScaler = nullptr, std::shared_ptr<Scene> prevScene = nullptr);
    Scene(const std::string &sceneName, const ScreenData& screen, std::shared_ptr<FlexibleScaler> parentScaler = nullptr, std::shared_ptr<Scene> prevScene = nullptr);
    ~Scene();

    void Update(float deltaTime) override;

    virtual void SceneUpdate(float deltaTime) {};
    virtual void Draw() {};

	void ReCalculateScreen();
    bool RefreshDrawBuffer();

    std::shared_ptr<FlexibleScaler> GetDefaultScaler();

	void SetScreen(ScreenData screen);
	void SetPositionX(float px);
	void SetPositionY(float py);
	void SetScreenWidth(float width);
	void SetScreenHeight(float height);
	float GetPositionX() const;
	float GetPositionY() const;
	float GetScreenWidth() const;
	float GetScreenHeight() const;

	float GetRawPositionX() const;
	float GetRawPositionY() const;
	float GetRawScreenWidth() const;
	float GetRawScreenHeight() const;

    void SetVisible(bool visible);
    void SetTransparent(float transparent);
    float GetTransparent() const;

    bool IsVisible() const;
    bool IsChangedScreen() const;
    bool IsOnMouse() const;
    bool IsDownMouse() const;
    bool IsHoldMouse() const;
    bool IsClickedMouse() const;
};