#pragma once
#include "../../../System/Task/Scene.hpp"
#include "../../../System/GUI/Button.hpp"

// キャラクター共通の情報
#define CHARA_PLAY_ANIM_SPEED			25000.0f		// アニメーション速度
#define CHARA_MOVE_SPEED			3000.0f		// 移動速度
#define CHARA_ANIM_BLEND_SPEED			10.f		// アニメーションのブレンド率変化速度
#define CHARA_ANGLE_SPEED			20.f		// 角度変化速度
#define CHARA_JUMP_POWER			5000.0f		// ジャンプ力
#define CHARA_FALL_UP_POWER			200.0f		// 足を踏み外した時のジャンプ力
#define CHARA_GRAVITY				10000.0f		// 重力
#define CHARA_MAX_HITCOLL			2048		// 処理するコリジョンポリゴンの最大数
#define CHARA_ENUM_DEFAULT_SIZE			800.0f		// 周囲のポリゴン検出に使用する球の初期サイズ
#define CHARA_HIT_WIDTH				200.0f		// 当たり判定カプセルの半径
#define CHARA_HIT_HEIGHT			700.0f		// 当たり判定カプセルの高さ
#define CHARA_HIT_TRYNUM			16		// 壁押し出し処理の最大試行回数
#define CHARA_HIT_SLIDE_LENGTH			5.0f		// 一度の壁押し出し処理でスライドさせる距離
#define CHARA_HIT_PUSH_POWER			40.0f		// キャラクター同士で当たったときの押し出される力
#define CHARA_SHADOW_SIZE			200.0f		// 影の大きさ
#define CHARA_SHADOW_HEIGHT			700.0f		// 影が落ちる高さ

// カメラ関係の定義
#define CAMERA_ANGLE_SPEED			2.5f		// 旋回速度
#define CAMERA_PLAYER_TARGET_HEIGHT		400.0f		// プレイヤー座標からどれだけ高い位置を注視点とするか
#define CAMERA_PLAYER_LENGTH			1600.0f		// プレイヤーとの距離
#define CAMERA_COLLISION_SIZE			50.0f		// カメラの当たり判定サイズ

// プレイヤーキャラ以外キャラの情報
#define NOTPLAYER_NUM				100		// プレイヤー以外のキャラの数
#define NOTPLAYER_MOVETIME			120		// 一つの方向に移動する時間
#define NOTPLAYER_JUMPRATIO			250		// プレイヤー以外のキャラがジャンプする確率

class Game3DTest : public Scene
{
public:
    // キャラクター情報
    struct CHARA
    {
        VECTOR Position;				// 座標
        VECTOR TargetMoveDirection;			// モデルが向くべき方向のベクトル
        float Angle;					// モデルが向いている方向の角度
        float JumpPower;				// Ｙ軸方向の速度
        int ModelHandle;				// モデルハンドル
        int State;					// 状態( 0:立ち止まり  1:走り  2:ジャンプ )
        int PlayAnim1;				// 再生しているアニメーション１のアタッチ番号( -1:何もアニメーションがアタッチされていない )
        float AnimPlayCount1;			// 再生しているアニメーション１の再生時間
        int PlayAnim2;				// 再生しているアニメーション２のアタッチ番号( -1:何もアニメーションがアタッチされていない )
        float AnimPlayCount2;			// 再生しているアニメーション２の再生時間
        float AnimBlendRate;				// 再生しているアニメーション１と２のブレンド率
    };

    // プレイヤー情報構造体
    struct PLAYER
    {
        CHARA CharaInfo;				// キャラクター情報
    };

    // プレイヤー以外キャラ情報構造体
    struct NOTPLAYER
    {
        CHARA CharaInfo;				// キャラクター情報
        int MoveTime;				// 移動時間
        float MoveAngle;				// 移動方向
    };

    // キャラクター共通情報
    struct CHARA_COMMON
    {
        int BaseModelHandle;			// 共通の派生元モデルハンドル
        int ShadowHandle;				// 影描画用のグラフィックハンドル
    };

    // ステージ情報構造体
    struct STAGE
    {
        int ModelHandle;				// モデルハンドル
    };

    // カメラ情報構造体
    struct CAMERA
    {
        float AngleH;				// 水平角度
        float AngleV;				// 垂直角度
        VECTOR Eye;					// カメラ座標
        VECTOR Target;				// 注視点座標
    };

private:
    PLAYER pl;		// プレイヤー情報の実体宣言
    NOTPLAYER npl[NOTPLAYER_NUM];	// プレイヤーではないキャラの実体宣言
    CHARA_COMMON chcmn;	// キャラクターの共通情報の実体宣言
    STAGE stg;		// ステージ情報の実体宣言
    CAMERA cam;		// カメラ情報の実体宣言
    float DeltaTime_ = 0.f;
    float FixedCharaMoveSpeed_ = 0.f;
    float FixedCharaAnimSpeed_ = 0.f;
    float FixedCameraRotSpeed_ = 0.f;

    // 影用の深度値を保存する描画可能グラフィック
    int DepthBufferGraphHandle;
    int ScaleDownGraphHandle;
    int BloomBufferGraphHandle;

    // 影用の深度記録画像を作成した際のカメラのビュー行列と射影行列
    MATRIX LightCamera_ViewMatrix;
    MATRIX LightCamera_ProjectionMatrix;

    // 深度記録画像への描画用の剛体メッシュ用頂点シェーダーと
    // スキニングメッシュ用の頂点シェーダー
    int Skin4_DepthShadow_Step1_VertexShader;
    int Normal_DepthShadow_Step1_VertexShader;

    // 深度記録画像への描画用のピクセルシェーダー
    int DepthShadow_Step1_PixelShader;

    // 深度記録画像を使ったディレクショナルライト一つ付きの描画用の剛体メッシュ用頂点シェーダーと
    // スキニングメッシュ用の頂点シェーダー
    int Skin4_DirLight_DepthShadow_Step2_VertexShader;
    int Normal_DirLight_DepthShadow_Step2_VertexShader;

    // 深度記録画像を使ったディレクショナルライト一つ付きの描画用のピクセルシェーダー
    int DirLight_DepthShadow_Step2_PixelShader;
    std::shared_ptr<Button> BackButton_;
    std::shared_ptr<Label> DebugLabel_;
public:
    Game3DTest();
    ~Game3DTest();
    void OnStartedFadeIn() override;
    void OnStartedFadeOut() override;
    void SceneFadeIn(float deltaTime) override;
    void SceneFadeOut(float deltaTime) override;
    void SceneUpdate(float deltaTime) override;
    void Draw() override;
    void CharaCommon_Initialize(void);
    void CharaCommon_Terminate(void);
    void Chara_Initialize(CHARA* ch, VECTOR Position);			// キャラクターの初期化
    void Chara_Terminate(CHARA* ch);					// キャラクターの後始末
    void Chara_Process(CHARA* ch, VECTOR MoveVec, int JumpFlag);		// キャラクターの処理
    void Chara_Move(CHARA* ch, VECTOR MoveVector);			// キャラクターの移動処理
    void Chara_Collision(CHARA* ch, VECTOR* ch_MoveVec,
                         CHARA* chk_ch);	// キャラクターに当たっていたら押し出す処理を行う( chk_ch に ch が当たっていたら ch が離れる )
    void Chara_AngleProcess(CHARA* ch);					// キャラクターの向きを変える処理
    void Chara_PlayAnim(CHARA* ch, int PlayAnim);			// キャラクターに新たなアニメーションを再生する
    void Chara_AnimProcess(CHARA* ch);					// キャラクターのアニメーション処理
    void Player_Initialize(void);		// プレイヤーの初期化
    void Player_Terminate(void);			// プレイヤーの後始末
    void Player_Process(void);			// プレイヤーの処理
    void NotPlayer_Initialize(void);
    void NotPlayer_Terminate(void);
    void NotPlayer_Process(void);
    void Stage_Initialize(void);			// ステージの初期化処理
    void Stage_Terminate(void);			// ステージの後始末処理
    void Camera_Initialize(void);		// カメラの初期化処理
    void Camera_Process(void);			// カメラの処理
    void SetupDepthImage(void);			// 影描画用の深度記録画像を準備する
    void DrawModelWithDepthShadow(void);	// 影用の深度記録画像を使った影を落とす処理も含めたモデルの描画
    void Render_Process(void);			// 描画処理
};
