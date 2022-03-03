#ifndef __AIRHOCKEY_AIATTACKSTATE_H__
#define __AIRHOCKEY_AIATTACKSTATE_H__

#include "IFSMState.h"
#include "cocos2d.h"
#include "AirHockey_fwd.h"
#include "AIPlayerSettings.h"

namespace airhockey
{
	/* AI Attack state */

	class AIAttackState: public IFSMState
	{
	public:
		AIAttackState(GameField* game_field, PaddlePtr ai_paddle, PaddlePtr player_paddle, cocos2d::Sprite* puck, 
			AIPlayerSettings::AttackRadiusFunction attack_radius_func);
		~AIAttackState();

		bool onEnter() override;
		void onExit() override;
		//void update() override;
		void handleTransitions() override;
		void pause() override;
		void resume() override;

	private:
		bool calcEncounter(float paddle_vel_scalar, 
			/*OUT*/ float& predicted_time, 
			/*OUT*/ cocos2d::Vec2& predicted_x_paddle,
			/*OUT*/ cocos2d::Vec2& predicted_x_puck
		);

	private:
		GameField* m_field = nullptr;
		PaddlePtr m_aiPaddle = nullptr;
		PaddlePtr m_playerPaddle = nullptr;
		cocos2d::Sprite* m_puck = nullptr;
		AIPlayerSettings::AttackRadiusFunction m_attackRadiusFunc = nullptr;
		const int m_attackActionTag = 2606;
	};

}

#endif // __AIRHOCKEY_AIATTACKSTATE_H