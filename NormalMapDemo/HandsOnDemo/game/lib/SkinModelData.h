/*!
 *@brief	スキンモデルデータ
 */
 
#pragma once


struct D3DXFRAME_DERIVED : public D3DXFRAME {
	D3DXMATRIXA16	CombinedTransformationMatrix;	//合成済み行列。
};
struct D3DXMESHCONTAINER_DERIVED : public D3DXMESHCONTAINER {
	LPDIRECT3DTEXTURE9* ppTextures;
	LPD3DXMESH pOrigMesh;
	LPD3DXATTRIBUTERANGE pAttributeTable;
	DWORD NumAttributeGroups;
	DWORD NumInfl;
	LPD3DXBUFFER pBoneCombinationBuf;
	D3DXMATRIX** ppBoneMatrixPtrs;
	D3DXMATRIX* pBoneOffsetMatrices;
	DWORD NumPaletteEntries;
	bool UseSoftwareVP;
	DWORD iAttributeSW;
};
class Animation;
/*!
 *@brief	スキンモデルデータ
 */
class SkinModelData{
public:
	/*!
	 *@brief	コンストラクタ。
	 */
	SkinModelData();
	/*!
	 *@brief	デストラクタ。
	 */
	~SkinModelData();
	/*!
	 * @brief	モデルデータをロード。
	 *@param[in]	filePath	ファイルパス。
	 *@param[out]	anim		アニメーション付きモデルデータの場合、アニメーションクラスも構築されます。
	 */
	void LoadModelData( const char* filePath, Animation* anim );
	/*!
	* @brief	リリース。
	*/
	void Release();
	LPD3DXFRAME GetFrameRoot()
	{
		return frameRoot;
	}
	ID3DXAnimationController* GetAnimationController()
	{
		return pAnimController;
	}
	/*!
	* @brief	ボーン行列を更新。
	*/
	void UpdateBoneMatrix( const D3DXMATRIX& matWorld );
private:
	LPD3DXFRAME					frameRoot;		//フレームルート。
	ID3DXAnimationController*   pAnimController;	//アニメーションコントローラ。
};
