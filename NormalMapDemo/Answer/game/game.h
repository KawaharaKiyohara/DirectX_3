/*!
 * @brief	�Q�[���N���X�B
 */

#pragma once

#include "Player.h"

/*!
 * @brief	�Q�[���N���X�B
 */
class Game{
public:
	/*!
	 * @brief	�R���X�g���N�^�B
	 */
	Game();
	/*!
	 * @brief	�f�X�g���N�^�B
	 */
	~Game();
	/*!
	 * @brief	�Q�[�����N�����Ă����x�����Ă΂��֐��B
	 */
	void Start();
	/*!
	 * @brief	�X�V�B
	 */
	void Update();
	/*!
	 * @brief	�`��B
	 */
	void Render();
	/*!
	* @brief	�f�t�H���g���C�g���擾�B
	*/
	Light* GetDefaultLight()
	{
		return &light;
	}
	/*!
	* @brief	�J�������擾�B
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