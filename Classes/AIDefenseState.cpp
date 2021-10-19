#include "AIDefenseState.h"
#include "AIAttackState.h"
#include "AIIdleState.h"
#include "FSMContext.h"

namespace airhockey
{
	AIDefenseState::AIDefenseState(GameField* game_field, PaddlePtr ai_paddle, cocos2d::Sprite* puck, float attack_radius) :
		IFSMState(),
		m_field(game_field),
		m_aiPaddle(ai_paddle),
		m_puck(puck),
		m_attackRadius(attack_radius)
	{}

	AIDefenseState::~AIDefenseState()
	{}

	bool AIDefenseState::onEnter()
	{
		auto ai_defense_action = [this]() {
			cocos2d::Rect gate_rect = m_field->getGoalGate(airhockey::GoalGateLocationType::UPPER).getRect();
			auto move_back_to_gate = cocos2d::MoveTo::create(1.0f, cocos2d::Vec2(gate_rect.getMidX(), gate_rect.getMinY()));
			return move_back_to_gate;
		};
		m_aiPaddle->getStick()->runAction(ai_defense_action());
		return true;
	}

	void AIDefenseState::onExit()
	{
		m_aiPaddle->getStick()->stopAllActions();
	}


	void AIDefenseState::handleTransitions()
	{
		if (m_puck->getPosition().distance(m_aiPaddle->getPosition()) <= m_attackRadius)
		{
			m_context->pushState(std::make_unique<AIAttackState>(m_field, m_aiPaddle, m_puck, m_attackRadius));
		}
	}
}