#include "Paddle.h"

USING_NS_CC;

Paddle::Paddle(const std::string& image_name, float x, float y, float vel_x, float vel_y, float radius):
	m_ccSprite(cocos2d::Sprite::create(image_name)),
	m_centerX(x),
	m_centerY(y),
	m_velX(vel_x),
	m_velY(vel_y),
	m_radius(radius)
{
	m_ccSprite->setPosition(Vec2(m_centerX, m_centerY));
}

Paddle::~Paddle()
{}

void Paddle::attachInputController(IPlayerInputControllerPtr controller)
{
	if (m_pController)
		throw std::runtime_error("Controller already attached. Can't attach more than 1 controller");

	m_pController = controller;

	m_pController->bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_MOVE_LEFT, [=](EventCustom* event) { onMovingLeft();  });
	m_pController->bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_MOVE_RIGHT, [=](EventCustom* event) { onMovingRight();  });
	m_pController->bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_MOVE_UP, [=](EventCustom* event) { onMovingUp();  });
	m_pController->bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_MOVE_DOWN, [=](EventCustom* event) { onMovingDown();  });
	m_pController->bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_LEFT, [=](EventCustom* event) { onStopMovingLeft();  });
	m_pController->bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_RIGHT, [=](EventCustom* event) { onStopMovingRight();  });
	m_pController->bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_UP, [=](EventCustom* event) { onStopMovingUp();  });
	m_pController->bindActionEventListener(InputActionEvent::INPUT_ACTION_EVENT_STOP_MOVE_DOWN, [=](EventCustom* event) { onStopMovingDown();  });

}

void Paddle::onMovingLeft()
{
	if (m_movingRight >= 0) { m_movingRight = -1; m_velXnorm = -1; }
}

void Paddle::onMovingRight()
{
	if (m_movingRight <= 0) { m_movingRight = 1; m_velXnorm = 1; }
}

void Paddle::onMovingUp()
{
	if (m_movingUp <= 0) { m_movingUp = 1;  m_velYnorm = 1; }
}

void Paddle::onMovingDown()
{
	if (m_movingUp >= 0) { m_movingUp = -1; m_velYnorm = -1; }
}

void Paddle::onStopMovingLeft()
{
	if (m_movingRight < 0) { m_movingRight = 0; m_velXnorm = 0; }
}
void Paddle::onStopMovingRight()
{
	if (m_movingRight > 0) { m_movingRight = 0; m_velXnorm = 0; }
}

void Paddle::onStopMovingUp()
{
	if (m_movingUp > 0) { m_movingUp = 0;  m_velYnorm = 0; }
}

void Paddle::onStopMovingDown()
{
	if (m_movingUp < 0) { m_movingUp = 0; m_velYnorm = 0;  }
}

bool Paddle::move(float dt)
{
	m_centerX += dt * m_velX * m_velXnorm;
	m_centerY += dt * m_velY * m_velYnorm;

	m_ccSprite->setPosition(Vec2(m_centerX, m_centerY));

	return true;
}


