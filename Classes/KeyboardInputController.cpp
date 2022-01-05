#include "KeyboardInputController.h"

namespace airhockey
{
    USING_NS_CC;

    void KeyboardInputController::bindInputListeners()
    {
        m_listener = EventListenerKeyboard::create();
        m_listener->onKeyPressed = CC_CALLBACK_2(KeyboardInputController::onKeyPressed, this);
        m_listener->onKeyReleased = CC_CALLBACK_2(KeyboardInputController::onKeyReleased, this);

        Director::getInstance()->getEventDispatcher()->addEventListenerWithFixedPriority(m_listener, 1);
    }

    KeyboardInputController::~KeyboardInputController()
    {
        Director::getInstance()->getEventDispatcher()->removeEventListener(m_listener);
    }

    void KeyboardInputController::onKeyPressed(EventKeyboard::KeyCode keyCode, Event* event)
    {
        if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
        {
            m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_MOVE_LEFT));
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
        {
            m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_MOVE_RIGHT));
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW)
        {
            m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_MOVE_UP));
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW)
        {
            m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_MOVE_DOWN));
        }
    }

    void KeyboardInputController::onKeyReleased(EventKeyboard::KeyCode keyCode, Event* event)
    {
        if (keyCode == EventKeyboard::KeyCode::KEY_LEFT_ARROW)
        {
            m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_LEFT));
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_RIGHT_ARROW)
        {
            m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_RIGHT));
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_UP_ARROW)
        {
            m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_UP));
        }
        else if (keyCode == EventKeyboard::KeyCode::KEY_DOWN_ARROW)
        {
            m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_DOWN));
        }
    }

}