/*!
 *@brief	インデックスバッファ
 */

#include "tkIndexBuffer.h"

extern LPDIRECT3DDEVICE9 g_pd3dDevice;

CIndexBuffer::CIndexBuffer() :
	m_pIB(nullptr)
{

}
CIndexBuffer::~CIndexBuffer()
{
	Release();
}
void CIndexBuffer::Create(int numIndex, D3DFORMAT format, const void* pSrcIndexBuffer)
{
	Release();
	int size = 0;
	D3DFORMAT d3dFormat;
	if (format == D3DFMT_INDEX16) {
		d3dFormat = D3DFMT_INDEX16;
		size = numIndex * 2;
	}
	else if (format == D3DFMT_INDEX32) {
		d3dFormat = D3DFMT_INDEX32;
		size = numIndex * 4;
	}
	HRESULT hr = g_pd3dDevice->CreateIndexBuffer(
		size,
		0,
		(D3DFORMAT)d3dFormat,
		D3DPOOL_DEFAULT,
		&m_pIB,
		nullptr
		);
	if (pSrcIndexBuffer != nullptr) {
		//ソースが指定されている。
		//インデックスバッファをロックしてコピー。
		void* pDstIndexBuffer;
		hr = m_pIB->Lock(0, 0, &pDstIndexBuffer, D3DLOCK_DISCARD);
		//まるっとコピー。
		memcpy(pDstIndexBuffer, pSrcIndexBuffer, size);
		m_pIB->Unlock();
	}
}
void CIndexBuffer::Release()
{
	if (m_pIB != nullptr) {
		m_pIB->Release();
		m_pIB = nullptr;
	}
}