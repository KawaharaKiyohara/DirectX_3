#pragma once

/*!
*@brief	���f���N���X�B
*/
class CModel {
	LPD3DXMESH				pMesh = NULL;			//���b�V��
	LPDIRECT3DTEXTURE9*	 	pMeshTextures = NULL; 	// �e�N�X�`��
	DWORD              	 	dwNumMaterials = 0L;   	// �}�e���A���A�̐��B
	D3DXVECTOR3				position;				// ���W�B
	D3DXQUATERNION			rotation;				// ��]�B
	D3DXVECTOR3				scale;					// �g��B
public:
	CModel();
	~CModel();
	void Release();
	bool Load(char* filePath);
	void SetScale(const D3DXVECTOR3& scale)
	{
		this->scale = scale;
	}
	void SetPosition(const D3DXVECTOR3& pos)
	{
		position = pos;
	}
	void SetRotation(const D3DXQUATERNION& rot)
	{
		rotation = rot;
	}
	const D3DXVECTOR3& GetPosition()
	{
		return position;
	}
	void Draw(D3DXMATRIX* viewMatrix, D3DXMATRIX* projMatrix);
};
