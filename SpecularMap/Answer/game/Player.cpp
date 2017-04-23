#include "stdafx.h"
#include "Player.h"
#include "game.h"

Player::Player()
{
	rotation = D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f);
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

	//�X�y�L�����}�b�v�����[�h�B
	hr = D3DXCreateTextureFromFileA(g_pd3dDevice,
		"Assets/model/utc_spec.tga",
		&specularMap
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
	if (specularMap != NULL) {
		//�X�y�L�����}�b�v�̓ǂݍ��݂����������̂ŁACSkinModel�ɃX�y�����}�b�v��ݒ肷��B
		model.SetSpecularMap(specularMap);
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
	else if (GetAsyncKeyState('W')) {
		//�X�y�L�����}�b�v�̃I�t
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