#pragma once
#include "../../System/Task/Task.h"
#include "../../Util/Calculate/ScreenData.h"
#include "../../Util/Calculate/Screen/FlexibleScaler.hpp"

class Scene : public Task
{
private:
    int SceneBuffer_ = -1;
    ScreenData PrevScreen_;
    ScreenData Screen_;
    ScreenData PreLayoutScreen_ = { 0.f, 0.f, 0.f, 0.f };

    bool HasChangedScreen_ = false;

    bool IsEnable_ = true;
    bool IsVisible_ = true;
    float Transparency_ = 100.f;
    // デバッグ
    bool IsDrawFrame_;

protected:
    bool IsFadingIn_ = false;
    bool IsFadingOut_ = false;

    float CurrentParentWidth_ = 0.f, CurrentParentHeight_ = 0.f;
    std::shared_ptr<FlexibleScaler> ParentScaler_ = nullptr;
    std::shared_ptr<FlexibleScaler> DefaultScaler_ = nullptr;
	bool IsCalculated_ = false;

public:
    Scene(const std::string &sceneName, float sceneWidth = 100.f, float sceneHeight = 100.f, float sceneX = 0.f, float sceneY = 0.f, std::shared_ptr<FlexibleScaler> parentScaler = nullptr, Task::TaskPointer parentTask = nullptr);
    Scene(const std::string &sceneName, const ScreenData& screen, std::shared_ptr<FlexibleScaler> parentScaler = nullptr, Task::TaskPointer parentTask = nullptr);
    ~Scene();

    void Update(float deltaTime) override;
    void ReCalculateScreen();
    bool RefreshDrawBuffer();

    virtual void SceneFadeIn(float deltaTime) {};
    virtual void SceneFadeOut(float deltaTime) {};
    virtual void SceneUpdate(float deltaTime) {};
    virtual void Draw() {};

    void StartFadeIn();
    void StartFadeOut();
    void StopFade();
    bool IsFadingIn();
    bool IsFadingOut();

    std::shared_ptr<FlexibleScaler> GetDefaultScaler() const;
	void SetScreen(ScreenData screen);
	void SetPositionX(float px);
	void SetPositionY(float py);
	void SetScreenWidth(float width);
	void SetScreenHeight(float height);
    void AddPositionX(float px);
    void AddPositionY(float py);
    void AddScreenWidth(float width);
    void AddScreenHeight(float height);

	float GetPositionX() const;
	float GetPositionY() const;
	float GetScreenWidth() const;
	float GetScreenHeight() const;

    float GetRawPositionX() const;
    float GetRawPositionY() const;
    float GetRawScreenWidth() const;
    float GetRawScreenHeight() const;

    void SetEnable(bool enable);
    void SetVisible(bool visible);
    void SetTransparent(float transparent);
    float GetTransparent() const;

    bool IsEnable() const;
    bool IsVisible() const;
    bool IsChangedScreen() const;
    bool IsOnMouse() const;
    bool IsStartOverMouse() const;
    bool IsDownMouse() const;
    bool IsHoldMouse() const;
    bool IsClickedMouse() const;
};