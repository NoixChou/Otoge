#pragma once
#include "../../../System/Task/Scene.hpp"
#include "../../../Util/Beatmap/Beatmap.hpp"
#include "../../../System/GUI/Label.hpp"
#include "../../../System/GUI/Button.hpp"
#include "../../../Util/Visual/Color.hpp"
#include "../../../System/GUI/SlideBar.hpp"
#include "../../../Util/Beatmap/BeatmapScore.hpp"

class MainGameScene : public Scene
{
private:
    int* LaneKeys_;
    int LaneColor_ = color_preset::GREY;

    const float JudgeLinePosY_ = 75.f;
    int DefaultMusicStreamVol_;

    bool IsMusicPlaying_ = false;

    std::shared_ptr<BeatmapScore> ScoreData_;

    std::shared_ptr<Beatmap> Beatmap_;

    std::shared_ptr<Scene> GamePanel_;
    std::shared_ptr<Scene> InfoPanel_;

    std::shared_ptr<Label> BPMLabel_;
    std::shared_ptr<Label> BarCountLabel_;
    std::shared_ptr<Label> TimingCountLabel_;
    std::shared_ptr<Label> LoadingLabel_;

    std::shared_ptr<Scene> ScorePanel_;
    std::shared_ptr<Label> ScoreLabel_;
    std::shared_ptr<Label> AccuracyLabel_;
    std::shared_ptr<Label> ComboLabel_;

    std::map<Notes::HitsType, std::pair<std::string, unsigned>> JudgeText_;

public:
    MainGameScene(std::shared_ptr<Beatmap> map);
    ~MainGameScene();

    void OnInitialize() override;
    void OnTerminate() override;

    void OnStartedFadeIn() override;
    void OnStoppedFadeIn() override;
    void OnStartedFadeOut() override;
    void OnStoppedFadeOut() override;
    void SceneFadeIn(float deltaTime) override;
    void SceneFadeOut(float deltaTime) override;

    void SceneUpdate(float deltaTime) override;
    void Draw() override;

    void DrawGamePanel();
};
