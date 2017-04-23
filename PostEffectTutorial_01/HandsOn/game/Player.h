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
	LPDIRECT3DTEXTURE9 normalMap = NULL;		//法線マップ。
};

