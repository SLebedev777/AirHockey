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
		//m_aiPaddle->getStick()->runAction(ai_attack_action());
		
		 Vec2 x0_puck = m_puck->getPosition();
		 Vec2 x0_paddle = m_aiPaddle->getPosition();
		 Vec2 v_puck = m_puck->getPhysicsBody()->getVelocity();
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
		 float z1 = (A - C * sqrt(D)) / (1.0f + Cpow2);
		 float z2 = (A + C * sqrt(D)) / (1.0f + Cpow2);
		 // z = sin(alpha)
		 if (z1 > 1.0f || z1 < -1.0f)
			 return;
		 if (z2 > 1.0f || z2 < -1.0f)
			 return;
		 float alpha1 = asin(z1);
		 float alpha2 = asin(z2);
		 alpha = alpha2;  // here should be more correct way of choosing the right alpha
		 alpha = alpha + M_PI;
		 Vec2 v_paddle = Vec2(v_paddle_scalar * sin(alpha), v_paddle_scalar * cos(alpha));

		 m_aiPaddle->getStick()->getPhysicsBody()->setVelocity(v_paddle);
	}

	void AIAttackState::onExit()
	{
		m_aiPaddle->getStick()->getPhysicsBody()->setVelocity(cocos2d::Vec2::ZERO);
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