/*!
 * @brief	カプセルコライダー。
 */

#pragma once

#include "Physics/ICollider.h"

class CapsuleCollider : public ICollider
{
public:
	/*!
	 * @brief	コンストラクタ。
	 */
	CapsuleCollider();
	/*!
	 * @brief	デストラクタ。
	 */
	~CapsuleCollider();
	/*!
	* @brief	作成。
	*/
	void Create(float radius, float height)
	{
		shape = new btCapsuleShape(radius, height);
	}
	btCollisionShape* GetBody() override
	{
		return shape;
	}
private:
	btCapsuleShape*		shape;
};
