#pragma once

struct ScreenData
{
    ScreenData() = default;
    ScreenData(float px, float py, float w, float h, bool lock = false)
    {
        posX = px;
        posY = py;
        width = w;
        height = h;
        lockAspectRate = lock;
    }

    float posX = 0.f;
    float posY = 0.f;
    float width = 0.f;
    float height = 0.f;
    bool lockAspectRate = false;
};
