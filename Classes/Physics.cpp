#include "Physics.h"

namespace airhockey
{
	namespace Physics
	{
		USING_NS_CC;

		const int contactTestBitmask_None = CCBM_NONE;
		const int contactTestBitmask_All = CCBM_GAME_FIELD | CCBM_PADDLE1 | CCBM_PADDLE2 | CCBM_PUCK;
		const int contactTestBitmask_GameField = contactTestBitmask_All & (~CCBM_GAME_FIELD);
		const int contactTestBitmask_Paddle1 = CCBM_PUCK;
		const int contactTestBitmask_Paddle2 = CCBM_PUCK;
		const int contactTestBitmask_Puck = contactTestBitmask_All & (~CCBM_PUCK);

	}
}