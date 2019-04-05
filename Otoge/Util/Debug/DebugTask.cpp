#include "DebugScene.hpp"
#include "../../System/Task/TaskManager.hpp"
#include "../Setting/SettingManager.h"
#include "../../System/Config.h"

DebugScene::DebugScene(): Task("DebugScene")
{
    FpsLabel_ = std::make_shared<Label>("FPS", &ScreenData(0, SettingManager::GetGlobal()->Get<float>(SETTINGS_RES_WIDTH).get() - 48.f, 140.f, 16.f));
    FpsLabel_->baseColor = GetColor(180, 255, 180);
    AddChildTask(std::static_pointer_cast<Task>(FpsLabel_));

    DeltaTimeLabel_ = std::make_shared<Label>("ms", &ScreenData(0, SettingManager::GetGlobal()->Get<float>(SETTINGS_RES_WIDTH).get() - 32.f, 140.f, 16.f));
    DeltaTimeLabel_->baseColor = GetColor(180, 255, 180);
    AddChildTask(std::static_pointer_cast<Task>(DeltaTimeLabel_));
}

DebugScene::~DebugScene()
{
}

void DebugScene::Update(float deltaTime)
{
    printfDxDebugMessage(0, *Window_SizeY - 48, dx_GetColor(180, 255, 180), "%fFPS", FramePerSecond);
    printfDxDebugMessage(0, *Window_SizeY - 32, dx_GetColor(255, 255, 180), "%fms   %ftps", GetTickTime() / 1000.f, GetTickSpeed());

    printfDx("%.2f FPS   fixed deltaTime: %f", TaskManager::GetInstance()->GetFrameRate(), deltaTime);
}
