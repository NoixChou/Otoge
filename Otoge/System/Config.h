#pragma once

// アプリケーション
#define GAME_APP_NAME "おとげー" // アプリ名
#define GAME_APP_VER "0.0.1" // バージョン

// 画面
#define GAME_SCREEN_WIDTH 1024
#define GAME_SCREEN_HEIGHT 720

constexpr auto GAME_APP_DEFAULT_FONT = "Noto Sans CJK JP Thin"; // デフォルトのフォント

// 設定ファイル
constexpr auto SETTINGS_PATH = "system.";

constexpr auto SETTINGS_FULLSCREEN = "system.graphic.fullscreen";
constexpr auto SETTINGS_VSYNC = "system.graphic.vsync";
constexpr auto SETTINGS_RES_WIDTH = "system.graphic.resolution.width";
constexpr auto SETTINGS_RES_HEIGHT = "system.graphic.resolution.height";
constexpr auto SETTINGS_AA_SAMPLE = "system.graphic.antialiasing.sample";
constexpr auto SETTINGS_AA_QUALITY = "system.graphic.antialiasing.quality";

constexpr auto SETTINGS_FONT_NAME = "system.display.font.default";
constexpr auto SETTINGS_FONT_DRAWTYPE = "system.display.font.drawType";

constexpr auto SETTINGS_DEBUG_DRAW_SCENE_FRAME = "system.debug.scene.isDrawFrame";
constexpr auto SETTINGS_DEBUG_DRAW_DTASK_POINT = "system.debug.drawable.isDrawPoint";
