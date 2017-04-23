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
	LPD3DXEFFECT effect;						//!<シェーダーエフェクト。
	CRenderTarget luminanceRenderTarget;		//!<輝度を抽出するためのレンダリングターゲット。
	CRenderTarget downSamplingRenderTarget[2];	//!<ブラーをかけるためのダウンサンプリング用のレンダリングターゲット。
	static const int NUM_WEIGHTS = 8;			//!<ガウスブラーの重み。
	float weights[NUM_WEIGHTS];					//!<ガウスブラーで使う重みテーブル。
};