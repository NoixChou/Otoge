#pragma once
#include "../../../System/Task/Scene.hpp"
#include "../../../Util/Beatmap/BeatmapScore.hpp"
#include "../../../Util/Beatmap/Beatmap.hpp"
#include "../../../System/GUI/Label.hpp"
#include "../../../System/GUI/Button.hpp"

class GameResultScene : public Scene
{
private:
    std::shared_ptr<BeatmapScore> ResultScore_;
    std::shared_ptr<Beatmap> Beatmap_;

    std::shared_ptr<Scene> MapInfoPanel_;
    std::shared_ptr<Button> BackButton_;
    std::shared_ptr<Label> TitleLabel_;
    std::shared_ptr<Label> ArtistLabel_;

    std::shared_ptr<Scene> ScoreInfoPanel_;
    std::shared_ptr<Label> ScoreLabel_;
    std::shared_ptr<Label> MaxComboCaptionLabel_;
    std::shared_ptr<Label> MaxComboLabel_;
    std::shared_ptr<Label> FullComboLabel_;
    std::shared_ptr<Label> AccuracyCaptionLabel_;
    std::shared_ptr<Label> AccuracyLabel_;
    std::shared_ptr<Label> PerfectLabel_;
    std::shared_ptr<Scene> AccuracyGraph_;

    bool IsFullScreenGraph_ = false;
    bool IsMovingGraph_ = false;
    ScreenData DefaultGraphScreen_;

public:
    GameResultScene(std::shared_ptr<BeatmapScore> score, std::shared_ptr<Beatmap> map);
    ~GameResultScene();

    //void OnInitialize() override;
    void OnTerminate() override;

    /*
    void OnStartedFadeIn() override;
    void OnStoppedFadeIn() override;
    void OnStartedFadeOut() override;
    void OnStoppedFadeOut() override;
    void SceneFadeIn(float deltaTime) override;
    void SceneFadeOut(float deltaTime) override;
    */

    void SceneUpdate(float deltaTime) override;
    void Draw() override;

    void DrawAccuracyGraph();
};
