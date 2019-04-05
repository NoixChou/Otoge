#include "DebugScene.hpp"
#include "../../System/Task/TaskManager.hpp"
#include "../Setting/SettingManager.h"
#include "../../System/Config.h"
#include "../../System/Input/KeyboardManager.hpp"

DebugScene::DebugScene() : Scene("DebugScene")
{
    DefaultScaler_->lockTop = false;
    DefaultScaler_->lockBottom = true;
    FpsLabel_ = std::make_shared<Label>("FPS", ScreenData(1.f, 94.f, 10.f,2.5f), DefaultScaler_);
    FpsLabel_->SetTransparent(50.f);
    FpsLabel_->baseColor = GetColor(180, 255, 180);
    
    AddChildTask(std::static_pointer_cast<Task>(FpsLabel_));

    DeltaTimeLabel_ = std::make_shared<Label>("ms", ScreenData(1.f, 96.f, 10.f, 2.0f), DefaultScaler_);
    DeltaTimeLabel_->baseColor = GetColor(255, 255, 180);
    DeltaTimeLabel_->SetTransparent(20.f);
    AddChildTask(std::static_pointer_cast<Task>(DeltaTimeLabel_));
}

DebugScene::~DebugScene()
{
}

void DebugScene::SceneUpdate(float deltaTime)
{
    if (KeyboardManager::GetInstance()->IsDownKey(KEY_INPUT_ESCAPE))
        TaskManager::GetInstance()->GameExit();
    if(KeyboardManager::GetInstance()->IsDownKey(KEY_INPUT_1))
    {
        FpsLabel_->SetVisible(!FpsLabel_->IsVisible());
        DeltaTimeLabel_->SetVisible(!DeltaTimeLabel_->IsVisible());
    }

    

    FpsLabel_->SetLabel(std::to_string(static_cast<int>(floor(TaskManager::GetInstance()->GetFrameRate()))) + " FPS");
    if(timerCount > 0.25f)
        DeltaTimeLabel_->SetLabel(std::to_string(deltaTime * 1000.f) + "ms"), timerCount = 0.f;
}

void DebugScene::Draw()
{
}
