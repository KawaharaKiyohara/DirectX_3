/*!
 * @brief	頂点バッファ
 */

#include "tkVertexBuffer.h"


extern LPDIRECT3DDEVICE9 g_pd3dDevice;

CVertexBuffer::CVertexBuffer() :
	m_pVB(nullptr),
	m_stride(0),
	m_numVertex(0),
	m_size(0)
{
}
CVertexBuffer::~CVertexBuffer()
{
	Release();
}
void CVertexBuffer::Create(
		int numVertex, 
		int stride, 
		const D3DVERTEXELEMENT9* vertexLayout, 
		const void* pSrcVertexBuffer )
{
	Release();
	m_numVertex = numVertex;
	m_stride = stride;
	m_size = m_numVertex * m_stride;
	HRESULT hr = g_pd3dDevice->CreateVertexBuffer(
		numVertex * stride,
		0,
		0,
		D3DPOOL_DEFAULT,
		&m_pVB,
		NULL
	);
	if(pSrcVertexBuffer != nullptr){
		//ソースが指定されている。
		//頂点バッファをロックしてコピー。
		void* pDstVertexBuffer;
		hr = m_pVB->Lock( 0, 0, &pDstVertexBuffer, D3DLOCK_DISCARD );
		//まるっとコピー。

		memcpy( pDstVertexBuffer, pSrcVertexBuffer, m_size);
		m_pVB->Unlock();
	}
	//頂点定義を作成。
	g_pd3dDevice->CreateVertexDeclaration(vertexLayout, &m_pVertexDecl);
}
void CVertexBuffer::Release()
{
	if(m_pVB != nullptr){
		m_pVB->Release();
		m_pVB = nullptr;
	}
}
