#pragma once
#include "../ScreenData.h"

class FlexibleScaler
{
private:
    float GlobalDiffX_ = 0.f;
    float GlobalDiffY_ = 0.f;
    float ScreenWidth_;
    float ScreenHeight_;
    float Scale_;
    float RatioX_;
    float RatioY_;

    static std::shared_ptr<FlexibleScaler> GlobalInstance_;
    static std::vector<FlexibleScaler*> Scalers_;

public:
    bool lockTop = true;
    bool lockBottom = false;
    bool lockLeft = true;
    bool lockRight = false;

    FlexibleScaler(float screenWidth, float screenHeight, float scale = 1.0f, float ratioX = 1.f, float ratioY = 1.f);
    ~FlexibleScaler();

    static void ApplyWindowSizeChanges();

    /// <summary>
    /// ウィンドウサイズを基準にしたスケーラーのインスタンスを取得
    /// </summary>
    /// <returns>共有ポインタインスタンス</returns>
    static std::shared_ptr<FlexibleScaler> GetWindowBasedInstance();
    static void CreateWindowBasedInstance();
    static void DestroyWindowBasedInstance();

    void AddDiffX(float dx);
    void AddDiffY(float dy);

    /// <summary>
    /// X方向のオフセットを取得
    /// </summary>
    /// <returns>X方向のオフセット</returns>
    float GetDiffX() const;

    /// <summary>
    /// Y方向のオフセットを取得
    /// </summary>
    /// <returns>Y方向のオフセット</returns>
    float GetDiffY() const;

    /// <summary>
    /// スクリーン幅(100%幅)を取得
    /// </summary>
    /// <returns>生スクリーン幅</returns>
    float GetScreenWidth() const;

    /// <summary>
    /// スクリーン高(100%高)を取得
    /// </summary>
    /// <returns>生スクリーン高</returns>
    float GetScreenHeight() const;

    /// <summary>
    /// スクリーン倍率を取得
    /// </summary>
    /// <returns>スケール(1.0=等倍)</returns>
    float GetScale() const;

    /// <summary>
    /// X倍率を取得
    /// </summary>
    /// <returns>スケール(1.0=等倍)</returns>
    float GetRatioX() const;

    /// <summary>
    /// Y倍率を取得
    /// </summary>
    /// <returns>スケール(1.0=等倍)</returns>
    float GetRatioY() const;

    /// <summary>
    /// X方向のオフセットを設定
    /// </summary>
    /// <param name="offsetX">生Xオフセット</param>
    void SetDiffX(float offsetX);

    /// <summary>
    /// Y方向のオフセットを設定
    /// </summary>
    /// <param name="offsetY">生Yオフセット</param>
    void SetDiffY(float offsetY);

    /// <summary>
    /// スクリーン幅(100%幅)を設定
    /// </summary>
    /// <param name="width">生スクリーン幅</param>
    void SetScreenWidth(float width);

    /// <summary>
    /// スクリーン高(100%高)を設定
    /// </summary>
    /// <param name="height">生スクリーン高</param>
    void SetScreenHeight(float height);

    /// <summary>
    /// スクリーン倍率を設定
    /// </summary>
    /// <param name="scale">スケール(1.0=等倍)</param>
    void SetScale(float scale);

    /// <summary>
    /// X倍率を設定
    /// </summary>
    /// <param name="ratioX">スケール(1.0=等倍)</param>
    void SetRatioX(float ratioX);

    /// <summary>
    /// Y倍率を設定
    /// </summary>
    /// <param name="ratioY">スケール(1.0=等倍)</param>
    void SetRatioY(float ratioY);


    /// <summary>
    /// 描画座標Xから割合Yを算出
    /// </summary>
    /// <param name="rawX">生座標X</param>
    /// <returns>スケーラ幅に対するXの割合(百分率)</returns>
	float CalculatePositionRateX(float rawX) const;

    /// <summary>
    /// 描画座標Yから割合Yを算出
    /// </summary>
    /// <param name="rawY">生座標Y</param>
    /// <returns>スケーラ幅に対するYの割合(百分率)</returns>
	float CalculatePositionRateY(float rawY) const;

	/// <summary>
	/// 割合Xから描画座標Xを算出
	/// </summary>
	/// <param name="px">割合X(百分率)</param>
	/// <returns>生座標X</returns>
    float CalculatePositionX(float px) const;

    /// <summary>
    /// 割合Yから描画座標Yを算出
    /// </summary>
    /// <param name="py">割合Y(百分率)</param>
    /// <returns>生座標Y</returns>
    float CalculatePositionY(float py) const;

    /// <summary>
    /// 割合幅から描画幅を算出
    /// </summary>
    /// <param name="width">割合幅(百分率)</param>
    /// <returns>描画幅</returns>
    float CalculateWidth(float width) const;

    /// <summary>
    /// 割合高から描画高を算出
    /// </summary>
    /// <param name="height">割合高(百分率)</param>
    /// <returns>描画高</returns>
    float CalculateHeight(float height) const;

    /// <summary>
    /// 割合スクリーンデータから描画スクリーンデータを算出
    /// </summary>
    /// <param name="dataOfPercent">各割合を保持したScreenData</param>
    /// <returns>直描画できる座標を保持したScreenData</returns>
    ScreenData Calculate(const ScreenData& dataOfPercent) const;

    /// <summary>
    /// 割合座標から描画スクリーンデータを算出
    /// </summary>
    /// <param name="px">割合X(百分率)</param>
    /// <param name="py">割合Y(百分率)</param>
    /// <param name="width">割合幅(百分率)</param>
    /// <param name="height">割合高(百分率)</param>
    /// <returns>直描画できる座標を保持したScreenData</returns>
    ScreenData Calculate(float px, float py, float width, float height) const;
};