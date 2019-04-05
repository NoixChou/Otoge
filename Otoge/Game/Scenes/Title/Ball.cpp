#include "Ball.hpp"

Ball::Ball(float x, float y, float ballSize, std::shared_ptr<FlexibleScaler> parentScaler): DrawableTask("Ball", x, y, parentScaler)
{
	//isAutoScaling = false;
	Size_ = ballSize;

	moveVec = VGet(static_cast<float>((-100 + GetRand(200))), static_cast<float>((-100 + GetRand(200))), 0.f);
}

Ball::~Ball()
{
}

void Ball::PreUpdate(float deltaTime)
{
	drawX += moveVec.x * deltaTime;
	drawY += moveVec.y * deltaTime;

	bool isChanged = false;

	if(timerCount > 1.f)
	{
		SetTickSpeed(2.0f);
		Size_ += Size_ * 6.0f * deltaTime;
		SetTransparent(GetTransparent() - (120.f * deltaTime));
		isChanged = true;
	}
	if(timerCount > 2.5f)
	{
		Terminate();
	}

	if (isChanged)
	{
		//RefreshDrawBuffer();
	}
}

void Ball::Draw()
{
	ScreenData d;
	d.posX = drawX;
	d.posY = drawY;
	d.width = Size_;
	d.height = Size_;
	ScreenData fixed = ParentScaler_->Calculate(&d);
	DrawCircle(fixed.posX, fixed.posY, fixed.width, GetColor(120, 120, 255), TRUE);
}
