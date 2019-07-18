#pragma once
#include "AudioStream.h"
#include "../Debug/Logger.h"

class AudioManager
{
private:
    AudioManager();
    static std::shared_ptr<AudioManager> Instance_;

    std::shared_ptr<Logger> Logger_;

    std::map<std::string, int> SoundHandles_;
    std::map<std::string, AudioStream> AudioStreams_;

public:
    ~AudioManager();

    static inline const std::string STREAM_NAME_BGM = "music";
    static inline const std::string STREAM_NAME_SE = "se";
    static std::shared_ptr<AudioManager> GetInstance();
    static void CreateInstance();
    static void DestroyInstance();

    void AddAudioStream(const AudioStream& stream);
    void SetStreamVolume(const std::string& stream, int volume);
    void SetStreamPan(const std::string& stream, int pan);
    int GetStreamVolume(const std::string& stream);
    int GetStreamPan(const std::string& stream);
    void RegisterSound(const std::string& soundName, int soundHandle);
    int GetSoundHandle(const std::string& soundName);
    void PlayAudio(const std::string& soundName, const std::string& stream);
    void StopAudio(const std::string& soundName);
};
