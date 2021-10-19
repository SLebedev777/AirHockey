#include "FSMContext.h"

namespace airhockey
{
	FSMContext::FSMContext(IFSMStatePtr start_state)
	{
		m_states.push(std::move(start_state));
		m_states.top()->setContext(this);
		onEnterCurrentState();
	}

	FSMContext::~FSMContext()
	{}

	void FSMContext::pushState(IFSMStatePtr state)
	{
		if (!m_isEnabled)
			return;

		if (!m_states.empty())
		{
			m_states.top()->onExit();
		}
		m_states.push(std::move(state));
		m_states.top()->setContext(this);
		onEnterCurrentState();
	}

	void FSMContext::replaceState(IFSMStatePtr state)
	{
		if (!m_isEnabled)
			return;

		if (!m_states.empty())
		{
			m_states.top()->onExit();
			m_states.pop();
		}
		m_states.push(std::move(state));
		m_states.top()->setContext(this);
		onEnterCurrentState();
	}

	IFSMStatePtr FSMContext::popState()
	{
		if (!m_isEnabled)
			return nullptr;

		if (m_states.empty())
			return nullptr;

		m_states.top()->onExit();
		auto top = std::move(m_states.top());
		m_states.pop();
		top->setContext(nullptr);
		if (!m_states.empty())
		{
			onEnterCurrentState();
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
		onEnterCurrentState();
	}

	void FSMContext::update()
	{
		if (m_states.empty())
			return;

		if (!m_isEnabled)
			return;

		m_states.top()->handleTransitions();
		m_states.top()->update();
	}

	void FSMContext::onEnterCurrentState()
	{
		if (m_states.empty())
			return;
		
		if (!m_states.top()->onEnter())
		{
			popState();
		}
	}

	void FSMContext::setEnabled(bool is_enabled)
	{
		if (m_states.empty())
			return;
		
		if (!is_enabled && m_isEnabled)
		{
			m_isEnabled = false;
			m_states.top()->pause();
		}
		else if (is_enabled && !m_isEnabled)
		{
			m_isEnabled = true;
			m_states.top()->resume();
		}
	}
}
