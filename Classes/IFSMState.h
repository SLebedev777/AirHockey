#ifndef __AIRHOCKEY_IFSMSTATE_H__
#define __AIRHOCKEY_IFSMSTATE_H__

#include <memory>

namespace airhockey
{
	// fwd
	class FSMContext;
	typedef std::shared_ptr<FSMContext> FSMContextPtr;

	/* Finite State Machine state interface */

	class IFSMState
	{
	public:
		IFSMState() = default;
		IFSMState(const IFSMState& other) = default;
		virtual ~IFSMState() {}

		virtual void onEnter() {}
		virtual void onExit() {}
		virtual void update() {}

		FSMContext* getContext() const { return m_context; }
		void setContext(FSMContext* context) { m_context = context; }

	protected:
		FSMContext* m_context = nullptr;
	};

	typedef std::unique_ptr<IFSMState> IFSMStatePtr;
}

#endif // __AIRHOCKEY_IFSMSTATE_H