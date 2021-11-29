#ifndef __AIRHOCKEY_AINULLSTATE_H__
#define __AIRHOCKEY_AINULLSTATE_H__

#include "IFSMState.h"
#include "AirHockey_fwd.h"

namespace airhockey
{
	/* AI Null state */

	class AINullState: public IFSMState
	{
	public:
		AINullState() = default;
		~AINullState() {}

		bool onEnter() override { return true; }
		void onExit() override {}
		void handleTransitions() override {}
		void pause() override {}
		void resume() override {}
	};

}

#endif // __AIRHOCKEY_AINULLSTATE_H