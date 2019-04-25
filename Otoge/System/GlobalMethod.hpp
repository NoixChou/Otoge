#pragma once

namespace engine
{
    template<typename T>
    inline float CastToFloat(T value)
    {
        return static_cast<float>(value);
    }

    template<typename T>
    inline int CastToInt(T value)
    {
        return static_cast<int>(floor(value));
    }
}