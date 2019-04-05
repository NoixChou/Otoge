#include "MouseManager.hpp"

std::shared_ptr<MouseManager> MouseManager::Instance_ = nullptr;

MouseManager::MouseManager() : Task("MouseManager")
{
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

bool MouseManager::IsMovedMouse()
{
    return (PrevMousePosX_ != MousePosX_) || (PrevMousePosY_ != MousePosY_);
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
    return static_cast<float>(MousePosX_);
}

float MouseManager::GetMouseYf()
{
    return static_cast<float>(MousePosY_);
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
    return static_cast<float>(MouseDownPosX_);
}

float MouseManager::GetDownPosYf()
{
    return static_cast<float>(MouseDownPosY_);
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
    return static_cast<float>(MouseReleasePosX_);
}

float MouseManager::GetReleasePosYf()
{
    return static_cast<float>(MouseReleasePosY_);
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
