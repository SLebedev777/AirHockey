#include "AIDefenseState.h"
#include "AIAttackState.h"
#include "AIIdleState.h"
#include "FSMContext.h"
#include "AIPlayer.h"
#include "GameField.h"
#include "Paddle.h"
#include "cocos2d.h"
#include "CCHelpers.h"

namespace airhockey
{
	AIDefenseState::AIDefenseState(GameField* game_field, PaddlePtr ai_paddle, PaddlePtr player_paddle, cocos2d::Sprite* puck, 
		AIPlayerSettings::AttackRadiusFunction attack_radius_func,
		const Pyramid& pyramid) :
		IFSMState(),
		m_field(game_field),
		m_aiPaddle(ai_paddle),
		m_playerPaddle(player_paddle),
		m_puck(puck),
		m_pyramid(pyramid),
		m_attackRadiusFunc(attack_radius_func)
	{}

	AIDefenseState::~AIDefenseState()
	{}

	bool AIDefenseState::onEnter()
	{
		getContext()->getLogger()->log("AIDefenseState::onEnter(): enter");

		Vec2 v_puck = m_puck->getPhysicsBody()->getVelocity();

		auto ai_move_to_pyramid_action = [this]() {
			cocos2d::Rect gate_rect = m_field->getGoalGate(airhockey::GoalGateLocationType::UPPER).getRect();
			float puck_x_offset = m_puck->getPosition().x - m_field->getCenter().x;
			float puck_width = m_puck->getBoundingBox().size.width;
			float paddle_x_offset = 0.0f;
			float puck_x_offset_thres = puck_width * 2;
			cocos2d::Vec2* defense_point = &m_pyramid.pyramidBase;
			if (puck_x_offset < -puck_x_offset_thres)
			{
				paddle_x_offset = -gate_rect.size.width / 4;
				defense_point = &m_pyramid.pyramidLeft;
			}
			else if (puck_x_offset > puck_x_offset_thres)
			{
				paddle_x_offset = gate_rect.size.width / 4;
				defense_point = &m_pyramid.pyramidRight;
			}
			float paddle_radius = m_aiPaddle->getRadius();
			//auto move_back_to_gate = cocos2d::MoveTo::create(0.25f, cocos2d::Vec2(gate_rect.getMidX() + paddle_x_offset, gate_rect.getMinY() - paddle_radius));
			//return move_back_to_gate;
			auto move_to_pyramid = cocos2d::MoveTo::create(0.25f, *defense_point);
			return move_to_pyramid;
		};

		cocos2d::Action* defense_action = nullptr;

		getContext()->getLogger()->log("AIDefenseState::onEnter(): move to pyramid");
		defense_action = ai_move_to_pyramid_action();
		defense_action->setTag(m_defenseActionTag);

		m_aiPaddle->getStick()->runAction(defense_action);
		return true;
	}

	void AIDefenseState::onExit()
	{
		getContext()->getLogger()->log("AIDefenseState::onExit()");

		m_aiPaddle->getStick()->stopAllActions();
	}


	void AIDefenseState::handleTransitions()
	{
		if (m_puck->getPosition().distance(m_aiPaddle->getPosition()) <= m_attackRadiusFunc(m_puck->getPhysicsBody()->getVelocity()) &&
			m_puck->getPosition().y < m_aiPaddle->getPosition().y
			)
		{

			getContext()->getLogger()->log("AIDefenseState::handleTransitions(): making transition to Attack State");

			auto ai_player = static_cast<AIPlayer*>(m_context);
			ai_player->pushState(std::move(ai_player->createAttackState()));
		}
	}

	void AIDefenseState::update()
	{
		if (m_aiPaddle->getStick()->getActionByTag(m_defenseActionTag))
			return;

		using namespace cocos2d;
		static float puck_old_x = 0.0f;
		float puck_dx = (puck_old_x != 0.0f) ? (m_puck->getPosition().x - puck_old_x) : 0.0f;
		puck_old_x = m_puck->getPosition().x;
		float puck_x_center_offset = m_puck->getPosition().x - m_field->getCenter().x;
		float shift = 5;
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