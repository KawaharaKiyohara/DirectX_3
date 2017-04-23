#include "global.h"
#include "Model.h"

CModel::CModel()
{
	position.x = 0.0f;
	position.y = 0.0f;
	position.z = 0.0f;
	scale.x = 1.0f;
	scale.y = 1.0f;
	scale.z = 1.0f;
	D3DXQuaternionIdentity(&rotation);
}
CModel::~CModel()
{
	Release();
}
void CModel::Release()
{
	if (pMeshTextures != NULL) {
		for (int i = 0; i < dwNumMaterials; i++) {
			pMeshTextures[i]->Release();
		}
		delete[] pMeshTextures;
		pMeshTextures = NULL;
	}
	if (pMesh != NULL) {
		pMesh->Release();
		pMesh = NULL;
	}
}
bool CModel::Load(char* filePath)
{
	LPD3DXBUFFER pD3DXMtrlBuffer;

	// ���b�V����x�t�@�C�����烍�[�h�B
	if (FAILED(D3DXLoadMeshFromX(filePath, D3DXMESH_SYSTEMMEM,
		g_pd3dDevice, NULL,
		&pD3DXMtrlBuffer, NULL, &dwNumMaterials,
		&pMesh)))
	{
		//�ǂݍ��ݎ��s�B
		return false;
	}

	// We need to extract the material properties and texture names from the 
	// pD3DXMtrlBuffer
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

	pMeshTextures = new LPDIRECT3DTEXTURE9[dwNumMaterials];
	if (pMeshTextures == NULL)
		return false;

	for (DWORD i = 0; i < dwNumMaterials; i++)
	{
		pMeshTextures[i] = NULL;
		if (d3dxMaterials[i].pTextureFilename != NULL &&
			lstrlenA(d3dxMaterials[i].pTextureFilename) > 0)
		{
			// Create the texture
			if (FAILED(D3DXCreateTextureFromFileA(g_pd3dDevice,
				d3dxMaterials[i].pTextureFilename,
				&pMeshTextures[i])))
			{
				//�e�N�X�`���̓ǂݍ��ݎ��s�B
				return false;
			}
		}
	}

	// Done with the material buffer
	pD3DXMtrlBuffer->Release();
	return true;
}

//�`��֐�
//viewMatrix		�r���[�s��B
//projMatrix		�v���W�F�N�V�����s��B
void CModel::Draw(D3DXMATRIX* viewMatrix, D3DXMATRIX* projMatrix)
{
	D3DXMATRIX mWorld, mRot, mScale;
	D3DXMatrixTranslation(&mWorld, position.x, position.y, position.z);
	D3DXMatrixRotationQuaternion(&mRot, &rotation);
	D3DXMatrixScaling(&mScale, scale.x, scale.y, scale.z);
	mWorld = mScale * mRot * mWorld;
	//�V�F�[�_�[�K�p�J�n�B
	//�ʏ�`��
	g_pModelEffect->SetTechnique("SkinModel");
	g_pModelEffect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
	g_pModelEffect->BeginPass(0);
	//���[���h�s��̓]���B
	g_pModelEffect->SetMatrix("g_worldMatrix", &mWorld);
	//�r���[�s��̓]���B
	g_pModelEffect->SetMatrix("g_viewMatrix", viewMatrix);
	//�v���W�F�N�V�����s��̓]���B
	g_pModelEffect->SetMatrix("g_projectionMatrix", projMatrix);
	// Meshes are divided into subsets, one for each material. Render them in
	// a loop
	
	for (DWORD i = 0; i < dwNumMaterials; i++)
	{
		g_pModelEffect->SetTexture("g_diffuseTexture", pMeshTextures[i]);
		// Draw the mesh subset
		g_pModelEffect->CommitChanges();						//���̊֐����Ăяo�����ƂŁA�f�[�^�̓]�����m�肷��B�`����s���O�Ɉ�񂾂��Ăяo���B
		pMesh->DrawSubset(i);
	}

	g_pModelEffect->EndPass();
	g_pModelEffect->End();
}
