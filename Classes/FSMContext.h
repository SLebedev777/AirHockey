#ifndef __AIRHOCKEY_FSMCONTEXT_H__
#define __AIRHOCKEY_FSMCONTEXT_H__

#include "IFSMState.h"
#include <stack>

namespace airhockey
{
	/* Finite State Machine context */

	class FSMContext
	{
	public:
		FSMContext() = default;
		FSMContext(const FSMContext& other) = delete;
		FSMContext(IFSMStatePtr start_state);
		~FSMContext();

		void pushState(IFSMStatePtr state);
		IFSMStatePtr popState();
		void replaceState(IFSMStatePtr state);
		IFSMState* getCurrentState() const { return !m_states.empty() ? m_states.top().get() : nullptr; };
		void reset();  // erase all states in stack except the base.
		void onEnterCurrentState();

		void update();

	private:
		std::stack<IFSMStatePtr> m_states;
	};

}

#endif // __AIRHOCKEY_FSMCONTEXT_H__