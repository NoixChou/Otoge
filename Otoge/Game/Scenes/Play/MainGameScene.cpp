#include "MainGameScene.hpp"
#include "../../../System/Task/TaskManager.hpp"
#include "../../../Util/Beatmap/Timing.hpp"
#include "../../../System/GlobalMethod.hpp"
#include "../../../Util/Visual/Color.hpp"
#include "MusicSelect/MusicSelectScene.hpp"
#include "../../../System/Input/KeyboardManager.hpp"
#include "../../../Util/Audio/AudioManager.hpp"
#include "../../../Util/Encoding/EncodingConverter.h"
#include "../../../Util/Calculate/Animation/Easing.hpp"

MainGameScene::MainGameScene(std::shared_ptr<Beatmap> map) : Scene("MainGameScene[" + map->GetTitle() + "]")
{
    Beatmap_ = map;

    BPMLabel_ = std::make_shared<Label>("- BPM", ScreenData(50.f, 50.f, 20.f, 5.f), DefaultScaler_);
    BPMLabel_->baseColor = color_preset::YELLOW;
    BPMLabel_->adjustmentFontSize = true;
    AddChildTask(std::static_pointer_cast<Task>(BPMLabel_));

    BarCountLabel_ = std::make_shared<Label>("- Beats", ScreenData(50.f, 55.f, 15.f, 3.f), DefaultScaler_);
    BarCountLabel_->baseColor = color_preset::LIGHT_GREY;
    BarCountLabel_->adjustmentFontSize = true;
    AddChildTask(std::static_pointer_cast<Task>(BarCountLabel_));

    TimingCountLabel_ = std::make_shared<Label>("- Counts", ScreenData(50.f, 58.f, 18.f, 3.f), DefaultScaler_);
    TimingCountLabel_->baseColor = color_preset::GREY;
    TimingCountLabel_->adjustmentFontSize = true;
    AddChildTask(std::static_pointer_cast<Task>(TimingCountLabel_));

    LoadingLabel_ = std::make_shared<Label>("Loading...", ScreenData(3.f, JudgeLinePosY_ - 8.f, 36.f, 5.f), DefaultScaler_);
    LoadingLabel_->baseColor = color_preset::CYAN;
    LoadingLabel_->adjustmentFontSize = true;
    AddChildTask(std::static_pointer_cast<Task>(LoadingLabel_));

    Beatmap_->Reset();
    SetUseASyncLoadFlag(TRUE);
    Beatmap_->LoadMapData();
    SetUseASyncLoadFlag(FALSE);
}

MainGameScene::~MainGameScene()
{
    Beatmap_->Reset();
}

void MainGameScene::OnInitialize()
{
    StopFade();
}

void MainGameScene::OnTerminate()
{
    AudioManager::GetInstance()->StopAudio("map_" + Beatmap_->GetTitle());
    //AudioManager::GetInstance()->SetStreamVolume("music", defaultMusicStreamVol_);
}


void MainGameScene::OnStartedFadeIn()
{
}

void MainGameScene::OnStoppedFadeIn()
{
    SetTransparent(100.f);
}

void MainGameScene::OnStartedFadeOut()
{
    defaultMusicStreamVol_ = AudioManager::GetInstance()->GetStreamVolume(AudioManager::STREAM_NAME_BGM);
    TaskManager::GetInstance()->AddTaskByTypename<MusicSelectScene>();
}

void MainGameScene::OnStoppedFadeOut()
{
    SetTransparent(0.f);
    Terminate();
}

void MainGameScene::SceneFadeIn(float deltaTime)
{
}

void MainGameScene::SceneFadeOut(float deltaTime)
{
    float totalTime = 0.5f;
    Easing::EaseFunction ease = Easing::OutExp;

    SetTransparent(engine::CastToFloat(ease(timerCount, totalTime, 0.f, 100.f)));
    ChangeVolumeSoundMem(engine::CastToInt(ease(timerCount, totalTime, 0.f, engine::CastToFloat(defaultMusicStreamVol_))), AudioManager::GetInstance()->GetSoundHandle("map_" + Beatmap_->GetTitle()));

    if (timerCount > totalTime)
    {
        StopFade();
    }
}

void MainGameScene::SceneUpdate(float deltaTime)
{

    if (KeyboardManager::GetInstance()->IsDownKey(KEY_INPUT_ESCAPE))
    {
        StartFadeOut();
    }

    if (!IsMusicPlaying_ && !Beatmap_->IsPlaying())
    {
        if (CheckHandleASyncLoad(AudioManager::GetInstance()->GetSoundHandle("map_" + Beatmap_->GetTitle())) == TRUE)
        {
            return;
        }
        if (CheckHandleASyncLoad(AudioManager::GetInstance()->GetSoundHandle("map_" + Beatmap_->GetTitle())) == -1)
        {
            MessageBox(GetMainWindowHandle(), encoding::ConvertUtf8ToSJIS("音声ファイルの読み込みに失敗しました。").c_str(), encoding::ConvertUtf8ToSJIS("エラー").c_str(), MB_APPLMODAL | MB_ICONERROR | MB_OK);
            StartFadeOut();
        }
        LoadingLabel_->SetVisible(false);
    }

    // BGMのほうが早く処理し始める場合
    if(Beatmap_->GetSoundOffset() < 0.f)
    {
        if (!IsMusicPlaying_)
        {
            timerCount = 0.f;
            AudioManager::GetInstance()->PlayAudio("map_" + Beatmap_->GetTitle(), AudioManager::STREAM_NAME_BGM);
            IsMusicPlaying_ = true;
        }
        if(timerCount >= -Beatmap_->GetSoundOffset() && !Beatmap_->IsPlaying())
        {
            AudioManager::GetInstance()->PlayAudio("beat", AudioManager::STREAM_NAME_SE);
            Beatmap_->SetPlayingStatus(true);
        }
    }
    // BGMも譜面も同時の場合
    if(Beatmap_->GetSoundOffset() == 0.f)
    {
        if (!IsMusicPlaying_)
        {
            AudioManager::GetInstance()->PlayAudio("map_" + Beatmap_->GetTitle(), AudioManager::STREAM_NAME_BGM);
            IsMusicPlaying_ = true;
        }
        if (!Beatmap_->IsPlaying())
        {
            AudioManager::GetInstance()->PlayAudio("beat", AudioManager::STREAM_NAME_SE);
            Beatmap_->SetPlayingStatus(true);
        }
    }
    // 譜面のほうが早く処理し始める場合
    if (Beatmap_->GetSoundOffset() > 0.f)
    {
        if (!Beatmap_->IsPlaying())
        {
            timerCount = 0.f;
            AudioManager::GetInstance()->PlayAudio("beat", AudioManager::STREAM_NAME_SE);
            Beatmap_->SetPlayingStatus(true);
        }

        if (timerCount >= Beatmap_->GetSoundOffset() && !IsMusicPlaying_)
        {
            AudioManager::GetInstance()->PlayAudio("map_" + Beatmap_->GetTitle(), AudioManager::STREAM_NAME_BGM);
            IsMusicPlaying_ = true;
        }
    }

    if (Beatmap_->IsPlaying())
    {
        Beatmap_->Update(deltaTime);

        for (Notes* note : Beatmap_->GetMapNotes())
        {
            if (note->TimingCount_ < engine::CastToInt(Beatmap_->GetCurrentPlayCount() + timing::HI_SPEED))
            {
                // 処理済みならスキップ
                if (note->IsProcessed_) continue;

                // BPM変更
                if (note->Type_ == Notes::NoteType::changeBPM)
                {
                    if (note->IsPast(Beatmap_->GetCurrentPlayCount()))
                    {
                        if (Beatmap_->GetCurrentTempoByBPM() != 0.f)
                        {
                            float l_CountDiff = Beatmap_->GetCurrentPlayCount() - engine::CastToFloat(note->TimingCount_);
                            float l_CurrentCount = Beatmap_->GetCurrentPlayCount();
                            float l_BPMChangeCount = l_CurrentCount - l_CountDiff;
                            float l_DiffSecond = timing::GetTimeByCount(l_CountDiff, Beatmap_->GetCurrentTempoByBPM());
                            float l_DiffCountByNewBPM = l_DiffSecond * (engine::CastToFloat(timing::BAR_COUNT) / 4.f) * (note->BPM_ / 60.f);

                            Logger_->Debug("change BPM, count diff: " + std::_Floating_to_string("%.4f", l_CountDiff) + ", BPM: " + std::_Floating_to_string(" % .4f", note->BPM_));
                            Logger_->Debug("fixed diff, DiffSecond: " + std::_Floating_to_string("%.4f", l_DiffSecond) + ", DiffByNewBPM: " + std::_Floating_to_string(" % .4f", l_DiffCountByNewBPM));
                            Beatmap_->SetCurrentPlayCount(l_BPMChangeCount + l_DiffCountByNewBPM);
                        }else
                        {
                            Beatmap_->SetCurrentPlayCount(note->TimingCount_);
                            Logger_->Debug("set BPM, offset fixed");
                        }

                        Beatmap_->SetCurrentTempoByBPM(note->BPM_);
                        BPMLabel_->SetLabel(std::_Floating_to_string("%.2f", note->BPM_) + " BPM");
                    }
                }

                // シンプルタップノーツ
                if (note->Type_ == Notes::NoteType::simple)
                {
                    if (note->IsPast(Beatmap_->GetCurrentPlayCount()))
                    {
                        AudioManager::GetInstance()->PlayAudio("hit", AudioManager::STREAM_NAME_SE);
                    }
                }

                // タイミングポイントを過ぎていたら処理済みにする
                if (note->IsPast(Beatmap_->GetCurrentPlayCount()))
                {
                    note->IsProcessed_ = true;
                }
            }
        }

        int l_Beat = timing::GetBeatByCount(Beatmap_->GetCurrentPlayCount());
        int l_Bar = timing::GetBarByBeat(l_Beat, 4);
        BarCountLabel_->SetLabel(std::to_string(l_Bar + 1) + ":" + std::to_string(l_Beat % 4 + 1) + " / " + std::to_string(l_Beat + 1));
        TimingCountLabel_->SetLabel(std::to_string(engine::CastToInt(Beatmap_->GetCurrentPlayCount())) + " Counts");

        
        if(timing::GetBeatByCount(Beatmap_->GetPrevPlayCount()) != timing::GetBeatByCount(Beatmap_->GetCurrentPlayCount()))
        {
            AudioManager::GetInstance()->PlayAudio("beat", AudioManager::STREAM_NAME_SE);
        }
    }
}

void MainGameScene::Draw()
{
    ScreenData fixed = DefaultScaler_->Calculate(ScreenData(0.f, 0.f, 100.f, 100.f));

    DrawBox(engine::CastToInt(fixed.posX),
        engine::CastToInt(fixed.posY),
        engine::CastToInt(fixed.width),
        engine::CastToInt(fixed.height), color_preset::BLACK, TRUE);

    for (Notes* note : Beatmap_->GetMapNotes())
    {
        if (note->TimingCount_ < engine::CastToInt(Beatmap_->GetCurrentPlayCount() + timing::HI_SPEED))
        {
            // 処理済みなら半透明化
            if (note->IsProcessed_) SetDrawBlendMode(DX_BLENDMODE_ALPHA, 20);

            float noteTimingDiff = engine::CastToFloat(note->TimingCount_) - Beatmap_->GetCurrentPlayCount();
            float notePositionY = JudgeLinePosY_ - (noteTimingDiff / 72.f);
            ScreenData noteBox;

            // BPM変更
            if(note->Type_ == Notes::NoteType::changeBPM)
            {
                noteBox = DefaultScaler_->Calculate(ScreenData(3.f, notePositionY, 36.f, 0.f));
            }

            // シンプルタップノーツ
            if (note->Type_ == Notes::NoteType::simple)
            {
                noteBox = DefaultScaler_->Calculate(3.f + (note->Position_ * 6.f), notePositionY, 6.f, 2.f);
            }

            noteBox.posY -= noteBox.height / 2.f;
            note->Draw(noteBox);
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
        }
    }

    ScreenData l_LineSD = DefaultScaler_->Calculate(3.f, JudgeLinePosY_, 36.f, 0.5f);
    DrawBox(l_LineSD.posX, l_LineSD.posY, l_LineSD.posX + l_LineSD.width, l_LineSD.posY + l_LineSD.height, color_preset::LIGHT_GREY, TRUE);
}
