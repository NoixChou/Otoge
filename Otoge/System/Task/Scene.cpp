#include "Scene.hpp"
#include "../Config.h"
#include "../../Util/Setting/SettingManager.h"
#include "../Input/MouseManager.hpp"
#include "../../Util/Calculate/Screen/FontStringCalculator.hpp"
#include "TaskManager.hpp"


Scene::Scene(const std::string& sceneName, float sceneWidth, float sceneHeight, float sceneX, float sceneY, std::shared_ptr<FlexibleScaler> parentScaler, std::shared_ptr<Scene> prevScene) : Task(sceneName)
{
    isAutoUpdateChildren = false;
    if (sceneWidth == -1.f) sceneWidth = static_cast<float>(SettingManager::GetGlobal()->Get<int>(SETTINGS_RES_WIDTH).get()), isAutoScaling = false;
    if (sceneHeight == -1.f) sceneHeight = static_cast<float>(SettingManager::GetGlobal()->Get<int>(SETTINGS_RES_HEIGHT).get()), isAutoScaling = false;
    Screen_.width = sceneWidth;
    Screen_.height = sceneHeight;
    Screen_.posX = sceneX;
    Screen_.posY = sceneY;

    ParentScaler_ = parentScaler;

	ReCalculateScreen();

    isDrawFrame = SettingManager::GetGlobal()->Get<bool>(SETTINGS_DEBUG_DRAW_SCENE_FRAME).get();
}

Scene::Scene(const std::string& sceneName, const ScreenData& screen, std::shared_ptr<FlexibleScaler> parentScaler, std::shared_ptr<Scene> prevScene) : Scene(sceneName, screen.width, screen.height, screen.posX, screen.posY, parentScaler, prevScene) { }

Scene::~Scene()
{
    DeleteGraph(SceneBuffer_);
    Logger_->Debug("シーンバッファ開放");
}

void Scene::Update(float deltaTime)
{
    if (PrevScreen_.posX != Screen_.posX) HasChangedScreen_ = true;
    else if (PrevScreen_.posY != Screen_.posY) HasChangedScreen_ = true;
    else if (PrevScreen_.width != Screen_.width) HasChangedScreen_ = true;
    else if (PrevScreen_.height != Screen_.height) HasChangedScreen_ = true;
    else HasChangedScreen_ = false;

    if (ParentScaler_ != nullptr && isAutoScaling)
    {
        if (CurrentParentWidth_ != ParentScaler_->GetScreenWidth()) HasChangedScreen_ = true;
        if (CurrentParentHeight_ != ParentScaler_->GetScreenHeight()) HasChangedScreen_ = true;

        CurrentParentWidth_ = ParentScaler_->GetScreenWidth();
        CurrentParentHeight_ = ParentScaler_->GetScreenHeight();
    }

    PrevScreen_ = Screen_;

    SceneUpdate(deltaTime);

    if (HasChangedScreen_)
    {
		ReCalculateScreen();
    }

    if (IsVisible_ && static_cast<int>(Transparency_) > 0)
    {
        int currentBuffer = GetDrawScreen();
        int currentBlendMode = DX_BLENDMODE_NOBLEND, currentBlendParam = 255;
        GetDrawBlendMode(&currentBlendMode, &currentBlendParam);

        SetDrawScreen(SceneBuffer_);
        SetDrawBlendMode(currentBlendMode, currentBlendParam);
        ClearDrawScreen();

        Draw();

		TaskManager::UpdateTasks(children, childrenQueues, tickSpeed, deltaTime);

        if (isDrawFrame)
        {
            SetDrawBlendMode(DX_BLENDMODE_PMA_ALPHA, 127);
            VECTOR center = FontStringCalculator::GetStringCenterInBox(-1, TaskName_, Screen_);
            DrawString(center.x, center.y, TaskName_.c_str(), GetColor(255, 255, 255));
            DrawFormatString(0, 0, GetColor(0, 0, 255), "+%.2f", DefaultScaler_->GetOffsetX());
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
        }
        SetDrawScreen(currentBuffer);
        SetDrawBlendMode(currentBlendMode, currentBlendParam);

        if (isDrawFrame)
        {
            DrawBox(static_cast<int>(floor(Screen_.posX)), static_cast<int>(floor(Screen_.posY)), static_cast<int>(floor(Screen_.posX + Screen_.width)), static_cast<int>(floor(Screen_.posY + Screen_.height)), GetColor(255, 0, 0), FALSE);
        }
        if (1.f > Screen_.width) Screen_.width = 1.f;
        if (1.f > Screen_.height) Screen_.height = 1.f;

        GetDrawBlendMode(&currentBlendMode, &currentBlendParam);
		if (static_cast<int>(Transparency_) < 100)
			SetDrawBlendMode(DX_BLENDMODE_PMA_ALPHA, static_cast<int>((Transparency_ / 100.f) * 255.f));
        DrawRectGraph(static_cast<int>(floor(Screen_.posX)), static_cast<int>(floor(Screen_.posY)), 0, 0, static_cast<int>(floor(Screen_.posX + Screen_.width)), static_cast<int>(floor(Screen_.posY + Screen_.height)), SceneBuffer_, TRUE);
        SetDrawBlendMode(currentBlendMode, currentBlendParam);
    }
}

void Scene::ReCalculateScreen()
{
	bool doRefreshBuffer = false;

	if (isAutoScaling)
	{
		if (!IsCalculated_)
		{
			PreLayoutScreen_ = Screen_;
			IsCalculated_ = true;
			doRefreshBuffer = true;
		}
		if (ParentScaler_ != nullptr)
		{
			if (ParentScaler_->Calculate(&PreLayoutScreen_).width > Screen_.width) doRefreshBuffer = true;
			if (ParentScaler_->Calculate(&PreLayoutScreen_).height > Screen_.height) doRefreshBuffer = true;
			Screen_ = ParentScaler_->Calculate(&PreLayoutScreen_);
		}
	}

	if (DefaultScaler_ == nullptr)
	{
		DefaultScaler_ = std::make_shared<FlexibleScaler>(Screen_.width, Screen_.height, 1.0f);
		doRefreshBuffer = true;
	}
	else
	{
		DefaultScaler_->SetScreenWidth(Screen_.width);
		DefaultScaler_->SetScreenHeight(Screen_.height);
		DefaultScaler_->SetScale(1.0f);
	}

	if(doRefreshBuffer)
		RefreshDrawBuffer();
}

bool Scene::RefreshDrawBuffer()
{
    if (SceneBuffer_ != -1)
    {
        DeleteGraph(SceneBuffer_);
        SceneBuffer_ = -1;
    }

    SceneBuffer_ = MakeScreen(Screen_.width, Screen_.height, TRUE);

    if (SceneBuffer_ == -1)
    {
        Logger_->Critical("シーンバッファ作成に失敗しました。");
        return false;
    }
    return true;
}

std::shared_ptr<FlexibleScaler> Scene::GetDefaultScaler()
{
    return DefaultScaler_;
}

void Scene::SetScreen(ScreenData screen)
{
    Screen_ = screen;
	if (isAutoScaling && ParentScaler_ != nullptr)
	{
		PreLayoutScreen_.posX = ParentScaler_->CalculatePositionRateX(screen.posX);
		PreLayoutScreen_.posY = ParentScaler_->CalculatePositionRateY(screen.posY);
		PreLayoutScreen_.width = ParentScaler_->CalculatePositionRateX(screen.width);
		PreLayoutScreen_.height = ParentScaler_->CalculatePositionRateY(screen.height);
	}

}

void Scene::SetPositionX(float px)
{
    if(isAutoScaling && ParentScaler_)
        px = ParentScaler_->CalculatePositionX(px);
	SetScreen(ScreenData(px, Screen_.posY, Screen_.width, Screen_.height));
}

void Scene::SetPositionY(float py)
{
    if (isAutoScaling && ParentScaler_)
        py = ParentScaler_->CalculatePositionY(py);
	SetScreen(ScreenData(Screen_.posX, py, Screen_.width, Screen_.height));
}

void Scene::SetScreenWidth(float width)
{
    if (isAutoScaling && ParentScaler_)
        width = ParentScaler_->CalculateWidth(width);
	SetScreen(ScreenData(Screen_.posX, Screen_.posY, width, Screen_.height));
}

void Scene::SetScreenHeight(float height)
{
    if (isAutoScaling && ParentScaler_)
        height = ParentScaler_->CalculateHeight(height);
	SetScreen(ScreenData(Screen_.posX, Screen_.posY, Screen_.width, height));
}


float Scene::GetPositionX() const
{
    if(isAutoScaling && ParentScaler_ != nullptr)
    	return ParentScaler_->CalculatePositionRateX(Screen_.posX);

	return GetRawPositionX();
}

float Scene::GetPositionY() const
{
	if (isAutoScaling && ParentScaler_ != nullptr)
		return ParentScaler_->CalculatePositionRateY(Screen_.posY);

	return GetRawPositionY();
}

float Scene::GetScreenWidth() const
{
	if (isAutoScaling && ParentScaler_ != nullptr)
		return ParentScaler_->CalculatePositionRateX(Screen_.width);

	return GetRawScreenWidth();
}

float Scene::GetScreenHeight() const
{
	if (isAutoScaling && ParentScaler_ != nullptr)
		return ParentScaler_->CalculatePositionRateY(Screen_.height);

	return GetRawScreenHeight();
}


float Scene::GetRawPositionX() const
{
	return Screen_.posX;
}

float Scene::GetRawPositionY() const
{
	return Screen_.posY;
}

float Scene::GetRawScreenWidth() const
{
	return Screen_.width;
}

float Scene::GetRawScreenHeight() const
{
	return Screen_.height;
}


void Scene::SetVisible(bool visible)
{
    IsVisible_ = visible;
}

void Scene::SetTransparent(float transparent)
{
    Transparency_ = transparent;
}

float Scene::GetTransparent() const
{
    return Transparency_;
}

bool Scene::IsVisible() const
{
    return IsVisible_;
}

bool Scene::IsChangedScreen() const
{
    return HasChangedScreen_;
}

bool Scene::IsOnMouse() const
{
    if (isAutoScaling && ParentScaler_ != nullptr)
    {
        return
            (MouseManager::GetInstance()->GetMouseRateX(ParentScaler_) > ParentScaler_->CalculatePositionRateX(Screen_.posX)) &&
            (MouseManager::GetInstance()->GetMouseRateX(ParentScaler_) < ParentScaler_->CalculatePositionRateX(Screen_.posX + Screen_.width)) &&
            (MouseManager::GetInstance()->GetMouseRateY(ParentScaler_) > ParentScaler_->CalculatePositionRateY(Screen_.posY)) &&
            (MouseManager::GetInstance()->GetMouseRateY(ParentScaler_) < ParentScaler_->CalculatePositionRateY(Screen_.posY + Screen_.height));
    }
    return (MouseManager::GetInstance()->GetMouseXf() > Screen_.posX) && (MouseManager::GetInstance()->GetMouseXf() < Screen_.posX + Screen_.width) &&
           (MouseManager::GetInstance()->GetMouseYf() > Screen_.posY) && (MouseManager::GetInstance()->GetMouseYf() < Screen_.posY + Screen_.height);
}

bool Scene::IsDownMouse() const
{
    return IsOnMouse() && MouseManager::GetInstance()->IsDownButton(MOUSE_INPUT_LEFT);
}

bool Scene::IsHoldMouse() const
{
    return IsOnMouse() && MouseManager::GetInstance()->IsHoldButton(MOUSE_INPUT_LEFT);
}

bool Scene::IsClickedMouse() const
{
    bool onMouseAtDown = ((MouseManager::GetInstance()->GetDownPosXf() > Screen_.posX) && (MouseManager::GetInstance()->GetDownPosXf() < Screen_.posX + Screen_.width) &&
        (MouseManager::GetInstance()->GetDownPosYf() > Screen_.posY) && (MouseManager::GetInstance()->GetDownPosYf() < Screen_.posY + Screen_.height));

    return onMouseAtDown && IsOnMouse() && MouseManager::GetInstance()->IsReleaseButton(MOUSE_INPUT_LEFT);
}
