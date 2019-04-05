#include "DrawableTask.hpp"
#include "../Config.h"
#include "../../Util/Setting/SettingManager.h"
#include "../Input/MouseManager.hpp"
#include "../../Util/Calculate/Screen/FontStringCalculator.hpp"

int DrawableTask::TemporaryDrawBuffer_ = -1;
int DrawableTask::BufferWidth_ = -1;
int DrawableTask::BufferHeight_ = -1;
bool DrawableTask::isDrawFrame = false;

DrawableTask::DrawableTask(const std::string& sceneName, float x, float y, std::shared_ptr<FlexibleScaler> parentScaler) : Task(sceneName)
{
	if(TemporaryDrawBuffer_ == -1)
	{
		if(BufferWidth_ = -1)
			BufferWidth_ = SettingManager::GetGlobal()->Get<int>(SETTINGS_RES_WIDTH).get();
		if(BufferHeight_ = -1)
			BufferHeight_ = SettingManager::GetGlobal()->Get<int>(SETTINGS_RES_HEIGHT).get();

		isDrawFrame = SettingManager::GetGlobal()->Get<bool>(SETTINGS_DEBUG_DRAW_SCENE_FRAME).get();

		TemporaryDrawBuffer_ = MakeScreen(BufferWidth_, BufferHeight_, TRUE);
		Logger_->Info("一時描画バッファを生成しました。");
	}
	isAutoUpdateChildren = false;
	this->drawX = x;
	this->drawY = y;

	ParentScaler_ = parentScaler;

	if (ParentScaler_ == nullptr)
	{
		ParentScaler_ = FlexibleScaler::GetWindowBasedInstance();
	}
}

DrawableTask::~DrawableTask()
{

}

void DrawableTask::Update(float deltaTime)
{
	PreUpdate(deltaTime);

	if (IsVisible_ && static_cast<int>(Transparency_) > 0)
	{
		int currentBuffer = GetDrawScreen();
		int currentBlendMode = DX_BLENDMODE_NOBLEND, currentBlendParam = 0;
		GetDrawBlendMode(&currentBlendMode, &currentBlendParam);
		SetDrawScreen(TemporaryDrawBuffer_);
		ClearDrawScreen();
		/* 描画始め */

		Draw();
		ChildUpdate(deltaTime);

		/* 描画終わり */
		SetDrawScreen(currentBuffer);
		SetDrawBlendMode(currentBlendMode, currentBlendParam);

		if (static_cast<int>(Transparency_) < 100)
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>((Transparency_ / 100.f) * 255.f));
		DrawExtendGraph(0, 0, BufferWidth_, BufferHeight_, TemporaryDrawBuffer_, TRUE);

		SetDrawBlendMode(currentBlendMode, currentBlendParam);
	}
}

void DrawableTask::SetVisible(bool visible)
{
	IsVisible_ = visible;
}

void DrawableTask::SetTransparent(float transparent)
{
	Transparency_ = transparent;
}

float DrawableTask::GetTransparent()
{
	return Transparency_;
}

bool DrawableTask::IsVisible() const
{
	return IsVisible_;
}