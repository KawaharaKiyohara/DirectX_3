/*!
 *@brief	シェーダーのチュートリアル00
 */
#include <d3d9.h>
#include <d3dx9effect.h>
#pragma warning( disable : 4996 ) 
#include <strsafe.h>
#pragma warning( default : 4996 )
#include <cstdlib>
#include "ParticleEmitter.h"

//-----------------------------------------------------------------------------
// グローバル変数。
//-----------------------------------------------------------------------------
LPDIRECT3D9             g_pD3D = NULL;		
LPDIRECT3DDEVICE9       g_pd3dDevice = NULL;

D3DXMATRIX				g_viewMatrix;		//ビュー行列。カメラ行列とも言う。
D3DXMATRIX				g_projectionMatrix;	//プロジェクション行列。ビュー空間から射影空間に変換する行列。

CParticleEmitter		g_particleEmitter;	//パーティクルエミッター
D3DXVECTOR3 vEyePt(0.0f, 3.0f, -10.0f);


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


	return S_OK;
}

/*!
 *@brief	プロジェクション行列の初期化。
 */
void InitProjectionMatrix()
{
	
    D3DXVECTOR3 vLookatPt( 0.0f, 1.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &g_viewMatrix, &vEyePt, &vLookatPt, &vUpVec );
    
    D3DXMATRIXA16 matProj;
    D3DXMatrixPerspectiveFovLH( &g_projectionMatrix, D3DX_PI / 4, 1280.0f/720.0f, 1.0f, 100.0f );
}


//-----------------------------------------------------------------------------
// Name: Cleanup()
// Desc: Releases all previously initialized objects
//-----------------------------------------------------------------------------
VOID Cleanup()
{
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
	g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
	static int renderCount = 0;
	if (SUCCEEDED(g_pd3dDevice->BeginScene()))
	{
		// Turn on the zbuffer
		g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
	
		D3DXMATRIX mRot;
		//単位行列に初期化する。
		D3DXMatrixIdentity(&mRot);			
		if (GetAsyncKeyState(VK_LEFT)) {
			//2.0度回転する行列を作成。
			D3DXMatrixRotationY(&mRot, D3DXToRadian(-2.0f));
		}
		if (GetAsyncKeyState(VK_RIGHT)) {
			//-2.0度回転する行列を作成。
			D3DXMatrixRotationY(&mRot, D3DXToRadian(2.0f));
		}
		//作成した行列で視点を回す。
		D3DXVECTOR4 vEyePtOut;
		D3DXVec3Transform(&vEyePtOut, &vEyePt, &mRot);
		vEyePt.x = vEyePtOut.x;
		vEyePt.y = vEyePtOut.y;
		vEyePt.z = vEyePtOut.z;
		//カメラ行列の再計算。
		D3DXVECTOR3 vLookatPt(0.0f, 1.0f, 0.0f);
		D3DXVECTOR3 vUpVec(0.0f, 1.0f, 0.0f);
		D3DXMatrixLookAtLH(&g_viewMatrix, &vEyePt, &vLookatPt, &vUpVec);

		g_particleEmitter.Render(g_viewMatrix, g_projectionMatrix);
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
		WS_OVERLAPPEDWINDOW, 100, 100, 1380, 820,
		NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (SUCCEEDED(InitD3D(hWnd)))
	{
		// Create the vertex buffer
		SParicleEmitParameter param;
		param.texturePath = "star.png";
		param.w = 0.5f;
		param.h = 0.5f;
		param.intervalTime = 0.2f;
		param.initSpeed = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		g_particleEmitter.Init(param);
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
			else {
				g_particleEmitter.Update();
				Render();
			}
		}
	}

	UnregisterClass("Shader Tutorial", wc.hInstance);
	return 0;
}
