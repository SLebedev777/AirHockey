#ifndef __AIRHOCKEY_AIPLAYER_H__
#define __AIRHOCKEY_AIPLAYER_H__

#include "cocos2d.h"
#include "AirHockey_fwd.h"
#include "FSMContext.h"


namespace airhockey
{
	using namespace cocos2d;

	struct Pyramid
	{
		explicit Pyramid(const Vec2& p_top, const Vec2& p_left, const Vec2& p_right):
			pyramidTop(p_top),
			pyramidLeft(p_left),
			pyramidRight(p_right)
		{}

		Vec2 pyramidTop, pyramidLeft, pyramidRight;
	};

	struct AIPlayerSettings
	{
		explicit AIPlayerSettings(const Pyramid& pyramid, float attack_radius) :
			pyramid(pyramid),
			attackRadius(attack_radius)
		{}
		Pyramid pyramid;
		float attackRadius;
	};

	class AIPlayer : public FSMContext
	{
	public:
		AIPlayer() = delete;
		AIPlayer(const AIPlayer& other) = delete;
		AIPlayer& operator=(const AIPlayer& other) = delete;
		AIPlayer(GameField* game_field, PaddlePtr ai_paddle, PaddlePtr enemy_paddle, cocos2d::Sprite* puck, const AIPlayerSettings& settings);

		IFSMStatePtr createIdleState();
		IFSMStatePtr createDefenseState();
		IFSMStatePtr createAttackState();

	private:
		GameField* m_field = nullptr;
		PaddlePtr m_aiPaddle = nullptr;
		PaddlePtr m_enemyPaddle = nullptr;
		cocos2d::Sprite* m_puck = nullptr;
		AIPlayerSettings m_settings;

	};

	typedef std::shared_ptr<AIPlayer> AIPlayerPtr;
}

#endif // __AIRHOCKEY_AIPLAYER_H__