#ifndef __AIRHOCKEY_FSMCONTEXT_H__
#define __AIRHOCKEY_FSMCONTEXT_H__

#include "IFSMState.h"
#include "DebugLogger.h"
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

		virtual void update();
		void setEnabled(bool is_enabled = true);

		DebugLogger* getLogger() const { return m_logger.get(); }
		void setLogger(DebugLoggerPtr logger) { m_logger = logger; }

	protected:
		std::stack<IFSMStatePtr> m_states;
		bool m_isEnabled = true;
		DebugLoggerPtr m_logger = nullptr;
	};

	typedef std::shared_ptr<FSMContext> FSMContextPtr;

}

#endif // __AIRHOCKEY_FSMCONTEXT_H__