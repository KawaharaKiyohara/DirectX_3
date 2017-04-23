/*!
 * @brief	ゲームクラス。
 */

#pragma once

#include "Player.h"
#include "Map.h"
#include "Physics/Physics.h"
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
	//ゲームカメラのインスタンスを取得。
	Camera* GetCamera()
	{
		return &camera;
	}
	//物理ワールドの取得。
	PhysicsWorld* GetPhysicsWorld()
	{
		return &physicsWorld;
	}
private:
	D3DXVECTOR3 toCameraPos;
	Camera camera;
	Player player;
	Map map;
	PhysicsWorld physicsWorld;
};

extern Game* game;