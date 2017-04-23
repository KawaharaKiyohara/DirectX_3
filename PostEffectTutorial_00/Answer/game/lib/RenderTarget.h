#pragma once

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