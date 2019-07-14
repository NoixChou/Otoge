#pragma once
#include "../../../../System/Task/Scene.hpp"
#include "../../../../System/GUI/Button.hpp"
#include "../../../../System/GUI/ScrollablePanel.hpp"
class MusicInfoPanel;

class MusicSelectScene :
    public Scene
{
private:
    int SelectedMusic_ = 0;
    std::shared_ptr<Scene> HeaderPanel_;
    std::shared_ptr<Button> BackButton_;
    std::shared_ptr<ScrollablePanel> ListPanel_;
    std::vector<std::shared_ptr<MusicInfoPanel>> MusicPanels_;

public:
    MusicSelectScene();
    ~MusicSelectScene();

    void OnStartedFadeIn() override;
    void SceneFadeIn(float deltaTime) override;
    void OnStoppedFadeIn() override;

    void SceneFadeOut(float deltaTime) override;
    void OnStoppedFadeOut() override;

    void SceneUpdate(float deltaTime) override;
    void Draw() override;
};

// TODO: âπäyèÓïÒÉNÉâÉX
class MusicInfoPanel :
    public Scene
{
private:
    static int TitleFontHandle_;
    static int MiddleFontHandle_;
    static int SmallFontHandle_;
    static int GlobalPanelCount_;

    std::string MusicName_;
    std::string ArtistName_;
    float Difficulty_;

    float PreLayoutPosX_ = 0.f;

    float DefaultPosX_ = 0.f;

    bool isHoverAnimate = false;

public:
    MusicInfoPanel(const std::string& musicName, const std::string& artistName, float difficulty, std::shared_ptr<FlexibleScaler> parentScaler);
    ~MusicInfoPanel();
    /*
    void SceneFadeIn(float deltaTime) override;
    void OnStoppedFadeIn() override;

    void SceneFadeOut(float deltaTime) override;
    void OnStoppedFadeOut() override;
    */
    void SceneUpdate(float deltaTime) override;
    void Draw() override;
};