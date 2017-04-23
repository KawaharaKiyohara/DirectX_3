#include "stdafx.h"
#include "Player.h"
#include "game.h"

Player::Player()
{
	rotation = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);
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

	//スペキュラマップをロード。
	hr = D3DXCreateTextureFromFileA(g_pd3dDevice,
		"Assets/model/utc_spec.tga",
		&specularMap
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
	if (specularMap != NULL) {
		//スペキュラマップの読み込みが成功したので、CSkinModelにスペきゅらマップを設定する。
		model.SetSpecularMap(specularMap);
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
	else if (GetAsyncKeyState('W')) {
		//スペキュラマップのオフ
		model.SetSpecularMap(NULL);
	}
	else if (GetAsyncKeyState('D')) {
		model.SetSpecularMap(specularMap);
	}
	if (GetAsyncKeyState(VK_SHIFT)) {
		float angle = 0.0f;
		if (GetAsyncKeyState(VK_LEFT)) {
			angle = D3DXToRadian(3.0f);
		}
		else if (GetAsyncKeyState(VK_RIGHT)) {
			angle = D3DXToRadian(-3.0f);
		}
		D3DXQUATERNION rot;
		D3DXQuaternionRotationAxis(&rot, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), angle);
		rotation *= rot;
	}
	animation.Update(1.0f / 60.0f);
	model.UpdateWorldMatrix(D3DXVECTOR3(0.0f, 0.0f, 0.0f), rotation, D3DXVECTOR3(1.0f, 1.0f, 1.0f));
}
void Player::Render()
{
	Camera* cam = game->GetCamera();
	model.Draw(&cam->GetViewMatrix(), &cam->GetProjectionMatrix());
}