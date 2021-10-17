#include "AIAttackState.h"
#include "AIDefenseState.h"
#include "FSMContext.h"
#include <math.h>

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
		using namespace cocos2d;

		auto ai_attack_action = [this]() {
			const float attack_duration = 0.5f;  // in sec
			cocos2d::Vec2 puck_future_offset = attack_duration * m_puck->getPhysicsBody()->getVelocity();
			auto move_push_puck = cocos2d::MoveTo::create(attack_duration, m_puck->getPosition() + puck_future_offset);
			return move_push_puck;
		};
		
		Vec2 x0_puck = m_puck->getPosition();
		Vec2 x0_paddle = m_aiPaddle->getPosition();
		Vec2 v_puck = m_puck->getPhysicsBody()->getVelocity();
		if (v_puck.fuzzyEquals(Vec2::ZERO, 5))
		{
			m_aiPaddle->getStick()->runAction(ai_attack_action());
			return;
		}
		Vec2 dx0 = x0_paddle - x0_puck;
		float v_paddle_scalar = 500.0f;
		float alpha = 0.0f;
		if (dx0.y == 0.0f)
			return;
		float C = dx0.x / dx0.y;
		float Cpow2 = C * C;
		float A = (v_puck.x - C * v_puck.y) / v_paddle_scalar;
		float D = 4 * Cpow2 * (Cpow2 + 1 - A * A);  // discriminant of sqr equation
		if (D < 0.0f)
			return;
		float z1 = (-A - C * sqrt(D)) / (1.0f + Cpow2);
		float z2 = (-A + C * sqrt(D)) / (1.0f + Cpow2);
		// substitution: z = sin(alpha)
		if (z1 > 1.0f || z1 < -1.0f)
			return;
		if (z2 > 1.0f || z2 < -1.0f)
			return;
		float alpha1 = asin(z1);
		float alpha2 = asin(z2);
		alpha1 += M_PI;
		alpha2 += M_PI;
		float alpha1_deg = CC_RADIANS_TO_DEGREES(alpha1);
		float alpha2_deg = CC_RADIANS_TO_DEGREES(alpha2);
		alpha = 0.5f * (alpha1 + alpha2);  // here should be more correct way of choosing the right alpha
		Vec2 v_paddle = Vec2(v_paddle_scalar * sin(alpha), v_paddle_scalar * cos(alpha)); // alpha grows counterclockwise from 0 at y axis

		Vec2 dv = v_paddle - v_puck;
		float tx = dx0.x / (-dv.x);
		float ty = dx0.y / (-dv.y);
		float t = (tx > ty) ? tx : ty;
		Vec2 x_new_puck = x0_puck + v_puck * t;
		Vec2 x_new_paddle = x0_paddle + v_paddle * t;
		if (!m_field->getPlayRect().containsPoint(x_new_paddle))
			return;

		m_aiPaddle->getStick()->getPhysicsBody()->setVelocity(v_paddle);

	}

	void AIAttackState::onExit()
	{
		m_aiPaddle->getStick()->getPhysicsBody()->setVelocity(cocos2d::Vec2::ZERO);
		m_aiPaddle->getStick()->stopAllActions();
	}


	void AIAttackState::handleTransitions()
	{
		if (m_puck->getPosition().distance(m_aiPaddle->getPosition()) > m_attackRadius ||
			m_puck->getPosition().y > m_aiPaddle->getPosition().y ||
			m_aiPaddle->getPosition().y <= m_field->getCenter().y)
		{
			// back to defense state
			m_context->popState();
		}
	}
}