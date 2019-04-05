#include "Scene.hpp"
#include "../Config.h"
#include "../../Util/Setting/SettingManager.h"
#include "../Input/MouseManager.hpp"
#include "../../Util/Calculate/Screen/FontStringCalculator.hpp"


Scene::Scene(const std::string& sceneName, float sceneWidth, float sceneHeight, float sceneX, float sceneY, std::shared_ptr<FlexibleScaler> parentScaler, std::shared_ptr<Scene> prevScene) : Task(sceneName)
{
    isAutoUpdateChildren = false;
    if (sceneWidth == -1.f) sceneWidth = SettingManager::GetGlobal()->Get<int>(SETTINGS_RES_WIDTH).get(), isAutoScaling = false;
    if (sceneHeight == -1.f) sceneHeight = SettingManager::GetGlobal()->Get<int>(SETTINGS_RES_HEIGHT).get(), isAutoScaling = false;
    screen.width = sceneWidth;
    screen.height = sceneHeight;
    screen.posX = sceneX;
    screen.posY = sceneY;

    ParentScaler_ = parentScaler;
    DefaultScaler_ = std::make_shared<FlexibleScaler>(screen.width, screen.height, 1.0f);

    if (isAutoScaling)
    {
        PreLayoutScreen_ = screen;
        if (ParentScaler_ != nullptr)
        {
            Logger_->Info("PreLayouting: x" + std::to_string(screen.posX) + ", y" + std::to_string(screen.posY) + ", w" + std::to_string(screen.width) + ", h" + std::to_string(screen.height));
            screen = ParentScaler_->Calculate(&PreLayoutScreen_);
            Logger_->Info("PreLayouted: x" + std::to_string(screen.posX) + ", y" + std::to_string(screen.posY) + ", w" + std::to_string(screen.width) + ", h" + std::to_string(screen.height));
        }
    }

    SceneBuffer_ = -1;
    RefreshDrawBuffer();

    isDrawFrame = SettingManager::GetGlobal()->Get<bool>(SETTINGS_DEBUG_DRAW_SCENE_FRAME).get();
}

Scene::Scene(const std::string& sceneName, const ScreenData& screen, std::shared_ptr<FlexibleScaler> parentScaler, std::shared_ptr<Scene> prevScene) : Scene(sceneName, screen.width, screen.height, screen.posX, screen.posY, parentScaler, prevScene) { }

Scene::~Scene()
{
    DeleteGraph(SceneBuffer_);
    Logger_->Info("シーンバッファ開放");
}

void Scene::Update(float deltaTime)
{
    if (PrevScreen_.posX != screen.posX) HasChangedScreen_ = true;
    else if (PrevScreen_.posY != screen.posY) HasChangedScreen_ = true;
    else if (PrevScreen_.width != screen.width) HasChangedScreen_ = true;
    else if (PrevScreen_.height != screen.height) HasChangedScreen_ = true;
    else HasChangedScreen_ = false;

    /*
    if (ParentScaler_ != nullptr && isAutoScaling)
    {
        if (CurrentParentWidth_ != ParentScaler_->GetScreenWidth()) HasChangedScreen_ = true;
        if (CurrentParentHeight_ != ParentScaler_->GetScreenHeight()) HasChangedScreen_ = true;

        CurrentParentWidth_ = ParentScaler_->GetScreenWidth();
        CurrentParentHeight_ = ParentScaler_->GetScreenHeight();
    }*/

    PrevScreen_ = screen;

    SceneUpdate(deltaTime);

    if (HasChangedScreen_)
    {
        RefreshDrawBuffer();
        DefaultScaler_->SetScreenWidth(screen.width);
        DefaultScaler_->SetScreenHeight(screen.height);
        DefaultScaler_->SetScale(1.0f);
        /*
        if(ParentScaler_ != nullptr && isAutoScaling)
            screen = ParentScaler_->Calculate(&PreLayoutScreen_);*/
    }

    if (IsVisible_ && static_cast<int>(Transparency_) > 0)
    {
        int currentBuffer = GetDrawScreen();
        int currentBlendMode = DX_BLENDMODE_NOBLEND, currentBlendParam = 0;
        GetDrawBlendMode(&currentBlendMode, &currentBlendParam);

        SetDrawScreen(SceneBuffer_);
        SetDrawBlendMode(currentBlendMode, currentBlendParam);
        ClearDrawScreen();
        clsDx();

        Draw();
        ChildUpdate(deltaTime);

        if (isDrawFrame)
        {
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 127);
            VECTOR center = FontStringCalculator::GetStringCenterInBox(-1, TaskName_, screen);
            DrawString(center.x, center.y, TaskName_.c_str(), GetColor(255, 255, 255));
            SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
        }
        SetDrawScreen(currentBuffer);
        SetDrawBlendMode(currentBlendMode, currentBlendParam);

        if (isDrawFrame)
        {
            DrawBox(static_cast<int>(floor(screen.posX)), static_cast<int>(floor(screen.posY)), static_cast<int>(floor(screen.posX + screen.width)), static_cast<int>(floor(screen.posY + screen.height)), GetColor(255, 0, 0), FALSE);
        }
        int bufSX, bufSY;
        GetGraphSize(SceneBuffer_, &bufSX, &bufSY);
        if (bufSX < static_cast<int>(screen.width)) screen.width = static_cast<float>(bufSX);
        if (bufSY < static_cast<int>(screen.height)) screen.height = static_cast<float>(bufSY);
        if (0.f > screen.width) screen.width = 0.f;
        if (0.f > screen.height) screen.height = 0.f;

        currentBlendMode = DX_BLENDMODE_NOBLEND, currentBlendParam = 0;
        GetDrawBlendMode(&currentBlendMode, &currentBlendParam);
        if (static_cast<int>(Transparency_) < 100)
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, static_cast<int>((Transparency_ / 100.f) * 255.f));
        DrawExtendGraph(static_cast<int>(floor(screen.posX)), static_cast<int>(floor(screen.posY)), static_cast<int>(floor(screen.posX + screen.width)), static_cast<int>(floor(screen.posY + screen.height)), SceneBuffer_, TRUE);
        SetDrawBlendMode(currentBlendMode, currentBlendParam);
    }
}

bool Scene::RefreshDrawBuffer()
{
    if (SceneBuffer_ != -1)
    {
        DeleteGraph(SceneBuffer_);
        SceneBuffer_ = -1;
        Logger_->Info("シーンバッファを再生成");
    }

    SceneBuffer_ = MakeScreen(screen.width, screen.height, TRUE);

    if (SceneBuffer_ == -1)
    {
        Logger_->Critical("シーンバッファ作成に失敗しました。");
        return false;
    }
    return true;
}

void Scene::SetVisible(bool visible)
{
    IsVisible_ = visible;
}

void Scene::SetTransparent(float transparent)
{
    Transparency_ = transparent;
}

float Scene::GetTransparent()
{
    return Transparency_;
}

bool Scene::IsVisible() const
{
    return IsVisible_;
}

bool Scene::IsChangedScreen()
{
    return HasChangedScreen_;
}

bool Scene::IsOnMouse() const
{
    return (MouseManager::GetInstance()->GetMouseXf() > screen.posX) && (MouseManager::GetInstance()->GetMouseXf() < screen.posX + screen.width) &&
           (MouseManager::GetInstance()->GetMouseYf() > screen.posY) && (MouseManager::GetInstance()->GetMouseYf() < screen.posY + screen.height);
}

bool Scene::IsDownMouse() const
{
    return IsOnMouse() && MouseManager::GetInstance()->IsDownButton(MOUSE_INPUT_LEFT);
}

bool Scene::IsHoldMouse() const
{
    return IsOnMouse() && MouseManager::GetInstance()->IsHoldButton(MOUSE_INPUT_LEFT);
}

bool Scene::IsClickedMouse()
{
    bool onMouseAtDown = ((MouseManager::GetInstance()->GetDownPosXf() > screen.posX) && (MouseManager::GetInstance()->GetDownPosXf() < screen.posX + screen.width) &&
        (MouseManager::GetInstance()->GetDownPosYf() > screen.posY) && (MouseManager::GetInstance()->GetDownPosYf() < screen.posY + screen.height));

    return onMouseAtDown && IsOnMouse() && MouseManager::GetInstance()->IsReleaseButton(MOUSE_INPUT_LEFT);
}
