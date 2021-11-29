#ifndef __AIRHOCKEY_AIPLAYER_H__
#define __AIRHOCKEY_AIPLAYER_H__

#include "cocos2d.h"
#include "AirHockey_fwd.h"
#include "FSMContext.h"
#include "AIPlayerSettings.h"


namespace airhockey
{
	using namespace cocos2d;

	class AIPlayer : public FSMContext
	{
	public:
		AIPlayer() = delete;
		AIPlayer(const AIPlayer& other) = delete;
		AIPlayer& operator=(const AIPlayer& other) = delete;
		AIPlayer(GameField* game_field, PaddlePtr ai_paddle, PaddlePtr enemy_paddle, cocos2d::Sprite* puck, const AIPlayerSettings& settings);

		IFSMStatePtr createIdleState();
		IFSMStatePtr createDefenseState();
		IFSMStatePtr createAttackState();

	private:
		GameField* m_field = nullptr;
		PaddlePtr m_aiPaddle = nullptr;
		PaddlePtr m_enemyPaddle = nullptr;
		cocos2d::Sprite* m_puck = nullptr;
		AIPlayerSettings m_settings;

	};

	typedef std::shared_ptr<AIPlayer> AIPlayerPtr;
}

#endif // __AIRHOCKEY_AIPLAYER_H__