#pragma once
#include "../../System/GlobalMethod.hpp"

namespace timing
{
    constexpr int BAR_COUNT = 9600;
    constexpr int HI_SPEED = 9600;

    // 拍から譜面カウントに変換
    inline float GetCountByBeat(int beats, int beatType)
    {
        return (engine::CastToFloat(BAR_COUNT) / beatType) * beats;
    }

    // 拍から小節に変換
    inline int GetBarByBeat(int beats, int beatType)
    {
        return engine::CastToInt(ceil(beats / beatType));
    }

    // 譜面カウントから拍に変換
    inline int GetBeatByCount(float count)
    {
        return engine::CastToInt(count / (BAR_COUNT / 4));
    }

    // 秒から譜面カウントに変換
    inline float GetCountByTime(float second, float BPM)
    {
        return second * (engine::CastToFloat(BAR_COUNT) / 4.f) * (BPM / 60.f);
    }

    // 譜面カウントから秒に変換
    inline float GetTimeByCount(float count, float BPM)
    {
        return (count / 2400.f) / (BPM / 60.f);
    }
}
