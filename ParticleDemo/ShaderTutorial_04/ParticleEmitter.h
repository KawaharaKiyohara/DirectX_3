/*!
 * @brief	パーティクルの発生機。
 */

#ifndef _TKPARTICLEEMITTER_H_
#define _TKPARTICLEEMITTER_H_

#include <list>
#include <d3d9.h>
#include <d3dx9effect.h>

class CParticle;
/*!
* @brief	パーティクル生成パラメータ
*/
struct SParicleEmitParameter {
	//初期化。
	void Init()
	{
		memset(this, 0, sizeof(SParicleEmitParameter));
	}
	const char* texturePath;			//!<テクスチャのファイルパス。
	float w;							//!<パーティクルの幅。
	float h;							//!<パーティクルの高さ。
	float intervalTime;					//!<パーティクルの発生間隔。
	D3DXVECTOR3 initSpeed;				//!<初速度

};
/*!
 * @brief	パーティクルの発生機
 */
class CParticleEmitter{
public:
	
	/*!
	 * @brief	コンストラクタ。
	 */
	CParticleEmitter();
	/*!
	 * @brief	デストラクタ。
	 */
	~CParticleEmitter();
	/*!
	 *@brief	初期化。
	 *@param[in]	camera		パーティクルの描画で使用するカメラ。
	 *@param[in]	param		パーティクル生成用のパラメータ。
	 */
	void Init(const SParicleEmitParameter& param);
	void Update();
	void Render(const D3DXMATRIX& viewMatrix, const D3DXMATRIX& projMatrix);
private:
	SParicleEmitParameter	param;			//!<パラメータ。
	std::list<CParticle*>	particleList;	//!<パーティクルのリスト。
	float					timer;			//!<タイマー
};


#endif // _TKPARTICLEEMITTER_H_