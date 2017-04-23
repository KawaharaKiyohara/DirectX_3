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
LPDIRECT3DVERTEXBUFFER9 g_pVB = NULL;		
ID3DXEffect*			g_pEffect = NULL;	

D3DXMATRIX				g_viewMatrix;		//ビュー行列。カメラ行列とも言う。
D3DXMATRIX				g_projectionMatrix;	//プロジェクション行列。ビュー空間から射影空間に変換する行列。
D3DXMATRIX				g_worldMatrix;		//ワールド行列。モデルローカル空間から、ワールド空間に変換する行列。
/*!
 *@brief	頂点。
 */
struct SVertex
{
	FLOAT x, y, z, w;
	DWORD color;
};

/*!
 *@brief	頂点定義。
 */
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZW|D3DFVF_DIFFUSE)



/*!
 *@brief	シェーダーエフェクトファイル(*.fx)をロード。
 */
void LoadEffectFile()
{
	LPD3DXBUFFER  compileErrorBuffer = NULL;
	//シェーダーをコンパイル。
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
	d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
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
		{ -10000.0f,  -10000.0f, 0.0f, 1.0f, 0xffffff00, }, 
		{ 0.0f, 10000.0f, 0.0f, 1.0f, 0xff00ff00, },
		{ 10000.0f,  -10000.0f, 0.0f, 1.0f, 0xff00ffff, },
	};

	// Create the vertex buffer. Here we are allocating enough memory
	// (from the default pool) to hold all our 3 custom vertices. We also
	// specify the FVF, so the vertex buffer knows what data it contains.
	if (FAILED(g_pd3dDevice->CreateVertexBuffer(3 * sizeof(SVertex),
		0, D3DFVF_CUSTOMVERTEX,
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
}



void InitProjectionMatrix()
{
	D3DXMatrixIdentity(&g_worldMatrix);

	D3DXVECTOR3 vEyePt(0.0f, 0.0f, -4.0f);
	D3DXVECTOR3 vLookatPt(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
	D3DXMATRIXA16 matView;
	D3DXMatrixLookAtLH(&g_viewMatrix, &vEyePt, &vLookatPt, &vUpVec);

	D3DXMATRIXA16 matProj;
	D3DXMatrixPerspectiveFovLH(&g_projectionMatrix, D3DX_PI / 4, 1.0f, 1.0f, 90000.0f);
}
//-----------------------------------------------------------------------------
// Name: Render()
// Desc: Draws the scene
//-----------------------------------------------------------------------------
VOID Render()
{
	// Clear the backbuffer to a blue color
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET| D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);

	static int renderCount = 0;
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		D3DXVECTOR4 colorTbl[] = {
			D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f),
			D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f),
		};
		renderCount++;
		D3DXVECTOR3 trans[] = {
			D3DXVECTOR3(0.0f, 0.0f, 80000.0f),
			D3DXVECTOR3(0.0f, 5000.0f, 80001.0f),
		};
		for (int i = 0; i < 2; i++) {
			//シェーダー適用開始。
			g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
			g_pd3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			g_pEffect->SetTechnique("ColorPrim");
			g_pEffect->Begin(NULL, D3DXFX_DONOTSAVESHADERSTATE);
			g_pEffect->BeginPass(0);

			//定数レジスタに設定するカラー。
			D3DXVECTOR4 color(1.0f, 0.0f, 0.0f, 1.0f);
			
			D3DXMATRIXA16 matTrans;
			D3DXMatrixRotationY(&g_worldMatrix, renderCount / 50.0f);
			D3DXMatrixTranslation(&matTrans, trans[i].x, trans[i].y, trans[i].z);
			g_worldMatrix = g_worldMatrix * matTrans;

			//ワールド行列の転送。
			g_pEffect->SetMatrix("g_worldMatrix", &g_worldMatrix);
			//ビュー行列の転送。
			g_pEffect->SetMatrix("g_viewMatrix", &g_viewMatrix);
			//プロジェクション行列の転送。
			g_pEffect->SetMatrix("g_projectionMatrix", &g_projectionMatrix);
			g_pEffect->SetVector("g_color", &colorTbl[i]);

			g_pEffect->CommitChanges();						//この関数を呼び出すことで、データの転送が確定する。描画を行う前に一回だけ呼び出す。
			g_pd3dDevice->SetStreamSource(0, g_pVB, 0, sizeof(SVertex));
			g_pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
			g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

			g_pEffect->EndPass();
			g_pEffect->End();
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
		WS_OVERLAPPEDWINDOW, 100, 100, 300, 300,
		NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		// Create the vertex buffer
		if (SUCCEEDED(InitVB()))
		{
			//プロジェクション行列の初期化。
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
