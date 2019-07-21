#include "Notes.hpp"
#include "../Visual/Color.hpp"
#include "../../System/GlobalMethod.hpp"
#include "Timing.hpp"
#include "../../System/Task/TaskManager.hpp"

Notes::Notes()
{
}

Notes::~Notes()
{
}

bool Notes::IsPast(float currentPlayTime)
{
    return engine::CastToInt(currentPlayTime) >= TimingCount_;
}

Notes::HitsType Notes::Judgment(float currentPlayTime, float BPM)
{
    float rawDiff = currentPlayTime - engine::CastToFloat(TimingCount_);
    float diff = abs(rawDiff);
    //float diffSecond = timing::GetTimeByCount(diff, BPM);

    JudgeDiff_ = diff;

    if (diff <= JUST_RANGE)
    {
        return ReturnJudgeResult(HitsType::just);
    }
    if (diff <= GREAT_RANGE)
    {
        return ReturnJudgeResult(HitsType::great);
    }
    if (diff <= BAD_RANGE)
    {
        return ReturnJudgeResult(HitsType::bad);
    }

    if (diff > FAIL_RANGE) return HitsType::outside;

    return ReturnJudgeResult(HitsType::fail);
}

void Notes::Draw(const ScreenData& notesBox)
{
    if(Type_ == NoteType::changeBPM)
    {
        DrawLine(notesBox.posX, notesBox.posY, notesBox.posX + notesBox.width, notesBox.posY, color_preset::YELLOW);
        DrawFormatString(notesBox.posX + notesBox.width, notesBox.posY, color_preset::YELLOW, "%.2f BPM", BPM_);
    }
    if(Type_ == NoteType::simple)
    {
        if(Position_ == 1 || Position_ == 4)
            DrawBox(notesBox.posX, notesBox.posY, notesBox.posX + notesBox.width, notesBox.posY + notesBox.height, color_preset::BLUE, TRUE);
        else
            DrawBox(notesBox.posX, notesBox.posY, notesBox.posX + notesBox.width, notesBox.posY + notesBox.height, color_preset::WHITE, TRUE);
    }
}

Notes::HitsType Notes::ReturnJudgeResult(HitsType result)
{
    JudgeResult_ = result;
    return result;
}
