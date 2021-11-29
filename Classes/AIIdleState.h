#ifndef __AIRHOCKEY_AIIDLESTATE_H__
#define __AIRHOCKEY_AIIDLESTATE_H__

#include "IFSMState.h"
#include "cocos2d.h"
#include "AirHockey_fwd.h"

namespace airhockey
{

	/* AI Idle state */

	class AIIdleState: public IFSMState
	{
	public:
		AIIdleState(GameField* game_field, PaddlePtr ai_paddle, PaddlePtr player_paddle, cocos2d::Sprite* puck, float attack_radius);
		~AIIdleState();

		bool onEnter() override;
		void onExit() override;
		//void update() override {};
		void handleTransitions() override;
		void pause() override;
		void resume() override;

	private:
		GameField* m_field = nullptr;
		PaddlePtr m_aiPaddle = nullptr;
		PaddlePtr m_playerPaddle = nullptr;
		cocos2d::Sprite* m_puck = nullptr;
		float m_attackRadius;
	};

}

#endif // __AIRHOCKEY_AIIDLESTATE_H