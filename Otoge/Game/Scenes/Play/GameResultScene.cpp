#include "pch.h"
#include "GameResultScene.hpp"
#include "../../../Util/Visual/Color.hpp"
#include "MusicSelect/MusicSelectScene.hpp"
#include "../../../System/Task/TaskManager.hpp"
#include "../../../System/Input/KeyboardManager.hpp"
#include "../../../Util/Calculate/Animation/Easing.hpp"
#include "../../../System/Input/MouseManager.hpp"

GameResultScene::GameResultScene(std::shared_ptr<BeatmapScore> score, std::shared_ptr<Beatmap> map) : Scene("GameResultScene[" + map->GetTitle() + "]")
{
    ResultScore_ = score;
    Beatmap_ = map;

    MapInfoPanel_ = std::make_shared<Scene>("MapInfoPanel", ScreenData(0.f, 0.f, 100.f, 15.f), DefaultScaler_);
    MapInfoPanel_->SetDrawFunction([=]
        {
            ScreenData fixed = MapInfoPanel_->GetDefaultScaler()->Calculate(0.f, 0.f, 100.f, 100.f);
            DrawBox(engine::CastToInt(fixed.posX), engine::CastToInt(fixed.posY), engine::CastToInt(fixed.posX + fixed.width), engine::CastToInt(fixed.posY + fixed.height), color_preset::GREY, TRUE);
        });
    AddChildTask(std::static_pointer_cast<Task>(MapInfoPanel_));

    BackButton_ = std::make_shared<Button>("< Back", ScreenData(0.f, 0.f, 10.f, 100.f), MapInfoPanel_->GetDefaultScaler());
    BackButton_->isDrawBase = false;
    BackButton_->GetTextLabelInstance()->adjustmentFontSize = false;
    BackButton_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(2.5f)));
    BackButton_->GetTextLabelInstance()->ChangeFontThickness(2);
    BackButton_->SetTransparent(100.f);
    MapInfoPanel_->AddChildTask(std::static_pointer_cast<Task>(BackButton_));

    TitleLabel_ = std::make_shared<Label>(map->GetTitle(), ScreenData(20.f, 0.f, 60.f, 70.f), MapInfoPanel_->GetDefaultScaler());
    TitleLabel_->baseColor = color_preset::WHITE;
    TitleLabel_->adjustmentFontSize = true;
    TitleLabel_->SetTextAlign(Label::TextAlignment::middle | Label::TextAlignment::center);
    MapInfoPanel_->AddChildTask(std::static_pointer_cast<Task>(TitleLabel_));

    ArtistLabel_ = std::make_shared<Label>(map->GetArtist(), ScreenData(0.f, TitleLabel_->GetScreenHeight(), 100.f, 30.f), MapInfoPanel_->GetDefaultScaler());
    ArtistLabel_->baseColor = color_preset::LIGHT_GREY;
    ArtistLabel_->adjustmentFontSize = true;
    ArtistLabel_->SetTextAlign(Label::TextAlignment::middle | Label::TextAlignment::center);
    MapInfoPanel_->AddChildTask(std::static_pointer_cast<Task>(ArtistLabel_));


    ScoreInfoPanel_ = std::make_shared<Scene>("ScoreInfoPanel", ScreenData(25.f, 25.f, 50.f, 75.f), DefaultScaler_);
    AddChildTask(std::static_pointer_cast<Task>(ScoreInfoPanel_));

    ScoreLabel_ = std::make_shared<Label>("---------------", ScreenData(0.f, 0.f, 100.f, 16.f), ScoreInfoPanel_->GetDefaultScaler());
    ScoreLabel_->baseColor = color_preset::WHITE_BLUE;
    ScoreLabel_->adjustmentFontSize = true;
    ScoreLabel_->SetTextAlign(Label::TextAlignment::middle | Label::TextAlignment::center);
    ScoreInfoPanel_->AddChildTask(std::static_pointer_cast<Task>(ScoreLabel_));

    MaxComboCaptionLabel_ = std::make_shared<Label>("Combo", ScreenData(0.f, ScoreLabel_->GetScreenHeight(), 22.f, 7.f), ScoreInfoPanel_->GetDefaultScaler());
    MaxComboCaptionLabel_->baseColor = color_preset::WHITE;
    MaxComboCaptionLabel_->adjustmentFontSize = true;
    MaxComboCaptionLabel_->SetTextAlign(Label::TextAlignment::middle | Label::TextAlignment::left);
    ScoreInfoPanel_->AddChildTask(std::static_pointer_cast<Task>(MaxComboCaptionLabel_));

    MaxComboLabel_ = std::make_shared<Label>("--------x", ScreenData(MaxComboCaptionLabel_->GetScreenWidth(), ScoreLabel_->GetScreenHeight(), 32.f, MaxComboCaptionLabel_->GetScreenHeight()), ScoreInfoPanel_->GetDefaultScaler());
    MaxComboLabel_->baseColor = color_preset::WHITE;
    MaxComboLabel_->adjustmentFontSize = true;
    MaxComboLabel_->SetTextAlign(Label::TextAlignment::middle | Label::TextAlignment::right);
    ScoreInfoPanel_->AddChildTask(std::static_pointer_cast<Task>(MaxComboLabel_));

    FullComboLabel_ = std::make_shared<Label>("FULL COMBO!", ScreenData(MaxComboLabel_->GetPositionX() + MaxComboLabel_->GetScreenWidth(), ScoreLabel_->GetScreenHeight(), 100.f - MaxComboLabel_->GetPositionX() - MaxComboLabel_->GetScreenWidth(), MaxComboLabel_->GetScreenHeight()), ScoreInfoPanel_->GetDefaultScaler());
    FullComboLabel_->baseColor = color_preset::LIGHT_GREEN;
    FullComboLabel_->adjustmentFontSize = true;
    FullComboLabel_->SetTextAlign(Label::TextAlignment::middle | Label::TextAlignment::center);
    FullComboLabel_->SetVisible(ResultScore_->maxCombo == Beatmap_->GetMaxCombo());
    ScoreInfoPanel_->AddChildTask(std::static_pointer_cast<Task>(FullComboLabel_));

    AccuracyCaptionLabel_ = std::make_shared<Label>("Accuracy", ScreenData(0.f, MaxComboLabel_->GetPositionY() + MaxComboLabel_->GetScreenHeight(), MaxComboCaptionLabel_->GetScreenWidth(), 7.f), ScoreInfoPanel_->GetDefaultScaler());
    AccuracyCaptionLabel_->baseColor = color_preset::PINK;
    AccuracyCaptionLabel_->adjustmentFontSize = true;
    AccuracyCaptionLabel_->SetTextAlign(Label::TextAlignment::middle | Label::TextAlignment::left);
    ScoreInfoPanel_->AddChildTask(std::static_pointer_cast<Task>(AccuracyCaptionLabel_));

    AccuracyLabel_ = std::make_shared<Label>("--------%", ScreenData(AccuracyCaptionLabel_->GetScreenWidth(), MaxComboLabel_->GetPositionY() + MaxComboLabel_->GetScreenHeight(), MaxComboLabel_->GetScreenWidth(), 7.f), ScoreInfoPanel_->GetDefaultScaler());
    AccuracyLabel_->baseColor = color_preset::PINK;
    AccuracyLabel_->adjustmentFontSize = true;
    AccuracyLabel_->SetTextAlign(Label::TextAlignment::middle | Label::TextAlignment::right);
    ScoreInfoPanel_->AddChildTask(std::static_pointer_cast<Task>(AccuracyLabel_));

    PerfectLabel_ = std::make_shared<Label>("PERFECT!!!", ScreenData(AccuracyLabel_->GetPositionX() + AccuracyLabel_->GetScreenWidth(), AccuracyLabel_->GetPositionY(), 100.f - AccuracyLabel_->GetPositionX() - AccuracyLabel_->GetScreenWidth(), AccuracyLabel_->GetScreenHeight()), ScoreInfoPanel_->GetDefaultScaler());
    PerfectLabel_->baseColor = color_preset::YELLOW;
    PerfectLabel_->adjustmentFontSize = true;
    PerfectLabel_->SetTextAlign(Label::TextAlignment::middle | Label::TextAlignment::center);
    PerfectLabel_->SetVisible(ResultScore_->accuracy == 100.f);
    ScoreInfoPanel_->AddChildTask(std::static_pointer_cast<Task>(PerfectLabel_));

    DefaultGraphScreen_ = ScreenData(0.f, 70.f, 100.f, 30.f);
    AccuracyGraph_ = std::make_shared<Scene>("AccuracyGraph", DefaultGraphScreen_, DefaultScaler_);
    AccuracyGraph_->SetDrawFunction([&] {DrawAccuracyGraph(); });
    AddChildTask(std::static_pointer_cast<Task>(AccuracyGraph_));
}

GameResultScene::~GameResultScene()
{
}

void GameResultScene::OnTerminate()
{
    TaskManager::GetInstance()->AddTaskByTypename<MusicSelectScene>();
}

void GameResultScene::OnStartedFadeIn()
{
}

void GameResultScene::SceneFadeIn(float deltaTime)
{
    float totalTime = 1.0f;
    float startPos_ = 20.f;
    Easing::EaseFunction ease = Easing::OutBounce;

    SetTransparent(engine::CastToFloat(ease(timerCount, totalTime, 100.f, 0.f)));

    SetPositionX(engine::CastToFloat(ease(timerCount, totalTime, 0.f, -startPos_)));
    SetPositionY(engine::CastToFloat(ease(timerCount, totalTime, 0.f, -startPos_)));
    SetScreenWidth(engine::CastToFloat(ease(timerCount, totalTime, 100.f, 100.f + startPos_ * 2.f)));
    SetScreenHeight(engine::CastToFloat(ease(timerCount, totalTime, 100.f, 100.f + startPos_ * 2.f)));

    RefreshScaler();
    RefreshDrawBuffer();

    if (timerCount > totalTime)
    {
        StopFade();
    }
}

void GameResultScene::OnStoppedFadeIn()
{
    SetTransparent(100.f);

    SetPositionX(0.f);
    SetPositionY(0.f);
    SetScreenWidth(100.f);
    SetScreenHeight(100.f);

    RefreshScaler();
    RefreshDrawBuffer();
}

void GameResultScene::SceneUpdate(float deltaTime)
{
    ScoreLabel_->SetLabel(std::to_string(ResultScore_->score));
    MaxComboLabel_->SetLabel(std::to_string(ResultScore_->maxCombo) + "x");
    AccuracyLabel_->SetLabel(std::_Floating_to_string("%.2f", ResultScore_->accuracy) + "%");

    if(KeyboardManager::GetInstance()->IsDownKey(KEY_INPUT_ESCAPE) || BackButton_->IsClickedMouse())
    {
        if(IsFullScreenGraph_)
        {
            IsFullScreenGraph_ = !IsFullScreenGraph_;
            timerCount = 0.f;
            IsMovingGraph_ = true;
        }else
        {
            Terminate();
        }
    }

    if(AccuracyGraph_->IsDownMouse())
    {
        AccuracyGraph_->AddChildTask(std::static_pointer_cast<Task>(std::make_shared<ButtonPushedAnimate>(
            AccuracyGraph_->GetDefaultScaler()->
            CalculatePositionRateX(
                MouseManager::GetInstance()->GetMouseXf() - GetRawPositionX() - AccuracyGraph_->GetDefaultScaler()->GetDiffX()),
            AccuracyGraph_->GetDefaultScaler()->CalculatePositionRateY(
                MouseManager::GetInstance()->GetMouseYf() - GetRawPositionY() - AccuracyGraph_->GetDefaultScaler()->GetDiffY()),
            color_preset::DARK_GREY, 12.f, AccuracyGraph_->GetDefaultScaler())));
    }

    if(AccuracyGraph_->IsClickedMouse())
    {
        IsFullScreenGraph_ = !IsFullScreenGraph_;
        timerCount = 0.f;
        IsMovingGraph_ = true;
    }

    if (IsMovingGraph_)
    {
        const float totalTime = 0.5f;
        Easing::EaseFunction ease = Easing::OutBounce;

        if (IsFullScreenGraph_)
        {
            AccuracyGraph_->SetPositionY(ease(timerCount, totalTime, MapInfoPanel_->GetScreenHeight(), DefaultGraphScreen_.posY));
            AccuracyGraph_->SetScreenHeight(ease(timerCount, totalTime, 100.f - MapInfoPanel_->GetScreenHeight(), DefaultGraphScreen_.height));
        }
        else
        {
            AccuracyGraph_->SetPositionY(ease(timerCount, totalTime, DefaultGraphScreen_.posY, MapInfoPanel_->GetScreenHeight()));
            AccuracyGraph_->SetScreenHeight(ease(timerCount, totalTime, DefaultGraphScreen_.height, 100.f - MapInfoPanel_->GetScreenHeight()));
        }

        if(timerCount > totalTime)
        {
            IsMovingGraph_ = false;
        }

        AccuracyGraph_->RefreshScaler();
        AccuracyGraph_->RefreshDrawBuffer();
    }
}

void GameResultScene::Draw()
{
    ScreenData fixed = DefaultScaler_->Calculate(0.f, 0.f, 100.f, 100.f);
    DrawBox(engine::CastToInt(fixed.posX), engine::CastToInt(fixed.posY), engine::CastToInt(fixed.posX + fixed.width), engine::CastToInt(fixed.posY + fixed.height), color_preset::DARK_BLUE, TRUE);
}

void GameResultScene::DrawAccuracyGraph()
{
    DrawBoxAA(0.f, 0.f, AccuracyGraph_->GetRawScreenWidth(), AccuracyGraph_->GetRawScreenHeight(), color_preset::GREEN, TRUE);

    int l_MapEoF = Beatmap_->GetLastComboCount();
    ScreenData l_PrevPoint = AccuracyGraph_->GetDefaultScaler()->Calculate(0.f, 100.f, 0.f, 100.f);

    for(Notes* note : Beatmap_->GetMapNotes())
    {
        if (note->Type_ == Notes::NoteType::simple)
        {
            float l_NotePoint = (engine::CastToFloat(note->TimingCount_) / engine::CastToFloat(l_MapEoF)) * 100.f;
            ScreenData l_Point = AccuracyGraph_->GetDefaultScaler()->Calculate(l_NotePoint, (abs(note->JudgeDiff_) / Notes::FAIL_RANGE) * 100.f, 0.f, 100.f);

            unsigned l_LineColor = color_preset::DARK_GREY;
            if (note->JudgeResult_ == Notes::HitsType::just) l_LineColor = color_preset::YELLOW;
            if (note->JudgeResult_ == Notes::HitsType::great) l_LineColor = color_preset::LIGHT_BLUE;
            if (note->JudgeResult_ == Notes::HitsType::bad) l_LineColor = color_preset::PURPLE;
            if(note->JudgeResult_ == Notes::HitsType::fail) l_LineColor = color_preset::LIGHT_GREY;
            DrawLineAA(l_PrevPoint.posX, l_PrevPoint.posY, l_Point.posX, l_Point.posY, l_LineColor, TRUE);
            if (l_PrevPoint.posY > l_Point.posY)
            {
                DrawTriangle(l_PrevPoint.posX, l_PrevPoint.posY, l_Point.posX, l_Point.posY, l_Point.posX, l_PrevPoint.posY, l_LineColor, TRUE);
                DrawBox(l_PrevPoint.posX, l_PrevPoint.posY, l_Point.posX, AccuracyGraph_->GetRawScreenHeight(), l_LineColor, TRUE);
            }
            else
            {
                DrawTriangle(l_PrevPoint.posX, l_PrevPoint.posY, l_Point.posX, l_Point.posY, l_PrevPoint.posX, l_Point.posY, l_LineColor, TRUE);
                DrawBox(l_PrevPoint.posX, l_Point.posY, l_Point.posX, AccuracyGraph_->GetRawScreenHeight(), l_LineColor, TRUE);
            }


            l_PrevPoint = l_Point;
        }
    }
}
