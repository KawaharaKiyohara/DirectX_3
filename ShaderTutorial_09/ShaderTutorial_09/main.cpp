/*!
 *@brief	�V�F�[�_�[�̃`���[�g���A��00
 */
#include <d3d9.h>
#include <d3dx9effect.h>
#pragma warning( disable : 4996 ) 
#include <strsafe.h>
#pragma warning( default : 4996 )
#include <cstdlib>



//-----------------------------------------------------------------------------
// �O���[�o���ϐ��B
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D = NULL;		
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;		
ID3DXEffect*			g_pEffect = NULL;	
LPDIRECT3DTEXTURE9		g_pNormalMap;		//�@���}�b�v�B
LPDIRECT3DTEXTURE9		g_pDiffuseMap;		//�f�B�t���[�Y�}�b�v�B
D3DXMATRIX				g_viewMatrix;		//�r���[�s��B�J�����s��Ƃ������B
D3DXMATRIX				g_projectionMatrix;	//�v���W�F�N�V�����s��B�r���[��Ԃ���ˉe��Ԃɕϊ�����s��B
D3DXMATRIX				g_worldMatrix;		//���[���h�s��B���f�����[�J����Ԃ���A���[���h��Ԃɕϊ�����s��B
D3DXMATRIX				g_rotationMatrix;	//��]�s��B

static const int		LIGHT_NUM = 4;
D3DXVECTOR4 			g_diffuseLightDirection[LIGHT_NUM];	//���C�g�̕����B
D3DXVECTOR4				g_diffuseLightColor[LIGHT_NUM];		//���C�g�̐F�B
D3DXVECTOR4				g_ambientLight;						//����

/*!
 *@brief	���_�B
 */
struct SVertex
{
	FLOAT x, y, z, w;
	DWORD color;
	FLOAT normal_x, normal_y, normal_z;
	FLOAT tangent_x, tangent_y, tangent_z;
	FLOAT u, v;
};

D3DVERTEXELEMENT9 vertexElement[] =
{
	{ 0, 0 ,	D3DDECLTYPE_FLOAT4		, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION	, 0 },
	{ 0, 16,	D3DDECLTYPE_FLOAT1		, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR		, 0 },
	{ 0, 20,	D3DDECLTYPE_FLOAT3		, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL	, 0 },
	{ 0, 32,	D3DDECLTYPE_FLOAT3		, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT	, 0 },
	{ 0, 44,	D3DDECLTYPE_FLOAT2		, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD	, 0 },
	D3DDECL_END()
};
//���_��`
IDirect3DVertexDeclaration9*	m_vertexDecl;

/*!
 *@brief	���_��`�B
 */
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZW|D3DFVF_DIFFUSE|D3DFVF_TEX1)



/*!
 *@brief	�V�F�[�_�[�G�t�F�N�g�t�@�C��(*.fx)�����[�h�B
 */
void LoadEffectFile()
{
	LPD3DXBUFFER  compileErrorBuffer = NULL;
	//�V�F�[�_�[���R���p�C���B
	HRESULT hr = D3DXCreateEffectFromFile(
		g_pd3dDevice,
		"basic.fx",
		NULL,
		NULL,
#ifdef _DEBUG
		D3DXSHADER_DEBUG,
#else
		D3DXSHADER_SKIPVALIDATION,
#endif
		NULL,
		&g_pEffect,
		&compileErrorBuffer
		);
	if (FAILED(hr)) {
		MessageBox(NULL, (char*)(compileErrorBuffer->GetBufferPointer()), "error", MB_OK);
		std::abort();
	}
}
/*!
 * @brief	�e�N�X�`�������[�h�B
 */
void LoadTexture()
{
	if( FAILED( D3DXCreateTextureFromFileA( g_pd3dDevice,
                                                    "normal.jpg",
                                                    &g_pNormalMap ) ) )
	{
		std::abort();
	}
	if (FAILED(D3DXCreateTextureFromFileA(g_pd3dDevice,
		 "diffuse.jpg",
		 &g_pDiffuseMap)))
	{
		 std::abort();
	}

}
/*!
*@brief	���C�g���X�V�B
*/
void UpdateLight()
{
	static float angleX = 0.0f;
	static float angleY = 0.0f;
	if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) == 0 ){
		if (GetAsyncKeyState(VK_UP) & 0x8000) {
			angleX += 3.14 * 0.004f;
		}
		if (GetAsyncKeyState(VK_DOWN)) {
			angleX -= 3.14 * 0.004f;
		}
		if (GetAsyncKeyState(VK_LEFT)) {
			angleY += 3.14 * 0.004f;
		}
		if (GetAsyncKeyState(VK_RIGHT)) {
			angleY -= 3.14 * 0.004f;
		}
	}
	D3DXMATRIX rotMat, rotTmp;
	D3DXMatrixIdentity(&rotMat);
	D3DXMatrixRotationX(&rotMat, angleX);
	D3DXMatrixRotationAxis(&rotTmp, (D3DXVECTOR3*)(&rotMat.m[1]), angleY);
	D3DXMatrixMultiply(&rotMat, &rotMat, &rotTmp);
	memset(g_diffuseLightDirection, 0, sizeof(g_diffuseLightDirection));
	memset(g_diffuseLightColor, 0, sizeof(g_diffuseLightColor));
	D3DXVec4Transform(&g_diffuseLightDirection[0], &D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), &rotMat);
	//D3DXVec4Transform(&g_diffuseLightDirection[1], &D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f), &rotMat);
	//D3DXVec4Transform(&g_diffuseLightDirection[2], &D3DXVECTOR4(0.0f, -1.0f, 0.0f, 1.0f), &rotMat);
	//D3DXVec4Transform(&g_diffuseLightDirection[3], &D3DXVECTOR4(-1.0f, 0.0f, 0.0f, 1.0f), &rotMat);
	//�f�B�t���[�Y���C�g�B
	g_diffuseLightColor[0] = D3DXVECTOR4(0.5f, 0.5f, 0.5f, 1.0f);
	//g_diffuseLightColor[1] = D3DXVECTOR4(0.5f, 0.5f, 0.5f, 1.0f);
	//g_diffuseLightColor[2] = D3DXVECTOR4(0.5f, 0.4f, 0.6f, 1.0f);
	//g_diffuseLightColor[3] = D3DXVECTOR4(0.2f, 0.2f, 0.2f, 1.0f);

	//�����B
	g_ambientLight = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
}
//-----------------------------------------------------------------------------
// Name: InitD3D()
// Desc: Initializes Direct3D
//-----------------------------------------------------------------------------
HRESULT InitD3D(HWND hWnd)
{
	// Create the D3D object.
	if (NULL == (g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
		return E_FAIL;

	// Set up the structure used to create the D3DDevice
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

	// Create the D3DDevice
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&d3dpp, &g_pd3dDevice)))
	{
		return E_FAIL;
	}

	// Device state would normally be set here
	//�V�F�[�_�[�G�t�F�N�g�t�@�C���̃��[�h�B
	LoadEffectFile();
	//�e�N�X�`���̃��[�h�B
	LoadTexture();
	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InitVB()
// Desc: Creates a vertex buffer and fills it with our vertices. The vertex
//       buffer is basically just a chuck of memory that holds vertices. After
//       creating it, we must Lock()/Unlock() it to fill it. For indices, D3D
//       also uses index buffers. The special thing about vertex and index
//       buffers is that they can be created in device memory, allowing some
//       cards to process them in hardware, resulting in a dramatic
//       performance gain.
//-----------------------------------------------------------------------------
HRESULT InitVB()
{
	// Initialize three vertices for rendering a triangle
	SVertex vertices[] =
	{
		{ 
			-1.0f,  -1.0f, 0.0f, 1.0f,		//���W
			0xffffff00,						//�J���[
			0.0f, 0.0f, -1.0f,				//�@��
			1.0f, 0.0f, 0.0f,				//�ڃx�N�g��
			0.0f, 1.0f						//uv
		}, 
		{ 
			-1.0f, 1.0f, 0.0f, 1.0f,		//���W
			0xff00ff00,						//�J���[ 
			0.0f, 0.0f, -1.0f,				//�@��
			1.0f, 0.0f, 0.0f,				//�ڃx�N�g��
			0.0f, 0.0f						//uv
		},
		{ 
			1.0f,  -1.0f, 0.0f, 1.0f,		//���W
			0xff00ffff,						//�J���[
			0.0f, 0.0f, -1.0f,				//�@��
			1.0f, 0.0f, 0.0f,				//�ڃx�N�g��
			1.0f, 1.0f						//uv
		},
		{ 
			1.0f,   1.f, 0.0f, 1.0f,		//���W
			0xff00ffff,						//�J���[
			0.0f, 0.0f, -1.0f,				//�@��
			1.0f, 0.0f, 0.0f,				//�ڃx�N�g��
			1.0f, 0.0f						//uv
		},
	};

	// Create the vertex buffer. Here we are allocating enough memory
	// (from the default pool) to hold all our 3 custom vertices. We also
	// specify the FVF, so the vertex buffer knows what data it contains.
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(4 * sizeof(SVertex),
		0, 0,
		D3DPOOL_DEFAULT, &g_pVB, NULL)))
	{
		return E_FAIL;
	}

	// Now we fill the vertex buffer. To do this, we need to Lock() the VB to
	// gain access to the vertices. This mechanism is required becuase vertex
	// buffers may be in device memory.
	VOID* pVertices;
	if (FAILED(g_pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0)))
		return E_FAIL;
	memcpy(pVertices, vertices, sizeof(vertices));
	g_pVB->Unlock();

	//���_��`���쐬�B
	g_pd3dDevice->CreateVertexDeclaration(vertexElement, &m_vertexDecl);

	return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	if (g_pVB != NULL)
		g_pVB->Release();

	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();
	if (m_vertexDecl != NULL) {
		m_vertexDecl->Release();
	}
}



void InitProjectionMatrix()
{
	D3DXMatrixIdentity(&g_worldMatrix);
	g_rotationMatrix = g_worldMatrix;
	D3DXVECTOR3 vEyePt(0.0f, 0.0f, -2.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&g_viewMatrix, &vEyePt, &vLookatPt, &vUpVec);

	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&g_projectionMatrix, D3DX_PI / 4, 1.0f, 1.0f, 100.0f);
}
//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
	// Clear the backbuffer to a blue color
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

	static int renderCount = 0;
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		renderCount++;
		//���C�g���X�V
		UpdateLight();
		//�V�F�[�_�[�K�p�J�n�B
		g_pEffect->SetTechnique("ColorPrim");
		g_pEffect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
		g_pEffect->BeginPass(0);

		//�萔���W�X�^�ɐݒ肷��J���[�B
		D3DXVECTOR4 color( 1.0f, 0.0f, 0.0f, 1.0f);
		static float angleX = 0.0f;
		static float angleY = 0.0f;
		if (GetAsyncKeyState(VK_SHIFT) & 0x8000) {
			if (GetAsyncKeyState(VK_UP) & 0x8000) {
				angleX += 3.14 * 0.004f;
			}
			if (GetAsyncKeyState(VK_DOWN)) {
				angleX -= 3.14 * 0.004f;
			}
			if (GetAsyncKeyState(VK_LEFT)) {
				angleY += 3.14 * 0.004f;
			}
			if (GetAsyncKeyState(VK_RIGHT)) {
				angleY -= 3.14 * 0.004f;
			}
		}
		D3DXMATRIX rotMat, rotTmp;
		D3DXMatrixIdentity(&rotMat);
		D3DXMatrixRotationX(&rotMat, angleX);
		D3DXMatrixRotationAxis(&rotTmp, (D3DXVECTOR3*)(&rotMat.m[1]), angleY);
		D3DXMatrixMultiply(&g_worldMatrix, &rotMat, &rotTmp);
		g_rotationMatrix = g_worldMatrix;	//��]�������Ă��Ȃ��̂ł��̂܂ܑ���B
		//���[���h�s��̓]���B
		g_pEffect->SetMatrix("g_worldMatrix", &g_worldMatrix);
		//��]�s��̓]���B
		g_pEffect->SetMatrix("g_rotationMatrix", &g_rotationMatrix);
		//���C�g�̌�����]���B
		g_pEffect->SetVectorArray("g_diffuseLightDirection", g_diffuseLightDirection, LIGHT_NUM);
		//���C�g�̃J���[��]���B
		g_pEffect->SetVectorArray("g_diffuseLightColor", g_diffuseLightColor, LIGHT_NUM);
		//������ݒ�B
		g_pEffect->SetVector("g_ambientLight", &g_ambientLight);
		//�r���[�s��̓]���B
		g_pEffect->SetMatrix("g_viewMatrix", &g_viewMatrix);
		//�v���W�F�N�V�����s��̓]���B
		g_pEffect->SetMatrix("g_projectionMatrix", &g_projectionMatrix);
		g_pEffect->SetTexture("g_normalMap", g_pNormalMap);
		g_pEffect->SetTexture("g_diffuseMap", g_pDiffuseMap);
		
		g_pEffect->CommitChanges();						//���̊֐����Ăяo�����ƂŁA�f�[�^�̓]�����m�肷��B�`����s���O�Ɉ�񂾂��Ăяo���B
		g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(SVertex));
		g_pd3dDevice->SetVertexDeclaration(m_vertexDecl);
		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		g_pEffect->EndPass();
		g_pEffect->End();

		// End the scene
		g_pd3dDevice->EndScene();
	}

	// Present the backbuffer contents to the display
	g_pd3dDevice->Present(NULL, NULL, NULL, NULL);
}




//-----------------------------------------------------------------------------
// Name: MsgProc()
// Desc: The window's message handler
//-----------------------------------------------------------------------------
LRESULT WINAPI MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_DESTROY:
		Cleanup();
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}




//-----------------------------------------------------------------------------
// Name: wWinMain()
// Desc: The application's entry point
//-----------------------------------------------------------------------------
INT WINAPI wWinMain(HINSTANCE hInst, HINSTANCE, LPWSTR, INT)
{
	UNREFERENCED_PARAMETER(hInst);

	// Register the window class
	WNDCLASSEX wc =
	{
		sizeof(WNDCLASSEX), CS_CLASSDC, MsgProc, 0L, 0L,
		GetModuleHandle(NULL), NULL, NULL, NULL, NULL,
		"Shader Tutorial", NULL
	};
	RegisterClassEx(&wc);

	// Create the application's window
	HWND hWnd = CreateWindow("Shader Tutorial", "Shader Tutorial 00",
		WS_OVERLAPPEDWINDOW, 100, 100, 700, 700,
		NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		// Create the vertex buffer
		if (SUCCEEDED(InitVB()))
		{
			//�v���W�F�N�V�����s��̏������B
			InitProjectionMatrix();
			// Show the window
			ShowWindow(hWnd, SW_SHOWDEFAULT);
			UpdateWindow(hWnd);

			// Enter the message loop
			MSG msg;
			ZeroMemory(&msg, sizeof(msg));
			while (msg.message != WM_QUIT)
			{
				if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				else
					Render();
			}
		}
	}

	UnregisterClass("Shader Tutorial", wc.hInstance);
	return 0;
}
