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
	namespace
	{
		using namespace cocos2d;

		bool movingPointIntersectHorizontalLine(const Vec2& x0, const Vec2& vel, float y_line, /*OUT*/ float& x_line_out,
			float dist_threshold=0.0f, float vel_threshold=0.0f)
		{
			assert(vel_threshold >= 0.0f && dist_threshold >= 0.0f);

			if (vel.lengthSquared() <= vel_threshold * vel_threshold)
			{
				return false;
			}

			if ((x0.y < y_line && vel.y <= vel_threshold) ||
				(x0.y > y_line && vel.y >= -vel_threshold)
				)
			{
				return false;
			}

			if (abs(y_line - x0.y) <= dist_threshold)
			{
				x_line_out = x0.x;
				return true;
			}

			x_line_out = x0.x + abs(y_line - x0.y) * vel.x / vel.y;
			return true;
				
		}

		bool movingPointIntersectHorizontalSegment(const Vec2& x0, const Vec2& vel, float y_line, float x1_seg, float x2_seg,
			float dist_threshold = 0.0f, float vel_threshold = 0.0f)
		{
			assert(x1_seg != x2_seg);

			float x_line_out = 0.0f;
			bool res = movingPointIntersectHorizontalLine(x0, vel, y_line, x_line_out, dist_threshold, vel_threshold);
			if (!res)
				return false;

			if (x1_seg > x2_seg)
			{
				std::swap(x1_seg, x2_seg);
			}

			return x1_seg <= x_line_out && x_line_out <= x2_seg;
		}

	}

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

	bool AIDefenseState::defense()
	{
		getContext()->getLogger()->log("AIDefenseState::defense(): enter");

		Vec2 x0_puck = m_puck->getPosition();
		Vec2 x0_paddle = m_aiPaddle->getSprite()->getPosition();
		Vec2 v_puck = m_puck->getPhysicsBody()->getVelocity();
		float puck_radius = m_puck->getBoundingBox().size.width / 2;
		float paddle_radius = m_aiPaddle->getRadius();

		auto ai_move_to_pyramid_action = [this](float defense_time) {
			cocos2d::Rect gate_rect = m_field->getGoalGate(airhockey::GoalGateLocationType::UPPER).getRect();
			float puck_x_offset = m_puck->getPosition().x - m_field->getCenter().x;
			float puck_width = m_puck->getBoundingBox().size.width;
			float paddle_x_offset = 0.0f;
			float puck_x_offset_thres = puck_width * 2;
			cocos2d::Vec2* defense_point = &m_pyramid.pyramidBase;
			if (puck_x_offset < -puck_x_offset_thres)
			{
				defense_point = &m_pyramid.pyramidLeft;
			}
			else if (puck_x_offset > puck_x_offset_thres)
			{
				defense_point = &m_pyramid.pyramidRight;
			}
			auto move_to_pyramid = cocos2d::MoveTo::create(defense_time, *defense_point);
			return move_to_pyramid;
		};

		auto ai_move_along_pyramid_base_action = [this, x0_paddle](float defense_time) {
			cocos2d::Rect gate_rect = m_field->getGoalGate(airhockey::GoalGateLocationType::UPPER).getRect();
			float puck_x_offset = m_puck->getPosition().x - m_field->getPlayRect().getMinX();
			float frac = puck_x_offset / m_field->getPlayRect().size.width;
			float pyramid_width = m_pyramid.pyramidRight.x - m_pyramid.pyramidLeft.x;
			Vec2 defense_point(m_pyramid.pyramidLeft.x + frac * pyramid_width, m_pyramid.pyramidBase.y);

			float time_frac = defense_point.distance(x0_paddle) / pyramid_width;
			time_frac = time_frac > 1.0f ? 1.0f : time_frac;
			time_frac = time_frac < 0.0f ? 0.0f : time_frac;
			auto move_to_pyramid = cocos2d::MoveTo::create(defense_time * time_frac, defense_point);
			return move_to_pyramid;
		};


		cocos2d::Action* defense_action = nullptr;

		/*
		choice of defense strategy
		*/
		cocos2d::Rect gate_rect = m_field->getGoalGate(airhockey::GoalGateLocationType::UPPER).getRect();
		float dist_threshold = puck_radius - 1;
		float vel_threshold = 5;
		float defense_time = 0.2f;
		Vec2 x_new_puck = x0_puck + defense_time * v_puck;
		float max_y = m_field->getPlayRect().getMaxY() - paddle_radius;

		if (x0_puck.y < m_field->getCenter().y)
		{
			getContext()->getLogger()->log("AIDefenseState::defense(): puck is outside AI rect. Move paddle to pyramid top");

			defense_action = cocos2d::MoveTo::create(defense_time, m_pyramid.pyramidTop);
			defense_action->setTag(m_defenseActionTag);
			m_aiPaddle->getStick()->runAction(defense_action);
			return true;
		}

		bool is_puck_hits_the_goal = movingPointIntersectHorizontalSegment(x0_puck, v_puck, gate_rect.getMinY(), gate_rect.getMinX(), gate_rect.getMaxX(),
			dist_threshold, vel_threshold);

		if (x0_puck.y < x0_paddle.y)
		{
			getContext()->getLogger()->log("AIDefenseState::defense(): puck.y < paddle.y - puck lower than paddle");

			if (v_puck.y < 0.0f)
			{
				getContext()->getLogger()->log("AIDefenseState::defense(): puck goes away from the AI gate. Move paddle to pyramid");
				defense_action = ai_move_to_pyramid_action(defense_time);

			}
			else
			{
				if (is_puck_hits_the_goal)
				{
					getContext()->getLogger()->log("AIDefenseState::defense(): puck will hit the gate!");

					float target_y = x_new_puck.y + puck_radius + paddle_radius;
					target_y = target_y > max_y ? max_y : target_y;
					float target_x;
					if (movingPointIntersectHorizontalLine(x0_puck, v_puck, target_y, target_x, dist_threshold, vel_threshold))
					{
						getContext()->getLogger()->log("AIDefenseState::defense(): close the puck's way to the gate");
						defense_action = MoveTo::create(defense_time, Vec2(target_x, target_y));
					}
					else
					{
						getContext()->getLogger()->log("AIDefenseState::defense(): failed to close the puck's way to the gate. Move to pyramid base");
						defense_action = ai_move_along_pyramid_base_action(defense_time);
					}
				}
				else
				{
					getContext()->getLogger()->log("AIDefenseState::defense(): puck goes towards the gate, but won't hit it. Move paddle to pyramid");

					defense_action = ai_move_along_pyramid_base_action(defense_time);
				}
			}
		}
		else
		{
			getContext()->getLogger()->log("AIDefenseState::defense(): puck.y >= paddle.y - puck between paddle and gate!");

			if (v_puck.y < 0.0f)
			{
				getContext()->getLogger()->log("AIDefenseState::defense(): puck goes away from the AI gate. Move paddle up close to the gate");

				float target_y = x_new_puck.y + puck_radius;
				target_y = target_y > max_y ? max_y : target_y;
				defense_action = MoveTo::create(defense_time, Vec2(x0_paddle.x, target_y));
			}
			else
			{
				if (is_puck_hits_the_goal)
				{
					getContext()->getLogger()->log("AIDefenseState::defense(): puck will hit the gate!");

					float target_y = x_new_puck.y + puck_radius + paddle_radius;
					target_y = target_y > max_y ? max_y : target_y;
					float target_x;
					if (movingPointIntersectHorizontalLine(x0_puck, v_puck, target_y, target_x, dist_threshold, vel_threshold))
					{
						getContext()->getLogger()->log("AIDefenseState::defense(): close the puck's way to the gate");
						defense_action = MoveTo::create(defense_time, Vec2(target_x, target_y));
					}
					else
					{
						getContext()->getLogger()->log("AIDefenseState::defense(): failed to close the puck's way to the gate. Move paddle to gate center");
						defense_action = MoveTo::create(defense_time, Vec2(gate_rect.getMidX(), max_y));
					}
				}
				else if (v_puck.length() > 20)
				{
					getContext()->getLogger()->log("AIDefenseState::defense(): puck goes towards the gate, but won't hit it. Move paddle to gate center");
					defense_action = MoveTo::create(defense_time, Vec2(gate_rect.getMidX(), max_y));
				}
				else
				{
					getContext()->getLogger()->log("AIDefenseState::defense(): SLOW puck goes towards the gate, but won't hit it. Move paddle to the puck");
					defense_action = MoveTo::create(defense_time, x_new_puck);
				}
			}
		}

		defense_action->setTag(m_defenseActionTag);

		m_aiPaddle->getStick()->runAction(defense_action);
		return true;
	}

	bool AIDefenseState::onEnter()
	{
		getContext()->getLogger()->log("AIDefenseState::onEnter(): enter");

		return defense();
	}

	void AIDefenseState::onExit()
	{
		getContext()->getLogger()->log("AIDefenseState::onExit()");

		m_aiPaddle->getStick()->stopAllActions();
	}


	void AIDefenseState::handleTransitions()
	{
		Vec2 x_paddle = m_aiPaddle->getSprite()->getPosition();
		Vec2 x_puck = m_puck->getPosition();
		Vec2 v_puck = m_puck->getPhysicsBody()->getVelocity();

		if (x_puck.distance(x_paddle) <= m_attackRadiusFunc(m_puck->getPhysicsBody()->getVelocity()) &&
			x_puck.y < x_paddle.y &&
			x_puck.y >= m_field->getCenter().y
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

		defense();		
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