#pragma once
#include "../../../System/Task/DrawableTask.hpp"

class Ball : public DrawableTask
{
private:
	VECTOR moveVec;
	float Size_;
public:
	Ball(float x, float y, float ballSize = 2.f, std::shared_ptr<FlexibleScaler> parentScaler = nullptr);
	~Ball();

	void PreUpdate(float deltaTime) override;
	//void SceneUpdate(float deltaTime) override;
	void Draw() override;
};
