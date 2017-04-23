#pragma once

#include "Physics/MeshCollider.h"
#include "Physics/RigidBody.h"

//�}�b�v�`�b�v�̔z�u���B
struct SMapChipLocInfo {
	const char* modelName;		//���f���B
	D3DXVECTOR3	pos;			//���W�B
	D3DXQUATERNION	rotation;		//��]�B
};

class MapChip
{
public:
	MapChip();
	~MapChip();
	void Init(SMapChipLocInfo& locInfo);
	void Update();
	void Draw();
private:
	SkinModel model;
	SkinModelData modelData;
	D3DXVECTOR3	position;
	D3DXQUATERNION rotation;
	Light light;
	MeshCollider meshCollider;	//���b�V���R���C�_�[�B
	RigidBody rigidBody;		//���́B
};
