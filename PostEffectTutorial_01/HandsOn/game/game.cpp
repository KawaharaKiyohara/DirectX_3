/*!
 * @brief	�Q�[��
 */
#include "stdafx.h"
#include "game.h"


/*!
 * @brief	�R���X�g���N�^�B
 */
Game::Game()
{
}
/*!
 * @brief	�f�X�g���N�^�B
 */
Game::~Game()
{
}
/*!
 * @brief	�Q�[�����N�����Ă����x�����Ă΂��֐��B
 */
void Game::Start()
{
	//�J�����������B
	camera.Init();
	camera.SetEyePt(D3DXVECTOR3(0.0f, 1.0f, 2.0f));
	camera.SetLookatPt(D3DXVECTOR3(0.0f, 0.5f, 0.0f));
	camera.Update();

	//�f�t�H���g���C�g���������B
	light.SetDiffuseLightDirection(0, D3DXVECTOR4(0.707f, 0.0f, -0.707f, 1.0f));
	light.SetDiffuseLightDirection(1, D3DXVECTOR4(-0.707f, 0.0f, -0.707f, 1.0f));
	light.SetDiffuseLightDirection(2, D3DXVECTOR4(0.0f, 0.707f, -0.707f, 1.0f));
	light.SetDiffuseLightDirection(3, D3DXVECTOR4(0.0f, -0.707f, -0.707f, 1.0f));

	light.SetDiffuseLightColor(0, D3DXVECTOR4(10.2f, 0.2f, 0.2f, 1.0f));
	light.SetDiffuseLightColor(1, D3DXVECTOR4(0.2f, 0.2f, 0.2f, 1.0f));
	light.SetDiffuseLightColor(2, D3DXVECTOR4(0.3f, 0.3f, 0.3f, 1.0f));
	light.SetDiffuseLightColor(3, D3DXVECTOR4(0.2f, 0.2f, 0.2f, 1.0f));
	light.SetAmbientLight(D3DXVECTOR4(0.3f, 0.3f, 0.3f, 1.0f));
	//�v���C���[�̏������B
	player.Init();
}
/*!
 * @brief	�X�V�B
 */
void Game::Update()
{
	//���C�g�̉�]
	float angle = 0.0f;
	if (GetAsyncKeyState(VK_LEFT)) {
		angle = D3DXToRadian(3.0f);
	}
	else if (GetAsyncKeyState(VK_RIGHT)) {
		angle = D3DXToRadian(-3.0f);
	}
	//���C�g����]������s����쐬����B
	D3DXMATRIX mRot;
	D3DXMatrixRotationY(&mRot, angle);
	//��]������B
	for (int i = 0; i < 4; i++) {
		D3DXVECTOR4 dir;
		D3DXVec4Transform(&dir, &light.GetDiffuseLightDirection()[i], &mRot);
		light.SetDiffuseLightDirection(i, dir);
	}
	player.Update();
}
/*!
 * @brief	�`��B
 */
void Game::Render()
{
	//�V�[���̕`��B
	player.Render();

	//��������|�X�g�G�t�F�N�g�B
	bloom.Render();
}
