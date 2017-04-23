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
	//輝度抽出用のレンダリングターゲットを作成する。
	luminanceRenderTarget.Create(
		FRAME_BUFFER_WITDH,		//幅と高さはフレームバッファと同じにしておく。
		FRAME_BUFFER_HEIGHT,
		1,
		D3DFMT_A16B16G16R16F,	//ここも浮動小数点バッファにする。
		D3DFMT_D16,				//使わないので16bit。本来は作成する必要もない。
		D3DMULTISAMPLE_NONE,	//マルチサンプリングの種類。今回はマルチサンプリングは行わないのでD3DMULTISAMPLE_NONEでいい。
		0						//マルチサンプリングの品質レベル。今回はマルチサンプリングは行わないので0でいい。
	);

	int w = FRAME_BUFFER_WITDH;
	int h = FRAME_BUFFER_HEIGHT;
	//MGF用のレンダリングターゲットを作成。
	for (int i = 0; i < MGF_DOWN_SAMPLE_COUNT; i++) {
		//ブラーをかけるためのダウンサンプリング用のレンダリングターゲットを作成。
		//横ブラー用。
		w /= 2;	//横の解像度を半分にする。
		downSamplingRenderTarget[i][0].Create(
			w,
			h,
			1,
			D3DFMT_A16B16G16R16F,	//ここも浮動小数点バッファにする。
			D3DFMT_D16,				//使わないので16bit。本来は作成する必要もない。
			D3DMULTISAMPLE_NONE,	//マルチサンプリングの種類。今回はマルチサンプリングは行わないのでD3DMULTISAMPLE_NONEでいい。
			0						//マルチサンプリングの品質レベル。今回はマルチサンプリングは行わないので0でいい。
		);
		//縦ブラー用。
		h /= 2;	//縦の解像度を半分にする。
		downSamplingRenderTarget[i][1].Create(
			w,	//縦と横の解像度をフレームバッファの半分にする。
			h,
			1,
			D3DFMT_A16B16G16R16F,	//ここも浮動小数点バッファにする。
			D3DFMT_D16,				//使わないので16bit。本来は作成する必要もない。
			D3DMULTISAMPLE_NONE,	//マルチサンプリングの種類。今回はマルチサンプリングは行わないのでD3DMULTISAMPLE_NONEでいい。
			0						//マルチサンプリングの品質レベル。今回はマルチサンプリングは行わないので0でいい。
		);
	}

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
		//輝度抽出用のレンダリングターゲットに変更する。
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
	//ブラーをかける輝度テクスチャのサイズ。
	int luminanceTexW = luminanceRenderTarget.GetWidth();
	int luminanceTexH = luminanceRenderTarget.GetHeight();
	LPDIRECT3DTEXTURE9 blurTexture = luminanceRenderTarget.GetTexture();
	for(int i = 0; i < MGF_DOWN_SAMPLE_COUNT; i++){
		//輝度を抽出したテクスチャをXブラー
		g_pd3dDevice->SetRenderTarget(0, downSamplingRenderTarget[i][0].GetRenderTarget());
		
		effect->SetTechnique("XBlur");
		effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
		effect->BeginPass(0);
		//輝度テクスチャのサイズを転送。
		float size[2];
		size[0] = (float)luminanceTexW;
		size[1] = (float)luminanceTexH;
		effect->SetValue("g_luminanceTexSize", size, sizeof(size));

		//オフセットを転送。
		float offset[2];
		offset[0] = 16.0f / size[0];
		offset[1] = 0.0f;
		effect->SetValue( "g_offset", offset, sizeof(offset));

		//レンダリングターゲットのサイズを転送。
		float renderTargetSize[2];
		renderTargetSize[0] = (float)downSamplingRenderTarget[i][0].GetWidth();
		renderTargetSize[1] = (float)downSamplingRenderTarget[i][0].GetHeight();
		effect->SetValue("g_renderTargetSize", renderTargetSize, sizeof(renderTargetSize));

		effect->SetValue("g_weight", weights, sizeof(weights));

		effect->SetTexture( "g_blur", blurTexture);
		effect->CommitChanges();
		DrawQuadPrimitive();

		effect->EndPass();
		effect->End();

		
	
		//輝度を抽出したテクスチャをYブラー
	
		g_pd3dDevice->SetRenderTarget(0, downSamplingRenderTarget[i][1].GetRenderTarget());

		effect->SetTechnique("YBlur");
		effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
		effect->BeginPass(0);
		
		blurTexture = downSamplingRenderTarget[i][0].GetTexture();
		//輝度テクスチャのサイズを転送。
		size[0] = (float)downSamplingRenderTarget[i][0].GetWidth();
		size[1] = (float)downSamplingRenderTarget[i][0].GetHeight();
		effect->SetValue("g_luminanceTexSize", size, sizeof(size));

		//オフセットを転送。
		offset[0] = 0.0f;
		offset[1] = 16.0f / size[1];
		effect->SetValue("g_offset", offset, sizeof(offset));

		//レンダリングターゲットのサイズを転送。
		renderTargetSize[0] = (float)downSamplingRenderTarget[i][1].GetWidth();
		renderTargetSize[1] = (float)downSamplingRenderTarget[i][1].GetHeight();
		effect->SetValue("g_renderTargetSize", renderTargetSize, sizeof(renderTargetSize));

		effect->SetValue("g_weight", weights, sizeof(weights));

		effect->SetTexture("g_blur", blurTexture);
		effect->SetFloat("g_mulYBlurOutputColor", ((float)(MGF_DOWN_SAMPLE_COUNT - i)) / MGF_DOWN_SAMPLE_COUNT);
		effect->CommitChanges();
		DrawQuadPrimitive();

		effect->EndPass();
		effect->End();
		
		luminanceTexW = downSamplingRenderTarget[i][1].GetWidth();
		luminanceTexH = downSamplingRenderTarget[i][1].GetHeight();
		blurTexture = downSamplingRenderTarget[i][1].GetTexture();

		//加算合成。
		offset[0] = 0.5f / downSamplingRenderTarget[i][1].GetWidth();
		offset[1] = 0.5f / downSamplingRenderTarget[i][1].GetHeight();
		//メインレンダリングターゲットに戻す。
		g_pd3dDevice->SetRenderTarget(0, mainRenderTarget->GetRenderTarget());
		g_pd3dDevice->SetDepthStencilSurface(mainRenderTarget->GetDepthStencilBuffer());
		//加算合成。
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
		effect->SetTechnique("Final");
		effect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
		effect->BeginPass(0);
		effect->SetTexture("g_blur", blurTexture);
		effect->SetValue("g_offset", offset, sizeof(offset));
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