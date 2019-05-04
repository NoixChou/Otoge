#include "Util/Debug/Logger.h"
#include "Util/Debug/DebugScene.hpp"
#include "Util/Setting/SettingManager.h"
#include "Game/Scenes/Title/TitleScene.hpp"
#include "System/Task/TaskManager.hpp"
#include "System/Input/KeyboardManager.hpp"
#include "System/Input/MouseManager.hpp"

#include "System/Config.h"
#include "Util/Window/DxSettings.hpp"
#include "System/GlobalMethod.hpp"
#include "System/Task/CursorDrawer.hpp"
#include "Util/Calculate/Animation/Easing.hpp"
#include "Util/Visual/Color.hpp"
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
    g_SystemSettings->SetDefault(game_config::SETTINGS_FULLSCREEN, false);
    g_SystemSettings->SetDefault(game_config::SETTINGS_VSYNC, false);
    g_SystemSettings->SetDefault(game_config::SETTINGS_RES_WIDTH, 1920);
    g_SystemSettings->SetDefault(game_config::SETTINGS_RES_HEIGHT,1080);
    g_SystemSettings->SetDefault(game_config::SETTINGS_AA_SAMPLE, 2);
    g_SystemSettings->SetDefault(game_config::SETTINGS_AA_QUALITY, 2);

    g_SystemSettings->SetDefault<std::string>(game_config::SETTINGS_FONT_NAME, game_config::GAME_APP_DEFAULT_FONT);
    g_SystemSettings->SetDefault<std::string>(game_config::SETTINGS_ALPHABET_FONT_NAME, game_config::GAME_APP_DEFAULT_FONT);
    g_SystemSettings->SetDefault<std::string>(game_config::SETTINGS_NUMBER_FONT_NAME, game_config::GAME_APP_DEFAULT_FONT);
	g_SystemSettings->SetDefault(game_config::SETTINGS_FONT_DRAWTYPE, DX_FONTTYPE_NORMAL);

    g_SystemSettings->SetDefault(game_config::SETTINGS_DEBUG_DRAW_SCENE_FRAME, false);
	g_SystemSettings->SetDefault(game_config::SETTINGS_DEBUG_DRAW_DTASK_POINT, false);

    g_SystemSettings->SetDefault(game_config::SETTINGS_MOUSE_AREA_LIMIT, false);
    g_SystemSettings->SetDefault(game_config::SETTINGS_MOUSE_USEORIGINAL, true);
    g_SystemSettings->Save();
    g_SystemSettings->SetGlobal();

    DxSettings::windowWidth = g_SystemSettings->Get<int>(game_config::SETTINGS_RES_WIDTH).get();
    DxSettings::windowHeight = g_SystemSettings->Get<int>(game_config::SETTINGS_RES_HEIGHT).get();
    DxSettings::isFullScreen = g_SystemSettings->Get<bool>(game_config::SETTINGS_FULLSCREEN).get();
    DxSettings::doVSync = g_SystemSettings->Get<bool>(game_config::SETTINGS_VSYNC).get();
    DxSettings::antialiasingSample = g_SystemSettings->Get<int>(game_config::SETTINGS_AA_SAMPLE).get();
    DxSettings::antialiasingQuality = g_SystemSettings->Get<int>(game_config::SETTINGS_AA_QUALITY).get();

    DxSettings::useOriginalCursor = g_SystemSettings->Get<bool>(game_config::SETTINGS_MOUSE_USEORIGINAL).get();

    ChangeWindowMode(!DxSettings::isFullScreen); // ウィンドウモード/フルスクリーン
    SetUseCharCodeFormat(DX_CHARCODEFORMAT_UTF8); // 文字コード
    SetMainWindowText((static_cast<std::string>(game_config::GAME_APP_NAME) + " v" + static_cast<std::string>(game_config::GAME_APP_VER)).c_str()); // ウィンドウのタイトル
    SetAlwaysRunFlag(TRUE); // 常に処理
    SetWaitVSyncFlag(engine::CastToInt(DxSettings::doVSync)); // 垂直同期
    SetFontUseAdjustSizeFlag(FALSE);
    SetUseFPUPreserveFlag(TRUE);
    SetGraphMode(DxSettings::windowWidth, DxSettings::windowHeight, 32);
    SetFullSceneAntiAliasingMode(DxSettings::antialiasingSample, DxSettings::antialiasingQuality);

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

    SetDrawMode(DX_DRAWMODE_BILINEAR);
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

    if(DxSettings::useOriginalCursor)
    {
        auto cursorDrawer = make_shared<CursorDrawer>();
        cursorDrawer->SetDrawFunction([=]
            {
                //LoadGraphScreen(MouseManager::GetInstance()->GetMouseX() - 32, MouseManager::GetInstance()->GetMouseY() - 32, "cursor.png", TRUE);
                float cursorSize = 1.0f;
                static float currentSize = 0.f;
                const float totalTime = 0.5f;

                const float ReleasedSize = 1.4f;
                const float HoldingSize = 0.7f;
                Easing::EaseFunction ease = Easing::OutExp;

                if (MouseManager::GetInstance()->IsDownButton(MOUSE_INPUT_LEFT))
                    currentSize = cursorDrawer->GetDefaultScaler()->CalculateHeight(ReleasedSize), cursorDrawer->timerCount = 0.f;
                if (MouseManager::GetInstance()->IsReleaseButton(MOUSE_INPUT_LEFT))
                    currentSize = cursorDrawer->GetDefaultScaler()->CalculateHeight(HoldingSize), cursorDrawer->timerCount = 0.f;
                if (MouseManager::GetInstance()->IsHoldButton(MOUSE_INPUT_LEFT))
                    cursorSize = ease(cursorDrawer->timerCount, totalTime, cursorDrawer->GetDefaultScaler()->CalculateHeight(HoldingSize), currentSize);
                else
                    cursorSize = Easing::OutExp(cursorDrawer->timerCount, totalTime, cursorDrawer->GetDefaultScaler()->CalculateHeight(ReleasedSize), currentSize);

                if(cursorDrawer->timerCount > totalTime)
                {
                    cursorDrawer->timerCount = totalTime;
                }
                
                DrawCircle(MouseManager::GetInstance()->GetMouseX(), MouseManager::GetInstance()->GetMouseY(), engine::CastToInt(cursorSize) - 1, color_preset::WHITE, TRUE);
                DrawCircle(MouseManager::GetInstance()->GetMouseX(), MouseManager::GetInstance()->GetMouseY(), engine::CastToInt(cursorSize), color_preset::BLACK, FALSE);
            });
        TaskManager::GetInstance()->AddTask(static_pointer_cast<Task>(cursorDrawer));

    }
}

// メインループ
void Loop()
{
    while(ProcessMessage() != -1 && !TaskManager::GetInstance()->IsGameExit())
    {
		if(KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_A))
	        TaskManager::GetInstance()->Tick(0.5f);
		else
			TaskManager::GetInstance()->Tick(1.0f);
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
