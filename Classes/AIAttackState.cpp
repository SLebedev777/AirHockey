#include "AIAttackState.h"
#include "AIDefenseState.h"
#include "FSMContext.h"

namespace airhockey
{
	AIAttackState::AIAttackState(GameField* game_field, PaddlePtr ai_paddle, cocos2d::Sprite* puck, float attack_radius) :
		IFSMState(),
		m_field(game_field),
		m_aiPaddle(ai_paddle),
		m_puck(puck),
		m_attackRadius(attack_radius)
	{}

	AIAttackState::~AIAttackState()
	{}

	void AIAttackState::onEnter()
	{
		auto ai_attack_action = [this]() {
			auto move_push_puck = cocos2d::MoveTo::create(0.5f, m_puck->getPosition());
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
		if (m_puck->getPosition().distance(m_aiPaddle->getPosition()) > m_attackRadius)
		{
			m_context->pushState(std::make_unique<AIDefenseState>(m_field, m_aiPaddle, m_puck, m_attackRadius));
		}
	}
}