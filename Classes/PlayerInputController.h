#ifndef __PLAYERINPUTCONTROLLER_H__
#define __PLAYERINPUTCONTROLLER_H__


// interface that maps device- (or agent-) specific inputs to virtual input events ("actions") that are understandable by game objects.
// For example, Paddle can be controlled by human player from keyboard or mouse on PC, or by touchscreen on mobile, or even by
// AI agent.

#include <string>
#include <memory>
#include "cocos2d.h"
#include "Paddle.h"

namespace airhockey
{
	namespace InputActionEvent
	{
		const std::string INPUT_ACTION_EVENT_MOVE_UP = "IAE_MOVE_UP";
		const std::string INPUT_ACTION_EVENT_MOVE_DOWN = "IAE_MOVE_DOWN";
		const std::string INPUT_ACTION_EVENT_MOVE_LEFT = "IAE_MOVE_LEFT";
		const std::string INPUT_ACTION_EVENT_MOVE_RIGHT = "IAE_MOVE_RIGHT";
		const std::string INPUT_ACTION_EVENT_STOP_MOVE_UP = "IAE_STOP_MOVE_UP";
		const std::string INPUT_ACTION_EVENT_STOP_MOVE_DOWN = "IAE_STOP_MOVE_DOWN";
		const std::string INPUT_ACTION_EVENT_STOP_MOVE_LEFT = "IAE_STOP_MOVE_LEFT";
		const std::string INPUT_ACTION_EVENT_STOP_MOVE_RIGHT = "IAE_STOP_MOVE_RIGHT";
	}

	class IPlayerInputController
	{
	public:
		IPlayerInputController(const std::string& name, PaddlePtr paddle);
		IPlayerInputController(IPlayerInputController& other);

		virtual void bindInputListeners() = 0;  // bind device- or agent-specific events to virtual events
		virtual ~IPlayerInputController() {}
		std::string makeCustomEventName(const std::string& input_action_event_name);
		// bind virtual input event to owner's callback
		void bindActionEventListener(const std::string& input_action_event_name, const std::function<void(cocos2d::EventCustom*)>& callback);

		virtual void scheduleDebugOutput(cocos2d::Node* layer) {};

	protected:
		cocos2d::EventDispatcher* m_eventDispatcher = nullptr;
		std::string m_name;
		PaddlePtr m_myPaddle = nullptr;
	};

	typedef std::shared_ptr<IPlayerInputController> IPlayerInputControllerPtr;

}

#endif // __PLAYERINPUTCONTROLLER_H__