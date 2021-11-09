#include "AIAttackState.h"
#include "AIDefenseState.h"
#include "FSMContext.h"
#include "CCHelpers.h"
#include <math.h>
#include <tuple>

namespace airhockey
{
	namespace
	{
		using namespace cocos2d;

		Vec2 AzimuthVector(float alpha, float length=1.0f)
		{
			// alpha grows counterclockwise from 0 at y axis
			return Vec2(length * sin(alpha), length * cos(alpha));
		}

		std::tuple<float, float, float> CalcEncounterTime(const Vec2& dx0, const Vec2& dv)
		{
			/*
			* Input parameters:
			dx0 = x0_paddle - x0_puck
			dv = v_paddle - v_puck
			*/
			const float WRONG_TIME = -1.0f;
			const float MIN_THRES = 0.0f;
			const float MAX_THRES = 1.0f;
			const float DELTA_THRES = 0.01f;

			float tx = dx0.x / (-dv.x);
			float ty = dx0.y / (-dv.y);
			float t = WRONG_TIME;

			if (tx <= MIN_THRES || ty <= MIN_THRES ||
				tx >= MAX_THRES || ty >= MAX_THRES ||
				abs(tx - ty) > DELTA_THRES)
			{
				return std::make_tuple(tx, ty, WRONG_TIME);
			}
			t = 0.5f * (tx + ty);

			return std::make_tuple(tx, ty, t);
		}
	}

	AIAttackState::AIAttackState(GameField* game_field, PaddlePtr ai_paddle, cocos2d::Sprite* puck, float attack_radius) :
		IFSMState(),
		m_field(game_field),
		m_aiPaddle(ai_paddle),
		m_puck(puck),
		m_attackRadius(attack_radius)
	{}

	AIAttackState::~AIAttackState()
	{}

	bool AIAttackState::onEnter()
	{
		getContext()->getLogger()->log("AIAttackState::onEnter(): enter");

		using namespace cocos2d;

		auto ai_attack_action = [this]() {
			const float attack_duration = 0.5f;  // in sec
			cocos2d::Vec2 puck_future_offset = attack_duration * m_puck->getPhysicsBody()->getVelocity();
			auto move_push_puck = cocos2d::MoveTo::create(attack_duration, m_puck->getPosition() + puck_future_offset);
			return move_push_puck;
		};
		
		Vec2 x0_puck = m_puck->getPosition();
		if (!m_field->getPlayRect(airhockey::GoalGateLocationType::UPPER).containsPoint(x0_puck))
		{
			getContext()->getLogger()->log("AIAttackState::onEnter(): attack failed, puck is outside AI play rect");
			return false;
		}

		Vec2 x0_paddle = m_aiPaddle->getPosition();
		Vec2 v_puck = m_puck->getPhysicsBody()->getVelocity();
		if (v_puck.fuzzyEquals(Vec2::ZERO, 5))
		{
			getContext()->getLogger()->log("AIAttackState::onEnter(): runAction(ai_attack_action) because puck is not moving");

			m_aiPaddle->getStick()->runAction(ai_attack_action());
			return true;
		}
		Vec2 dx0 = x0_paddle - x0_puck;
		float v_paddle_scalar = 1000.0f;
		float alpha = 0.0f;
		float z1, z2;
		if (dx0.x < dx0.y)
		{
			float C = dx0.x / dx0.y;
			float Cpow2 = C * C;
			float A = (v_puck.x - C * v_puck.y) / v_paddle_scalar;
			float E = Cpow2 + 1 - A * A;
			float sqrtE = sqrt(E);
			float D = 4 * Cpow2 * E;  // discriminant of sqr equation
			if (D < 0.0f)
			{
				getContext()->getLogger()->log("AIAttackState::onEnter(): attack failed, D < 0");
				return false;
			}
			z1 = (-A - C * sqrtE) / (1.0f + Cpow2);
			z2 = (-A + C * sqrtE) / (1.0f + Cpow2);
			getContext()->getLogger()->log("dx0.x < dx0.y");
		}
		else
		{
			float C = dx0.y / dx0.x;
			float Cpow2 = C * C;
			float A = (v_puck.y - C * v_puck.x) / v_paddle_scalar;
			float E = Cpow2 + 1 - A * A;
			float sqrtE = sqrt(E);
			float D = 4 * E;  // discriminant of sqr equation
			if (D < 0.0f)
			{
				getContext()->getLogger()->log("AIAttackState::onEnter(): attack failed, D < 0");
				return false;
			}
			z1 = (C * A - sqrtE) / (1.0f + Cpow2);
			z2 = (C * A + sqrtE) / (1.0f + Cpow2);
			getContext()->getLogger()->log("dx0.x >= dx0.y");
		}
		// substitution: z = sin(alpha)
		if (z1 > 1.0f || z1 < -1.0f)
			return false;
		if (z2 > 1.0f || z2 < -1.0f)
			return false;
		float alpha1 = asin(z1);
		float alpha2 = asin(z2);
		alpha1 += M_PI;
		alpha2 += M_PI;
		float alpha1_deg = alpha1 * (180.0f / M_PI);
		float alpha2_deg = alpha2 * (180.0f / M_PI);
		Vec2 v_paddle1 = AzimuthVector(alpha1, v_paddle_scalar);
		Vec2 v_paddle2 = AzimuthVector(alpha2, v_paddle_scalar);
		
		float t1x, t1y, t1;
		float t2x, t2y, t2;
		std::tie(t1x, t1y, t1) = CalcEncounterTime(dx0, v_paddle1 - v_puck);
		std::tie(t2x, t2y, t2) = CalcEncounterTime(dx0, v_paddle2 - v_puck);
		if (t1 < 0.0f && t2 < 0.0f)
		{
			getContext()->getLogger()->log("AIAttackState::onEnter(): attack failed, both predicted times t1 and t2 < 0");
			return false;
		}
		float t = t2;
		Vec2 v_paddle = v_paddle2;

		Vec2 x_new_paddle1 = x0_paddle + v_paddle1 * t1;
		Vec2 x_new_paddle2 = x0_paddle + v_paddle2 * t2;
		Vec2 x_new_paddle = x0_paddle + v_paddle * t;

		Vec2 x_new_puck1 = x0_puck + v_puck * t1;
		Vec2 x_new_puck2 = x0_puck + v_puck * t2;
		Vec2 x_new_puck = x0_puck + v_puck * t;

		using namespace CCHelpers;
		getContext()->getLogger()->log("-----------------------------");
		getContext()->getLogger()->log("Paddle position prediction");
		getContext()->getLogger()->log("alpha1(deg) =" + std::to_string(alpha1_deg) + ",  " + "alpha2(deg) =" + std::to_string(alpha2_deg));
		getContext()->getLogger()->log("v_paddle1 = " + Vec2Str(v_paddle1) + ", " + "v_paddle2 = " + Vec2Str(v_paddle2));
		getContext()->getLogger()->log("t1x =" + std::to_string(t1x) + ",  " + "t1y =" + std::to_string(t1y) + ",  " + "t1 =" + std::to_string(t1));
		getContext()->getLogger()->log("t2x =" + std::to_string(t2x) + ",  " + "t2y =" + std::to_string(t2y) + ",  " + "t2 =" + std::to_string(t2));
		getContext()->getLogger()->log("x_new_paddle1 = " + Vec2Str(x_new_paddle1) + ", " + "x_new_paddle2 = " + Vec2Str(x_new_paddle2));
		getContext()->getLogger()->log("x_new_puck1 = " + Vec2Str(x_new_puck1) + ", " + "x_new_puck2 = " + Vec2Str(x_new_puck2));
		getContext()->getLogger()->log("-----------------------------");

		if (!m_field->getPlayRect(airhockey::GoalGateLocationType::UPPER).containsPoint(x_new_paddle))
		{
			getContext()->getLogger()->log("AIAttackState::onEnter(): attack failed, predicted paddle coords outside of AI play rect");
			return false;
		}

		m_aiPaddle->getStick()->runAction(MoveTo::create(t, x_new_paddle));
		return true;
	}

	void AIAttackState::onExit()
	{
		getContext()->getLogger()->log("AIAttackState::onExit()");

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
			getContext()->getLogger()->log("AIAttackState::handleTransitions(): back to defense state");
			m_context->popState();
		}
	}

	void AIAttackState::pause()
	{
		m_aiPaddle->getStick()->getPhysicsBody()->setVelocity(cocos2d::Vec2::ZERO);
		m_aiPaddle->getStick()->pause();
		m_aiPaddle->getSprite()->pause();
	}

	void AIAttackState::resume()
	{
		m_aiPaddle->getStick()->resume();
		m_aiPaddle->getSprite()->resume();
	}

}