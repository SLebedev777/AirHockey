#include "AIDefenseState.h"
#include "AIAttackState.h"
#include "AIIdleState.h"
#include "FSMContext.h"
#include "cocos2d.h"

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
		getContext()->getLogger()->log("AIDefenseState::onEnter(): enter");

		auto ai_defense_action = [this]() {
			cocos2d::Rect gate_rect = m_field->getGoalGate(airhockey::GoalGateLocationType::UPPER).getRect();
			float puck_x_offset = m_puck->getPosition().x - m_field->getCenter().x;
			float puck_width = m_puck->getBoundingBox().size.width;
			float paddle_x_offset = 0.0f;
			float puck_x_offset_thres = puck_width * 2;
			if (puck_x_offset < -puck_x_offset_thres)
			{
				paddle_x_offset = -gate_rect.size.width / 4;
			}
			else if (puck_x_offset > puck_x_offset_thres)
			{
				paddle_x_offset = gate_rect.size.width / 4;
			}
			auto move_back_to_gate = cocos2d::MoveTo::create(0.5f, cocos2d::Vec2(gate_rect.getMidX() + paddle_x_offset, gate_rect.getMinY()));
			return move_back_to_gate;
		};
		m_aiPaddle->getStick()->runAction(ai_defense_action());
		return true;
	}

	void AIDefenseState::onExit()
	{
		getContext()->getLogger()->log("AIDefenseState::onExit()");

		m_aiPaddle->getStick()->stopAllActions();
	}


	void AIDefenseState::handleTransitions()
	{
		if (m_puck->getPosition().distance(m_aiPaddle->getPosition()) <= m_attackRadius &&
			m_puck->getPosition().y < m_aiPaddle->getPosition().y)
		{
			getContext()->getLogger()->log("AIDefenseState::handleTransitions(): making transition to Attack State");

			m_context->pushState(std::make_unique<AIAttackState>(m_field, m_aiPaddle, m_puck, m_attackRadius));
		}
	}

	void AIDefenseState::update()
	{
		using namespace cocos2d;
		static float puck_old_x = 0.0f;
		float puck_dx = (puck_old_x != 0.0f) ? (m_puck->getPosition().x - puck_old_x) : 0.0f;
		puck_old_x = m_puck->getPosition().x;
		float puck_x_center_offset = m_puck->getPosition().x - m_field->getCenter().x;
		float shift = 2;
		if (puck_x_center_offset < 0 && puck_dx < 0)
		{
			m_aiPaddle->getStick()->runAction(MoveBy::create(0, Vec2(-shift, 0)));
		}
		else if (puck_x_center_offset > 0 && puck_dx > 0)
		{
			m_aiPaddle->getStick()->runAction(MoveBy::create(0, Vec2(shift, 0)));
		}
	}

	void AIDefenseState::pause()
	{
		m_aiPaddle->getStick()->pause();
	}

	void AIDefenseState::resume()
	{
		m_aiPaddle->getStick()->resume();
	}

}