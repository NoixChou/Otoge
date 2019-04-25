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

    template<typename T>
    inline T LimitRange(T value, T minimum, T maximum)
    {
        return max(minimum, min(value, maximum));
    }
}