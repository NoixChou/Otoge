#include "Scene.hpp"
#include "../Config.h"
#include "../../Util/Setting/SettingManager.h"
#include "../Input/MouseManager.hpp"
#include "../../Util/Calculate/Screen/FontStringCalculator.hpp"
#include "TaskManager.hpp"
#include "../GlobalMethod.hpp"
#include "../../Util/Visual/Color.hpp"

Scene::Scene(const std::string& sceneName, float sceneWidth, float sceneHeight, float sceneX, float sceneY,
             std::shared_ptr<FlexibleScaler> parentScaler, TaskPointer parentTask) : Task(sceneName)
{
    isAutoUpdateChildren = false;
    //if (sceneWidth == -1.f) sceneWidth = engine::CastToFloat(SettingManager::GetGlobal()->Get<int>(SETTINGS_RES_WIDTH).get());
    //if (sceneHeight == -1.f) sceneHeight = engine::CastToFloat(SettingManager::GetGlobal()->Get<int>(SETTINGS_RES_HEIGHT).get());
    // parentScalerがnullの場合 ウィンドウベースのスケーラをセット
    if(parentScaler == nullptr)
    {
        const auto parentScene = std::static_pointer_cast<Scene>(parentTask);
        if(parentScene != nullptr)
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
    Screen_ = ParentScaler_->Calculate(PreLayoutScreen_);
    ReCalculateScreen();
    IsDrawFrame_ = SettingManager::GetGlobal()->Get<bool>(game_config::SETTINGS_DEBUG_DRAW_SCENE_FRAME).get();
    Logger_->Info(GetName() + " 初期化完了");
}

Scene::Scene(const std::string& sceneName, const ScreenData& screen, std::shared_ptr<FlexibleScaler> parentScaler,
             TaskPointer parentTask) : Scene(sceneName, screen.width, screen.height, screen.posX, screen.posY,
                                             parentScaler, parentTask)
{
}

Scene::~Scene()
{
    DeleteGraph(SceneBuffer_);
    Logger_->Debug("シーンバッファ開放");
}

void Scene::Update(float deltaTime)
{
    // シーンサイズに変更があるか
    if(PrevScreen_.width != Screen_.width) IsChangedSize_ = true;
    else if(PrevScreen_.height != Screen_.height) IsChangedSize_ = true;

    // シーン位置に変更があるか
    if(PrevScreen_.posX != Screen_.posX || PrevScreen_.posY != Screen_.posY) IsChangedPosition_ = true;

    // スケーラサイズに変更があるか
    if(CurrentParentWidth_ != ParentScaler_->GetScreenWidth()) IsChangedSize_ = true;
    if(CurrentParentHeight_ != ParentScaler_->GetScreenHeight()) IsChangedSize_ = true;
    if(CurrentWidth_ != DefaultScaler_->GetScreenWidth()) IsChangedSize_ = true;
    if(CurrentHeight_ != DefaultScaler_->GetScreenHeight()) IsChangedSize_ = true;
    CurrentParentWidth_ = ParentScaler_->GetScreenWidth();
    CurrentParentHeight_ = ParentScaler_->GetScreenHeight();
    CurrentWidth_ = DefaultScaler_->GetScreenWidth();
    CurrentHeight_ = DefaultScaler_->GetScreenHeight();
    PrevScreen_ = Screen_;

    // シーンのフェード
    if(IsFadingIn_) SceneFadeIn(deltaTime);
    if(IsFadingOut_) SceneFadeOut(deltaTime);

    // シーンの更新
    SceneUpdate(deltaTime);

    // 変更がある場合 描画スクリーンの再計算
    if(IsChangedSize_)
    {
        ReCalculateScreen();
        IsChangedSize_ = false;
    }
    // 変更がある場合 スケーラ再計算
    if(IsChangedPosition_)
    {
        RefreshScaler();
        IsChangedPosition_ = false;
    }

    PrevOnMouse_ = IsOnMouse_;
    IsOnMouse_ = (MouseManager::GetInstance()->GetMouseXf() > Screen_.posX + ParentScaler_->GetDiffX()) && (MouseManager::
        GetInstance()->GetMouseXf() < Screen_.posX + Screen_.width + ParentScaler_->GetDiffX()) && (MouseManager::
            GetInstance()->GetMouseYf() > Screen_.posY + ParentScaler_->GetDiffY()) && (MouseManager::GetInstance()->
                GetMouseYf() < Screen_.posY + Screen_.height + ParentScaler_->GetDiffY()) && IsEnable();

    // Visibleか、透明度が0%以上の場合 描画処理
    if(IsVisible())
    {
        // 現在の描画設定を保持
        int l_CurrentBuffer = GetDrawScreen();
        int l_CurrentBlendMode = DX_BLENDMODE_NOBLEND, l_CurrentBlendParam = 255;
        int l_CurrentDrawMode;

        GetDrawBlendMode(&l_CurrentBlendMode, &l_CurrentBlendParam);
        l_CurrentDrawMode = GetDrawMode();

        // 描画
        SetDrawScreen(SceneBuffer_);
        SetDrawMode(DX_DRAWMODE_NEAREST);
        SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

        if(IsUpdateBuffer_ || IsChangedSize_)
        {
            ClearDrawScreen();
            if(isCallSceneDrawer) Draw();
            if(DrawerFunction_ != nullptr) DrawerFunction_();
        }

        // 元の描画設定に戻す
        SetDrawBlendMode(l_CurrentBlendMode, l_CurrentBlendParam);

        // 子タスクの更新処理
        TaskManager::UpdateTasks(children, childrenQueues, TickSpeed_, deltaTime);

        // デバッグ情報の描画
        if(IsDrawFrame_ && IsOnMouse())
        {
            SetDrawBlendMode(AlphaBlendMode_, 127);
            DrawFormatString(0, 0, color_preset::LIGHT_BLUE, "+%.2f", DefaultScaler_->GetDiffX());
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
        }

        // 元の描画設定に戻す
        SetDrawScreen(l_CurrentBuffer);
        SetDrawMode(l_CurrentDrawMode);
        SetDrawBlendMode(l_CurrentBlendMode, l_CurrentBlendParam);

        // シーンバッファを描画(透明度も考慮)
        if(static_cast<int>(Transparency_) < 100) SetDrawBlendMode(AlphaBlendMode_,
                                                                   static_cast<int>((Transparency_ / 100.f) * 255.f));
        else SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);

        //  親よりはみ出た部分を描画しないように調整
        float l_DrawPosX = Screen_.posX, l_DrawPosY = Screen_.posY;
        float l_DrawWidth = l_DrawPosX + Screen_.width, l_DrawHeight = l_DrawPosY + Screen_.height;
        float l_DrawSrcX = 0.f, l_DrawSrcY = 0.f;
        if(Screen_.posX < 0.f)
        {
            l_DrawWidth -= Screen_.posX;
            l_DrawPosX = 0.f;
            l_DrawSrcX += -Screen_.posX;
        }
        if(Screen_.posY < 0.f)
        {
            l_DrawHeight -= Screen_.posY;
            l_DrawPosY = 0.f;
            l_DrawSrcY += -Screen_.posY;
        }
        if(l_DrawWidth > ParentScaler_->GetScreenWidth()) l_DrawWidth -= l_DrawWidth - ParentScaler_->GetScreenWidth();
        if(l_DrawHeight > ParentScaler_->GetScreenHeight()) l_DrawHeight -= l_DrawHeight - ParentScaler_->
            GetScreenHeight();
        DrawRectGraph(engine::CastToInt(l_DrawPosX), engine::CastToInt(l_DrawPosY), engine::CastToInt(l_DrawSrcX),
                      engine::CastToInt(l_DrawSrcY), engine::CastToInt(l_DrawWidth), engine::CastToInt(l_DrawHeight),
                      SceneBuffer_, TRUE);

        // 元の描画設定に戻す
        SetDrawBlendMode(l_CurrentBlendMode, l_CurrentBlendParam);

        // デバッグ用の枠を描画
        if(IsDrawFrame_)
        {
            DrawBox(engine::CastToInt(Screen_.posX), engine::CastToInt(Screen_.posY),
                    engine::CastToInt(Screen_.posX + Screen_.width), engine::CastToInt(Screen_.posY + Screen_.height),
                    color_preset::RED, FALSE);
            DrawCircle(engine::CastToInt(DefaultScaler_->GetDiffX()), engine::CastToInt(DefaultScaler_->GetDiffY()), 3,
                       color_preset::CYAN, TRUE);
        }
    }
}

void Scene::ReCalculateScreen()
{
    bool doRefreshBuffer = false;
    if(!IsCalculated_)
    {
        //PreLayoutScreen_ = Screen_;
        IsCalculated_ = true;
        doRefreshBuffer = true;
    }
    if(ParentScaler_->Calculate(PreLayoutScreen_).width != Screen_.width) doRefreshBuffer = true;
    if(ParentScaler_->Calculate(PreLayoutScreen_).height != Screen_.height) doRefreshBuffer = true;
    if(DefaultScaler_ == nullptr)
    {
        Screen_ = ParentScaler_->Calculate(PreLayoutScreen_);
        if(PreLayoutScreen_.lockAspectRate)
        {
            Screen_.height = Screen_.width;
            PreLayoutScreen_.height = Screen_.height;
        }
        DefaultScaler_ = std::make_shared<FlexibleScaler>(Screen_.width, Screen_.height, 1.0f);
        doRefreshBuffer = true;
    }
    RefreshScaler();
    if(doRefreshBuffer) RefreshDrawBuffer();
    RefreshChildren();
    OnReCalculateScreen();
}

bool Scene::RefreshScaler()
{
    Screen_ = ParentScaler_->Calculate(PreLayoutScreen_);
    DefaultScaler_->SetScreenWidth(Screen_.width);
    DefaultScaler_->SetScreenHeight(Screen_.height);
    DefaultScaler_->SetScale(1.0f);
    DefaultScaler_->SetDiffX(ParentScaler_->GetDiffX() + Screen_.posX);
    DefaultScaler_->SetDiffY(ParentScaler_->GetDiffY() + Screen_.posY);
    RefreshChildren();
    return true;
}

bool Scene::RefreshDrawBuffer()
{
    // 不正バッファ防止
    if(1.f > Screen_.width) Screen_.width = 1.f;
    if(1.f > Screen_.height) Screen_.height = 1.f;
    if(SceneBuffer_ != -1)
    {
        DeleteGraph(SceneBuffer_);
        SceneBuffer_ = -1;
    }
    SceneBuffer_ = MakeScreen(engine::CastToInt(Screen_.width), engine::CastToInt(Screen_.height), TRUE);
    if(SceneBuffer_ == -1)
    {
        Logger_->Critical("シーンバッファ作成に失敗しました。");
        return false;
    }
    RefreshChildren();
    return true;
}

void Scene::RefreshChildren()
{
    for(const auto& l_Child : children)
    {
        auto l_ChildScene = std::dynamic_pointer_cast<Scene>(l_Child);
        if(l_ChildScene)
        {
            l_ChildScene->ReCalculateScreen();
        }
    }
}

bool Scene::IsChangedScaler()
{
    return false;
}

void Scene::StartFadeIn()
{
    IsFadingIn_ = true;
    IsFadingOut_ = false;
    timerCount = 0.f;
    OnStartedFadeIn();
}

void Scene::StartFadeOut()
{
    IsFadingIn_ = false;
    IsFadingOut_ = true;
    timerCount = 0.f;
    OnStartedFadeOut();
}

void Scene::StopFade()
{
    if(IsFadingIn_) OnStoppedFadeIn();
    if(IsFadingOut_) OnStartedFadeOut();
    IsFadingIn_ = false;
    IsFadingOut_ = false;
}

bool Scene::IsFadingIn()
{
    return IsFadingIn_;
}

bool Scene::IsFadingOut()
{
    return IsFadingOut_;
}

void Scene::SetDrawFunction(DrawFunction func)
{
    DrawerFunction_ = func;
}

void Scene::ChangeDrawFunction(DrawFunction func)
{
    SetDrawFunction(func);
    isCallSceneDrawer = false;
}

int Scene::GetDrawBuffer()
{
    return SceneBuffer_;
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

void Scene::SetAlphaBlendMode(int blendMode)
{
    AlphaBlendMode_ = blendMode;
}

int Scene::GetAlphaBlendMode() const
{
    return AlphaBlendMode_;
}

bool Scene::IsVisible() const
{
    if(!parentTask.expired())
    {
        auto parent = std::dynamic_pointer_cast<Scene>(parentTask.lock());
        if(parent != nullptr)
        {
            if(!parent->IsVisible()) return false;
        }
    }
    return IsVisible_ && (engine::CastToInt(Transparency_) > 0);
}

bool Scene::IsChangedSize() const
{
    return IsChangedSize_;
}

bool Scene::IsChangedPosition() const
{
    return IsChangedPosition_;
}

bool Scene::IsOnMouse() const
{
    return IsOnMouse_;
    /*return
        (MouseManager::GetInstance()->GetMouseRateX(ParentScaler_) > ParentScaler_->CalculatePositionRateX(Screen_.posX)) &&
        (MouseManager::GetInstance()->GetMouseRateX(ParentScaler_) < ParentScaler_->CalculatePositionRateX(Screen_.posX + Screen_.width)) &&
        (MouseManager::GetInstance()->GetMouseRateY(ParentScaler_) > ParentScaler_->CalculatePositionRateY(Screen_.posY)) &&
        (MouseManager::GetInstance()->GetMouseRateY(ParentScaler_) < ParentScaler_->CalculatePositionRateY(Screen_.posY + Screen_.height));
    */
}

bool Scene::IsBeginOnMouse() const
{
    return IsOnMouse_ && !PrevOnMouse_;
}

bool Scene::IsEndOnMouse() const
{
    return !IsOnMouse_ && PrevOnMouse_;
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
    bool onMouseAtDown = ((MouseManager::GetInstance()->GetDownPosXf() > Screen_.posX + ParentScaler_->GetDiffX()) && (
        MouseManager::GetInstance()->GetDownPosXf() < Screen_.posX + Screen_.width + ParentScaler_->GetDiffX()) && (
        MouseManager::GetInstance()->GetDownPosYf() > Screen_.posY + ParentScaler_->GetDiffY()) && (MouseManager::
        GetInstance()->GetDownPosYf() < Screen_.posY + Screen_.height + ParentScaler_->GetDiffY()));
    return onMouseAtDown && IsOnMouse() && MouseManager::GetInstance()->IsReleaseButton(MOUSE_INPUT_LEFT) && IsRunning()
        && IsVisible();
}
