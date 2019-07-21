#pragma once
#include "../../System/Task/DrawableTask.hpp"
#include "Timing.hpp"

class Notes
{
public:
    inline const static float JUST_RANGE = 100.f;
    inline const static float GREAT_RANGE = 300.f;
    inline const static float BAD_RANGE = 500.f;
    inline const static float FAIL_RANGE = 600.f;
    //inline const static float OUTSIDE_RANGE = 1.5f;

    enum NoteType
    {
        none,
        changeBPM,
        simple,
        hold,
        endMap,
    };

    enum HitsType
    {
        outside,
        fail,
        bad,
        great,
        just,
        mistake,
    };

    int TimingCount_;
    NoteType Type_;
    HitsType JudgeResult_ = HitsType::outside;
    float JudgeDiff_ = 0.f;
    int Position_;
    int SpriteImageHandle_;
    float BPM_;
    bool IsProcessed_ = false;
    bool IsDraw_ = true;

    Notes();
    ~Notes();

    //virtual bool IsOperate() {};
    bool IsPast(float currentPlayTime);

    HitsType Judgment(float currentPlayTime, float BPM);
    void Draw(const ScreenData& notesBox);

private:
    HitsType ReturnJudgeResult(HitsType result);
};
