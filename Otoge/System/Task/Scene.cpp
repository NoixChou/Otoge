#include "Scene.hpp"
#include "../Config.h"
#include "../../Util/Setting/SettingManager.h"
#include "../Input/MouseManager.hpp"
#include "../../Util/Calculate/Screen/FontStringCalculator.hpp"
#include "TaskManager.hpp"


Scene::Scene(const std::string& sceneName, float sceneWidth, float sceneHeight, float sceneX, float sceneY, std::shared_ptr<FlexibleScaler> parentScaler, Task::TaskPointer parentTask) : Task(sceneName)
{
    isAutoUpdateChildren = false;
    //if (sceneWidth == -1.f) sceneWidth = static_cast<float>(SettingManager::GetGlobal()->Get<int>(SETTINGS_RES_WIDTH).get());
    //if (sceneHeight == -1.f) sceneHeight = static_cast<float>(SettingManager::GetGlobal()->Get<int>(SETTINGS_RES_HEIGHT).get());

    // parentScalerがnullの場合 ウィンドウベースのスケーラをセット
    if (parentScaler == nullptr)
    {
        const auto parentScene = std::static_pointer_cast<Scene>(parentTask);

        if (parentScene != nullptr)
        {
            parentScaler = parentScene->GetDefaultScaler();
        }
        else
        {
            parentScaler = FlexibleScaler::GetWindowBasedInstance();
        }
    }

    PreLayoutScreen_.width = sceneWidth;
    PreLayoutScreen_.height = sceneHeight;
    PreLayoutScreen_.posX = sceneX;
    PreLayoutScreen_.posY = sceneY;

    ParentScaler_ = parentScaler;

    this->parentTask = parentTask;

    Screen_ = ParentScaler_->Calculate(&PreLayoutScreen_);

	ReCalculateScreen();

    IsDrawFrame_ = SettingManager::GetGlobal()->Get<bool>(SETTINGS_DEBUG_DRAW_SCENE_FRAME).get();

    Logger_->Info(GetName() + " 初期化完了");

    StartFadeIn();
}

Scene::Scene(const std::string& sceneName, const ScreenData& screen, std::shared_ptr<FlexibleScaler> parentScaler, Task::TaskPointer parentTask)
    : Scene(sceneName, screen.width, screen.height, screen.posX, screen.posY, parentScaler, parentTask) { }

Scene::~Scene()
{
    DeleteGraph(SceneBuffer_);
    Logger_->Debug("シーンバッファ開放");
}

void Scene::Update(float deltaTime)
{
    // シーンサイズに変更があるか
    if (PrevScreen_.posX != Screen_.posX) HasChangedScreen_ = true;
    else if (PrevScreen_.posY != Screen_.posY) HasChangedScreen_ = true;
    else if (PrevScreen_.width != Screen_.width) HasChangedScreen_ = true;
    else if (PrevScreen_.height != Screen_.height) HasChangedScreen_ = true;
    else HasChangedScreen_ = false;

    // スケーラサイズに変更があるか
    if (CurrentParentWidth_ != ParentScaler_->GetScreenWidth()) HasChangedScreen_ = true;
    if (CurrentParentHeight_ != ParentScaler_->GetScreenHeight()) HasChangedScreen_ = true;
    CurrentParentWidth_ = ParentScaler_->GetScreenWidth();
    CurrentParentHeight_ = ParentScaler_->GetScreenHeight();


    PrevScreen_ = Screen_;

    // シーンのフェード
    if (IsFadingIn_)
        SceneFadeIn(deltaTime);
    if (IsFadingOut_)
        SceneFadeOut(deltaTime);

    // シーンの更新
    SceneUpdate(deltaTime);

    // 変更がある場合 描画スクリーンの再計算
    if (HasChangedScreen_)
    {
		ReCalculateScreen();
    }

    // Visibleか、透明度が0%以上の場合 描画処理
    if (IsVisible())
    {
        // 現在の描画設定を保持
        int currentBuffer = GetDrawScreen();
        int currentBlendMode = DX_BLENDMODE_NOBLEND, currentBlendParam = 255;
        GetDrawBlendMode(&currentBlendMode, &currentBlendParam);

        // 描画
        SetDrawScreen(SceneBuffer_);
        SetDrawBlendMode(currentBlendMode, currentBlendParam);
        ClearDrawScreen();

        Draw();

        // 元の描画設定に戻す
        //SetDrawScreen(currentBuffer);
        SetDrawBlendMode(currentBlendMode, currentBlendParam);

        // 子タスクの更新処理
		TaskManager::UpdateTasks(children, childrenQueues, tickSpeed, deltaTime);

        // デバッグ情報の描画
        if (IsDrawFrame_)
        {
            SetDrawBlendMode(DX_BLENDMODE_PMA_ALPHA, 127);
            DrawFormatString(0, 0, GetColor(0, 0, 255), "+%.2f", DefaultScaler_->GetDiffX());
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
        }

        // 元の描画設定に戻す
        SetDrawScreen(currentBuffer);
        SetDrawBlendMode(currentBlendMode, currentBlendParam);

        // デバッグ用の枠を描画
        if (IsDrawFrame_)
        {
            DrawBox(static_cast<int>(floor(Screen_.posX)), static_cast<int>(floor(Screen_.posY)), static_cast<int>(floor(Screen_.posX + Screen_.width)), static_cast<int>(floor(Screen_.posY + Screen_.height)), GetColor(255, 0, 0), FALSE);
            DrawCircle(static_cast<int>(floor(DefaultScaler_->GetDiffX())), static_cast<int>(floor(DefaultScaler_->GetDiffY())), 3.f, GetColor(0, 255, 255));
        }

        // 不正バッファ防止
        if (1.f > Screen_.width) Screen_.width = 1.f;
        if (1.f > Screen_.height) Screen_.height = 1.f;

        // シーンバッファを描画(透明度も考慮)
		if (static_cast<int>(Transparency_) < 100)
			SetDrawBlendMode(DX_BLENDMODE_PMA_ALPHA, static_cast<int>((Transparency_ / 100.f) * 255.f));

        DrawRectGraph(static_cast<int>(floor(Screen_.posX)), static_cast<int>(floor(Screen_.posY)), 0, 0, static_cast<int>(floor(Screen_.posX + Screen_.width)), static_cast<int>(floor(Screen_.posY + Screen_.height)), SceneBuffer_, TRUE);

        // 元の描画設定に戻す
        SetDrawBlendMode(currentBlendMode, currentBlendParam);
    }
}

void Scene::ReCalculateScreen()
{
	bool doRefreshBuffer = false;

	if (!IsCalculated_)
	{
		//PreLayoutScreen_ = Screen_;
		IsCalculated_ = true;
		doRefreshBuffer = true;
	}
	if (ParentScaler_ != nullptr)
	{
		if (ParentScaler_->Calculate(&PreLayoutScreen_).width > Screen_.width) doRefreshBuffer = true;
		if (ParentScaler_->Calculate(&PreLayoutScreen_).height > Screen_.height) doRefreshBuffer = true;
		Screen_ = ParentScaler_->Calculate(&PreLayoutScreen_);
        if (PreLayoutScreen_.lockAspectRate)
        {
            Screen_.height = Screen_.width;
            PreLayoutScreen_.height = Screen_.height;
        }
	}

	if (DefaultScaler_ == nullptr)
	{
		DefaultScaler_ = std::make_shared<FlexibleScaler>(Screen_.width, Screen_.height, 1.0f);
        DefaultScaler_->SetDiffX(ParentScaler_->GetDiffX() + Screen_.posX);
        DefaultScaler_->SetDiffY(ParentScaler_->GetDiffY() + Screen_.posY);
		doRefreshBuffer = true;
	}
	else
	{
		DefaultScaler_->SetScreenWidth(Screen_.width);
		DefaultScaler_->SetScreenHeight(Screen_.height);
		DefaultScaler_->SetScale(1.0f);
        DefaultScaler_->SetDiffX(ParentScaler_->GetDiffX() + Screen_.posX);
        DefaultScaler_->SetDiffY(ParentScaler_->GetDiffY() + Screen_.posY);
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

    SceneBuffer_ = MakeScreen(static_cast<int>(floor(Screen_.width)), static_cast<int>(floor(Screen_.height)), TRUE);

    if (SceneBuffer_ == -1)
    {
        Logger_->Critical("シーンバッファ作成に失敗しました。");
        return false;
    }
    return true;
}

void Scene::StartFadeIn()
{
    IsFadingIn_ = true;
    IsFadingOut_ = false;
    timerCount = 0.f;
}

void Scene::StartFadeOut()
{
    IsFadingIn_ = false;
    IsFadingOut_ = true;
    timerCount = 0.f;
}

bool Scene::IsFadingIn()
{
    return IsFadingIn_;
}

bool Scene::IsFadingOut()
{
    return IsFadingOut_;
}

std::shared_ptr<FlexibleScaler> Scene::GetDefaultScaler() const
{
    return DefaultScaler_;
}

void Scene::SetScreen(ScreenData screen)
{
    Screen_ = screen;
	PreLayoutScreen_.posX = ParentScaler_->CalculatePositionRateX(screen.posX);
	PreLayoutScreen_.posY = ParentScaler_->CalculatePositionRateY(screen.posY);
	PreLayoutScreen_.width = ParentScaler_->CalculatePositionRateX(screen.width);
	PreLayoutScreen_.height = ParentScaler_->CalculatePositionRateY(screen.height);
}

void Scene::SetPositionX(float px)
{
    px = ParentScaler_->CalculatePositionX(px);
	SetScreen(ScreenData(px, Screen_.posY, Screen_.width, Screen_.height));
}

void Scene::SetPositionY(float py)
{
    py = ParentScaler_->CalculatePositionY(py);
	SetScreen(ScreenData(Screen_.posX, py, Screen_.width, Screen_.height));
}

void Scene::SetScreenWidth(float width)
{
    width = ParentScaler_->CalculateWidth(width);
	SetScreen(ScreenData(Screen_.posX, Screen_.posY, width, Screen_.height));
}

void Scene::SetScreenHeight(float height)
{
    height = ParentScaler_->CalculateHeight(height);
	SetScreen(ScreenData(Screen_.posX, Screen_.posY, Screen_.width, height));
}

void Scene::AddPositionX(float px)
{
    SetPositionX(GetPositionX() + px);
}

void Scene::AddPositionY(float py)
{
    SetPositionY(GetPositionY() + py);
}

void Scene::AddScreenWidth(float width)
{
    SetScreenWidth(GetScreenWidth() + width);
}

void Scene::AddScreenHeight(float height)
{
    SetScreenHeight(GetScreenHeight() + height);
}

float Scene::GetPositionX() const
{
    return ParentScaler_->CalculatePositionRateX(Screen_.posX);
}

float Scene::GetPositionY() const
{
    return ParentScaler_->CalculatePositionRateY(Screen_.posY);
}

float Scene::GetScreenWidth() const
{
    return ParentScaler_->CalculatePositionRateX(Screen_.width);
}

float Scene::GetScreenHeight() const
{
    return ParentScaler_->CalculatePositionRateY(Screen_.height);
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

void Scene::SetEnable(bool enable)
{
    IsEnable_ = enable;
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

bool Scene::IsEnable() const
{
    if (!parentTask.expired())
    {
        auto parent = std::dynamic_pointer_cast<Scene>(parentTask.lock());
        if (parent != nullptr)
        {
            if (!parent->IsEnable()) return false;
        }
    }

    return IsEnable_;
}

bool Scene::IsVisible() const
{
    
    if(!parentTask.expired())
    {
        auto parent = std::dynamic_pointer_cast<Scene>(parentTask.lock());
        if(parent != nullptr)
        {
            if (!parent->IsVisible()) return false;
        }
    }

    return IsVisible_ && (static_cast<int>(floor(Transparency_)) > 0);
}

bool Scene::IsChangedScreen() const
{
    return HasChangedScreen_;
}

bool Scene::IsOnMouse() const
{
    return
        (MouseManager::GetInstance()->GetMouseXf() > Screen_.posX + ParentScaler_->GetDiffX()) &&
        (MouseManager::GetInstance()->GetMouseXf() < Screen_.posX + Screen_.width + ParentScaler_->GetDiffX()) &&
        (MouseManager::GetInstance()->GetMouseYf() > Screen_.posY + ParentScaler_->GetDiffY()) &&
        (MouseManager::GetInstance()->GetMouseYf() < Screen_.posY + Screen_.height + ParentScaler_->GetDiffY()) &&
        IsEnable();
    /*return
        (MouseManager::GetInstance()->GetMouseRateX(ParentScaler_) > ParentScaler_->CalculatePositionRateX(Screen_.posX)) &&
        (MouseManager::GetInstance()->GetMouseRateX(ParentScaler_) < ParentScaler_->CalculatePositionRateX(Screen_.posX + Screen_.width)) &&
        (MouseManager::GetInstance()->GetMouseRateY(ParentScaler_) > ParentScaler_->CalculatePositionRateY(Screen_.posY)) &&
        (MouseManager::GetInstance()->GetMouseRateY(ParentScaler_) < ParentScaler_->CalculatePositionRateY(Screen_.posY + Screen_.height));
    */
}

bool Scene::IsStartOverMouse() const
{
    return false;
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
    bool onMouseAtDown = ((MouseManager::GetInstance()->GetDownPosXf() > Screen_.posX + ParentScaler_->GetDiffX()) && (MouseManager::GetInstance()->GetDownPosXf() < Screen_.posX + Screen_.width + ParentScaler_->GetDiffX()) &&
        (MouseManager::GetInstance()->GetDownPosYf() > Screen_.posY + ParentScaler_->GetDiffY()) && (MouseManager::GetInstance()->GetDownPosYf() < Screen_.posY + Screen_.height + ParentScaler_->GetDiffY()));

    return onMouseAtDown && IsOnMouse() && MouseManager::GetInstance()->IsReleaseButton(MOUSE_INPUT_LEFT) && IsRunning() && IsVisible();
}
