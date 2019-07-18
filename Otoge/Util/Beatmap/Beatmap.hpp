#pragma once
#include "Notes.hpp"

class Beatmap
{
private:
    static std::shared_ptr<Logger> Logger_;
    static int GlobalUser_;

    boost::property_tree::ptree BeatmapFile_;
    std::string FileName_;
    std::string Title_;
    std::string ArtistName_;
    float Difficulty_;
    std::string Tag_;
    std::string SoundFile_;
    float SoundOffset_;
    bool IsPlaying_ = false;
    float CurrentTempoByBPM_;
    float PrevPlayCount_;
    float CurrentPlayCount_;

    std::vector<Notes*> Notes_;

public:
    Beatmap(const std::string& fileName);
    ~Beatmap();

    bool Create();
    bool Load();
    bool Save();

    bool LoadMapData();
    void Reset();

    std::string GetTitle();
    std::string GetArtist();
    float GetDifficulty();
    std::string GetTag();
    std::string GetSoundFileName();
    float GetSoundOffset();
    float GetCurrentTempoByBPM();
    float GetCurrentPlayCount();
    float GetPrevPlayCount();
    bool IsPlaying();

    void SetTitle(const std::string& title);
    void SetArtist(const std::string& artist);
    void SetDifficulty(float difficulty);
    void SetTag(const std::string& tag);
    void SetSoundFileName(const std::string& soundFileName);
    void SetSoundOffset(float offset);
    void SetCurrentTempoByBPM(float BPM);
    void SetCurrentPlayCount(float count);
    void SetPlayingStatus(bool isPlaying);

    void Update(float deltaTime);

    std::vector<Notes*>& GetMapNotes();
};
