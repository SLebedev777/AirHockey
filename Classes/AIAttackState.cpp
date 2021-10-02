#include "AIAttackState.h"
#include "FSMContext.h"

namespace airhockey
{
	AIAttackState::AIAttackState(GameField* game_field, PaddlePtr ai_paddle, cocos2d::Sprite* puck) :
		IFSMState(),
		m_field(game_field),
		m_aiPaddle(ai_paddle),
		m_puck(puck)
	{}

	AIAttackState::~AIAttackState()
	{}

	void AIAttackState::onEnter()
	{
		auto ai_attack_action = [this]() {
			auto move_push_puck = cocos2d::MoveTo::create(1.0f, m_puck->getPosition());
			return move_push_puck;
		};
		m_aiPaddle->getStick()->runAction(ai_attack_action());
	}

	void AIAttackState::onExit()
	{
		m_aiPaddle->getStick()->stopAllActions();
	}


	void AIAttackState::handleTransitions()
	{
		if (m_puck->getPosition().y < m_field->getCenter().y)
		{
			if (m_context->getCurrentState())
			{
				m_context->popState();
			}
		}
	}
}