#pragma once

#include "lib/RenderTarget.h"

class Bloom{
public:
	/*!
	 * @brief	コンストラクタ。
	 */
	Bloom();
	/*!
	 * @brief	デストラクタ。
	 */
	~Bloom();
	/*!
	 * @brief	描画。
	 */
	void Render();
	//重みの更新。
	void UpdateWeight(float dispersion);
private:
	static const int MGF_DOWN_SAMPLE_COUNT = 9;							//!<MGFダウンサンプリングの回数。
	LPD3DXEFFECT effect;												//!<シェーダーエフェクト。
	CRenderTarget luminanceRenderTarget;								//!<輝度を抽出するためのレンダリングターゲット。
	CRenderTarget downSamplingRenderTarget[MGF_DOWN_SAMPLE_COUNT][2];	//!<MGFダウンサンプリング用のレンダリングターゲット。
	static const int NUM_WEIGHTS = 8;									//!<ガウスブラーの重み。
	float weights[NUM_WEIGHTS];											//!<ガウスブラーで使う重みテーブル。
};