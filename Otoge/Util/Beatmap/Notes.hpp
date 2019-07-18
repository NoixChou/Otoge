#pragma once
#include "../../System/Task/DrawableTask.hpp"

class Notes
{
private:

public:
    enum NoteType
    {
        none,
        changeBPM,
        simple,
        hold,
    };

    int ID_;
    int TimingCount_;
    NoteType Type_;
    int Position_;
    int SpriteImageHandle_;
    float BPM_;
    bool IsProcessed_ = false;

    enum HitsType
    {
        fail,
        displace,
        just,
        mistake,
    };

    Notes();
    ~Notes();

    //virtual bool IsOperate() {};
    bool IsPast(float currentPlayTime);

    HitsType Judgment(float currentPlayTime);
    void Draw(const ScreenData& notesBox);
};
