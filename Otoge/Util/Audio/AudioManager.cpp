#include "AudioManager.hpp"
#include "../Setting/SettingManager.h"
#include "../../System/Config.h"

std::shared_ptr<AudioManager> AudioManager::Instance_ = nullptr;

AudioManager::AudioManager()
{
    Logger_ = std::make_shared<Logger>("AudioManager");

    AddAudioStream(AudioStream{ STREAM_NAME_BGM, SettingManager::GetGlobal()->Get<int>(game_config::SETTINGS_AUDIO_MUSIC_VOLUME).get(), 0 });
    AddAudioStream(AudioStream{ STREAM_NAME_SE, SettingManager::GetGlobal()->Get<int>(game_config::SETTINGS_AUDIO_SE_VOLUME).get(), 0 });

    Logger_->Info("オーディオ機能 初期化完了");
}

AudioManager::~AudioManager()
{
    for(const auto& [soundName, handle] : SoundHandles_)
    {
        DeleteSoundMem(handle);
    }

    SoundHandles_.clear();
}

std::shared_ptr<AudioManager> AudioManager::GetInstance()
{
    return Instance_;
}

void AudioManager::CreateInstance()
{
    if (!Instance_)
    {
        Instance_.reset(new AudioManager);
    }
}

void AudioManager::DestroyInstance()
{
    Instance_.reset();
}

void AudioManager::AddAudioStream(const AudioStream& stream)
{
    AudioStreams_[stream.name] = stream;
    Logger_->Debug("オーディオストリーム追加: " + stream.name);
}

void AudioManager::SetStreamVolume(const std::string& stream, int volume)
{
    AudioStreams_[stream].volume = volume;
}

void AudioManager::SetStreamPan(const std::string& stream, int pan)
{
    AudioStreams_[stream].pan = pan;
}

int AudioManager::GetStreamVolume(const std::string& stream)
{
    return AudioStreams_[stream].volume;
}

int AudioManager::GetStreamPan(const std::string& stream)
{
    return AudioStreams_[stream].pan;
}

void AudioManager::RegisterSound(const std::string& soundName, int soundHandle)
{
    if (soundName.empty()) return;

    DeleteSoundMem(SoundHandles_[soundName]);
    SoundHandles_[soundName] = soundHandle;
    Logger_->Debug("サウンド登録: " + soundName + " (ハンドル: " + std::to_string(soundHandle) + ")");
}

int AudioManager::GetSoundHandle(const std::string& soundName)
{
    return SoundHandles_[soundName];
}

void AudioManager::PlayAudio(const std::string& soundName, const std::string& stream)
{
    int handle = SoundHandles_[soundName];
    ChangeNextPlayVolumeSoundMem(AudioStreams_[stream].volume, handle);
    ChangeNextPlayPanSoundMem(AudioStreams_[stream].pan, handle);
    PlaySoundMem(handle, DX_PLAYTYPE_BACK);
}

void AudioManager::StopAudio(const std::string& soundName)
{
    StopSoundMem(SoundHandles_[soundName]);
}
