#pragma once
#include "../Task/Task.h"

class MouseManager : public Task
{
private:
    int MousePosX_ = 0;
    int MousePosY_ = 0;
    int PrevMousePosX_ = 0;
    int PrevMousePosY_ = 0;

    int MouseDownPosX_ = 0;
    int MouseDownPosY_ = 0;
    int MouseReleasePosX_ = 0;
    int MouseReleasePosY_ = 0;

    int MouseInput_ = 0;
    int PrevMouseInput_ = 0;
    float MouseWheelSpeed_ = 0.f;

    static std::shared_ptr<MouseManager> Instance_;
    MouseManager();

public:
    ~MouseManager();

    static std::shared_ptr<MouseManager> GetInstance();
    static void CreateInstance();
    static void DestroyInstance();

    void Update(float deltaTime) override;
    bool IsMovedMouse();
    int GetMouseX();
    int GetMouseY();
    float GetMouseXf();
    float GetMouseYf();

    int GetDownPosX();
    int GetDownPosY();
    float GetDownPosXf();
    float GetDownPosYf();

    int GetReleasePosX();
    int GetReleasePosY();
    float GetReleasePosXf();
    float GetReleasePosYf();

    bool IsDownButton(int key);
    bool IsHoldButton(int key);
    bool IsReleaseButton(int key);
};
