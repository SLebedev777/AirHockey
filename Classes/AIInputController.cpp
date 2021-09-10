#include "AIInputController.h"

namespace airhockey
{
    USING_NS_CC;

    void AIInputController::bindInputListeners()
    {
        m_myPaddle->getSprite()->schedule([this](float dt) {onMyPaddleMove(); }, "AI_onMyPaddleMove");
    }

    void AIInputController::onMyPaddleMove()
    {
        m_eventDispatcher->dispatchCustomEvent(makeCustomEventName(InputActionEvent::INPUT_ACTION_EVENT_MOVE_LEFT));
    }

}