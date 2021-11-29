#ifndef __AIRHOCKEY_AIDEFENSESTATE_H__
#define __AIRHOCKEY_AIDEFENSESTATE_H__

#include "IFSMState.h"
#include "cocos2d.h"
#include "AirHockey_fwd.h"
#include "AIPlayerSettings.h"

namespace airhockey
{
	/* AI Defense state */

	class AIDefenseState: public IFSMState
	{
	public:
		AIDefenseState(GameField* game_field, PaddlePtr ai_paddle, PaddlePtr player_paddle, cocos2d::Sprite* puck, float attack_radius,
			const Pyramid& pyramid);
		~AIDefenseState();

		bool onEnter() override;
		void onExit() override;
		void update() override;
		void handleTransitions() override;
		void pause() override;
		void resume() override;

	private:
		GameField* m_field = nullptr;
		PaddlePtr m_aiPaddle = nullptr;
		PaddlePtr m_playerPaddle = nullptr;
		cocos2d::Sprite* m_puck = nullptr;
		Pyramid m_pyramid;
		float m_attackRadius;

	};

}

#endif // __AIRHOCKEY_AIDEFENSESTATE_H