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
ID3DXEffect*			g_pEffect = NULL;	

D3DXMATRIX				g_viewMatrix;		//�r���[�s��B�J�����s��Ƃ������B
D3DXMATRIX				g_projectionMatrix;	//�v���W�F�N�V�����s��B�r���[��Ԃ���ˉe��Ԃɕϊ�����s��B
D3DXMATRIX				g_worldMatrix;		//���[���h�s��B���f�����[�J����Ԃ���A���[���h��Ԃɕϊ�����s��B

LPD3DXMESH				g_pMesh = NULL;
LPDIRECT3DTEXTURE9*	 	g_pMeshTextures = NULL; 	// Textures for our mesh
DWORD              	 	g_dwNumMaterials = 0L;   	// Number of mesh materials

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
	d3dpp.EnableAutoDepthStencil = TRUE;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
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
	return S_OK;
}

/*!
 *@brief	�v���W�F�N�V�����s��̏������B
 */
void InitProjectionMatrix()
{
	D3DXMatrixIdentity( &g_worldMatrix );
	
	D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-5.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &g_viewMatrix, &vEyePt, &vLookatPt, &vUpVec );
    
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &g_projectionMatrix, D3DX_PI / 4, 1.0f, 1.0f, 100.0f );
}


//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	if (g_pMeshTextures != NULL) {
		for (int i = 0; i < g_dwNumMaterials; i++) {
			g_pMeshTextures[i]->Release();
		}
		delete[] g_pMeshTextures;
	}
	if (g_pMesh != NULL) {
		g_pMesh->Release();
	}
	if (g_pEffect != NULL) {
		g_pEffect->Release();
	}
	if (g_pd3dDevice != NULL)
		g_pd3dDevice->Release();

	if (g_pD3D != NULL)
		g_pD3D->Release();
}

//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
	// Clear the backbuffer to a blue color
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);
	static int renderCount = 0;
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		// Turn on the zbuffer
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		//�����������̐ݒ�B
		g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		g_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		g_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		renderCount++;
		D3DXMATRIXA16 matWorld;
    	D3DXMatrixRotationY( &g_worldMatrix, renderCount / 500.0f ); 
    	
		
		//�V�F�[�_�[�K�p�J�n�B
		g_pEffect->SetTechnique("SkinModel");
		g_pEffect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
		g_pEffect->BeginPass(0);

		//�萔���W�X�^�ɐݒ肷��J���[�B
		D3DXVECTOR4 color( 1.0f, 0.0f, 0.0f, 1.0f);
		//���[���h�s��̓]���B
		g_pEffect->SetMatrix("g_worldMatrix", &g_worldMatrix);
		//�r���[�s��̓]���B
		g_pEffect->SetMatrix("g_viewMatrix", &g_viewMatrix);
		//�v���W�F�N�V�����s��̓]���B
		g_pEffect->SetMatrix("g_projectionMatrix", &g_projectionMatrix);
		g_pEffect->CommitChanges();						//���̊֐����Ăяo�����ƂŁA�f�[�^�̓]�����m�肷��B�`����s���O�Ɉ�񂾂��Ăяo���B
		
		// Meshes are divided into subsets, one for each material. Render them in
        // a loop
        for( DWORD i = 0; i < g_dwNumMaterials; i++ )
        {
			g_pEffect->SetTexture("g_diffuseTexture", g_pMeshTextures[i]);
            // Draw the mesh subset
            g_pMesh->DrawSubset( i );
        }
        
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

/*!
 *@brief	X�t�@�C����ǂݍ���ŃW�I���g�����������B
 */
HRESULT InitGeometry()
{
    LPD3DXBUFFER pD3DXMtrlBuffer;

    // Load the mesh from the specified file
    if( FAILED( D3DXLoadMeshFromX( "Tiger.x", D3DXMESH_SYSTEMMEM,
                                   g_pd3dDevice, NULL,
                                   &pD3DXMtrlBuffer, NULL, &g_dwNumMaterials,
                                   &g_pMesh ) ) )
    {
        // If model is not in current folder, try parent folder
        if( FAILED( D3DXLoadMeshFromX( "..\\Tiger.x", D3DXMESH_SYSTEMMEM,
                                       g_pd3dDevice, NULL,
                                       &pD3DXMtrlBuffer, NULL, &g_dwNumMaterials,
                                       &g_pMesh ) ) )
        {
            MessageBox( NULL, "Could not find tiger.x", "Meshes.exe", MB_OK );
            return E_FAIL;
        }
    }

    // We need to extract the material properties and texture names from the 
    // pD3DXMtrlBuffer
    D3DXMATERIAL* d3dxMaterials = ( D3DXMATERIAL* )pD3DXMtrlBuffer->GetBufferPointer();
   
    g_pMeshTextures = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];
    if( g_pMeshTextures == NULL )
        return E_OUTOFMEMORY;

    for( DWORD i = 0; i < g_dwNumMaterials; i++ )
    {
        g_pMeshTextures[i] = NULL;
        if( d3dxMaterials[i].pTextureFilename != NULL &&
            lstrlenA( d3dxMaterials[i].pTextureFilename ) > 0 )
        {
            // Create the texture
            if( FAILED( D3DXCreateTextureFromFileA( g_pd3dDevice,
                                                    d3dxMaterials[i].pTextureFilename,
                                                    &g_pMeshTextures[i] ) ) )
            {
                // If texture is not in current folder, try parent folder
                const CHAR* strPrefix = "..\\";
                CHAR strTexture[MAX_PATH];
                strcpy_s( strTexture, MAX_PATH, strPrefix );
                strcat_s( strTexture, MAX_PATH, d3dxMaterials[i].pTextureFilename );
                // If texture is not in current folder, try parent folder
                if( FAILED( D3DXCreateTextureFromFileA( g_pd3dDevice,
                                                        strTexture,
                                                        &g_pMeshTextures[i] ) ) )
                {
                    MessageBox( NULL, "Could not find texture map", "Meshes.exe", MB_OK );
                }
            }
        }
    }

    // Done with the material buffer
    pD3DXMtrlBuffer->Release();

    return S_OK;
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
		WS_OVERLAPPEDWINDOW, 100, 100, 500, 500,
		NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		// Create the vertex buffer
		if (SUCCEEDED(InitGeometry()))
		{
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
