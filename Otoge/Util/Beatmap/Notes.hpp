#pragma once
#include "../../System/Task/DrawableTask.hpp"
#include "Timing.hpp"

class Notes
{
public:
    inline const static float JUST_RANGE = 150.f;
    inline const static float GREAT_RANGE = 400.f;
    inline const static float BAD_RANGE = 600.f;
    inline const static float FAIL_RANGE = 700.f;
    //inline const static float OUTSIDE_RANGE = 1.5f;

    enum NoteType
    {
        none,
        changeBPM,
        simple,
        hold,
        endMap,
        breakTime,
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

    int timingCount;
    int lengthCount;
    NoteType type;
    HitsType judgeResult = HitsType::outside;
    float judgeDiff = 0.f;
    int position;
    int spriteImageHandle;
    float BPM_;
    bool isProcessed = false;
    bool isDraw = true;

    Notes();
    ~Notes();

    //virtual bool IsOperate() {};
    bool IsPast(float currentPlayTime);

    HitsType Judgment(float currentPlayTime, float BPM);
    void Draw(const ScreenData& notesBox);

private:
    HitsType ReturnJudgeResult(HitsType result);
};
