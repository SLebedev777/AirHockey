#ifndef __AIRHOCKEY_AINULLSTATE_H__
#define __AIRHOCKEY_AINULLSTATE_H__

#include "IFSMState.h"

namespace airhockey
{
	// fwd
	class FSMContext;
	typedef std::shared_ptr<FSMContext> FSMContextPtr;

	/* AI Idle state */

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