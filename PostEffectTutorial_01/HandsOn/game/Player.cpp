#include "stdafx.h"
#include "Player.h"
#include "game.h"

Player::Player()
{
}


Player::~Player()
{
	//�e�N�X�`���͂����ƊJ������悤�ɁB
	if (normalMap != NULL) {
		//normalMap��NULL����Ȃ��Ȃ烍�[�h����Ă���̂ŊJ������B
		normalMap->Release();
	}
}
void Player::Init()
{
	//�@���}�b�v�����[�h�B
	HRESULT hr = D3DXCreateTextureFromFileA(g_pd3dDevice,
		"Assets/model/utc_nomal.tga",
		&normalMap
	);
	//D3DXCreateTextureFromFileA�̖߂�l���`�F�b�N�B
	if (FAILED(hr)) {
		//D3DXCreateTextureFromFileA�Ŏ��s�����B
		MessageBox(NULL, "�e�N�X�`���̃��[�h�Ɏ��s���܂����B�w�肵���p�X�����������m�F�����肢���܂��B", "�G���[", MB_OK);
	}
		//���f�������[�h�B
	modelData.LoadModelData("Assets/model/Unity.X", &animation);

	model.Init(&modelData);
	model.SetLight(game->GetDefaultLight());
	if (normalMap != NULL) {
		//�@���}�b�v�̓ǂݍ��݂����������̂ŁACSkinModel�ɖ@���}�b�v��ݒ肷��B
		model.SetNormalMap(normalMap);
	}
	animation.PlayAnimation(0);
}
void Player::Update()
{
	if (GetAsyncKeyState('A')) {
		//�@���}�b�v�̃I�t
		model.SetNormalMap(NULL);
	}
	else if (GetAsyncKeyState('S')) {
		//�@���}�b�v�̃I��
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