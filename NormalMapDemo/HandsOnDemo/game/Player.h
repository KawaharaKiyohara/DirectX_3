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
	//ƒnƒ“ƒYƒIƒ“ 1-1
};

