/*!
 *@brief	シェーダーのチュートリアル00
 */
#include <d3d9.h>
#include <d3dx9effect.h>
#pragma warning( disable : 4996 ) 
#include <strsafe.h>
#pragma warning( default : 4996 )
#include <cstdlib>


//-----------------------------------------------------------------------------
// グローバル変数。
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D = NULL;		
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;
ID3DXEffect*			g_pTigerEffect = NULL;	
ID3DXEffect*			g_pSpriteEffect = NULL;

D3DXMATRIX				g_viewMatrix;		//ビュー行列。カメラ行列とも言う。
D3DXMATRIX				g_projectionMatrix;	//プロジェクション行列。ビュー空間から射影空間に変換する行列。
D3DXMATRIX				g_worldMatrix;		//ワールド行列。モデルローカル空間から、ワールド空間に変換する行列。

LPD3DXMESH				g_pMesh = NULL;
LPDIRECT3DTEXTURE9*	 	g_pMeshTextures = NULL; 	// Textures for our mesh
DWORD              	 	g_dwNumMaterials = 0L;   	// Number of mesh materials

/*!
 *@brief	スプライトクラス。
 */
class CSprite{
private:
	 /*!
	 *@brief	頂点。
	 */
	struct SVertex
	{
		FLOAT x, y, z, w;
		DWORD color;
		FLOAT u, v;
	};

public:
	static const DWORD	D3DFVF_CUSTOMVERTEX = D3DFVF_XYZW| D3DFVF_DIFFUSE| D3DFVF_TEX1;
	/*!
	 *@brief	コンストラクタ。
	 */
	CSprite()
	{
		m_pVB = NULL;
	}
	/*!
	 *@brief	デストラクタ。
	 */
	~CSprite()
	{
		Release();
	}
	/*!
	 *@brief	解放。
	 */
	void Release()
	{
		if(m_pVB){
			m_pVB->Release();
			m_pVB = NULL;
		}
	}
	/*!
	 *@brief	初期化。
	 */
	HRESULT Init()
	{
		Release();
		//頂点バッファの作成。
		SVertex vertices[] =
		{
			{ -1.5f,  -1.5f, 0.0f, 1.0f, 0xFF888888, 0.0f, 1.0f  }, 
			{ -1.5f,   1.5f, 0.0f, 1.0f, 0xFF888888, 0.0f, 0.0f  },
			{ 1.5f,   -1.5f, 0.0f, 1.0f, 0xFF888888, 1.0f, 1.0f  },
			{ 1.5f,    1.5f, 0.0f, 1.0f, 0xFF888888, 1.0f, 0.0f  },
		};

		// Create the vertex buffer. Here we are allocating enough memory
		// (from the default pool) to hold all our 3 custom vertices. We also
		// specify the FVF, so the vertex buffer knows what data it contains.
		if (FAILED(g_pd3dDevice->CreateVertexBuffer(4 * sizeof(SVertex),
			0, D3DFVF_CUSTOMVERTEX,
			D3DPOOL_DEFAULT, &m_pVB, NULL)))
		{
			return E_FAIL;
		}

		// Now we fill the vertex buffer. To do this, we need to Lock() the VB to
		// gain access to the vertices. This mechanism is required becuase vertex
		// buffers may be in device memory.
		VOID* pVertices;
		if (FAILED(m_pVB->Lock(0, sizeof(vertices), (void**)&pVertices, 0)))
			return E_FAIL;
		memcpy(pVertices, vertices, sizeof(vertices));
		m_pVB->Unlock();
		
	}
	/*!
	 *@brief	描画
	 */
	void Draw( 
		const D3DXMATRIX& worldMatrix, 
		LPDIRECT3DTEXTURE9 texture 
	)
	{
		//シェーダー適用開始。
		g_pSpriteEffect->SetTechnique("Sprite");
		g_pSpriteEffect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
		g_pSpriteEffect->BeginPass(0);
		g_pSpriteEffect->SetTexture("g_texture", texture);
		//ワールド行列の転送。
		g_pSpriteEffect->SetMatrix("g_worldMatrix", &worldMatrix);
		//ビュー行列の転送。
		g_pSpriteEffect->SetMatrix("g_viewMatrix", &g_viewMatrix);
		//プロジェクション行列の転送。
		g_pSpriteEffect->SetMatrix("g_projectionMatrix", &g_projectionMatrix);
		
		g_pSpriteEffect->CommitChanges();
		
		g_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(SVertex));
		g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		g_pSpriteEffect->EndPass();
		g_pSpriteEffect->End();
	}
private:
	LPDIRECT3DVERTEXBUFFER9 m_pVB;		//頂点バッファ。
};
/*!
 *@brief	レンダリングターゲット。
 */
class CRenderTarget {
public:
	/*!
	 *@brief	コンストラクタ。
	 */
	CRenderTarget()
	{
		m_depthSurface = NULL;
		m_texture = NULL;
		m_surface = NULL;
		m_texture = NULL;
	}
	/*!
	 *@brief	デストラクタ。
	 */
	~CRenderTarget()
	{
		Release();
	}
	/*!
	 *@brief	解放。
	 */
	void Release()
	{
		if (m_texture != nullptr) {
			m_texture->Release();
			m_texture = NULL;
		}
		if (m_depthSurface != nullptr) {
			m_depthSurface->Release();
			m_depthSurface = NULL;
		}
		if (m_surface != NULL) {
			m_surface->Release();
			m_surface = NULL;
		}
	}
	/*!
	 *@brief	レンダリングターゲットの作成。
	 *@param[in]	w					レンダリングターゲットの幅。
	 *@param[in]	h					レンダリングターゲットの高さ。
	 *@param[in]	mipLevel			ミップマップレベル。
	 *@param[in]	colorFormat			カラーバッファのフォーマット。
	 *@param[in]	depthStencilFormat	デプスステンシルバッファのフォーマット。
	 *@param[in]	multiSampleType		マルチサンプリングの種類。
	 *@param[in]	multiSampleQuality	マルチサンプリングの品質。
	 */
	void Create(
		int w, 
		int h, 
		int mipLevel,
		D3DFORMAT colorFormat,
		D3DFORMAT depthStencilFormat,
		D3DMULTISAMPLE_TYPE multiSampleType,
		int multiSampleQuality)
	{
		//デプスステンシルバッファの作成。
		HRESULT hr = g_pd3dDevice->CreateDepthStencilSurface(
			w,
			h,
			static_cast<D3DFORMAT>(depthStencilFormat),
			static_cast<D3DMULTISAMPLE_TYPE>(multiSampleType),
			multiSampleQuality,
			TRUE,
			&m_depthSurface,
			NULL
		);
		//カラーバッファを作成。
		hr = g_pd3dDevice->CreateTexture(
			w, 
			h,
			mipLevel,
			D3DUSAGE_RENDERTARGET,
			static_cast<D3DFORMAT>(colorFormat),
			D3DPOOL_DEFAULT,
			&m_texture,
			NULL
		);
		m_texture->GetSurfaceLevel(0, &m_surface);
	}
	/*!
	 *@brief	レンダリングターゲットを取得。
	 */
	LPDIRECT3DSURFACE9 GetRenderTarget()
	{
		return m_surface;
	}
	/*!
	 *@brief	デプスステンシルバッファを取得。
	 */
	LPDIRECT3DSURFACE9 GetDepthStencilBuffer()
	{
		return m_depthSurface;
	}
	/*!
	 *@brief	テクスチャを取得。
	 */
	LPDIRECT3DTEXTURE9 GetTexture()
	{
		return m_texture;
	}
private:
	LPDIRECT3DSURFACE9		m_depthSurface;		//!<深度バッファ用のサーフェイス
	LPDIRECT3DTEXTURE9		m_texture;			//!<書き込み先のテクスチャ。
	LPDIRECT3DSURFACE9		m_surface;			//!<サーフェイス
};


CSprite			g_sprite;
CRenderTarget	g_renderTarget;		//レンダリングターゲット。

/*!
 *@brief	シェーダーエフェクトファイル(*.fx)をロード。
 */
void LoadEffectFile()
{
	LPD3DXBUFFER  compileErrorBuffer = NULL;
	//トラ用のシェーダーをコンパイル。
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
		&g_pTigerEffect,
		&compileErrorBuffer
		);
	if (FAILED(hr)) {
		MessageBox(NULL, (char*)(compileErrorBuffer->GetBufferPointer()), "error", MB_OK);
		std::abort();
	}
	
	//スプライト描画用のシェーダーをコンパイル。
	compileErrorBuffer = NULL;
	//シェーダーをコンパイル。
	hr = D3DXCreateEffectFromFile(
		g_pd3dDevice,
		"sprite.fx",
		NULL,
		NULL,
#ifdef _DEBUG
		D3DXSHADER_DEBUG,
#else
		D3DXSHADER_SKIPVALIDATION,
#endif
		NULL,
		&g_pSpriteEffect,
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
	//シェーダーエフェクトファイルのロード。
	LoadEffectFile();
	return S_OK;
}

/*!
 *@brief	プロジェクション行列の初期化。
 */
void InitProjectionMatrix()
{	
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
	if (g_pTigerEffect != NULL) {
		g_pTigerEffect->Release();
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
	renderCount++;
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		// Turn on the zbuffer
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
		
		LPDIRECT3DSURFACE9 renderTargetBackup;
		LPDIRECT3DSURFACE9 depthBufferBackup;
    	g_pd3dDevice->GetRenderTarget(0, &renderTargetBackup);		//元々のレンダリングターゲットを保存。後で戻す必要があるので。
		g_pd3dDevice->GetDepthStencilSurface(&depthBufferBackup);	//元々のデプスステンシルバッファを保存。後で戻す必要があるので。
		//トラをオフスクリーンレンダリング。
		{
			//レンダリングターゲットを変更する。
			g_pd3dDevice->SetRenderTarget(0, g_renderTarget.GetRenderTarget());
			g_pd3dDevice->SetDepthStencilSurface(g_renderTarget.GetDepthStencilBuffer());
			//書き込み先を変更したのでクリア。
			g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);
			
			//シェーダー適用開始。
			g_pTigerEffect->SetTechnique("SkinModel");
			g_pTigerEffect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
			g_pTigerEffect->BeginPass(0);

			//定数レジスタに設定するカラー。
			D3DXVECTOR4 color( 1.0f, 0.0f, 0.0f, 1.0f);
			D3DXMATRIX mTigerMatrix;
    		D3DXMatrixRotationY( &mTigerMatrix, renderCount / 10.0f ); 

			//ワールド行列の転送。
			g_pTigerEffect->SetMatrix("g_worldMatrix", &mTigerMatrix);
			//ビュー行列の転送。
			g_pTigerEffect->SetMatrix("g_viewMatrix", &g_viewMatrix);
			//プロジェクション行列の転送。
			g_pTigerEffect->SetMatrix("g_projectionMatrix", &g_projectionMatrix);
			g_pTigerEffect->CommitChanges();						//この関数を呼び出すことで、データの転送が確定する。描画を行う前に一回だけ呼び出す。
			
			// Meshes are divided into subsets, one for each material. Render them in
	        // a loop
	        for( DWORD i = 0; i < g_dwNumMaterials; i++ )
	        {
				g_pTigerEffect->SetTexture("g_diffuseTexture", g_pMeshTextures[i]);
	            // Draw the mesh subset
	            g_pMesh->DrawSubset( i );
	        }
	        
			g_pTigerEffect->EndPass();
			g_pTigerEffect->End();
		}	
		g_pd3dDevice->SetRenderTarget(0, renderTargetBackup);		//戻す。
		g_pd3dDevice->SetDepthStencilSurface(depthBufferBackup);	//戻す。
		//オフスクリーンレンダリングした絵を板ポリに描画する。
		{
			g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		
			D3DXMATRIXA16 matWorld;
    		D3DXMatrixRotationY( &matWorld, renderCount / 500.0f ); 

			g_sprite.Draw(
				matWorld,
				g_renderTarget.GetTexture()
			);
			g_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		}		
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
 *@brief	Xファイルを読み込んでジオメトリを初期化。
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
			//レンダリングターゲットを初期化。
			g_renderTarget.Create(
				400,
				400,
				1,						//レンダリングターゲットにはミップマップは不要なので一枚のみ。
				D3DFMT_A8R8G8B8,		//カラーバッファのフォーマットはARGBの32bit
				D3DFMT_D16,				//学生のＰＣで24bitの深度バッファを作成できなかったので、16ビットで深度バッファを作成する。
				D3DMULTISAMPLE_NONE,	//マルチサンプリングはなし。
				0						//マルチサンプリングしないので０を指定。
			);
			
			g_sprite.Init();
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
