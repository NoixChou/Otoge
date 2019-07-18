#include "Notes.hpp"
#include "../Visual/Color.hpp"
#include "../../System/GlobalMethod.hpp"

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

Notes::HitsType Notes::Judgment(float currentPlayTime)
{
    return HitsType::fail;
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
