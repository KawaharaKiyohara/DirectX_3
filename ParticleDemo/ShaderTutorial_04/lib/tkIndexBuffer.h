/*!
 *@brief	インデックスバッファ
 */

#ifndef _TKINDEXBUFFER_H_
#define _TKINDEXBUFFER_H_
#include <d3d9.h>
class CIndexBuffer{
public:
	CIndexBuffer();
	~CIndexBuffer();
	/*!
	 *@brief	インデックスバッファの作成。
	 *@param[in]	numIndex		インデックスの数
	 *@param[in]	format			インデックスバッファのフォーマット。
	 *@param[in]	pSrcIndexBuffer	ソースインデックスバッファ。作成されたインデックスバッファにコピーされます。NULLを指定可能。
	 */
	void Create( int numIndex, D3DFORMAT format, const void* pSrcIndexBuffer );
	/*!
	 *@brief	インデックスバッファの解放。
	 */
	void Release();
	/*!
	*@brief	LPDIRECT3DINDEXBUFFER9の取得。
	*/
	LPDIRECT3DINDEXBUFFER9 GetBody()
	{
		return m_pIB;
	}
private:
	LPDIRECT3DINDEXBUFFER9	m_pIB;	//!<インデックスバッファ。
};

#endif //_TKINDEXBUFFER_H_