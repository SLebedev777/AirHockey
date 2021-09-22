#include "AIIdleState.h"

namespace airhockey
{
	AIIdleState::AIIdleState(GameField* game_field, PaddlePtr ai_paddle, cocos2d::Sprite* puck) :
		IFSMState(),
		m_field(game_field),
		m_aiPaddle(ai_paddle),
		m_puck(puck)
	{}

	AIIdleState::~AIIdleState()
	{}

	void AIIdleState::onEnter()
	{
		auto ai_idle_action = [this]() {
			const float SHIFT_X = 500;
			auto move_right_1st = cocos2d::MoveBy::create(1.0f, cocos2d::Vec2(SHIFT_X, 0));
			auto move_left = cocos2d::MoveBy::create(2.0f, cocos2d::Vec2(-2*SHIFT_X, 0));
			auto move_right = cocos2d::MoveBy::create(2.0f, cocos2d::Vec2(2 * SHIFT_X, 0));
			auto delay = cocos2d::DelayTime::create(1.0f);
			auto move_cycle = cocos2d::RepeatForever::create(cocos2d::Sequence::create(move_left, move_right, delay, nullptr));
			auto seq = cocos2d::Sequence::create(move_right_1st, move_cycle, nullptr);
			return move_cycle;
		};
		m_aiPaddle->getSprite()->runAction(ai_idle_action());
	}

}