#pragma once

/*!
*@brief	モデルクラス。
*/
class CModel {
	LPD3DXMESH				pMesh = NULL;			//メッシュ
	LPDIRECT3DTEXTURE9*	 	pMeshTextures = NULL; 	// テクスチャ
	DWORD              	 	dwNumMaterials = 0L;   	// マテリアルアの数。
	D3DXVECTOR3				position;				// 座標。
	D3DXQUATERNION			rotation;				// 回転。
	D3DXVECTOR3				scale;					// 拡大。
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
