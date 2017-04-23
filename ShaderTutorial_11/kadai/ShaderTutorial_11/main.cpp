/*!
 *@brief	シェーダーのチュートリアル00
 */
#include "global.h"
#pragma warning( disable : 4996 ) 
#include <strsafe.h>
#pragma warning( default : 4996 )
#include <cstdlib>
#include "Model.h"
#include "RenderTarget.h"

//-----------------------------------------------------------------------------
// グローバル変数。
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D = NULL;		
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;
ID3DXEffect*			g_pModelEffect = NULL;	

D3DXMATRIX				g_viewMatrix;		//ビュー行列。カメラ行列とも言う。
D3DXMATRIX				g_projectionMatrix;	//プロジェクション行列。ビュー空間から射影空間に変換する行列。
D3DXMATRIX				g_worldMatrix;		//ワールド行列。モデルローカル空間から、ワールド空間に変換する行列。

CModel			g_tiger;			//虎。
CModel			g_ground;			//地面。

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
		&g_pModelEffect,
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
void InitViewProjectionMatrix()
{	
	D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-5.0f );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &g_viewMatrix, &vEyePt, &vLookatPt, &vUpVec );
    
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &g_projectionMatrix,D3DXToRadian(60.0f), 1.0f, 1.0f, 100.0f );
}


//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
	g_tiger.Release();
	if (g_pModelEffect != NULL) {
		g_pModelEffect->Release();
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
	//虎を回す。
	static int renderCount = 0;
	renderCount++;
	D3DXQUATERNION qRot;
	D3DXQuaternionRotationAxis(&qRot, &D3DXVECTOR3(0.0f, 1.0f, 0.0f), renderCount * 0.01f);
	g_tiger.SetRotation(qRot);
	// Clear the backbuffer to a blue color
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 255), 1.0f, 0);
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		// Turn on the zbuffer
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

		//虎を描画。
		g_tiger.Draw(&g_viewMatrix, &g_projectionMatrix);
		g_ground.Draw(&g_viewMatrix, &g_projectionMatrix);
				
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
bool InitGeometry()
{
	if ( g_tiger.Load("tiger.x") == false ) {
		return false;
	}
	if ( g_ground.Load("ground.x") == false ) {
		return false;
	}
	g_tiger.SetPosition(D3DXVECTOR3(0.0f, 2.0f, 0.0f));
	g_ground.SetPosition(D3DXVECTOR3(0.0f, -1.0f, 0.0f));
	g_ground.SetScale(D3DXVECTOR3(5.0f, 5.0f, 5.0f));
    return true;
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
		if (InitGeometry() == true)
		{
			InitViewProjectionMatrix();
			
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
