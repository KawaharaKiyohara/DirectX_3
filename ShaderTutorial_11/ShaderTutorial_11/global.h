#pragma once
#include <d3d9.h>
#include <d3dx9effect.h>



extern LPDIRECT3D9             g_pD3D;
extern LPDIRECT3DDEVICE9       g_pd3dDevice;
extern ID3DXEffect*			g_pModelEffect;
extern ID3DXEffect*			g_pSpriteEffect;

#include "Model.h"
#include "ShadowMap.h"

extern CModel			g_tiger;			//虎。
extern CModel			g_ground;			//地面。
extern CShadowMap g_shadowMap;	//シャドウマップ。
