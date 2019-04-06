#pragma once
#include "../../Util/Calculate/ScreenData.h"
#include "../../Util/Calculate/Screen/FlexibleScaler.hpp"
#include "Task.h"

class DrawableTask : public Task
{
private:
	static int TemporaryDrawBuffer_;
	static int BufferWidth_, BufferHeight_;

	bool IsEnabled3D_ = false;
	bool IsVisible_ = true;
	float Transparency_ = 100.f;

	// デバッグ
	static bool IsDrawPoint_;
	static float DrawPointSize_;
protected:
	std::shared_ptr<FlexibleScaler> ParentScaler_ = nullptr;
	//std::shared_ptr<FlexibleScaler> DefaultScaler_ = nullptr;

public:
	VECTOR position;

	DrawableTask(const std::string& taskName, float x = 0.f, float y = 0.f, float z = 0.f, std::shared_ptr<FlexibleScaler> scaler = nullptr);
	~DrawableTask();

	void Update(float deltaTime) override;

	virtual void PreUpdate(float deltaTime) = 0;
	virtual void Draw() = 0;

	void Enable3D();
	void Disable3D();
	void SetVisible(bool visible);
	void SetTransparent(float transparent);
	float GetTransparent();

	bool IsVisible() const;
	bool IsEnable3D() const;
};
