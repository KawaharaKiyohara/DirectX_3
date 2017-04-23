#include "stdafx.h"
#include "Player.h"
#include "game.h"

Player::Player()
{
}


Player::~Player()
{
	//テクスチャはちゃんと開放するように。
	if (normalMap != NULL) {
		//normalMapがNULLじゃないならロードされているので開放する。
		normalMap->Release();
	}
}
void Player::Init()
{
	//法線マップをロード。
	HRESULT hr = D3DXCreateTextureFromFileA(g_pd3dDevice,
		"Assets/model/utc_nomal.tga",
		&normalMap
	);
	//D3DXCreateTextureFromFileAの戻り値をチェック。
	if (FAILED(hr)) {
		//D3DXCreateTextureFromFileAで失敗した。
		MessageBox(NULL, "テクスチャのロードに失敗しました。指定したパスが正しいか確認をお願いします。", "エラー", MB_OK);
	}
		//モデルをロード。
	modelData.LoadModelData("Assets/model/Unity.X", &animation);

	model.Init(&modelData);
	model.SetLight(game->GetDefaultLight());
	if (normalMap != NULL) {
		//法線マップの読み込みが成功したので、CSkinModelに法線マップを設定する。
		model.SetNormalMap(normalMap);
	}
	animation.PlayAnimation(0);
}
void Player::Update()
{
	if (GetAsyncKeyState('A')) {
		//法線マップのオフ
		model.SetNormalMap(NULL);
	}
	else if (GetAsyncKeyState('S')) {
		//法線マップのオン
		model.SetNormalMap(normalMap);
	}
	animation.Update(1.0f / 60.0f);
	model.UpdateWorldMatrix(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f));
}
void Player::Render()
{
	Camera* cam = game->GetCamera();
	model.Draw(&cam->GetViewMatrix(), &cam->GetProjectionMatrix());
}