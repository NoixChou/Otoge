#pragma once
#include "../Task/Task.h"
#include "../../Util/Calculate/Screen/FlexibleScaler.hpp"

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

    bool MouseAreaLimit_;

    static std::shared_ptr<MouseManager> Instance_;
    MouseManager();

public:
    ~MouseManager();

    static std::shared_ptr<MouseManager> GetInstance();
    static void CreateInstance();
    static void DestroyInstance();

    void Update(float deltaTime) override;

    void ShowCursor();
    void HideCursor();
    bool IsVisibleCursor() const;

    void UpdateMousePosition();
    void SetMouseX(int x);
    void SetMouseY(int y);
    void SetMouseX(float x);
    void SetMouseY(float y);

    bool IsMovedMouse();
    int GetMouseXVel();
    int GetMouseYVel();
    float GetMouseXVelf();
    float GetMouseYVelf();

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

	float GetMouseRateX(std::shared_ptr<FlexibleScaler> scaler);
	float GetMouseRateY(std::shared_ptr<FlexibleScaler> scaler);

    float GetMouseWheelAccel();

    bool IsDownButton(int key);
    bool IsHoldButton(int key);
    bool IsReleaseButton(int key);
};
