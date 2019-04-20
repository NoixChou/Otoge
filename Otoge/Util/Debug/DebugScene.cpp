#include "DebugScene.hpp"
#include "../../System/Task/TaskManager.hpp"
#include "../Setting/SettingManager.h"
#include "../../System/Config.h"
#include "../../System/Input/KeyboardManager.hpp"
#include "../../System/GUI/Button.hpp"

DebugScene::DebugScene() : Scene("DebugScene")
{
    DefaultScaler_->lockTop = false;
    DefaultScaler_->lockBottom = true;

    FpsLabel_ = std::make_shared<Label>("FPS", ScreenData(1.f, 94.f, 10.f,2.5f), DefaultScaler_);
    //FpsLabel_->SetTransparent(80.f);
    FpsLabel_->baseColor = GetColor(180, 255, 180);
    
    AddChildTask(std::static_pointer_cast<Task>(FpsLabel_));

    DeltaTimeLabel_ = std::make_shared<Label>("ms", ScreenData(1.f, 96.f, 10.f, 2.0f), DefaultScaler_);
    DeltaTimeLabel_->baseColor = GetColor(255, 255, 180);
    //DeltaTimeLabel_->SetTransparent(80.f);
    AddChildTask(std::static_pointer_cast<Task>(DeltaTimeLabel_));

    SetTransparent(50.f);
    SetPriority(100.f);
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
        SetVisible(!IsVisible());
    }

    

    FpsLabel_->SetLabel(std::to_string(static_cast<int>(floor(TaskManager::GetInstance()->GetFrameRate()))) + " FPS");
    if(timerCount > 0.25f)
        DeltaTimeLabel_->SetLabel(std::to_string(deltaTime * 1000.f) + "ms"), timerCount = 0.f;
}

void DebugScene::Draw()
{
    ScreenData l_FpsScreen;
    l_FpsScreen.posX = FpsLabel_->GetRawPositionX();
    l_FpsScreen.posY = FpsLabel_->GetRawPositionY();
    l_FpsScreen.width = FpsLabel_->GetRawScreenWidth();
    l_FpsScreen.height = FpsLabel_->GetRawScreenHeight();
    ScreenData l_DeltaTimeScreen;
    l_DeltaTimeScreen.posX = DeltaTimeLabel_->GetRawPositionX();
    l_DeltaTimeScreen.posY = DeltaTimeLabel_->GetRawPositionY();
    l_DeltaTimeScreen.width = DeltaTimeLabel_->GetRawScreenWidth();
    l_DeltaTimeScreen.height = DeltaTimeLabel_->GetRawScreenHeight();

    SetDrawBlendMode(DX_BLENDMODE_PMA_ALPHA, 127);
    DrawBox(static_cast<int>(floor(l_FpsScreen.posX)),
            static_cast<int>(floor(l_FpsScreen.posY)),
            static_cast<int>(floor(l_FpsScreen.posX + l_FpsScreen.width)),
            static_cast<int>(floor(l_FpsScreen.posY + l_FpsScreen.height)),
               GetColor(0, 180, 255), TRUE
    );
    DrawBox(static_cast<int>(floor(l_DeltaTimeScreen.posX)),
            static_cast<int>(floor(l_DeltaTimeScreen.posY)),
            static_cast<int>(floor(l_DeltaTimeScreen.posX + l_DeltaTimeScreen.width)),
            static_cast<int>(floor(l_DeltaTimeScreen.posY + l_DeltaTimeScreen.height)),
               GetColor(180, 255, 0), TRUE
    );
}
