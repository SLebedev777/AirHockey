#include "FSMContext.h"

namespace airhockey
{
	FSMContext::FSMContext(IFSMStatePtr start_state)
	{
		m_states.push(std::move(start_state));
		m_states.top()->setContext(this);
		m_states.top()->onEnter();
	}

	FSMContext::~FSMContext()
	{}

	void FSMContext::pushState(IFSMStatePtr state)
	{
		if (!m_states.empty())
		{
			m_states.top()->onExit();
		}
		m_states.push(std::move(state));
		m_states.top()->setContext(this);
		m_states.top()->onEnter();
	}

	IFSMStatePtr FSMContext::popState()
	{
		if (m_states.empty())
			return nullptr;

		m_states.top()->onExit();
		auto top = std::move(m_states.top());
		m_states.pop();
		top->setContext(nullptr);
		if (!m_states.empty())
		{
			m_states.top()->onEnter();
		}

		return top;
	}

	void FSMContext::reset()
	{
		if (m_states.empty())
			return;

		while (m_states.size() > 1)
		{
			m_states.pop();
		}
		m_states.top()->onEnter();
	}

	void FSMContext::update()
	{
		if (m_states.empty())
			return;

		m_states.top()->handleTransitions();
		m_states.top()->update();
	}
}