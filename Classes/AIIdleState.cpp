#include "AIIdleState.h"
#include "AIAttackState.h"
#include "AIDefenseState.h"
#include "FSMContext.h"
#include "AIPlayer.h"
#include "CCHelpers.h"
#include "GameField.h"
#include "Paddle.h"

namespace airhockey
{
	AIIdleState::AIIdleState(GameField* game_field, PaddlePtr ai_paddle, PaddlePtr player_paddle, cocos2d::Sprite* puck, 
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

	AIIdleState::~AIIdleState()
	{}

	bool AIIdleState::onEnter()
	{
		using namespace cocos2d;

		getContext()->getLogger()->log("AIIdleState::onEnter(): enter");

		auto ai_idle_action = [this]() {
			const float SHIFT_X = m_aiPaddle->getRadius();
			auto move_to_top = MoveTo::create(1.0f, m_pyramid.pyramidTop);
			auto move_to_base  = MoveTo::create(1.0f, Vec2(m_pyramid.pyramidTop.x, m_pyramid.pyramidLeft.y));
			auto move_left = MoveBy::create(2.0f, Vec2(-2*SHIFT_X, 0));
			auto move_right = MoveBy::create(1.0f, Vec2(SHIFT_X, 0));
			auto delay = DelayTime::create(1.0f);
			auto move_cycle = RepeatForever::create(Sequence::create(move_to_top, move_to_base, delay, nullptr));
			return move_cycle;
		};
		m_aiPaddle->getStick()->runAction(ai_idle_action());
		return true;
	}

	void AIIdleState::onExit()
	{
		getContext()->getLogger()->log("AIIdleState::onExit()");

		m_aiPaddle->getStick()->stopAllActions();
	}

	void AIIdleState::handleTransitions()
	{
		Vec2 x_paddle = m_aiPaddle->getSprite()->getPosition();
		Vec2 x_puck = m_puck->getPosition();

		if (x_puck.y >= m_field->getCenter().y)
		{
			getContext()->getLogger()->log("AIIdleState::handleTransitions(): add DefenseState to stack");

			auto ai_player = static_cast<AIPlayer*>(m_context);
			ai_player->pushState(std::move(ai_player->createDefenseState()));
		}
	}

	void AIIdleState::pause()
	{
		m_aiPaddle->getStick()->pause();
	}

	void AIIdleState::resume()
	{
		m_aiPaddle->getStick()->resume();
	}
}