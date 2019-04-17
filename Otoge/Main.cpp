#include "Util/Debug/Logger.h"
#include "Util/Debug/DebugScene.hpp"
#include "Util/Setting/SettingManager.h"
#include "Game/Scenes/Title/TitleScene.hpp"
#include "System/Task/TaskManager.hpp"
#include "System/Input/KeyboardManager.hpp"
#include "System/Input/MouseManager.hpp"

#include "System/Config.h"

using namespace std;

// 前方宣言
void PreInitialize();
void Initialize();
void Loop();
void Terminate();

// 宣言
shared_ptr<Logger> g_Logger;
shared_ptr<SettingManager> g_SystemSettings;

// エントリー
int WINAPI WinMain(const HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    Initialize();
    
    Loop();

    Terminate();

    return 0;
}

// 基本機能/事前初期化
void PreInitialize()
{
    g_Logger = make_shared<Logger>();
    g_SystemSettings = make_shared<SettingManager>("config.json");
    g_SystemSettings->Load(true);
    g_SystemSettings->SetDefault(SETTINGS_FULLSCREEN, false);
    g_SystemSettings->SetDefault(SETTINGS_VSYNC, false);
    g_SystemSettings->SetDefault(SETTINGS_RES_WIDTH, 1024);
    g_SystemSettings->SetDefault(SETTINGS_RES_HEIGHT, 720);
    g_SystemSettings->SetDefault(SETTINGS_AA_SAMPLE, 2);
    g_SystemSettings->SetDefault(SETTINGS_AA_QUALITY, 2);

	g_SystemSettings->SetDefault<std::string>(SETTINGS_FONT_NAME, GAME_APP_DEFAULT_FONT);
	g_SystemSettings->SetDefault(SETTINGS_FONT_DRAWTYPE, DX_FONTTYPE_ANTIALIASING);

    g_SystemSettings->SetDefault(SETTINGS_DEBUG_DRAW_SCENE_FRAME, false);
	g_SystemSettings->SetDefault(SETTINGS_DEBUG_DRAW_DTASK_POINT, false);
    g_SystemSettings->Save();
    g_SystemSettings->SetGlobal();

    ChangeWindowMode(!g_SystemSettings->Get<bool>(SETTINGS_FULLSCREEN).get()); // ウィンドウモード/フルスクリーン
    SetMainWindowText(GAME_APP_NAME " v" GAME_APP_VER); // ウィンドウのタイトル
    SetAlwaysRunFlag(TRUE); // 常に処理
    SetWaitVSyncFlag(g_SystemSettings->Get<bool>(SETTINGS_VSYNC).get()); // 垂直同期
    SetUseFPUPreserveFlag(TRUE);
    SetGraphMode(g_SystemSettings->Get<int>(SETTINGS_RES_WIDTH).get(), g_SystemSettings->Get<int>(SETTINGS_RES_HEIGHT).get(), 32);
    SetFullSceneAntiAliasingMode(g_SystemSettings->Get<int>(SETTINGS_AA_SAMPLE).get(), g_SystemSettings->Get<int>(SETTINGS_AA_QUALITY).get());

	SetChangeScreenModeGraphicsSystemResetFlag(FALSE);
}

// 初期化
void Initialize()
{
    PreInitialize();

    if(DxLib_Init() == -1)
    {
        // DXライブラリ 初期化失敗
        g_Logger->Critical("DXライブラリ初期化 失敗");
        MessageBox(nullptr, "DXライブラリの初期化に失敗しました。", "error", MB_OK | MB_ICONERROR);
        exit(-1);
    }
    g_Logger->Info("DXライブラリ初期化 成功");

    // 3D設定
    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);
    SetDrawScreen(DX_SCREEN_BACK);

    SetMouseDispFlag(TRUE); // マウスカーソルの表示

    // コンポーネント初期化
    FlexibleScaler::CreateWindowBasedInstance();
    FlexibleScaler::GetWindowBasedInstance()->SetScale(1.0f);
    TaskManager::CreateInstance();
    KeyboardManager::CreateInstance();
    MouseManager::CreateInstance();

    // タスク追加
    TaskManager::GetInstance()->AddTask(static_pointer_cast<Task>(KeyboardManager::GetInstance()));
    TaskManager::GetInstance()->AddTask(static_pointer_cast<Task>(MouseManager::GetInstance()));
    TaskManager::GetInstance()->AddTask(static_pointer_cast<Task>(make_shared<DebugScene>()));
	TaskManager::GetInstance()->AddTask(static_pointer_cast<Task>(make_shared<TitleScene>()));
}

// メインループ
void Loop()
{
    while(ProcessMessage() != -1 && !TaskManager::GetInstance()->IsGameExit())
    {
		if(KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_A))
	        TaskManager::GetInstance()->Tick(0.05f);
		else
			TaskManager::GetInstance()->Tick(1.0f);

		if(KeyboardManager::GetInstance()->IsDownKey(KEY_INPUT_R))
		{
			Terminate();

			FlexibleScaler::CreateWindowBasedInstance();
			FlexibleScaler::GetWindowBasedInstance()->SetScale(1.0f);
			TaskManager::CreateInstance();
			KeyboardManager::CreateInstance();
			MouseManager::CreateInstance();
			
			TaskManager::GetInstance()->AddTask(static_pointer_cast<Task>(KeyboardManager::GetInstance()));
			TaskManager::GetInstance()->AddTask(static_pointer_cast<Task>(MouseManager::GetInstance()));
			TaskManager::GetInstance()->AddTask(static_pointer_cast<Task>(make_shared<DebugScene>()));
			TaskManager::GetInstance()->AddTask(static_pointer_cast<Task>(make_shared<TitleScene>()));
		}
    }
}

// 終了処理
void Terminate()
{
    TaskManager::DestroyInstance();
    KeyboardManager::DestroyInstance();
    MouseManager::DestroyInstance();
    FlexibleScaler::DestroyWindowBasedInstance();
}