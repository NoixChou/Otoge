#include "Game3DTest.hpp"
#include "../../../System/Input/KeyboardManager.hpp"
#include "../../../System/Task/TaskManager.hpp"
#include "../Title/TitleScene.hpp"
#include "../../../System/Input/MouseManager.hpp"
#include "../../../Util/Calculate/Animation/Easing.hpp"
#include "../../../System/GlobalMethod.hpp"
#include "../../../Util/Visual/Color.hpp"

Game3DTest::Game3DTest(): Scene("Game3DTest", 100.f, 100.f)
{
    // キャラクターの共通情報を初期化
    CharaCommon_Initialize();

    // プレイヤーの初期化
    Player_Initialize();

    // プレイヤー以外キャラの初期化
    NotPlayer_Initialize();

    // ステージの初期化
    Stage_Initialize();

    // カメラの初期化
    Camera_Initialize();

    BackButton_ = std::make_shared<Button>("< Back", ScreenData(0.f, 0.f, 10.f, 4.0f), DefaultScaler_);
    BackButton_->GetTextLabelInstance()->AdjustmentFontSize_ = false;
    BackButton_->GetTextLabelInstance()->ChangeFontSize(static_cast<int>(DefaultScaler_->CalculateHeight(2.f)));
    BackButton_->GetTextLabelInstance()->ChangeFontThickness(1);
    AddChildTask(std::static_pointer_cast<Task>(BackButton_));
    DebugLabel_ = std::make_shared<Label>("DebugLabel", ScreenData(0.f, 0.f, 10.f, 2.0f), DefaultScaler_);
    DebugLabel_->baseColor = color_preset::WHITE;
    StartFadeIn();
}

Game3DTest::~Game3DTest()
{
    // プレイヤー以外キャラの後始末
    NotPlayer_Terminate();

    // プレイヤーの後始末
    Player_Terminate();

    // キャラクター共通情報の後始末
    CharaCommon_Terminate();

    // ステージの後始末
    Stage_Terminate();
}

void Game3DTest::OnStartedFadeIn()
{
}

void Game3DTest::OnStartedFadeOut()
{
}

void Game3DTest::SceneFadeIn(float deltaTime)
{
    const float totalTime = 1.0f;
    Easing::EaseFunction ease = Easing::OutExp;
    SetTransparent(engine::CastToFloat(ease(timerCount, totalTime, 100.f, 0.f)));
    SetPositionX(engine::CastToFloat(ease(timerCount, totalTime, 0.f, -GetScreenWidth())));
    if(timerCount > totalTime)
    {
        IsFadingIn_ = false;
    }
}

void Game3DTest::SceneFadeOut(float deltaTime)
{
    const float totalTime = 1.0f;
    Easing::EaseFunction ease = Easing::OutExp;
    SetTransparent(engine::CastToFloat(ease(timerCount, totalTime, 0.f, 100.f)));
    SetPositionX(engine::CastToFloat(ease(timerCount, totalTime, -GetScreenWidth(), 0.f)));
    if(timerCount > totalTime)
    {
        IsFadingOut_ = false;
        TaskManager::GetInstance()->AddTask(std::static_pointer_cast<Task>(std::make_shared<TitleScene>()));
        Terminate();
    }
}

void Game3DTest::SceneUpdate(float deltaTime)
{
    DeltaTime_ = deltaTime;
    FixedCharaMoveSpeed_ = CHARA_MOVE_SPEED * deltaTime;
    FixedCharaAnimSpeed_ = CHARA_PLAY_ANIM_SPEED * deltaTime;
    FixedCameraRotSpeed_ = CAMERA_ANGLE_SPEED * deltaTime;
    if(BackButton_->IsClickedMouse())
    {
        StartFadeOut();
    }
}

void Game3DTest::Draw()
{
    // プレイヤー以外キャラの処理
    NotPlayer_Process();

    // プレイヤーの処理
    Player_Process();

    // カメラの処理
    Camera_Process();

    // 描画処理
    Render_Process();
}


// キャラクターの共通情報を初期化
void Game3DTest::CharaCommon_Initialize(void)
{
    // モデルの読み込み
    chcmn.BaseModelHandle = MV1LoadModel("DxChara.x");

    // 影描画用の画像の読み込み
    chcmn.ShadowHandle = LoadGraph("Shadow.tga");
}

// キャラクター共通情報の後始末
void Game3DTest::CharaCommon_Terminate(void)
{
    // モデルの削除
    MV1DeleteModel(chcmn.BaseModelHandle);

    // 影用画像の削除
    DeleteGraph(chcmn.ShadowHandle);
}

// キャラクターの初期化
void Game3DTest::Chara_Initialize(CHARA* ch, VECTOR Position)
{
    // 初期座標は原点
    ch->Position = Position;

    // 回転値は０
    ch->Angle = 0.0f;

    // ジャンプ力は初期状態では０
    ch->JumpPower = 0.0f;

    // モデルハンドルの作成
    ch->ModelHandle = MV1DuplicateModel(chcmn.BaseModelHandle);

    // 初期状態では「立ち止り」状態
    ch->State = 0;

    // 初期状態はＸ軸方向
    ch->TargetMoveDirection = VGet(1.0f, 0.0f, 0.0f);

    // アニメーションのブレンド率を初期化
    ch->AnimBlendRate = 1.0f;

    // 初期状態ではアニメーションはアタッチされていないにする
    ch->PlayAnim1 = -1;
    ch->PlayAnim2 = -1;

    // ただ立っているアニメーションを再生
    Chara_PlayAnim(ch, 4);
}

// キャラクターの後始末
void Game3DTest::Chara_Terminate(CHARA* ch)
{
    // モデルの削除
    MV1DeleteModel(ch->ModelHandle);
}

// キャラクターの処理
void Game3DTest::Chara_Process(CHARA* ch, VECTOR MoveVec, int JumpFlag)
{
    int MoveFlag;			// 移動したかどうかのフラグ( 1:移動した  0:移動していない )
    // ルートフレームのＺ軸方向の移動パラメータを無効にする
    {
        MATRIX LocalMatrix;

        // ユーザー行列を解除する
        MV1ResetFrameUserLocalMatrix(ch->ModelHandle, 2);

        // 現在のルートフレームの行列を取得する
        LocalMatrix = MV1GetFrameLocalMatrix(ch->ModelHandle, 2);

        // Ｚ軸方向の平行移動成分を無効にする
        LocalMatrix.m[3][2] = 0.0f;

        // ユーザー行列として平行移動成分を無効にした行列をルートフレームにセットする
        MV1SetFrameUserLocalMatrix(ch->ModelHandle, 2, LocalMatrix);
    }

    // 移動したかどうかのフラグをセット、少しでも移動していたら「移動している」を表す１にする
    MoveFlag = 0;
    if(MoveVec.x < -0.001f || MoveVec.x > 0.001f || MoveVec.y < -0.001f || MoveVec.y > 0.001f || MoveVec.z < -0.001f ||
        MoveVec.z > 0.001f)
    {
        MoveFlag = 1;
    }

    // キャラクターの状態が「ジャンプ」ではなく、且つジャンプフラグが立っていたらジャンプする
    if(ch->State != 2 && JumpFlag == 1)
    {
        // 状態を「ジャンプ」にする
        ch->State = 2;

        // Ｙ軸方向の速度をセット
        ch->JumpPower = CHARA_JUMP_POWER;

        // ジャンプアニメーションの再生
        Chara_PlayAnim(ch, 2);
    }

    // 移動ボタンが押されたかどうかで処理を分岐
    if(MoveFlag)
    {
        // 移動ベクトルを正規化したものをキャラクターが向くべき方向として保存
        ch->TargetMoveDirection = VNorm(MoveVec);

        // もし今まで「立ち止まり」状態だったら
        if(ch->State == 0)
        {
            // 走りアニメーションを再生する
            Chara_PlayAnim(ch, 1);

            // 状態を「走り」にする
            ch->State = 1;
        }
    }
    else
    {
        // このフレームで移動していなくて、且つ状態が「走り」だったら
        if(ch->State == 1)
        {
            // 立ち止りアニメーションを再生する
            Chara_PlayAnim(ch, 4);

            // 状態を「立ち止り」にする
            ch->State = 0;
        }
    }

    // 状態が「ジャンプ」の場合は
    if(ch->State == 2)
    {
        // Ｙ軸方向の速度を重力分減算する
        ch->JumpPower -= CHARA_GRAVITY * DeltaTime_;

        // もし落下していて且つ再生されているアニメーションが上昇中用のものだった場合は
        if(ch->JumpPower < 0.0f && MV1GetAttachAnim(ch->ModelHandle, ch->PlayAnim1) == 2)
        {
            // 落下中ようのアニメーションを再生する
            Chara_PlayAnim(ch, 3);
        }

        // 移動ベクトルのＹ成分をＹ軸方向の速度にする
        MoveVec.y = ch->JumpPower * DeltaTime_;
    }

    // キャラクターの移動方向にモデルの方向を近づける
    Chara_AngleProcess(ch);

    // 移動ベクトルを元にコリジョンを考慮しつつキャラクターを移動
    Chara_Move(ch, MoveVec);

    // アニメーション処理
    Chara_AnimProcess(ch);
}

// キャラクターの移動処理
void Game3DTest::Chara_Move(CHARA* ch, VECTOR MoveVector)
{
    int i, j, k;						// 汎用カウンタ変数
    int MoveFlag;						// 水平方向に移動したかどうかのフラグ( ０:移動していない  １:移動した )
    int HitFlag;						// ポリゴンに当たったかどうかを記憶しておくのに使う変数( ０:当たっていない  １:当たった )
    MV1_COLL_RESULT_POLY_DIM HitDim;			// キャラクターの周囲にあるポリゴンを検出した結果が代入される当たり判定結果構造体
    int KabeNum;						// 壁ポリゴンと判断されたポリゴンの数
    int YukaNum;						// 床ポリゴンと判断されたポリゴンの数
    MV1_COLL_RESULT_POLY* Kabe[CHARA_MAX_HITCOLL];	// 壁ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
    MV1_COLL_RESULT_POLY* Yuka[CHARA_MAX_HITCOLL];	// 床ポリゴンと判断されたポリゴンの構造体のアドレスを保存しておくためのポインタ配列
    MV1_COLL_RESULT_POLY* Poly;				// ポリゴンの構造体にアクセスするために使用するポインタ( 使わなくても済ませられますがプログラムが長くなるので・・・ )
    HITRESULT_LINE LineRes;				// 線分とポリゴンとの当たり判定の結果を代入する構造体
    VECTOR OldPos;						// 移動前の座標	
    VECTOR NowPos;						// 移動後の座標
    // 移動前の座標を保存
    OldPos = ch->Position;

    // 移動後の座標を算出
    NowPos = VAdd(ch->Position, MoveVector);

    // キャラクターの周囲にあるステージポリゴンを取得する
    // ( 検出する範囲は移動距離も考慮する )
    HitDim = MV1CollCheck_Sphere(stg.ModelHandle, -1, ch->Position, CHARA_ENUM_DEFAULT_SIZE + VSize(MoveVector));

    // x軸かy軸方向に 0.01f 以上移動した場合は「移動した」フラグを１にする
    if(fabs(MoveVector.x) > 0.01f || fabs(MoveVector.z) > 0.01f)
    {
        MoveFlag = 1;
    }
    else
    {
        MoveFlag = 0;
    }

    // 検出されたポリゴンが壁ポリゴン( ＸＺ平面に垂直なポリゴン )か床ポリゴン( ＸＺ平面に垂直ではないポリゴン )かを判断する
    {
        // 壁ポリゴンと床ポリゴンの数を初期化する
        KabeNum = 0;
        YukaNum = 0;

        // 検出されたポリゴンの数だけ繰り返し
        for(i = 0; i < HitDim.HitNum; i++)
        {
            // ＸＺ平面に垂直かどうかはポリゴンの法線のＹ成分が０に限りなく近いかどうかで判断する
            if(HitDim.Dim[i].Normal.y < 0.000001f && HitDim.Dim[i].Normal.y > -0.000001f)
            {
                // 壁ポリゴンと判断された場合でも、キャラクターのＹ座標＋１．０ｆより高いポリゴンのみ当たり判定を行う
                if(HitDim.Dim[i].Position[0].y > ch->Position.y + 1.0f || HitDim.Dim[i].Position[1].y > ch->Position.y +
                    1.0f || HitDim.Dim[i].Position[2].y > ch->Position.y + 1.0f)
                {
                    // ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
                    if(KabeNum < CHARA_MAX_HITCOLL)
                    {
                        // ポリゴンの構造体のアドレスを壁ポリゴンポインタ配列に保存する
                        Kabe[KabeNum] = &HitDim.Dim[i];

                        // 壁ポリゴンの数を加算する
                        KabeNum++;
                    }
                }
            }
            else
            {
                // ポリゴンの数が列挙できる限界数に達していなかったらポリゴンを配列に追加
                if(YukaNum < CHARA_MAX_HITCOLL)
                {
                    // ポリゴンの構造体のアドレスを床ポリゴンポインタ配列に保存する
                    Yuka[YukaNum] = &HitDim.Dim[i];

                    // 床ポリゴンの数を加算する
                    YukaNum++;
                }
            }
        }
    }

    // 壁ポリゴンとの当たり判定処理
    if(KabeNum != 0)
    {
        // 壁に当たったかどうかのフラグは初期状態では「当たっていない」にしておく
        HitFlag = 0;

        // 移動したかどうかで処理を分岐
        if(MoveFlag == 1)
        {
            // 壁ポリゴンの数だけ繰り返し
            for(i = 0; i < KabeNum; i++)
            {
                // i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
                Poly = Kabe[i];

                // ポリゴンとキャラクターが当たっていなかったら次のカウントへ
                if(HitCheck_Capsule_Triangle(NowPos, VAdd(NowPos, VGet(0.0f, CHARA_HIT_HEIGHT, 0.0f)), CHARA_HIT_WIDTH,
                                             Poly->Position[0], Poly->Position[1], Poly->Position[2]) == FALSE) continue
                    ;

                // ここにきたらポリゴンとキャラクターが当たっているということなので、ポリゴンに当たったフラグを立てる
                HitFlag = 1;

                // 壁に当たったら壁に遮られない移動成分分だけ移動する
                {
                    VECTOR SlideVec;	// キャラクターをスライドさせるベクトル
                    // 進行方向ベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出
                    SlideVec = VCross(MoveVector, Poly->Normal);

                    // 算出したベクトルと壁ポリゴンの法線ベクトルに垂直なベクトルを算出、これが
                    // 元の移動成分から壁方向の移動成分を抜いたベクトル
                    SlideVec = VCross(Poly->Normal, SlideVec);

                    // それを移動前の座標に足したものを新たな座標とする
                    NowPos = VAdd(OldPos, SlideVec);
                }

                // 新たな移動座標で壁ポリゴンと当たっていないかどうかを判定する
                for(j = 0; j < KabeNum; j++)
                {
                    // j番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
                    Poly = Kabe[j];

                    // 当たっていたらループから抜ける
                    if(HitCheck_Capsule_Triangle(NowPos, VAdd(NowPos, VGet(0.0f, CHARA_HIT_HEIGHT, 0.0f)),
                                                 CHARA_HIT_WIDTH, Poly->Position[0], Poly->Position[1],
                                                 Poly->Position[2]) == TRUE) break;
                }

                // j が KabeNum だった場合はどのポリゴンとも当たらなかったということなので
                // 壁に当たったフラグを倒した上でループから抜ける
                if(j == KabeNum)
                {
                    HitFlag = 0;
                    break;
                }
            }
        }
        else
        {
            // 移動していない場合の処理
            // 壁ポリゴンの数だけ繰り返し
            for(i = 0; i < KabeNum; i++)
            {
                // i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
                Poly = Kabe[i];

                // ポリゴンに当たっていたら当たったフラグを立てた上でループから抜ける
                if(HitCheck_Capsule_Triangle(NowPos, VAdd(NowPos, VGet(0.0f, CHARA_HIT_HEIGHT, 0.0f)), CHARA_HIT_WIDTH,
                                             Poly->Position[0], Poly->Position[1], Poly->Position[2]) == TRUE)
                {
                    HitFlag = 1;
                    break;
                }
            }
        }

        // 壁に当たっていたら壁から押し出す処理を行う
        if(HitFlag == 1)
        {
            // 壁からの押し出し処理を試みる最大数だけ繰り返し
            for(k = 0; k < CHARA_HIT_TRYNUM; k++)
            {
                // 壁ポリゴンの数だけ繰り返し
                for(i = 0; i < KabeNum; i++)
                {
                    // i番目の壁ポリゴンのアドレスを壁ポリゴンポインタ配列から取得
                    Poly = Kabe[i];

                    // キャラクターと当たっているかを判定
                    if(HitCheck_Capsule_Triangle(NowPos, VAdd(NowPos, VGet(0.0f, CHARA_HIT_HEIGHT, 0.0f)),
                                                 CHARA_HIT_WIDTH, Poly->Position[0], Poly->Position[1],
                                                 Poly->Position[2]) == FALSE) continue;

                    // 当たっていたら規定距離分キャラクターを壁の法線方向に移動させる
                    NowPos = VAdd(NowPos, VScale(Poly->Normal, CHARA_HIT_SLIDE_LENGTH));

                    // キャラクターの状態が「ジャンプ」ではなく、且つジャンプフラグが立っていたらジャンプする
                    if (ch->State != 2)
                    {
                        // 状態を「ジャンプ」にする
                        ch->State = 2;

                        // Ｙ軸方向の速度をセット
                        ch->JumpPower = CHARA_JUMP_POWER;

                        // ジャンプアニメーションの再生
                        Chara_PlayAnim(ch, 2);
                    }

                    // 移動した上で壁ポリゴンと接触しているかどうかを判定
                    for(j = 0; j < KabeNum; j++)
                    {
                        // 当たっていたらループを抜ける
                        Poly = Kabe[j];
                        if(HitCheck_Capsule_Triangle(NowPos, VAdd(NowPos, VGet(0.0f, CHARA_HIT_HEIGHT, 0.0f)),
                                                     CHARA_HIT_WIDTH, Poly->Position[0], Poly->Position[1],
                                                     Poly->Position[2]) == TRUE) break;
                    }

                    // 全てのポリゴンと当たっていなかったらここでループ終了
                    if(j == KabeNum) break;
                }

                // i が KabeNum ではない場合は全部のポリゴンで押し出しを試みる前に全ての壁ポリゴンと接触しなくなったということなのでループから抜ける
                if(i != KabeNum) break;
            }
        }
    }

    // 床ポリゴンとの当たり判定
    if(YukaNum != 0)
    {
        // ジャンプ中且つ上昇中の場合は処理を分岐
        if(ch->State == 2 && ch->JumpPower > 0.0f)
        {
            float MinY;

            // 天井に頭をぶつける処理を行う
            // 一番低い天井にぶつける為の判定用変数を初期化
            MinY = 0.0f;

            // 当たったかどうかのフラグを当たっていないを意味する０にしておく
            HitFlag = 0;

            // 床ポリゴンの数だけ繰り返し
            for(i = 0; i < YukaNum; i++)
            {
                // i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
                Poly = Yuka[i];

                // 足先から頭の高さまでの間でポリゴンと接触しているかどうかを判定
                LineRes = HitCheck_Line_Triangle(NowPos, VAdd(NowPos, VGet(0.0f, CHARA_HIT_HEIGHT, 0.0f)),
                                                 Poly->Position[0], Poly->Position[1], Poly->Position[2]);

                // 接触していなかったら何もしない
                if(LineRes.HitFlag == FALSE) continue;

                // 既にポリゴンに当たっていて、且つ今まで検出した天井ポリゴンより高い場合は何もしない
                if(HitFlag == 1 && MinY < LineRes.Position.y) continue;

                // ポリゴンに当たったフラグを立てる
                HitFlag = 1;

                // 接触したＹ座標を保存する
                MinY = LineRes.Position.y;
            }

            // 接触したポリゴンがあったかどうかで処理を分岐
            if(HitFlag == 1)
            {
                // 接触した場合はキャラクターのＹ座標を接触座標を元に更新
                NowPos.y = MinY - CHARA_HIT_HEIGHT;

                // Ｙ軸方向の速度は反転
                ch->JumpPower = -ch->JumpPower;
            }
        }
        else
        {
            float MaxY;

            // 下降中かジャンプ中ではない場合の処理
            // 床ポリゴンに当たったかどうかのフラグを倒しておく
            HitFlag = 0;

            // 一番高い床ポリゴンにぶつける為の判定用変数を初期化
            MaxY = 0.0f;

            // 床ポリゴンの数だけ繰り返し
            for(i = 0; i < YukaNum; i++)
            {
                // i番目の床ポリゴンのアドレスを床ポリゴンポインタ配列から取得
                Poly = Yuka[i];

                // ジャンプ中かどうかで処理を分岐
                if(ch->State == 2)
                {
                    // ジャンプ中の場合は頭の先から足先より少し低い位置の間で当たっているかを判定
                    LineRes = HitCheck_Line_Triangle(VAdd(NowPos, VGet(0.0f, CHARA_HIT_HEIGHT, 0.0f)),
                                                     VAdd(NowPos, VGet(0.0f, -1.0f, 0.0f)), Poly->Position[0],
                                                     Poly->Position[1], Poly->Position[2]);
                }
                else
                {
                    // 走っている場合は頭の先からそこそこ低い位置の間で当たっているかを判定( 傾斜で落下状態に移行してしまわない為 )
                    LineRes = HitCheck_Line_Triangle(VAdd(NowPos, VGet(0.0f, CHARA_HIT_HEIGHT, 0.0f)),
                                                     VAdd(NowPos, VGet(0.0f, -40.0f, 0.0f)), Poly->Position[0],
                                                     Poly->Position[1], Poly->Position[2]);
                }

                // 当たっていなかったら何もしない
                if(LineRes.HitFlag == FALSE) continue;

                // 既に当たったポリゴンがあり、且つ今まで検出した床ポリゴンより低い場合は何もしない
                if(HitFlag == 1 && MaxY > LineRes.Position.y) continue;

                // ポリゴンに当たったフラグを立てる
                HitFlag = 1;

                // 接触したＹ座標を保存する
                MaxY = LineRes.Position.y;
            }

            // 床ポリゴンに当たったかどうかで処理を分岐
            if(HitFlag == 1)
            {
                // 当たった場合
                // 接触したポリゴンで一番高いＹ座標をキャラクターのＹ座標にする
                NowPos.y = MaxY;

                // Ｙ軸方向の移動速度は０に
                ch->JumpPower = 0.0f;

                // もしジャンプ中だった場合は着地状態にする
                if(ch->State == 2)
                {
                    // 移動していたかどうかで着地後の状態と再生するアニメーションを分岐する
                    if(MoveFlag)
                    {
                        // 移動している場合は走り状態に
                        Chara_PlayAnim(ch, 1);
                        ch->State = 1;
                    }
                    else
                    {
                        // 移動していない場合は立ち止り状態に
                        Chara_PlayAnim(ch, 4);
                        ch->State = 0;
                    }

                    // 着地時はアニメーションのブレンドは行わない
                    ch->AnimBlendRate = 1.0f;
                }
            }
            else
            {
                // 床コリジョンに当たっていなくて且つジャンプ状態ではなかった場合は
                if(ch->State != 2)
                {
                    // ジャンプ中にする
                    ch->State = 2;

                    // ちょっとだけジャンプする
                    ch->JumpPower = CHARA_FALL_UP_POWER;

                    // アニメーションは落下中のものにする
                    Chara_PlayAnim(ch, 3);
                }
            }
        }
    }

    // 新しい座標を保存する
    ch->Position = NowPos;

    // キャラクターのモデルの座標を更新する
    MV1SetPosition(ch->ModelHandle, ch->Position);

    // 検出したキャラクターの周囲のポリゴン情報を開放する
    MV1CollResultPolyDimTerminate(HitDim);
}

// キャラクターに当たっていたら押し出す処理を行う( chk_ch に ch が当たっていたら ch が離れる )
void Game3DTest::Chara_Collision(CHARA* ch, VECTOR* ch_MoveVec, CHARA* chk_ch)
{
    VECTOR ChkChToChVec;
    VECTOR PushVec;
    VECTOR ChPosition;
    float Length;

    // 移動後の ch の座標を算出
    ChPosition = VAdd(ch->Position, *ch_MoveVec);

    // 当たっていなかったら何もしない
    if(HitCheck_Capsule_Capsule(ChPosition, VAdd(ChPosition, VGet(0.0f, CHARA_HIT_HEIGHT, 0.0f)), CHARA_HIT_WIDTH,
                                chk_ch->Position, VAdd(chk_ch->Position, VGet(0.0f, CHARA_HIT_HEIGHT, 0.0f)),
                                CHARA_HIT_WIDTH) == TRUE)
    {
        // 当たっていたら ch が chk から離れる処理をする
        // chk_ch から ch へのベクトルを算出
        ChkChToChVec = VSub(ChPosition, chk_ch->Position);

        // Ｙ軸は見ない
        ChkChToChVec.y = 0.0f;

        // 二人の距離を算出
        Length = VSize(ChkChToChVec);

        // chk_ch から ch へのベクトルを正規化( ベクトルの長さを 1.0f にする )
        PushVec = VScale(ChkChToChVec, 1.0f / Length);

        // 押し出す距離を算出、もし二人の距離から二人の大きさを引いた値に押し出し力を足して離れてしまう場合は、ぴったりくっつく距離に移動する
        if(Length - CHARA_HIT_WIDTH * 2.0f + CHARA_HIT_PUSH_POWER > 0.0f)
        {
            float TempY;
            TempY = ChPosition.y;
            ChPosition = VAdd(chk_ch->Position, VScale(PushVec, CHARA_HIT_WIDTH * 2.0f));

            // Ｙ座標は変化させない
            ChPosition.y = TempY;
        }
        else
        {
            // 押し出し
            ChPosition = VAdd(ChPosition, VScale(PushVec, CHARA_HIT_PUSH_POWER));
        }
    }

    // 当たり判定処理後の移動ベクトルをセット
    *ch_MoveVec = VSub(ChPosition, ch->Position);
}

// キャラクターの向きを変える処理
void Game3DTest::Chara_AngleProcess(CHARA* ch)
{
    float TargetAngle;			// 目標角度
    float SaAngle;				// 目標角度と現在の角度との差
    // 目標の方向ベクトルから角度値を算出する
    TargetAngle = atan2(ch->TargetMoveDirection.x, ch->TargetMoveDirection.z);

    // 目標の角度と現在の角度との差を割り出す
    {
        // 最初は単純に引き算
        SaAngle = TargetAngle - ch->Angle;

        // ある方向からある方向の差が１８０度以上になることは無いので
        // 差の値が１８０度以上になっていたら修正する
        if(SaAngle < -DX_PI_F)
        {
            SaAngle += DX_TWO_PI_F;
        }
        else if(SaAngle > DX_PI_F)
        {
            SaAngle -= DX_TWO_PI_F;
        }
    }

    // 角度の差が０に近づける
    if(SaAngle > 0.0f)
    {
        // 差がプラスの場合は引く
        SaAngle -= CHARA_ANGLE_SPEED * DeltaTime_;
        if(SaAngle < 0.0f)
        {
            SaAngle = 0.0f;
        }
    }
    else
    {
        // 差がマイナスの場合は足す
        SaAngle += CHARA_ANGLE_SPEED * DeltaTime_;
        if(SaAngle > 0.0f)
        {
            SaAngle = 0.0f;
        }
    }

    // モデルの角度を更新
    ch->Angle = TargetAngle - SaAngle;
    MV1SetRotationXYZ(ch->ModelHandle, VGet(0.0f, ch->Angle + DX_PI_F, 0.0f));
}

// キャラクターに新たなアニメーションを再生する
void Game3DTest::Chara_PlayAnim(CHARA* ch, int PlayAnim)
{
    // 再生中のモーション２が有効だったらデタッチする
    if(ch->PlayAnim2 != -1)
    {
        MV1DetachAnim(ch->ModelHandle, ch->PlayAnim2);
        ch->PlayAnim2 = -1;
    }

    // 今まで再生中のモーション１だったものの情報を２に移動する
    ch->PlayAnim2 = ch->PlayAnim1;
    ch->AnimPlayCount2 = ch->AnimPlayCount1;

    // 新たに指定のモーションをモデルにアタッチして、アタッチ番号を保存する
    ch->PlayAnim1 = MV1AttachAnim(ch->ModelHandle, PlayAnim);
    ch->AnimPlayCount1 = 0.0f;

    // ブレンド率は再生中のモーション２が有効ではない場合は１．０ｆ( 再生中のモーション１が１００％の状態 )にする
    ch->AnimBlendRate = ch->PlayAnim2 == -1 ? 1.0f : 0.0f;
}

// キャラクターのアニメーション処理
void Game3DTest::Chara_AnimProcess(CHARA* ch)
{
    float AnimTotalTime;		// 再生しているアニメーションの総時間
    // ブレンド率が１以下の場合は１に近づける
    if(ch->AnimBlendRate < 1.0f)
    {
        ch->AnimBlendRate += CHARA_ANIM_BLEND_SPEED * DeltaTime_;
        if(ch->AnimBlendRate > 1.0f)
        {
            ch->AnimBlendRate = 1.0f;
        }
    }

    // 再生しているアニメーション１の処理
    if(ch->PlayAnim1 != -1)
    {
        // アニメーションの総時間を取得
        AnimTotalTime = MV1GetAttachAnimTotalTime(ch->ModelHandle, ch->PlayAnim1);

        // 再生時間を進める
        ch->AnimPlayCount1 += FixedCharaAnimSpeed_;

        // 再生時間が総時間に到達していたら再生時間をループさせる
        if(ch->AnimPlayCount1 >= AnimTotalTime)
        {
            ch->AnimPlayCount1 = fmod(ch->AnimPlayCount1, AnimTotalTime);
        }

        // 変更した再生時間をモデルに反映させる
        MV1SetAttachAnimTime(ch->ModelHandle, ch->PlayAnim1, ch->AnimPlayCount1);

        // アニメーション１のモデルに対する反映率をセット
        MV1SetAttachAnimBlendRate(ch->ModelHandle, ch->PlayAnim1, ch->AnimBlendRate);
    }

    // 再生しているアニメーション２の処理
    if(ch->PlayAnim2 != -1)
    {
        // アニメーションの総時間を取得
        AnimTotalTime = MV1GetAttachAnimTotalTime(ch->ModelHandle, ch->PlayAnim2);

        // 再生時間を進める
        ch->AnimPlayCount2 += FixedCharaAnimSpeed_;

        // 再生時間が総時間に到達していたら再生時間をループさせる
        if(ch->AnimPlayCount2 > AnimTotalTime)
        {
            ch->AnimPlayCount2 = fmod(ch->AnimPlayCount2, AnimTotalTime);
        }

        // 変更した再生時間をモデルに反映させる
        MV1SetAttachAnimTime(ch->ModelHandle, ch->PlayAnim2, ch->AnimPlayCount2);

        // アニメーション２のモデルに対する反映率をセット
        MV1SetAttachAnimBlendRate(ch->ModelHandle, ch->PlayAnim2, 1.0f - ch->AnimBlendRate);
    }
}


// プレイヤーの初期化
void Game3DTest::Player_Initialize(void)
{
    // キャラクター情報を初期化
    Chara_Initialize(&pl.CharaInfo, VGet(0.0f, 0.0f, 0.0f));
}

// プレイヤーの後始末
void Game3DTest::Player_Terminate(void)
{
    // キャラクター情報の後始末
    Chara_Terminate(&pl.CharaInfo);
}

// プレイヤーの処理
void Game3DTest::Player_Process(void)
{
    VECTOR UpMoveVec;		// 方向ボタン「↑」を入力をしたときのプレイヤーの移動方向ベクトル
    VECTOR LeftMoveVec;		// 方向ボタン「←」を入力をしたときのプレイヤーの移動方向ベクトル
    VECTOR MoveVec;		// このフレームの移動ベクトル
    int JumpFlag;			// ジャンプフラグ
    int i;

    // プレイヤーの移動方向のベクトルを算出
    {
        // 方向ボタン「↑」を押したときのプレイヤーの移動ベクトルはカメラの視線方向からＹ成分を抜いたもの
        UpMoveVec = VSub(cam.Target, cam.Eye);
        UpMoveVec.y = 0.0f;

        // 方向ボタン「←」を押したときのプレイヤーの移動ベクトルは上を押したときの方向ベクトルとＹ軸のプラス方向のベクトルに垂直な方向
        LeftMoveVec = VCross(UpMoveVec, VGet(0.0f, 1.0f, 0.0f));

        // 二つのベクトルを正規化( ベクトルの長さを１．０にすること )
        UpMoveVec = VNorm(UpMoveVec);
        LeftMoveVec = VNorm(LeftMoveVec);
    }

    // このフレームでの移動ベクトルを初期化
    MoveVec = VGet(0.0f, 0.0f, 0.0f);

    // ジャンプフラグを倒す
    JumpFlag = 0;

    // 方向ボタン「←」が入力されたらカメラの見ている方向から見て左方向に移動する
    if(KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_A))
    {
        // 移動ベクトルに「←」が入力された時の移動ベクトルを加算する
        MoveVec = VAdd(MoveVec, LeftMoveVec);
    }
    else
        // 方向ボタン「→」が入力されたらカメラの見ている方向から見て右方向に移動する
        if(KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_D))
        {
            // 移動ベクトルに「←」が入力された時の移動ベクトルを反転したものを加算する
            MoveVec = VAdd(MoveVec, VScale(LeftMoveVec, -1.0f));
        }

    // 方向ボタン「↑」が入力されたらカメラの見ている方向に移動する
    if(KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_W))
    {
        // 移動ベクトルに「↑」が入力された時の移動ベクトルを加算する
        MoveVec = VAdd(MoveVec, UpMoveVec);
    }
    else
        // 方向ボタン「↓」が入力されたらカメラの方向に移動する
        if(KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_S))
        {
            // 移動ベクトルに「↑」が入力された時の移動ベクトルを反転したものを加算する
            MoveVec = VAdd(MoveVec, VScale(UpMoveVec, -1.0f));
        }

    // ボタン１が押されていたらジャンプフラグを立てる
    if(KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_SPACE))
    {
        JumpFlag = 1;
    }

    // 移動方向を移動速度でスケーリングする
    MoveVec = VScale(MoveVec, FixedCharaMoveSpeed_);

    // プレイヤーキャラ以外との当たり判定を行う
    for(i = 0; i < NOTPLAYER_NUM; i++)
    {
        Chara_Collision(&pl.CharaInfo, &MoveVec, &npl[i].CharaInfo);
    }

    // キャラクターを動作させる処理を行う
    Chara_Process(&pl.CharaInfo, MoveVec, JumpFlag);
}

// プレイヤー以外キャラの初期化
void Game3DTest::NotPlayer_Initialize(void)
{
    int i;
    static VECTOR FirstPosition[NOTPLAYER_NUM] = {
        { -3000.0f, 0.0f, 2300.0f }, { -2500.0f, 7300.0f, -2500.0f }, { -2600.0f, 0.0f, -3100.0f },
        { 2800.0f, 0.0f, 200.0f },
    };
    for(i = 0; i < NOTPLAYER_NUM; i++)
    {
        // キャラクター情報を初期化
        Chara_Initialize(&npl[i].CharaInfo, FirstPosition[i]);

        // 移動時間を初期化
        npl[i].MoveTime = 0;

        // 移動方向を初期化
        npl[i].MoveAngle = GetRand(1000) * DX_PI_F * 2.0f / 1000.0f;
    }
}

// プレイヤー以外キャラの後始末
void Game3DTest::NotPlayer_Terminate(void)
{
    int i;

    // キャラクタの数だけ繰り返し
    for(i = 0; i < NOTPLAYER_NUM; i++)
    {
        // キャラクター情報の後始末
        Chara_Terminate(&npl[i].CharaInfo);
    }
}

// プレイヤー以外キャラの処理
void Game3DTest::NotPlayer_Process(void)
{
    int i;
    int j;
    VECTOR MoveVec;
    int JumpFlag;

    // キャラクタの数だけ繰り返し
    for(i = 0; i < NOTPLAYER_NUM; i++)
    {
        // ジャンプフラグを倒しておく
        JumpFlag = 0;

        // 一定時間が経過したら移動する方向を変更する
        npl[i].MoveTime++;
        if(npl[i].MoveTime >= NOTPLAYER_MOVETIME)
        {
            npl[i].MoveTime = 0;

            // 新しい方向の決定
            npl[i].MoveAngle = GetRand(1000) * DX_PI_F * 2.0f / 1000.0f;

            // 一定確率でジャンプする
            if(GetRand(1000) < NOTPLAYER_JUMPRATIO)
            {
                JumpFlag = 1;
            }
        }

        // 新しい方向の角度からベクトルを算出
        MoveVec.x = cos(npl[i].MoveAngle) * FixedCharaMoveSpeed_;
        MoveVec.y = 0.0f;
        MoveVec.z = sin(npl[i].MoveAngle) * FixedCharaMoveSpeed_;

        // プレイヤーとの当たり判定を行う
        Chara_Collision(&npl[i].CharaInfo, &MoveVec, &pl.CharaInfo);

        // 自分以外のプレイヤーキャラとの当たり判定を行う
        for(j = 0; j < NOTPLAYER_NUM; j++)
        {
            // 自分との当たり判定はしない
            if(i == j) continue;
            Chara_Collision(&npl[i].CharaInfo, &MoveVec, &npl[j].CharaInfo);
        }

        // 移動処理を行う
        Chara_Process(&npl[i].CharaInfo, MoveVec, JumpFlag);
    }
}

// ステージの初期化処理
void Game3DTest::Stage_Initialize(void)
{
    // ステージモデルの読み込み
    stg.ModelHandle = MV1LoadModel("ColTestStage.mqo");

    // モデル全体のコリジョン情報のセットアップ
    MV1SetupCollInfo(stg.ModelHandle, -1);


    // 作成する画像のフォーマットを不動小数点型で１チャンネル、１６ビットにする
    SetDrawValidFloatTypeGraphCreateFlag(TRUE);
    SetCreateDrawValidGraphChannelNum(1);
    SetCreateGraphColorBitDepth(16);

    // 影用深度記録画像の作成
    DepthBufferGraphHandle = MakeScreen(8192, 8192, FALSE);

    // 設定を元に戻す
    SetDrawValidFloatTypeGraphCreateFlag(FALSE);
    SetCreateDrawValidGraphChannelNum(4);
    SetCreateGraphColorBitDepth(32);
    BloomBufferGraphHandle = MakeScreen(engine::CastToInt(GetRawScreenWidth()), engine::CastToInt(GetRawScreenHeight()),
                                        TRUE);
    ScaleDownGraphHandle = MakeScreen(engine::CastToInt(GetRawScreenWidth()) / 8,
                                      engine::CastToInt(GetRawScreenHeight()) / 8, TRUE);

    // 深度記録画像描画用の頂点シェーダーを読み込む
    Skin4_DepthShadow_Step1_VertexShader = LoadVertexShader("SkinMesh4_DepthShadow_Step1VS.vso");
    Normal_DepthShadow_Step1_VertexShader = LoadVertexShader("NormalMesh_DepthShadow_Step1VS.vso");

    // 深度記録画像描画用のピクセルシェーダーを読み込む
    DepthShadow_Step1_PixelShader = LoadPixelShader("DepthShadow_Step1PS.pso");

    // 深度記録画像を使ったディレクショナルライト一つの描画用頂点シェーダーを読み込む
    Skin4_DirLight_DepthShadow_Step2_VertexShader = LoadVertexShader("SkinMesh4_DirLight_DepthShadow_Step2VS.vso");
    Normal_DirLight_DepthShadow_Step2_VertexShader = LoadVertexShader("NormalMesh_DirLight_DepthShadow_Step2VS.vso");

    // 深度記録画像を使ったディレクショナルライト一つの描画用ピクセルシェーダーを読み込む
    DirLight_DepthShadow_Step2_PixelShader = LoadPixelShader("DirLight_DepthShadow_Step2PS.pso");
}

// ステージの後始末処理
void Game3DTest::Stage_Terminate(void)
{
    // ステージモデルの後始末
    MV1DeleteModel(stg.ModelHandle);
}

// カメラの初期化処理
void Game3DTest::Camera_Initialize(void)
{
    // カメラの初期水平角度は１８０度
    cam.AngleH = DX_PI_F;

    // 垂直角度は０度
    cam.AngleV = 0.0f;
}

// カメラの処理
void Game3DTest::Camera_Process(void)
{
    if(!KeyboardManager::GetInstance()->IsHoldKey(KEY_INPUT_LALT))
    {
        if(MouseManager::GetInstance()->IsMovedMouse())
        {
            cam.AngleH += MouseManager::GetInstance()->GetMouseXVelf() / 500.f;
            cam.AngleV -= MouseManager::GetInstance()->GetMouseYVelf() / 500.f;
            MouseManager::GetInstance()->SetMouseX(DefaultScaler_->CalculateWidth(50.f));
            MouseManager::GetInstance()->SetMouseY(DefaultScaler_->CalculateHeight(50.f));
        }
    }
    //cam.AngleH -= FixedCameraRotSpeed_;
    // －１８０度以下になったら角度値が大きくなりすぎないように３６０度を足す
    if(cam.AngleH < -DX_PI_F)
    {
        cam.AngleH += DX_TWO_PI_F;
    }
    //cam.AngleH += FixedCameraRotSpeed_;
    // １８０度以上になったら角度値が大きくなりすぎないように３６０度を引く
    if(cam.AngleH > DX_PI_F)
    {
        cam.AngleH -= DX_TWO_PI_F;
    }

    //cam.AngleV -= FixedCameraRotSpeed_;
    cam.AngleV = engine::LimitRange(cam.AngleV, -DX_PI_F / 2.0f + 0.1f, DX_PI_F / 2.0f - 0.1f);

    // カメラの注視点はプレイヤー座標から規定値分高い座標
    cam.Target = VAdd(pl.CharaInfo.Position, VGet(0.0f, CAMERA_PLAYER_TARGET_HEIGHT, 0.0f));

    // カメラの座標を決定する
    {
        MATRIX RotZ, RotY;
        float Camera_Player_Length;
        MV1_COLL_RESULT_POLY_DIM HRes;
        int HitNum;

        // 水平方向の回転はＹ軸回転
        RotY = MGetRotY(cam.AngleH);

        // 垂直方向の回転はＺ軸回転 )
        RotZ = MGetRotZ(cam.AngleV);

        // カメラからプレイヤーまでの初期距離をセット
        Camera_Player_Length = CAMERA_PLAYER_LENGTH;

        // カメラの座標を算出
        // Ｘ軸にカメラとプレイヤーとの距離分だけ伸びたベクトルを
        // 垂直方向回転( Ｚ軸回転 )させたあと水平方向回転( Ｙ軸回転 )して更に
        // 注視点の座標を足したものがカメラの座標
        cam.Eye = VAdd(VTransform(VTransform(VGet(-Camera_Player_Length, 0.0f, 0.0f), RotZ), RotY), cam.Target);

        // 注視点からカメラの座標までの間にステージのポリゴンがあるか調べる
        HRes = MV1CollCheck_Capsule(stg.ModelHandle, -1, cam.Target, cam.Eye, CAMERA_COLLISION_SIZE);
        HitNum = HRes.HitNum;
        MV1CollResultPolyDimTerminate(HRes);
        if(HitNum != 0)
        {
            float NotHitLength;
            float HitLength;
            float TestLength;
            VECTOR TestPosition;

            // あったら無い位置までプレイヤーに近づく
            // ポリゴンに当たらない距離をセット
            NotHitLength = 0.0f;

            // ポリゴンに当たる距離をセット
            HitLength = Camera_Player_Length;
            do
            {
                // 当たるかどうかテストする距離をセット( 当たらない距離と当たる距離の中間 )
                TestLength = NotHitLength + (HitLength - NotHitLength) / 2.0f;

                // テスト用のカメラ座標を算出
                TestPosition = VAdd(VTransform(VTransform(VGet(-TestLength, 0.0f, 0.0f), RotZ), RotY), cam.Target);

                // 新しい座標で壁に当たるかテスト
                HRes = MV1CollCheck_Capsule(stg.ModelHandle, -1, cam.Target, TestPosition, CAMERA_COLLISION_SIZE);
                HitNum = HRes.HitNum;
                MV1CollResultPolyDimTerminate(HRes);
                if(HitNum != 0)
                {
                    // 当たったら当たる距離を TestLength に変更する
                    HitLength = TestLength;
                }
                else
                {
                    // 当たらなかったら当たらない距離を TestLength に変更する
                    NotHitLength = TestLength;
                }

                // HitLength と NoHitLength が十分に近づいていなかったらループ
            } while(HitLength - NotHitLength > 0.1f);

            // カメラの座標をセット
            cam.Eye = TestPosition;
        }
    }
}


// 影用の深度記録画像の準備を行う
void Game3DTest::SetupDepthImage(void)
{
    int i;
    VECTOR LightDirection;
    VECTOR LightPosition;
    VECTOR LightTarget;
    static auto LightAddVec = VGet(0.f, 0.f, 0.f);

    //LightAddVec = VAdd(LightAddVec, VGet(50.f * DeltaTime_, 0.f, 0.f));
    // 描画先を影用深度記録画像にする
    SetDrawScreen(DepthBufferGraphHandle);
    // 影用深度記録画像を真っ白にクリア
    SetBackgroundColor(255, 255, 255);
    ClearDrawScreen();
    SetBackgroundColor(0, 0, 0);

    // カメラのタイプを正射影タイプにセット、描画範囲も指定
    SetupCamera_Ortho(13250.0f);

    // 描画する奥行き範囲をセット
    SetCameraNearFar(10.0f, 13050.0f);
    DebugLabel_->SetLabel("LightVec: " + std::to_string(LightAddVec.x));
    //SetLightDirection(LightAddVec);
    // カメラの向きはライトの向き
    LightDirection = GetLightDirection();

    // カメラの位置と注視点はステージ全体が見渡せる位置
    LightTarget.x = 3620.0f;
    LightTarget.y = 0.0f;
    LightTarget.z = 3830.0f;
    LightPosition = VAdd(LightTarget, VScale(LightDirection, -12400.0f));
    SetCameraPositionAndTarget_UpVecY(LightPosition, LightTarget);


    // 設定したカメラのビュー行列と射影行列を取得しておく
    LightCamera_ViewMatrix = GetCameraViewMatrix();
    LightCamera_ProjectionMatrix = GetCameraProjectionMatrix();


    // モデルの描画にオリジナルのシェーダーを使用するように設定する
    MV1SetUseOrigShader(TRUE);

    // 深度記録画像への描画用のピクセルシェーダーをセット
    SetUsePixelShader(DepthShadow_Step1_PixelShader);


    // 深度記録画像への剛体メッシュ描画用の頂点シェーダーをセット
    SetUseVertexShader(Normal_DepthShadow_Step1_VertexShader);

    // ステージを描画
    MV1DrawModel(stg.ModelHandle);


    // 深度記録画像へのスキニングメッシュ描画用の頂点シェーダーをセット
    SetUseVertexShader(Skin4_DepthShadow_Step1_VertexShader);

    // プレイヤーモデルの描画
    MV1DrawModel(pl.CharaInfo.ModelHandle);

    // プレイヤー以外キャラモデルの描画
    for(i = 0; i < NOTPLAYER_NUM; i++)
    {
        MV1DrawModel(npl[i].CharaInfo.ModelHandle);
    }
    DrawBillboard3D(VAdd(pl.CharaInfo.Position, VGet(0.f, 1000.f, 0.f)), 0.5f, 0.5f, 500.f, 0.f,
                    DebugLabel_->GetDrawBuffer(), TRUE);
    DrawBillboard3D(VAdd(pl.CharaInfo.Position, VGet(0.f, 1000.f, 0.f)), 0.5f, 0.5f, 500.f, 0.f, GetDrawBuffer(), TRUE);


    // モデルの描画にオリジナルのシェーダーを使用するようにした設定を解除
    MV1SetUseOrigShader(FALSE);

    // 描画先を裏画面に戻す
    SetDrawScreen(GetDrawBuffer());
    ClearDrawScreen();
}

// 影用の深度記録画像を使った影を落とす処理も含めたモデルの描画
void Game3DTest::DrawModelWithDepthShadow(void)
{
    int i;

    // カメラの設定を行う
    SetUseZBuffer3D(TRUE);
    SetWriteZBuffer3D(TRUE);
    SetupCamera_Perspective(60.0 * DX_PI_F / 180.0f);
    SetCameraPositionAndTarget_UpVecY(cam.Eye, cam.Target);
    SetCameraScreenCenter(DefaultScaler_->CalculateWidth(50.f), DefaultScaler_->CalculateHeight(50.f));

    // モデルの描画にオリジナルのシェーダーを使用するように設定する
    MV1SetUseOrigShader(TRUE);

    // 深度記録画像を使った影＋ディレクショナルライト一つ描画用のピクセルシェーダーをセット
    SetUsePixelShader(DirLight_DepthShadow_Step2_PixelShader);

    // 影用深度記録画像を描画したときのカメラのビュー行列と射影行列を定数に設定する
    SetVSConstFMtx(43, LightCamera_ViewMatrix);
    SetVSConstFMtx(47, LightCamera_ProjectionMatrix);

    // 影用深度記録画像をテクスチャ１にセット
    SetUseTextureToShader(1, DepthBufferGraphHandle);


    // 深度記録画像を使った影＋ディレクショナルライト一つの剛体メッシュ描画用の頂点シェーダーをセット
    SetUseVertexShader(Normal_DirLight_DepthShadow_Step2_VertexShader);
    MV1SetTextureGraphHandle(stg.ModelHandle, 0, -1, TRUE);
    // ステージモデルの描画
    MV1DrawModel(stg.ModelHandle);


    // 深度記録画像を使った影＋ディレクショナルライト一つのスキニングメッシュ描画用の頂点シェーダーをセット
    SetUseVertexShader(Skin4_DirLight_DepthShadow_Step2_VertexShader);

    // プレイヤーモデルの描画
    MV1DrawModel(pl.CharaInfo.ModelHandle);

    // プレイヤー以外キャラモデルの描画
    for(i = 0; i < NOTPLAYER_NUM; i++)
    {
        MV1DrawModel(npl[i].CharaInfo.ModelHandle);
    }

    // モデルの描画にオリジナルのシェーダーを使用するようにした設定を解除
    MV1SetUseOrigShader(FALSE);

    // 使用テクスチャの設定を解除
    SetUseTextureToShader(1, -1);

    // 設定した定数を解除
    ResetVSConstF(43, 8);

    //DrawSphere3D(VAdd(pl.CharaInfo.Position, VGet(0.f, 1000.f, 0.f)), 500.f, 40, GetColor(255, 255, 255), GetColor(255, 255, 255), TRUE);
    //DrawBillboard3D(VAdd(pl.CharaInfo.Position, VGet(0.f, 1000.f, 0.f)), 0.5f, 0.5f, 500.f, 0.f, GetDrawBuffer(), TRUE);
    DrawBillboard3D(VAdd(pl.CharaInfo.Position, VGet(0.f, 1000.f, 0.f)), 0.5f, 0.5f, 500.f, 0.f,
                    DebugLabel_->GetDrawBuffer(), TRUE);
    // ステージモデルの描画
    MV1DrawModel(stg.ModelHandle);


    // 深度記録画像を使った影＋ディレクショナルライト一つのスキニングメッシュ描画用の頂点シェーダーをセット
    SetUseVertexShader(Skin4_DirLight_DepthShadow_Step2_VertexShader);

    // プレイヤーモデルの描画
    MV1DrawModel(pl.CharaInfo.ModelHandle);

    // プレイヤー以外キャラモデルの描画
    for(i = 0; i < NOTPLAYER_NUM; i++)
    {
        MV1DrawModel(npl[i].CharaInfo.ModelHandle);
    }

    // 描画結果から高輝度部分のみを抜き出した画像を得る
    GraphFilterBlt(GetDrawBuffer(), BloomBufferGraphHandle, DX_GRAPH_FILTER_BRIGHT_CLIP, DX_CMP_LESS, 150, TRUE,
                   GetColor(0, 0, 0), 255);

    // 高輝度部分を８分の１に縮小した画像を得る
    GraphFilterBlt(BloomBufferGraphHandle, ScaleDownGraphHandle, DX_GRAPH_FILTER_DOWN_SCALE, 8);

    // ８分の１に縮小した画像をガウスフィルタでぼかす
    GraphFilter(ScaleDownGraphHandle, DX_GRAPH_FILTER_GAUSS, 8, 900);

    // 描画ブレンドモードを加算にする
    SetDrawBlendMode(DX_BLENDMODE_PMA_ADD, 255);
    // 高輝度部分を縮小してぼかした画像を画面いっぱいに２回描画する( ２回描画するのはより明るくみえるようにするため )
    DrawExtendGraph(0, 0, engine::CastToInt(GetRawScreenWidth()), engine::CastToInt(GetRawScreenHeight()),
                    ScaleDownGraphHandle, TRUE);
    DrawExtendGraph(0, 0, engine::CastToInt(GetRawScreenWidth()), engine::CastToInt(GetRawScreenHeight()),
                    ScaleDownGraphHandle, TRUE);
}

// 描画処理
void Game3DTest::Render_Process(void)
{
    SetDrawScreen(DebugLabel_->GetDrawBuffer());
    ClearDrawScreen();
    DebugLabel_->SceneUpdate(DeltaTime_);
    DebugLabel_->Draw();

    // 影用の深度記録画像の準備を行う
    SetupDepthImage();
    // 影用の深度記録画像を使った影を落とす処理も含めたモデルの描画
    DrawModelWithDepthShadow();
}
