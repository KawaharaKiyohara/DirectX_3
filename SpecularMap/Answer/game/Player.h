#pragma once
class Player
{
public:
	Player();
	~Player();
	void Init();
	void Update();
	void Render();
private:
	SkinModel model;
	SkinModelData modelData;
	Animation animation;
	D3DXQUATERNION rotation;
	LPDIRECT3DTEXTURE9 normalMap = NULL;		//�@���}�b�v�B
	LPDIRECT3DTEXTURE9 specularMap = NULL;		//�X�y�L�����}�b�v�B
};

