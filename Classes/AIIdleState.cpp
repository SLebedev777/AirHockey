#include "AIIdleState.h"
#include "AIAttackState.h"
#include "AIDefenseState.h"
#include "FSMContext.h"

namespace airhockey
{
	AIIdleState::AIIdleState(GameField* game_field, PaddlePtr ai_paddle, cocos2d::Sprite* puck, float attack_radius) :
		IFSMState(),
		m_field(game_field),
		m_aiPaddle(ai_paddle),
		m_puck(puck),
		m_attackRadius(attack_radius)
	{}

	AIIdleState::~AIIdleState()
	{}

	bool AIIdleState::onEnter()
	{
		auto ai_idle_action = [this]() {
			const float SHIFT_X = m_aiPaddle->getRadius();
			auto move_left = cocos2d::MoveBy::create(2.0f, cocos2d::Vec2(-2*SHIFT_X, 0));
			auto move_right = cocos2d::MoveBy::create(1.0f, cocos2d::Vec2(SHIFT_X, 0));
			auto delay = cocos2d::DelayTime::create(1.0f);
			auto move_cycle = cocos2d::RepeatForever::create(cocos2d::Sequence::create(move_right, move_left, move_right, delay, nullptr));
			return move_cycle;
		};
		m_aiPaddle->getStick()->runAction(ai_idle_action());
		return true;
	}

	void AIIdleState::onExit()
	{
		m_aiPaddle->getStick()->stopAllActions();
	}

	void AIIdleState::handleTransitions()
	{
		if (m_puck->getPosition().distance(m_aiPaddle->getPosition()) <= m_attackRadius)
		{
			m_context->pushState(std::make_unique<AIDefenseState>(m_field, m_aiPaddle, m_puck, m_attackRadius));
			m_context->pushState(std::make_unique<AIAttackState>(m_field, m_aiPaddle, m_puck, m_attackRadius));
		}
	}

}