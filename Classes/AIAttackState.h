#ifndef __AIRHOCKEY_AIATTACKSTATE_H__
#define __AIRHOCKEY_AIATTACKSTATE_H__

#include "IFSMState.h"
#include "cocos2d.h"
#include "AirHockey_fwd.h"


namespace airhockey
{
	/* AI Attack state */

	class AIAttackState: public IFSMState
	{
	public:
		AIAttackState(GameField* game_field, PaddlePtr ai_paddle, PaddlePtr player_paddle, cocos2d::Sprite* puck, float attack_radius);
		~AIAttackState();

		bool onEnter() override;
		void onExit() override;
		//void update() override;
		void handleTransitions() override;
		void pause() override;
		void resume() override;

	private:
		GameField* m_field = nullptr;
		PaddlePtr m_aiPaddle = nullptr;
		PaddlePtr m_playerPaddle = nullptr;
		cocos2d::Sprite* m_puck = nullptr;
		float m_attackRadius;
		int m_attackActionTag = 2606;
	};

}

#endif // __AIRHOCKEY_AIATTACKSTATE_H