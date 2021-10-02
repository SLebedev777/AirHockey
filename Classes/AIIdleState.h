#ifndef __AIRHOCKEY_AIIDLESTATE_H__
#define __AIRHOCKEY_AIIDLESTATE_H__

#include "IFSMState.h"
#include "cocos2d.h"
#include "GameField.h"  // TODO: refactor to fwd decl
#include "Paddle.h"

namespace airhockey
{
	// fwd
	class FSMContext;
	typedef std::shared_ptr<FSMContext> FSMContextPtr;

	/* AI Idle state */

	class AIIdleState: public IFSMState
	{
	public:
		AIIdleState(GameField* game_field, PaddlePtr ai_paddle, cocos2d::Sprite* puck);
		~AIIdleState();

		void onEnter() override;
		void onExit() override;
		//void update() override {};
		void handleTransitions() override;

	private:
		GameField* m_field = nullptr;
		PaddlePtr m_aiPaddle = nullptr;
		cocos2d::Sprite* m_puck = nullptr;
	};

}

#endif // __AIRHOCKEY_AIIDLESTATE_H