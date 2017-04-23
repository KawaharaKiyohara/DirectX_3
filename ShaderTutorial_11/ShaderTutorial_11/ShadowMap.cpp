#include "global.h"
#include "ShadowMap.h"
#include "Model.h"
CShadowMap::CShadowMap()
{
	D3DXMatrixIdentity(&lightViewMatrix);
	D3DXMatrixIdentity(&lightProjMatrix);
	viewPosition.x = 0.0f;
	viewPosition.y = 0.0f;
	viewPosition.z = 0.0f;
	viewTarget.x = 0.0f;
	viewTarget.y = 0.0f;
	viewTarget.z = 0.0f;
}
CShadowMap::~CShadowMap()
{
}

//初期化。
void CShadowMap::Init()
{
	//レンダリングターゲットを初期化。
	renderTarget.Create(
		512,
		512,
		1,						//レンダリングターゲットにはミップマップは不要なので一枚のみ。
		D3DFMT_A8R8G8B8,		//カラーバッファのフォーマットはARGBの32bit
		D3DFMT_D16,				//学生のＰＣで24bitの深度バッファを作成できなかったので、16ビットで深度バッファを作成する。
		D3DMULTISAMPLE_NONE,	//マルチサンプリングはなし。
		0						//マルチサンプリングしないので０を指定。
		);
}


//更新。
void CShadowMap::Update()
{
	//ライトビュープロジェクション行列を更新。
	//普通のカメラと同じ。
	//カメラの上方向を決める計算だけ入れておく。
	D3DXVECTOR3 tmp = viewTarget - viewPosition;
	D3DXVec3Normalize(&tmp, &tmp);
	if (fabsf(tmp.y) > 0.9999f) {
		//カメラがほぼ真上or真下を向いている。
		D3DXMatrixLookAtLH(&lightViewMatrix, &viewPosition, &viewTarget, &D3DXVECTOR3(1.0f, 0.0f, 0.0f));
	}
	else {
		D3DXMatrixLookAtLH(&lightViewMatrix, &viewPosition, &viewTarget, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	}
	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&lightProjMatrix, D3DXToRadian(60.0f), 1.0f, 0.1f, 100.0f);
}
//シャドウマップに書き込み。
void CShadowMap::Draw()
{
	LPDIRECT3DSURFACE9 renderTargetBackup;
	LPDIRECT3DSURFACE9 depthBufferBackup;
	g_pd3dDevice->GetRenderTarget(0, &renderTargetBackup);		//元々のレンダリングターゲットを保存。後で戻す必要があるので。
	g_pd3dDevice->GetDepthStencilSurface(&depthBufferBackup);	//元々のデプスステンシルバッファを保存。後で戻す必要があるので。
																//レンダリングターゲットを変更する。
	g_pd3dDevice->SetRenderTarget(0, renderTarget.GetRenderTarget());
	g_pd3dDevice->SetDepthStencilSurface(renderTarget.GetDepthStencilBuffer());
	//書き込み先を変更したのでクリア。
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);
	g_tiger.Draw(&lightViewMatrix, &lightProjMatrix, true, false);

	g_pd3dDevice->SetRenderTarget(0, renderTargetBackup);		//戻す。
	g_pd3dDevice->SetDepthStencilSurface(depthBufferBackup);	//戻す。
}
