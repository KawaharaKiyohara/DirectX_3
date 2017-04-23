#include "stdafx.h"
#include "bloom.h"

void Bloom::UpdateWeight(float dispersion)
{
	float total = 0;
	for (int i = 0; i<NUM_WEIGHTS; i++) {
		weights[i] = expf(-0.5f*(float)(i*i) / dispersion);
		total += 2.0f*weights[i];

	}
	// 規格化
	for (int i = 0; i < NUM_WEIGHTS; i++) {
		weights[i] /= total;
	}
}

Bloom::Bloom()
{
	//ブルームのシェーダーをロード
	effect = g_effectManager->LoadEffect("Assets/Shader/bloom.fx");
	
	//HandOn-2 輝度抽出用のレンダリングターゲットを作成する。
	luminanceRenderTarget.Create(
		FRAME_BUFFER_WITDH,
		FRAME_BUFFER_HEIGHT,
		1,
		D3DFMT_A16B16G16R16F,
		D3DFMT_D16,
		D3DMULTISAMPLE_NONE,
		0
	);
	//ブラーをかけるためのダウンサンプリング用のレンダリングターゲットを作成。
	//横ブラー用。
	downSamplingRenderTarget[0].Create(
		FRAME_BUFFER_WITDH >> 1,	//横の解像度をフレームバッファの半分にする。
		FRAME_BUFFER_HEIGHT,
		1,
		D3DFMT_A16B16G16R16F,	//ここも浮動小数点バッファにする。
		D3DFMT_D16,				//使わないので16bit。本来は作成する必要もない。
		D3DMULTISAMPLE_NONE,	//マルチサンプリングの種類。今回はマルチサンプリングは行わないのでD3DMULTISAMPLE_NONEでいい。
		0						//マルチサンプリングの品質レベル。今回はマルチサンプリングは行わないので0でいい。
	);
	//縦ブラー用。
	downSamplingRenderTarget[1].Create(
		FRAME_BUFFER_WITDH >> 1,	//縦と横の解像度をフレームバッファの半分にする。
		FRAME_BUFFER_HEIGHT >> 1,
		1,
		D3DFMT_A16B16G16R16F,	//ここも浮動小数点バッファにする。
		D3DFMT_D16,				//使わないので16bit。本来は作成する必要もない。
		D3DMULTISAMPLE_NONE,	//マルチサンプリングの種類。今回はマルチサンプリングは行わないのでD3DMULTISAMPLE_NONEでいい。
		0						//マルチサンプリングの品質レベル。今回はマルチサンプリングは行わないので0でいい。
		);

}
Bloom::~Bloom()
{
}
void Bloom::Render()
{

	// Zテストで失敗してもらったら困るので、Zテストは無効にしておく。
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	//まずは輝度を抽出する。
	{
		// αブレンドもいらない。
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		//HandsOn-3 輝度抽出用のレンダリングターゲットに変更する。
		g_pd3dDevice->SetRenderTarget(0, luminanceRenderTarget.GetRenderTarget());
		g_pd3dDevice->SetDepthStencilSurface(luminanceRenderTarget.GetDepthStencilBuffer());
		//黒でクリア。
		g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		// 輝度抽出テクニックをセット。
		effect->SetTechnique("SamplingLuminance");
		effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
		effect->BeginPass(0);
		//シーンテクスチャを設定する。
		effect->SetTexture("g_scene", mainRenderTarget->GetTexture());
		//定数レジスタへの変更をコミット。
		effect->CommitChanges();
		DrawQuadPrimitive();

		effect->EndPass();
		effect->End();
		// 変更したレンダリングステートを元に戻す。
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	}

	//ガウスブラーで使う重みテーブルを更新。
	UpdateWeight(25.0f);
	//輝度を抽出したテクスチャをXブラー
	{
		//HandsOn-4 ダウンサンプリング用のレンダリングターゲットを追加。
		g_pd3dDevice->SetRenderTarget(
			0, 
			downSamplingRenderTarget[0].GetRenderTarget()
		);
		//HandsOn-5 横ブラー用のシェーダーテクニックを設定する。
		effect->SetTechnique("XBlur");

		effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
		effect->BeginPass(0);
		float size[2] = {
			(float)(luminanceRenderTarget.GetWidth()),
			(float)(luminanceRenderTarget.GetHeight())
		};
		float offset[] = {
			16.0f / (float)(luminanceRenderTarget.GetWidth()),
			0.0f
		};
		effect->SetValue( "g_luminanceTexSize", size, sizeof(size));
		effect->SetValue( "g_offset", offset, sizeof(size));
		effect->SetValue( "g_weight", weights, sizeof(weights));

		effect->SetTexture( "g_blur", luminanceRenderTarget.GetTexture());
		effect->CommitChanges();
		DrawQuadPrimitive();

		effect->EndPass();
		effect->End();
	}
	//輝度を抽出したテクスチャをYブラー
	{
		//HandsOn-6 ダウンサンプリング用のレンダリングターゲットを追加。
		g_pd3dDevice->SetRenderTarget(
			0,
			downSamplingRenderTarget[1].GetRenderTarget()
		);

		//HandsOn-7 縦ブラー用のシェーダーテクニックを設定する。
		effect->SetTechnique("YBlur");

		effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
		effect->BeginPass(0);
		float size[2] = {
			(float)(downSamplingRenderTarget[0].GetWidth()),
			(float)(downSamplingRenderTarget[0].GetHeight())
		};
		float offset[] = {
			0.0f ,
			16.0f / (float)(downSamplingRenderTarget[0].GetHeight())
		};
		effect->SetValue("g_luminanceTexSize", size, sizeof(size));
		effect->SetValue("g_offset", offset, sizeof(size));
		effect->SetValue("g_weight", weights, sizeof(weights));

		effect->SetTexture("g_blur", downSamplingRenderTarget[0].GetTexture());
		effect->CommitChanges();
		DrawQuadPrimitive();

		effect->EndPass();
		effect->End();
	}

	//メインのレンダリングターゲットに戻す。
	g_pd3dDevice->SetRenderTarget(0, mainRenderTarget->GetRenderTarget());
	g_pd3dDevice->SetDepthStencilSurface(mainRenderTarget->GetDepthStencilBuffer());


	{
		//最終合成。
		float offset[] = {
			0.5f / downSamplingRenderTarget[1].GetWidth() ,
			0.5f / downSamplingRenderTarget[1].GetHeight()
		};
		
		//加算合成。
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		effect->SetTechnique( "Final");
		effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
		effect->BeginPass(0);
		effect->SetTexture("g_blur", downSamplingRenderTarget[1].GetTexture());
		effect->SetValue( "g_offset", offset, sizeof(offset));
		effect->CommitChanges();
		DrawQuadPrimitive();

		effect->EndPass();
		effect->End();

		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}

	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
}