#include "MusicSelectScene.hpp"
#include "../../../../Util/Calculate/Animation/Easing.hpp"
#include "../../../../System/GlobalMethod.hpp"
#include "../../../../Util/Visual/Color.hpp"
#include "../../../../System/Task/TaskManager.hpp"
#include "../../Title/TitleScene.hpp"
#include "../../../../System/Input/KeyboardManager.hpp"
#include "../../../../System/Config.h"
#include "../../../../Util/Setting/SettingManager.h"
#include "../../../../Util/Calculate/Screen/FontStringCalculator.hpp"

MusicSelectScene::MusicSelectScene() : Scene("MusicSelectScene")
{
    BackButton_ = std::make_shared<Button>("< Back", ScreenData(0.f, 0.f, 10.f, 6.f), DefaultScaler_);
    BackButton_->GetTextLabelInstance()->adjustmentFontSize = false;
    BackButton_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(2.5f)));
    BackButton_->GetTextLabelInstance()->ChangeFontThickness(2);
    BackButton_->SetTransparent(100.f);
    AddChildTask(std::static_pointer_cast<Task>(BackButton_));

    TestPanel_ = std::make_shared<MusicInfoPanel>("TEST", "hoge", 10.2);
    AddChildTask(TestPanel_);
}

MusicSelectScene::~MusicSelectScene()
{
}

void MusicSelectScene::SceneFadeIn(float deltaTime)
{
    float totalTime = 0.5f;
    Easing::EaseFunction ease = Easing::OutExp;

    SetTransparent(engine::CastToFloat(ease(timerCount, totalTime, 100.f, 0.f)));

    if (timerCount > totalTime)
    {
        StopFade();
    }
}

void MusicSelectScene::OnStoppedFadeIn()
{
    SetTransparent(100.f);
}

void MusicSelectScene::SceneFadeOut(float deltaTime)
{
    float totalTime = 0.5f;
    Easing::EaseFunction ease = Easing::OutExp;

    SetTransparent(engine::CastToFloat(ease(timerCount, totalTime, 0.f, 100.f)));

    if (timerCount > totalTime)
    {
        StopFade();
    }
}

void MusicSelectScene::OnStoppedFadeOut()
{
    SetTransparent(0.f);
    Terminate();
}

void MusicSelectScene::SceneUpdate(float deltaTime)
{
    if (BackButton_->IsClickedMouse())
    {
        TaskManager::GetInstance()->AddTaskByTypename<TitleScene>();
        StartFadeOut();
    }
}

void MusicSelectScene::Draw()
{
}


/*** 楽曲情報パネルシーン ***/
int MusicInfoPanel::TitleFontHandle_ = -1;
int MusicInfoPanel::MiddleFontHandle_ = -1;
int MusicInfoPanel::SmallFontHandle_ = -1;
int MusicInfoPanel::GlobalPanelCount_ = 0;

MusicInfoPanel::MusicInfoPanel(const std::string& musicName, const std::string& artistName, float difficulty) : Scene("MusicSelectScene{" + musicName + " , " + artistName + ": "+ std::_Floating_to_string("%.2f", difficulty) +"}", 30.f, 30.f)
{
    if(TitleFontHandle_ == -1 && MiddleFontHandle_ == -1 && SmallFontHandle_ == -1 && GlobalPanelCount_ == 0)
    {
        TitleFontHandle_ = CreateFontToHandle(SettingManager::GetGlobal()->Get<std::string>(game_config::SETTINGS_FONT_NAME).get().c_str(), engine::CastToInt(DefaultScaler_->CalculateHeight(15.f)), 8, GUI::GetDefaultFontDrawType());
        MiddleFontHandle_ = CreateFontToHandle(SettingManager::GetGlobal()->Get<std::string>(game_config::SETTINGS_FONT_NAME).get().c_str(), engine::CastToInt(DefaultScaler_->CalculateHeight(7.f)), 6, GUI::GetDefaultFontDrawType());
        SmallFontHandle_ = CreateFontToHandle(SettingManager::GetGlobal()->Get<std::string>(game_config::SETTINGS_FONT_NAME).get().c_str(), engine::CastToInt(DefaultScaler_->CalculateHeight(5.f)), 3, GUI::GetDefaultFontDrawType());

        GlobalPanelCount_++;
    }

    MusicName_ = musicName;
    ArtistName_ = artistName;
    Difficulty_ = difficulty;

    PreLayoutPosY_ = ParentScaler_->CalculatePositionRateY(GetRawPositionY());
}

MusicInfoPanel::~MusicInfoPanel()
{
    GlobalPanelCount_--;
    if (GlobalPanelCount_ == 0)
    {
        DeleteFontToHandle(TitleFontHandle_);
        DeleteFontToHandle(MiddleFontHandle_);
        DeleteFontToHandle(SmallFontHandle_);
    }
}

void MusicInfoPanel::SceneUpdate(float deltaTime)
{
    if(timerCount <= 0.333f / 2.f)
    {
        SetPositionY(Easing::InExp(timerCount, (0.333f / 2.f), PreLayoutPosY_ - 1.f, PreLayoutPosY_));
        //AddPositionY(-5.f * deltaTime);
    }
    if (timerCount > 0.333f / 2.f)
    {
        SetPositionY(Easing::OutExp(timerCount - 0.333f / 2.f, (0.333f / 2.f), PreLayoutPosY_, PreLayoutPosY_ - 1.f));
    }
    if (timerCount >= 0.666f / 2.f)
    {
        timerCount = 0.f;
    }
}

void MusicInfoPanel::Draw()
{
    ScreenData fixed = DefaultScaler_->Calculate(0.f, 0.f, 100.f, 100.f);
    DrawBoxAA(fixed.posX, fixed.posY, fixed.posX + fixed.width, fixed.posY + fixed.height, color_preset::DARK_GREY, TRUE);
    DrawBoxAA(fixed.posX, fixed.posY, fixed.posX + fixed.width, fixed.posY + fixed.height, color_preset::WHITE, FALSE);

    float TitleTextCenterH = FontStringCalculator::GetStringCenterHorizontal(TitleFontHandle_, MusicName_);
    float TitleTextCenterV = FontStringCalculator::GetStringCenterVertical(TitleFontHandle_);
    float ArtistTextCenterH = FontStringCalculator::GetStringCenterHorizontal(MiddleFontHandle_, MusicName_);
    float ArtistTextCenterV = FontStringCalculator::GetStringCenterVertical(MiddleFontHandle_);
    float DifficultyTextCenterH = FontStringCalculator::GetStringCenterHorizontal(SmallFontHandle_, MusicName_);
    float DifficultyTextCenterV = FontStringCalculator::GetStringCenterVertical(SmallFontHandle_);

    ScreenData TitlePos = DefaultScaler_->Calculate(50.f, 70.f, 0.f, 0.f);
    TitlePos.posX -= TitleTextCenterH;
    TitlePos.posY -= TitleTextCenterV;
    ScreenData ArtistPos = DefaultScaler_->Calculate(50.f, 79.f, 0.f, 0.f);
    ArtistPos.posX -= ArtistTextCenterH;
    ArtistPos.posY -= ArtistTextCenterV;

    DrawStringFToHandle(TitlePos.posX, TitlePos.posY, MusicName_.c_str(), color_preset::WHITE, TitleFontHandle_);
    DrawStringFToHandle(ArtistPos.posX, ArtistPos.posY, ArtistName_.c_str(), color_preset::LIGHT_GREY, MiddleFontHandle_);
}
