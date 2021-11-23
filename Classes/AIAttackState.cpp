#include "AIAttackState.h"
#include "AIDefenseState.h"
#include "FSMContext.h"
#include "CCHelpers.h"
#include <math.h>
#include <tuple>
#include <cassert>

namespace airhockey
{
	namespace
	{
		using namespace cocos2d;

		/*
		Calculates vector starting at origin (0, 0) with given length and angle alpha.
		alpha grows counterclockwise from 0 at y axis.
		*/
		Vec2 azimuthVector(float alpha, float length=1.0f)
		{
			return Vec2(length * sin(alpha), length * cos(alpha));
		}

		/*
		Calculates vector starting at origin (0, 0) with given length along given direction.
		Angle of direction grows counterclockwise from 0 at y axis.
		*/
		Vec2 directionVector(const Vec2& direction, float length)
		{
			float c = direction.length();
			float sin_a = direction.x / c;
			float cos_a = direction.y / c;
			return Vec2(length * sin_a, length * cos_a);
		}

		/*
		Calculates time when paddle and puck meet each other at the same point.
		Input parameters:
			dx0 = x0_paddle - x0_puck
			dv = v_paddle - v_puck
		Returns:
			std::tuple<float, float, float>(tx, ty, t), where
			tx - time calculated using x axis
			ty - time calculated using y axis
			t - final calculated time
		*/
		std::tuple<float, float, float> calcEncounterTime(const Vec2& dx0, const Vec2& dv)
		{
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

		/*
		Make correction of predicted paddle attack position in such a way that after paddle hits the puck,
		the puck will change its direction right towards the target point.
		Input parameters:
			encounter - predicted point where paddle and puck will meet each other
			target - target point (for ex., center of enemy's gate)
			x0_paddle - AI paddle position before making an attack motion
			puck_radius
			paddle_radius
			dx_threshold - threshold to consider no correction is needed
			logger
		Returns:
			Vec2 - corrected paddle attack position
		*/
		Vec2 correctXnewPaddle(const Vec2& encounter, const Vec2& target, const Vec2& x0_paddle, 
			float puck_radius, float paddle_radius, float dx_threshold, DebugLogger* logger)
		{
			logger->log("Correcting X_new paddle to hit target " + CCHelpers::Vec2Str(target));

			Vec2 d = encounter - target;
			float R = puck_radius + paddle_radius;
			Vec2 corr;
			if (abs(d.x) < dx_threshold)
			{
				logger->log("dx < dx_threshold: " + std::to_string(d.x) + " < " + std::to_string(dx_threshold));
				return encounter;
			}
			else
			{
				corr = encounter + directionVector(d, R);
				Vec2 paddle_direction = corr - x0_paddle;
				Vec2 corr2 = corr + directionVector(paddle_direction, R);
				return corr2;
			}
		}

		/*
		Given 2 1d intervals (margins) [a1;a2] and [b1;b2], do they overlap?
		*/
		bool isMarginsOverlap(float a1, float a2, float b1, float b2)
		{
			assert(a1 < a2);
			assert(b1 < b2);

			if (b1 > a2 && b2 > a2)
				return false;
			if (b1 < a1 && b2 < a1)
				return false;

			return true;
		}

		/*
		Calculate if there is the clear way from starting point puck_pos to target point if enemy paddle can be on the way?
		Input parameters:
			target - target point where we want the puck to be
			puck_pos - starting puck pos before attacking kick
			puck_radius
			player_paddle_pos - position of other player (human) paddle that can close the way to the target
			player_paddle_radius
			dx_threshold

		*/
		bool isTargetClear(const Vec2& target, const Vec2& puck_pos, float puck_radius,
			const Vec2& player_paddle_pos, float player_paddle_radius, float dx_threshold)
		{
			Vec2 d = puck_pos - target;
			float dx2 = 0.0f;
			if (abs(d.x) >= dx_threshold)
			{
				float tan = d.y / d.x;
				Vec2 d2 = player_paddle_pos - target;
				dx2 = tan * d2.y;
			}
			Vec2 puck_margin_center(target.x + dx2, player_paddle_pos.y);
			Vec2 puck_margin(puck_margin_center.x - puck_radius, puck_margin_center.x + puck_radius);
			Vec2 player_paddle_margin(player_paddle_pos.x - player_paddle_radius, player_paddle_pos.x + player_paddle_radius);
			bool result = isMarginsOverlap(player_paddle_margin.x, player_paddle_margin.y, puck_margin.x, puck_margin.y);

			return result;
		}
	}

	AIAttackState::AIAttackState(GameField* game_field, PaddlePtr ai_paddle, PaddlePtr player_paddle, cocos2d::Sprite* puck, float attack_radius) :
		IFSMState(),
		m_field(game_field),
		m_aiPaddle(ai_paddle),
		m_playerPaddle(player_paddle),
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
			move_push_puck->setTag(m_attackActionTag);
			return move_push_puck;
		};
		
		Vec2 x0_puck = m_puck->getPosition();
		if (!m_field->getPlayRect(airhockey::GoalGateLocationType::UPPER).containsPoint(x0_puck))
		{
			getContext()->getLogger()->log("AIAttackState::onEnter(): attack failed, puck is outside AI play rect");
			return false;
		}

		Vec2 x0_paddle = m_aiPaddle->getSprite()->getPosition();
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
		Vec2 v_paddle1 = azimuthVector(alpha1, v_paddle_scalar);
		Vec2 v_paddle2 = azimuthVector(alpha2, v_paddle_scalar);
		
		float t1x, t1y, t1;
		float t2x, t2y, t2;
		std::tie(t1x, t1y, t1) = calcEncounterTime(dx0, v_paddle1 - v_puck);
		std::tie(t2x, t2y, t2) = calcEncounterTime(dx0, v_paddle2 - v_puck);
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

		float puck_radius = 50;  // !!!
		float paddle_radius = m_aiPaddle->getRadius();

		cocos2d::Rect lower_gate_rect = m_field->getGoalGate(airhockey::GoalGateLocationType::LOWER).getRect();

		Vec2 target_center(lower_gate_rect.getMidX(), lower_gate_rect.getMaxY());
		Vec2 target_left_corner(lower_gate_rect.getMinX() + puck_radius, lower_gate_rect.getMaxY());
		Vec2 target_right_corner(lower_gate_rect.getMaxX() - puck_radius, lower_gate_rect.getMaxY());

		Vec2 x_new_paddle_correct = correctXnewPaddle(x_new_paddle, target_center, x0_paddle,
			puck_radius, m_aiPaddle->getRadius(), /*dx_threshold*/puck_radius/2,
			getContext()->getLogger());

		if (!m_field->getPlayRect(airhockey::GoalGateLocationType::UPPER).containsPoint(x_new_paddle_correct))
		{
			getContext()->getLogger()->log("AIAttackState::onEnter(): attack failed, predicted correct paddle coords outside of AI play rect");
			return false;
		}

		/*
		choice of attack strategy
			strategy 1 - move horizontally along puck's vector and then make straight attack to the gate
			strategy 2 - straight attack to the gate
		*/

		cocos2d::Action* attack_action = nullptr;
		
		auto attack_action_strategy1 = [=](const Vec2& target, float prepare_y_offset) {
			const float KICK_TIME_FRAC = 0.25f;
			Vec2 prepare_dir_vector = directionVector(x_new_puck - target, prepare_y_offset);
			Vec2 x_paddle_prepare_to_kick = x_new_puck + prepare_dir_vector;
			Vec2 x_paddle_kick = x_new_puck - 0.5*prepare_dir_vector;
			auto move_to_prepare_point = MoveTo::create(t * (1.0f - KICK_TIME_FRAC), x_paddle_prepare_to_kick);
			auto kick = MoveTo::create(t * KICK_TIME_FRAC, x_paddle_kick);
			auto seq = Sequence::create(move_to_prepare_point, kick, nullptr);
			return seq;
		};

		auto attack_action_strategy2 = MoveTo::create(t, x_new_paddle_correct);

		const float V_PUCK_THRESHOLD_SQR = pow(puck_radius * 30, 2);
		const float CORRIDOR_WIDTH = 0.66f * m_field->getPlayRect().size.width;
		const float MIN_CORRIDOR_X = m_field->getCenter().x - CORRIDOR_WIDTH / 2;
		const float MAX_CORRIDOR_X = m_field->getCenter().x + CORRIDOR_WIDTH / 2;
		const float PADDLE_PREDICTED_PATH_LENGTH_X_THRESHOLD = m_field->getPlayRect().size.width / 2;
		const float PREPARE_Y_OFFSET = paddle_radius * 4 + puck_radius;
		if (v_puck.lengthSquared() < V_PUCK_THRESHOLD_SQR &&
			x_new_puck.x > MIN_CORRIDOR_X && x_new_puck.x < MAX_CORRIDOR_X  &&
			(x_new_puck - x0_paddle).x <= PADDLE_PREDICTED_PATH_LENGTH_X_THRESHOLD &&
			x_new_puck.y + PREPARE_Y_OFFSET + paddle_radius < m_field->getPlayRect().getMaxY() )
		{
			getContext()->getLogger()->log("AIAttackState::onEnter(): chosen Strategy 1 (move along puck then straight attack)");
			
			auto player_paddle_pos = m_playerPaddle->getSprite()->getPosition();
			float player_paddle_radius = m_playerPaddle->getRadius();
			Vec2 final_target;
			if (isTargetClear(target_center, x_new_puck, puck_radius, player_paddle_pos, player_paddle_radius, 0.5f*puck_radius))
			{
				final_target = target_center;
			}
			else if (isTargetClear(target_left_corner, x_new_puck, puck_radius, player_paddle_pos, player_paddle_radius, 0.5f*puck_radius))
			{
				final_target = target_left_corner;
			}
			else if (isTargetClear(target_right_corner, x_new_puck, puck_radius, player_paddle_pos, player_paddle_radius, 0.5f*puck_radius))
			{
				final_target = target_right_corner;
			}
			else
			{
				return false;
			}
			attack_action = attack_action_strategy1(final_target, PREPARE_Y_OFFSET);
		}
		else
		{
			getContext()->getLogger()->log("AIAttackState::onEnter(): chosen Strategy 2 (straight attack)");
			attack_action = attack_action_strategy2;
		}

		if (!attack_action)
			return false;

		attack_action->setTag(m_attackActionTag);
		m_aiPaddle->getStick()->runAction(attack_action);
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
			m_aiPaddle->getPosition().y <= m_field->getCenter().y ||
			!m_aiPaddle->getStick()->getActionByTag(m_attackActionTag))
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