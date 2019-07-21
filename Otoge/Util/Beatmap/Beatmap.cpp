#include "Beatmap.hpp"
#include "../Encoding/EncodingConverter.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/foreach.hpp>
#include "Timing.hpp"
#include "../../System/GlobalMethod.hpp"
#include <filesystem>
#include "../Audio/AudioManager.hpp"
std::shared_ptr<Logger> Beatmap::Logger_ = nullptr;
int Beatmap::GlobalUser_ = 0;

Beatmap::Beatmap(const std::string& fileName)
{
    if(!Logger_)
    {
        Logger_ = std::make_shared<Logger>("Beatmap");
    }
    GlobalUser_++;
    Logger_->Debug("Beatmap count: " + std::to_string(GlobalUser_));

    FileName_ = fileName;
}

Beatmap::~Beatmap()
{
    Reset();

    GlobalUser_--;
    Logger_->Debug("Beatmap count: " + std::to_string(GlobalUser_));
    if(GlobalUser_ <= 0)
    {
        Logger_.reset();
    }
}

bool Beatmap::Create()
{
    Logger_->Info("ビートマップ生成: " + FileName_);

    std::wofstream f(FileName_);
    f.imbue(std::locale(std::locale(), new std::codecvt_utf8<wchar_t>));
    f.write(encoding::ConvertUtf8ToUtf16("{}").c_str(), 2);
    f.close();

    SetTitle(Title_);
    SetArtist(ArtistName_);
    SetTag(Tag_);
    SetSoundFileName(SoundFile_);

    Logger_->Info("生成完了。" + FileName_);
    return true;
}

bool Beatmap::Load()
{
    Logger_->Info("楽曲データ" + FileName_ + "読み込み中...");
    try
    {
        read_json(FileName_, BeatmapFile_);

        Title_ = BeatmapFile_.get_optional<std::string>("meta.title").get();
        ArtistName_ = BeatmapFile_.get_optional<std::string>("meta.artist").get();
        Difficulty_ = BeatmapFile_.get_optional<float>("meta.difficulty").get();
        Tag_ = BeatmapFile_.get_optional<std::string>("meta.tag").get();
        SoundFile_ = BeatmapFile_.get_optional<std::string>("music.soundFile").get();
        SoundOffset_ = BeatmapFile_.get_optional<float>("music.soundOffset").get();
        Logger_->Info("完了。" + Title_);

        return true;
    }
    catch(boost::property_tree::json_parser::json_parser_error e)
    {
        Logger_->Error("楽曲データ" + FileName_ + "の読み込みに失敗。\n詳細: " + e.what());
        return false;
    }
}

bool Beatmap::Save()
{
    Logger_->Info("楽曲データ" + FileName_ + "保存中...");
    try
    {
        write_json(FileName_, BeatmapFile_);
        Logger_->Info("楽曲データ保存完了。");
        return true;
    }
    catch (boost::property_tree::json_parser::json_parser_error e)
    {
        Logger_->Error("楽曲データ" + FileName_ + "の保存に失敗。\n詳細: " + e.what());
        return false;
    }
}

bool Beatmap::LoadMapData()
{
    Logger_->Info("譜面データ" + FileName_ + "(" + Title_ + ") 解析中...");

    BOOST_FOREACH(const boost::property_tree::ptree::value_type& note, BeatmapFile_.get_child("music.notes")) {
        const boost::property_tree::ptree& info = note.second;

        Notes* noteObj = new Notes();

        // ノートタイミング
        if (boost::optional<int> time = info.get_optional<int>("time")) {
            noteObj->TimingCount_ = time.get();
        }
        else {
            Logger_->Warn("ノートタイミングがセットされていません。");
        }

        // ノートタイプ
        if (boost::optional<int> type = info.get_optional<int>("type")) {
            noteObj->Type_ = static_cast<Notes::NoteType>(type.get());
        }
        else {
            Logger_->Warn("ノート種類がセットされていません。");
        }

        // ノート位置
        if (boost::optional<int> position = info.get_optional<int>("position")) {
            noteObj->Position_ = static_cast<int>(position.get());
        }
        else {
            Logger_->Warn("ノート位置がセットされていません。");
        }

        // BPM
        if (boost::optional<float> bpm = info.get_optional<float>("bpm")) {
            noteObj->BPM_ = bpm.get();
        }
        else {
            noteObj->BPM_ = 0;
        }

        Notes_.push_back(noteObj);
        Logger_->Debug("ノート { time: " + std::to_string(noteObj->TimingCount_) + ", type: " + std::to_string(noteObj->Type_) + " }");
    }

    Logger_->Info("音楽データ" + SoundFile_ + " 読み込み中...");
    AudioManager::GetInstance()->RegisterSound("map_" + Title_, LoadSoundMem(GetSoundFileName().c_str()));

    Logger_->Info("譜面データ" + FileName_ + "(" + Title_ + ") 解析完了");

    return true;
}

void Beatmap::Reset()
{
    IsPlaying_ = false;
    CurrentTempoByBPM_ = 0.f;
    CurrentPlayCount_ = 0.f;

    for (int i = 0; i < Notes_.size(); i++)
    {
        if (Notes_[i])
        {
            delete Notes_[i];
            Notes_[i] = nullptr;
        }
    }
    Notes_.clear();

    Logger_->Info("読み込み済み譜面データ初期化");
}

std::string Beatmap::GetTitle()
{
    return Title_;
}

std::string Beatmap::GetArtist()
{
    return ArtistName_;
}

float Beatmap::GetDifficulty()
{
    return Difficulty_;
}

std::string Beatmap::GetTag()
{
    return Tag_;
}

std::string Beatmap::GetSoundFileName()
{
    std::filesystem::path p = FileName_;
    return p.parent_path().generic_string() + "/" + SoundFile_;
}

float Beatmap::GetSoundOffset()
{
    return SoundOffset_;
}

float Beatmap::GetCurrentTempoByBPM()
{
    return CurrentTempoByBPM_;
}

float Beatmap::GetCurrentPlayCount()
{
    return CurrentPlayCount_;
}

float Beatmap::GetPrevPlayCount()
{
    return PrevPlayCount_;
}

bool Beatmap::IsPlaying()
{
    return IsPlaying_;
}

void Beatmap::SetTitle(const std::string& title)
{
    Title_ = title;
    BeatmapFile_.put("meta.title", Title_);
}

void Beatmap::SetArtist(const std::string& artist)
{
    ArtistName_ = artist;
    BeatmapFile_.put("meta.artist", ArtistName_);
}

void Beatmap::SetDifficulty(float difficulty)
{
    Difficulty_ = difficulty;
    BeatmapFile_.put("meta.difficulty", Difficulty_);
}

void Beatmap::SetTag(const std::string& tag)
{
    Tag_ = tag;
    BeatmapFile_.put("meta.tag", Tag_);
}

void Beatmap::SetSoundFileName(const std::string& soundFileName)
{
    SoundFile_ = soundFileName;
    BeatmapFile_.put("music.soundFile", SoundFile_);
}

void Beatmap::SetSoundOffset(float offset)
{
    SoundOffset_ = offset;
    BeatmapFile_.put("music.soundOffset", SoundOffset_);
}

void Beatmap::SetCurrentTempoByBPM(float BPM)
{
    CurrentTempoByBPM_ = BPM;
}

void Beatmap::SetCurrentPlayCount(float count)
{
    CurrentPlayCount_ = count;
}

void Beatmap::SetPlayingStatus(bool isPlaying)
{
    IsPlaying_ = isPlaying;
}

void Beatmap::Update(float deltaTime)
{
    PrevPlayCount_ = CurrentPlayCount_;
    //CurrentPlayCount_ = (engine::CastToFloat(GetSoundCurrentTime(AudioManager::GetInstance()->GetSoundHandle("music"))) / 1000.f) * (timing::BAR_COUNT / 4) * (CurrentTempoByBPM_ / 60.f);
    CurrentPlayCount_ += deltaTime * (engine::CastToFloat(timing::BAR_COUNT) / 4.f) * (CurrentTempoByBPM_ / 60.f);
}

std::vector<Notes*>& Beatmap::GetMapNotes()
{
    return Notes_;
}

int Beatmap::GetMaxCombo()
{
    int count = 0;
    for(Notes* note : Notes_)
    {
        if(note->Type_ == Notes::NoteType::simple)
        {
            count++;
        }
    }
    return count;
}

int Beatmap::GetLastComboCount()
{
    float l_PrevTiming = 0.f;
    for (Notes* note : Notes_)
    {
        if(note->Type_ == Notes::NoteType::simple)
        {
            l_PrevTiming = note->TimingCount_;
        }
        if (note->Type_ == Notes::NoteType::endMap)
        {
            return l_PrevTiming;
        }
    }
    return l_PrevTiming;
}
