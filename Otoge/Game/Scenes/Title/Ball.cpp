#include "Ball.hpp"

Ball::Ball(float x, float y, float ballSize, std::shared_ptr<FlexibleScaler> parentScaler): DrawableTask("Ball", x, y, 0.f, parentScaler)
{
	//isAutoScaling = false;
	Size_ = ballSize;
	SetTransparent(0.0f);
	hasLifespan = true;
	lifespan = 2.5f;
	moveVec = VGet(static_cast<float>((-100 + GetRand(200))), static_cast<float>((-100 + GetRand(200))), 0.f);
}

Ball::~Ball()
{
}

void Ball::PreUpdate(float deltaTime)
{
	position.x += moveVec.x * deltaTime;
	position.y += moveVec.y * deltaTime;

	bool isChanged = false;

	if (timerCount < 0.5f)
	{
		SetTransparent(GetTransparent() + (200.f * deltaTime));
	}
	if (timerCount > 1.f)
	{
		SetTickSpeed(2.0f);
		Size_ += Size_ * 6.0f * deltaTime;
		SetTransparent(GetTransparent() - (120.f * deltaTime));
		isChanged = true;
	}
}

void Ball::Draw()
{
	ScreenData d;
	d.posX = position.x;
	d.posY = position.y;
	d.width = Size_;
	d.height = Size_;
	ScreenData fixed = ParentScaler_->Calculate(&d);
	DrawCircle(fixed.posX, fixed.posY, fixed.width, GetColor(120, 120, 255), TRUE);
}
