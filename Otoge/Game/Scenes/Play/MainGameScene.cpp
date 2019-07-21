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
#include "GameResultScene.hpp"

MainGameScene::MainGameScene(std::shared_ptr<Beatmap> map) : Scene("MainGameScene[" + map->GetTitle() + "]")
{
    Beatmap_ = map;

    LaneKeys_ = new int[6];
    LaneKeys_[0] = KEY_INPUT_Z;
    LaneKeys_[1] = KEY_INPUT_X;
    LaneKeys_[2] = KEY_INPUT_C;
    LaneKeys_[3] = KEY_INPUT_J;
    LaneKeys_[4] = KEY_INPUT_K;
    LaneKeys_[5] = KEY_INPUT_L;

    JudgeText_[Notes::HitsType::just] = std::make_pair("JUST", color_preset::YELLOW);
    JudgeText_[Notes::HitsType::great] = std::make_pair("GREAT", color_preset::LIGHT_BLUE);
    JudgeText_[Notes::HitsType::bad] = std::make_pair("BAD", color_preset::PURPLE);
    JudgeText_[Notes::HitsType::fail] = std::make_pair("FAIL", color_preset::LIGHT_GREY);

    ScoreData_ = std::make_shared<BeatmapScore>();


    GamePanel_ = std::make_shared<Scene>("MainGamePanel", ScreenData(3.f, 0.f, 44.f, 100.f), DefaultScaler_);
    GamePanel_->SetDrawFunction([&] {DrawGamePanel(); });
    AddChildTask(std::static_pointer_cast<Task>(GamePanel_));

    LoadingLabel_ = std::make_shared<Label>("Loading...", ScreenData(0.f, JudgeLinePosY_ - 14.f, 100.f, 5.f), GamePanel_->GetDefaultScaler());
    LoadingLabel_->baseColor = color_preset::CYAN;
    LoadingLabel_->adjustmentFontSize = true;
    LoadingLabel_->SetTextAlign(Label::TextAlignment::middle | Label::TextAlignment::center);
    GamePanel_->AddChildTask(std::static_pointer_cast<Task>(LoadingLabel_));


    InfoPanel_ = std::make_shared<Scene>("InfoPanel", ScreenData(50.f, 0.f, 50.f, 100.f), DefaultScaler_);
    AddChildTask(std::static_pointer_cast<Task>(InfoPanel_));

    BPMLabel_ = std::make_shared<Label>("------------- BPM", ScreenData(3.f, 50.f, 40.f, 5.f), InfoPanel_->GetDefaultScaler());
    BPMLabel_->baseColor = color_preset::YELLOW;
    BPMLabel_->adjustmentFontSize = true;
    InfoPanel_->AddChildTask(std::static_pointer_cast<Task>(BPMLabel_));

    BarCountLabel_ = std::make_shared<Label>("---:--- / ---", ScreenData(3.f, 55.f, 30.f, 3.f), InfoPanel_->GetDefaultScaler());
    BarCountLabel_->baseColor = color_preset::LIGHT_GREY;
    BarCountLabel_->adjustmentFontSize = true;
    InfoPanel_->AddChildTask(std::static_pointer_cast<Task>(BarCountLabel_));

    TimingCountLabel_ = std::make_shared<Label>("------- Counts", ScreenData(3.f, 58.f, 36.f, 3.f), InfoPanel_->GetDefaultScaler());
    TimingCountLabel_->baseColor = color_preset::GREY;
    TimingCountLabel_->adjustmentFontSize = true;
    InfoPanel_->AddChildTask(std::static_pointer_cast<Task>(TimingCountLabel_));


    ScorePanel_ = std::make_shared<Scene>("ScorePanel", ScreenData(4.f, 67.f, 70.f, 30.f), InfoPanel_->GetDefaultScaler());
    InfoPanel_->AddChildTask(std::static_pointer_cast<Task>(ScorePanel_));

    ScoreLabel_ = std::make_shared<Label>("--------------- pt", ScreenData(0.f, 2.f, 80.f, 12.f), ScorePanel_->GetDefaultScaler());
    ScoreLabel_->baseColor = color_preset::WHITE_BLUE;
    ScoreLabel_->adjustmentFontSize = true;
    ScoreLabel_->SetTextAlign(Label::TextAlignment::middle | Label::TextAlignment::left);
    ScorePanel_->AddChildTask(std::static_pointer_cast<Task>(ScoreLabel_));

    ComboLabel_ = std::make_shared<Label>("--------x", ScreenData(0.f, 14.f, 80.f, 10.f), ScorePanel_->GetDefaultScaler());
    ComboLabel_->baseColor = color_preset::WHITE;
    ComboLabel_->adjustmentFontSize = true;
    ComboLabel_->SetTextAlign(Label::TextAlignment::middle | Label::TextAlignment::left);
    ScorePanel_->AddChildTask(std::static_pointer_cast<Task>(ComboLabel_));

    AccuracyLabel_ = std::make_shared<Label>("--------%", ScreenData(0.f, 24.f, 80.f, 15.f), ScorePanel_->GetDefaultScaler());
    AccuracyLabel_->baseColor = color_preset::WHITE;
    AccuracyLabel_->adjustmentFontSize = true;
    AccuracyLabel_->SetTextAlign(Label::TextAlignment::middle | Label::TextAlignment::left);
    ScorePanel_->AddChildTask(std::static_pointer_cast<Task>(AccuracyLabel_));

    Beatmap_->Reset();
    SetUseASyncLoadFlag(TRUE);
    Beatmap_->LoadMapData();
    SetUseASyncLoadFlag(FALSE);
}

MainGameScene::~MainGameScene()
{
    delete[] LaneKeys_;
    Logger_->Debug("MainGameScene 終了");
}

void MainGameScene::OnInitialize()
{
    StartFadeIn();
}

void MainGameScene::OnTerminate()
{
    AudioManager::GetInstance()->StopAudio("map_" + Beatmap_->GetTitle());
    DeleteSoundMem(AudioManager::GetInstance()->GetSoundHandle("map_" + Beatmap_->GetTitle()));
}


void MainGameScene::OnStartedFadeIn()
{
}

void MainGameScene::OnStoppedFadeIn()
{
    SetTransparent(100.f);

    GamePanel_->SetPositionX(3.f);
    InfoPanel_->SetPositionX(50.f);
}

void MainGameScene::OnStartedFadeOut()
{
    SetEnable(false);
    DefaultMusicStreamVol_ = AudioManager::GetInstance()->GetStreamVolume(AudioManager::STREAM_NAME_BGM);
    TaskManager::GetInstance()->AddTask(std::make_shared<GameResultScene>(ScoreData_, Beatmap_));
}

void MainGameScene::OnStoppedFadeOut()
{
    SetTransparent(0.f);
    Terminate();
}

void MainGameScene::SceneFadeIn(float deltaTime)
{
    float totalTime = 1.0f;
    Easing::EaseFunction ease = Easing::OutExp;

    SetTransparent(engine::CastToFloat(ease(timerCount, totalTime, 100.f, 0.f)));

    GamePanel_->SetPositionX(engine::CastToFloat(ease(timerCount, totalTime, 3.f, -GamePanel_->GetScreenWidth())));
    InfoPanel_->SetPositionX(engine::CastToFloat(ease(timerCount, totalTime, 50.f, 50.f + InfoPanel_->GetScreenWidth())));

    if (timerCount > totalTime)
    {
        StopFade();
    }
}

void MainGameScene::SceneFadeOut(float deltaTime)
{
    float totalTime = 1.0f;
    Easing::EaseFunction ease = Easing::OutExp;

    SetTransparent(engine::CastToFloat(ease(timerCount, totalTime, 0.f, 100.f)));

    GamePanel_->SetPositionX(engine::CastToFloat(ease(timerCount, totalTime, -GamePanel_->GetScreenWidth(), 3.f)));
    InfoPanel_->SetPositionX(engine::CastToFloat(ease(timerCount, totalTime, 50.f + InfoPanel_->GetScreenWidth(), 50.f)));

    ChangeVolumeSoundMem(engine::CastToInt(ease(timerCount, totalTime, 0.f, engine::CastToFloat(DefaultMusicStreamVol_))), AudioManager::GetInstance()->GetSoundHandle("map_" + Beatmap_->GetTitle()));

    if (timerCount > totalTime)
    {
        StopFade();
    }
}

void MainGameScene::SceneUpdate(float deltaTime)
{

    if (KeyboardManager::GetInstance()->IsDownKey(KEY_INPUT_ESCAPE) && IsEnable())
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
        LoadingLabel_->SetTransparent(80.f);
        LoadingLabel_->SetVisible(false);
    }

    // BGMのほうが早く処理し始める場合
    if(Beatmap_->GetSoundOffset() < 0.f)
    {
        if (!IsMusicPlaying_)
        {
            StopFade();
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

        // 精度/スコア計算用のカウンタ
        int l_ProcessedNoteCount = 0;
        float l_MapAccuracy = 0.f;

        for (Notes* note : Beatmap_->GetMapNotes())
        {
            if (note->TimingCount_ < engine::CastToInt(Beatmap_->GetCurrentPlayCount() + timing::HI_SPEED))
            {
                // 処理済み
                if (note->IsProcessed_)
                {
                    // 精度計算
                    if (note->JudgeResult_ == Notes::HitsType::outside)
                    {
                        // 処理済みのためスキップ
                        continue;
                    }
                    l_ProcessedNoteCount++;

                    float l_NoteAccuracy = 0.f;
                    if (note->JudgeResult_ == Notes::HitsType::just)
                    {
                        l_NoteAccuracy = 1.f;
                    }
                    if (note->JudgeResult_ == Notes::HitsType::great)
                    {
                        l_NoteAccuracy = 0.8f;
                    }
                    if (note->JudgeResult_ == Notes::HitsType::bad)
                    {
                        l_NoteAccuracy = 0.5f;
                    }

                    l_MapAccuracy += l_NoteAccuracy;

                    continue;
                }

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

                        note->IsProcessed_ = true;
                    }
                }

                // 譜面終わり
                if(note->Type_ == Notes::NoteType::endMap)
                {
                    if(note->IsPast(Beatmap_->GetCurrentPlayCount()))
                    {
                        StartFadeOut();
                        note->IsProcessed_ = true;
                    }
                }

                // シンプルタップノーツ
                if (note->Type_ == Notes::NoteType::simple)
                {
                    Notes::HitsType judge = note->Judgment(Beatmap_->GetCurrentPlayCount(), Beatmap_->GetCurrentTempoByBPM());
                    if (judge != Notes::HitsType::outside)
                    {
                        // 判定範囲内
                        if (KeyboardManager::GetInstance()->IsDownKey(LaneKeys_[note->Position_]))
                        {
                            // FAIL判定ではないなら音を鳴らしてノーツ表示を消す
                            if (judge != Notes::HitsType::fail)
                            {
                                AudioManager::GetInstance()->PlayAudio("hit", AudioManager::STREAM_NAME_SE);
                                note->IsDraw_ = false;

                                // スコア・コンボ加算
                                ScoreData_->currentCombo++;
                                if (ScoreData_->maxCombo < ScoreData_->currentCombo) ScoreData_->maxCombo = ScoreData_->currentCombo;

                                if(judge == Notes::HitsType::just)
                                {
                                    ScoreData_->score += 500;
                                }
                                if (judge == Notes::HitsType::great)
                                {
                                    ScoreData_->score += 200;
                                }
                                if (judge == Notes::HitsType::bad)
                                {
                                    ScoreData_->score += 50;
                                }
                            }else
                            {
                                ScoreData_->currentCombo = 0;
                            }

                            LoadingLabel_->SetLabel(JudgeText_[judge].first);
                            LoadingLabel_->baseColor = JudgeText_[judge].second;
                            LoadingLabel_->SetVisible(true);

                            note->IsProcessed_ = true;
                            Logger_->Debug("judge: " + std::to_string(judge));
                        }
                    }else
                    {
                        // タイミングポイントを過ぎていたら処理済みにする
                        if (note->IsPast(Beatmap_->GetCurrentPlayCount()))
                        {
                            note->IsProcessed_ = true;
                            LoadingLabel_->SetLabel("FAIL");
                            LoadingLabel_->baseColor = color_preset::GREY;
                            LoadingLabel_->SetVisible(true);

                            ScoreData_->currentCombo = 0;
                        }
                    }
                }
            }
        }

        // 小節/拍数計算
        int l_Beat = timing::GetBeatByCount(Beatmap_->GetCurrentPlayCount());
        int l_Bar = timing::GetBarByBeat(l_Beat, 4);
        BarCountLabel_->SetLabel(std::to_string(l_Bar + 1) + ":" + std::to_string(l_Beat % 4 + 1) + " / " + std::to_string(l_Beat + 1));
        TimingCountLabel_->SetLabel(std::to_string(engine::CastToInt(Beatmap_->GetCurrentPlayCount())) + " Counts");

        // 精度計算、スコアボード更新
        ScoreData_->accuracy = (l_MapAccuracy / engine::CastToFloat(l_ProcessedNoteCount)) * 100.f;
        if (isnan(ScoreData_->accuracy)) ScoreData_->accuracy = 0.f;
        ScoreLabel_->SetLabel(std::to_string(ScoreData_->score) + " pts");
        ComboLabel_->SetLabel(std::to_string(ScoreData_->currentCombo) + "x / Max " + std::to_string(ScoreData_->maxCombo) + "x");
        AccuracyLabel_->SetLabel(std::_Floating_to_string("%.2f", ScoreData_->accuracy) + "%");
        
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
}

void MainGameScene::DrawGamePanel()
{
    float LaneWidth_ = 100.f / 6.f;

    for (Notes* note : Beatmap_->GetMapNotes())
    {
        if (note->TimingCount_ < engine::CastToInt(Beatmap_->GetCurrentPlayCount() + timing::HI_SPEED) && note->IsDraw_)
        {
            // 処理済みなら半透明化
            if (note->IsProcessed_) SetDrawBlendMode(DX_BLENDMODE_ALPHA, 20);

            float noteTimingDiff = engine::CastToFloat(note->TimingCount_) - Beatmap_->GetCurrentPlayCount();
            float notePositionY = JudgeLinePosY_ - (noteTimingDiff / (timing::HI_SPEED / 100.f));
            ScreenData noteBox;

            // BPM変更
            if (note->Type_ == Notes::NoteType::changeBPM)
            {
                noteBox = GamePanel_->GetDefaultScaler()->Calculate(ScreenData(0.f, notePositionY, 100.f, 0.f));
            }

            // シンプルタップノーツ
            if (note->Type_ == Notes::NoteType::simple)
            {
                noteBox = GamePanel_->GetDefaultScaler()->Calculate(note->Position_ * LaneWidth_, notePositionY, LaneWidth_, 2.f);
            }

            noteBox.posY -= noteBox.height / 2.f;
            note->Draw(noteBox);
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
        }
    }

    ScreenData l_LineSD = GamePanel_->GetDefaultScaler()->Calculate(0.f, JudgeLinePosY_, 100.f, 0.5f);
    DrawBox(l_LineSD.posX, l_LineSD.posY, l_LineSD.posX + l_LineSD.width, l_LineSD.posY + l_LineSD.height, color_preset::LIGHT_GREY, TRUE);

    ScreenData l_OutLine = GamePanel_->GetDefaultScaler()->Calculate(0.f, -5.f, 100.f, 110.f);
    DrawBoxAA(l_OutLine.posX, l_OutLine.posY, l_OutLine.posX + l_OutLine.width, l_OutLine.posY + l_OutLine.height, color_preset::LIGHT_GREY, FALSE, 2.f);


    for (int i = 0; i < 7; i++)
    {
        ScreenData l_VLine = GamePanel_->GetDefaultScaler()->Calculate(i * LaneWidth_, 0.f, 0.f, 100.f);
        DrawLineAA(l_VLine.posX, l_VLine.posY, l_VLine.posX, l_VLine.height, color_preset::LIGHT_GREY, 1.f);

        if (i < 6 && KeyboardManager::GetInstance()->IsHoldKey(LaneKeys_[i]))
        {
            ScreenData l_KeyEffect = GamePanel_->GetDefaultScaler()->Calculate(i * LaneWidth_, JudgeLinePosY_, LaneWidth_, 100.f - JudgeLinePosY_);
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 40);
            DrawBoxAA(l_KeyEffect.posX, l_KeyEffect.posY, l_KeyEffect.posX + l_KeyEffect.width, l_KeyEffect.posY + l_KeyEffect.height, LaneColor_, TRUE);
            SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
        }
    }
}
