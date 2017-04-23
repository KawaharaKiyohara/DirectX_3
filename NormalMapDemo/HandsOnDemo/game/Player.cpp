#include "stdafx.h"
#include "Player.h"
#include "game.h"

Player::Player()
{
}


Player::~Player()
{
	//�n���Y�I�� 1-3
}
void Player::Init()
{
	
		//���f�������[�h�B
	modelData.LoadModelData("Assets/model/Unity.X", &animation);

	model.Init(&modelData);
	model.SetLight(game->GetDefaultLight());

	//�n���Y�I�� 1-2
	animation.PlayAnimation(0);
}
void Player::Update()
{
	/*if (GetAsyncKeyState('A')) {
		//�@���}�b�v�̃I�t
		model.SetNormalMap(NULL);
	}
	else if (GetAsyncKeyState('S')) {
		//�@���}�b�v�̃I��
		model.SetNormalMap(normalMap);
	}*/
	animation.Update(1.0f / 60.0f);
	model.UpdateWorldMatrix(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXQUATERNION(0.0f, 0.0f, 0.0f, 1.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f));
}
void Player::Render()
{
	Camera* cam = game->GetCamera();
	model.Draw(&cam->GetViewMatrix(), &cam->GetProjectionMatrix());
}