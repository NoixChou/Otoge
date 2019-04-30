#include "DrawableTask.hpp"
#include "../Config.h"
#include "../../Util/Setting/SettingManager.h"
#include "../Input/MouseManager.hpp"
#include "../../Util/Calculate/Screen/FontStringCalculator.hpp"
#include "../../Util/Visual/Color.hpp"
int DrawableTask::TemporaryDrawBuffer_ = -1;
int DrawableTask::BufferWidth_ = -1;
int DrawableTask::BufferHeight_ = -1;
bool DrawableTask::IsDrawPoint_ = false;
float DrawableTask::DrawPointSize_ = 1.5f;

DrawableTask::DrawableTask(const std::string& sceneName, float x, float y, float z, std::shared_ptr<FlexibleScaler> parentScaler) : Task(sceneName)
{
	if(TemporaryDrawBuffer_ == -1)
	{
		if(BufferWidth_ == -1)
			BufferWidth_ = SettingManager::GetGlobal()->Get<int>(game_config::SETTINGS_RES_WIDTH).get();
		if(BufferHeight_ == -1)
			BufferHeight_ = SettingManager::GetGlobal()->Get<int>(game_config::SETTINGS_RES_HEIGHT).get();

		IsDrawPoint_ = SettingManager::GetGlobal()->Get<bool>(game_config::SETTINGS_DEBUG_DRAW_DTASK_POINT).get();
		SettingManager::GetGlobal()->SetDefault<float>("system.debug.drawable.drawPointSize", DrawPointSize_);
		SettingManager::GetGlobal()->Save();

		DrawPointSize_ = SettingManager::GetGlobal()->Get<float>("system.debug.drawable.drawPointSize").get();
		TemporaryDrawBuffer_ = MakeScreen(BufferWidth_, BufferHeight_, TRUE);
		Logger_->Info("一時描画バッファを生成しました。");
	}
	isAutoUpdateChildren = false;
	this->position.x = x;
	this->position.y = y;
	this->position.z = z;

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
		int currentBlendMode = DX_BLENDMODE_NOBLEND, currentBlendParam = 255;
		GetDrawBlendMode(&currentBlendMode, &currentBlendParam);
		SetDrawScreen(TemporaryDrawBuffer_);
		ClearDrawScreen();
		/* 描画始め */

		Draw();
		ChildUpdate(deltaTime);

		// デバッグ
		if (IsDrawPoint_)
		{
			float x = ParentScaler_->CalculatePositionX(position.x);
			float y = ParentScaler_->CalculatePositionY(position.y);
			float rx = ParentScaler_->CalculateWidth(DrawPointSize_);
			float ry = ParentScaler_->CalculateHeight(DrawPointSize_);
			DrawOvalAA(x, y, rx, ry, 50, color_preset::LIGHT_GREEN, TRUE);
		}

		/* 描画終わり */
		SetDrawScreen(currentBuffer);
		SetDrawBlendMode(currentBlendMode, currentBlendParam);

		if (static_cast<int>(Transparency_) < 100)
			SetDrawBlendMode(DX_BLENDMODE_PMA_ALPHA, static_cast<int>((Transparency_ / 100.f) * 255.f));
		DrawExtendGraph(0, 0, BufferWidth_, BufferHeight_, TemporaryDrawBuffer_, TRUE);

		SetDrawBlendMode(currentBlendMode, currentBlendParam);
	}
}

void DrawableTask::Enable3D()
{
	IsEnabled3D_ = true;
}

void DrawableTask::Disable3D()
{
	IsEnabled3D_ = false;
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

bool DrawableTask::IsEnable3D() const
{
	return IsEnabled3D_;
}
