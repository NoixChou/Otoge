#include "MouseManager.hpp"
#include "../GlobalMethod.hpp"
#include "../../Util/Setting/SettingManager.h"
#include "../Config.h"
#include "../../Util/Window/DxSettings.hpp"
#include "../Task/TaskManager.hpp"
std::shared_ptr<MouseManager> MouseManager::Instance_ = nullptr;

MouseManager::MouseManager() : Task("MouseManager")
{
    MouseAreaLimit_ = SettingManager::GetGlobal()->Get<bool>(game_config::SETTINGS_MOUSE_AREA_LIMIT).get();
    if(DxSettings::useOriginalCursor)
        HideCursor();

    Logger_->Info("マウス入力管理 初期化完了");
}

MouseManager::~MouseManager()
{
    //delete[] KeyInputs_;
    Logger_->Info("マウス入力管理 終了");
}

std::shared_ptr<MouseManager> MouseManager::GetInstance()
{
    return Instance_;
}

void MouseManager::CreateInstance()
{
    if (!Instance_)
    {
        Instance_.reset(new MouseManager);
    }
}

void MouseManager::DestroyInstance()
{
    Instance_.reset();
}


void MouseManager::Update(float deltaTime)
{
    PrevMousePosX_ = MousePosX_;
    PrevMousePosY_ = MousePosY_;
    PrevMouseInput_ = MouseInput_;

    GetMousePoint(&MousePosX_, &MousePosY_);
    MouseWheelSpeed_ = GetMouseWheelRotVolF();
    MouseInput_ = GetMouseInput();

    if (MouseAreaLimit_)
    {
        MousePosX_ = engine::LimitRange(MousePosX_, 0, DxSettings::windowWidth);
        MousePosY_ = engine::LimitRange(MousePosY_, 0, DxSettings::windowHeight);
    }
    
    if (MouseInput_ != 0 && PrevMouseInput_ == 0)
    {
        MouseDownPosX_ = MousePosX_;
        MouseDownPosY_ = MousePosY_;
    }else if(MouseInput_ == 0 && PrevMouseInput_ != 0)
    {
        MouseReleasePosX_ = MousePosX_;
        MouseReleasePosY_ = MousePosY_;
    }
}

void MouseManager::ShowCursor()
{
    SetMouseDispFlag(TRUE);
}

void MouseManager::HideCursor()
{
    SetMouseDispFlag(FALSE);
}

bool MouseManager::IsVisibleCursor() const
{
    return GetMouseDispFlag();
}

void MouseManager::UpdateMousePosition()
{
    SetMousePoint(MousePosX_, MousePosY_);
}

void MouseManager::SetMouseX(int x)
{
    MousePosX_ = x;
    UpdateMousePosition();
}

void MouseManager::SetMouseY(int y)
{
    MousePosY_ = y;
    UpdateMousePosition();
}

void MouseManager::SetMouseX(float x)
{
    SetMouseX(engine::CastToInt(x));
}

void MouseManager::SetMouseY(float y)
{
    SetMouseY(engine::CastToInt(y));
}

bool MouseManager::IsMovedMouse()
{
    auto currentTask = TaskManager::GetInstance()->GetCurrentProcessTask();
    if ((*currentTask) != nullptr && !(*currentTask)->IsEnable())
        return false;
    return (PrevMousePosX_ != MousePosX_) || (PrevMousePosY_ != MousePosY_);
}

int MouseManager::GetMouseXVel()
{
    return MousePosX_ - PrevMousePosX_;
}

int MouseManager::GetMouseYVel()
{
    return MousePosY_ - PrevMousePosY_;
}

float MouseManager::GetMouseXVelf()
{
    return engine::CastToFloat(GetMouseXVel());
}

float MouseManager::GetMouseYVelf()
{
    return engine::CastToFloat(GetMouseYVel());
}

int MouseManager::GetMouseX()
{
    return MousePosX_;
}

int MouseManager::GetMouseY()
{
    return MousePosY_;
}

float MouseManager::GetMouseXf()
{
    return engine::CastToFloat(MousePosX_);
}

float MouseManager::GetMouseYf()
{
    return engine::CastToFloat(MousePosY_);
}


int MouseManager::GetDownPosX()
{
    return MouseDownPosX_;
}

int MouseManager::GetDownPosY()
{
    return MouseDownPosY_;
}

float MouseManager::GetDownPosXf()
{
    return engine::CastToFloat(MouseDownPosX_);
}

float MouseManager::GetDownPosYf()
{
    return engine::CastToFloat(MouseDownPosY_);
}


int MouseManager::GetReleasePosX()
{
    return MouseReleasePosX_;
}

int MouseManager::GetReleasePosY()
{
    return MouseReleasePosY_;
}

float MouseManager::GetReleasePosXf()
{
    return engine::CastToFloat(MouseReleasePosX_);
}

float MouseManager::GetReleasePosYf()
{
    return engine::CastToFloat(MouseReleasePosY_);
}

float MouseManager::GetMouseRateX(std::shared_ptr<FlexibleScaler> scaler)
{
	return scaler->CalculatePositionRateX(engine::CastToFloat(MousePosX_));
}

float MouseManager::GetMouseRateY(std::shared_ptr<FlexibleScaler> scaler)
{
	return scaler->CalculatePositionRateY(engine::CastToFloat(MousePosY_));
}

float MouseManager::GetMouseWheelAccel()
{
    return MouseWheelSpeed_;
}

bool MouseManager::IsDownButton(int key)
{
    return (MouseInput_ & key) && !(PrevMouseInput_ & key);
}

bool MouseManager::IsHoldButton(int key)
{
    return (MouseInput_ & key) && (PrevMouseInput_ & key);
}

bool MouseManager::IsReleaseButton(int key)
{
    return !(MouseInput_ & key) && (PrevMouseInput_ & key);
}
