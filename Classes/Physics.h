#pragma once

#include "cocos2d.h"

namespace airhockey
{
	namespace Physics
	{
		enum CollisionCategoryBitMask
		{
			CCBM_NONE = 0x00,       // 0000
			CCBM_GAME_FIELD = 0x01, // 0001
			CCBM_PADDLE1 = 0x02,    // 0010
			CCBM_PADDLE2 = 0x04,    // 0100
			CCBM_PUCK = 0x08        // 1000
		};

		extern const int contactTestBitmask_None;
		extern const int contactTestBitmask_All;
		extern const int contactTestBitmask_GameField;
		extern const int contactTestBitmask_Paddle1;
		extern const int contactTestBitmask_Paddle2;
		extern const int contactTestBitmask_Puck;

	}
}