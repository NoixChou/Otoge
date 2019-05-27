#pragma once
#include "../../../../System/Task/Scene.hpp"
#include "../../../../System/GUI/Button.hpp"

class MusicInfoPanel;

class MusicSelectScene :
    public Scene
{
private:
    std::shared_ptr<MusicInfoPanel> TestPanel_;
    std::shared_ptr<Button> BackButton_;

public:
    MusicSelectScene();
    ~MusicSelectScene();

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

    float PreLayoutPosY_ = 0.f;

public:
    MusicInfoPanel(const std::string& musicName, const std::string& artistName, float difficulty);
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