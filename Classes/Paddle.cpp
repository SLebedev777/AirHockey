#include "Paddle.h"

USING_NS_CC;

Paddle::Paddle(const std::string& image_name, float x, float y, float vel_x, float vel_y, float radius, Rect& field_rect, Node* parent):
	m_ccsSprite(cocos2d::Sprite::create(image_name)),
	m_centerX(x),
	m_centerY(y),
	m_velX(vel_x),
	m_velY(vel_y),
	m_radius(radius),
	m_fieldRect(field_rect),
	m_ccnParent(parent)
{
	if (!m_ccnParent)
	{
		throw std::runtime_error("Parent node for paddle is null");
	}

	m_ccnParent->addChild(m_ccsSprite, 1);

	setPosition(Vec2(m_centerX, m_centerY));
}

Paddle::~Paddle()
{}

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

	setPosition(Vec2(m_centerX, m_centerY));

	return true;
}

Vec2 Paddle::boundToFieldRect(Vec2 pos)
{
	float cx = pos.x;
	float cy = pos.y;
	auto box = Rect(pos.x - m_radius, pos.y - m_radius, m_radius*2, m_radius * 2);

	if (box.getMinX() < m_fieldRect.getMinX())
	{
		cx = m_fieldRect.getMinX() + m_radius;
	}
	else if (box.getMaxX() > m_fieldRect.getMaxX())
	{
		cx = m_fieldRect.getMaxX() - m_radius;
	}
	if (box.getMinY() < m_fieldRect.getMinY())
	{
		cy = m_fieldRect.getMinY() + m_radius;
	}
	else if (box.getMaxY() > m_fieldRect.getMaxY())
	{
		cy = m_fieldRect.getMaxY() - m_radius;
	}
	return Vec2(cx, cy);
}

void Paddle::setPosition(Vec2 pos)
{ 
	Vec2 bound_pos = boundToFieldRect(pos);
	m_centerX = bound_pos.x; 
	m_centerY = bound_pos.y; 
	m_ccsSprite->setPosition(Vec2(m_centerX, m_centerY));
}
