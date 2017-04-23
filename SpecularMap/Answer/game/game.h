/*!
 * @brief	ゲームクラス。
 */

#pragma once

#include "Player.h"

/*!
 * @brief	ゲームクラス。
 */
class Game{
public:
	/*!
	 * @brief	コンストラクタ。
	 */
	Game();
	/*!
	 * @brief	デストラクタ。
	 */
	~Game();
	/*!
	 * @brief	ゲームが起動してから一度だけ呼ばれる関数。
	 */
	void Start();
	/*!
	 * @brief	更新。
	 */
	void Update();
	/*!
	 * @brief	描画。
	 */
	void Render();
	/*!
	* @brief	デフォルトライトを取得。
	*/
	Light* GetDefaultLight()
	{
		return &light;
	}
	/*!
	* @brief	カメラを取得。
	*/
	Camera* GetCamera()
	{
		return &camera;
	}
private:
	Camera camera;
	Light light;
	Player player;
};

extern Game* game;