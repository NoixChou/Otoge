#pragma once
#include "../../../System/Task/Scene.hpp"
#include "../../../Util/Beatmap/Beatmap.hpp"
#include "../../../System/GUI/Label.hpp"
#include "../../../System/GUI/Button.hpp"

class MainGameScene : public Scene
{
private:
    const float JudgeLinePosY_ = 75.f;
    int defaultMusicStreamVol_;

    bool IsMusicPlaying_ = false;

    std::shared_ptr<Beatmap> Beatmap_;

    std::shared_ptr<Label> BPMLabel_;
    std::shared_ptr<Label> BarCountLabel_;
    std::shared_ptr<Label> TimingCountLabel_;
    std::shared_ptr<Label> LoadingLabel_;

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
};
