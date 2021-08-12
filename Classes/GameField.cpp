#include "GameField.h"

USING_NS_CC;

/// <summary>
/// GameFieldSidePart
/// </summary>

GameFieldSidePart::GameFieldSidePart(const cocos2d::Size& size, const cocos2d::Color4F& fill_color)
{
	m_node = DrawNode::create();
	static_cast<DrawNode*>(m_node)->drawSolidRect(Vec2::ZERO, Vec2(size.width, size.height), fill_color);
	m_node->setAnchorPoint(Vec2(0, 0));
	m_node->setPosition(Vec2::ZERO);
	m_node->setContentSize(size);
}

void GameFieldSidePart::moveBy(const cocos2d::Vec2& shift)
{
	m_node->setPosition(m_node->getPosition() + shift);
}

void GameFieldSidePart::setParent(Node* parent)
{
	parent->addChild(m_node, 1);
}

/// <summary>
/// GameFieldSide
/// </summary>

void GameFieldSide::addSidePart(GameFieldSidePartPtr part)
{
	if (m_parts.empty())
	{
		m_parts.push_back(std::move(part));
		return;
	}

	Vec2 new_pos = Vec2::ZERO;
	Rect last_rect = m_parts.back()->getRect();

	switch (m_direction)
	{
	case GameFieldSide::DIRECTION::UP:
		new_pos = Vec2(last_rect.getMinX(), last_rect.getMaxY());
		break;
	case GameFieldSide::DIRECTION::RIGHT:
		new_pos = Vec2(last_rect.getMaxX(), last_rect.getMinY());
		break;
	case GameFieldSide::DIRECTION::DOWN:
		new_pos = Vec2(last_rect.getMinX(), last_rect.getMinY() - part->getRect().size.height);
		break;
	case GameFieldSide::DIRECTION::LEFT:
		new_pos = Vec2(last_rect.getMinX() - part->getRect().size.width, last_rect.getMinY());
		break;
	default: break;
	}
	part->setPosition(new_pos);
	m_parts.push_back(std::move(part));
}

void GameFieldSide::moveBy(const cocos2d::Vec2& shift)
{
	for (auto& part : m_parts)
	{
		part->moveBy(shift);
	}
}

void GameFieldSide::moveTo(const cocos2d::Vec2& pos)
{
	Vec2 shift(pos);
	shift -= getOrigin();
	moveBy(shift);
}

const cocos2d::Vec2& GameFieldSide::getOrigin() const
{
	if (m_parts.empty())
		return Vec2::ZERO;

	Rect first_rect = m_parts.front()->getRect();

	switch(m_direction)
	{
	case GameFieldSide::DIRECTION::UP:
		return Vec2(first_rect.getMaxX(), first_rect.getMinY());
		break;
	case GameFieldSide::DIRECTION::RIGHT:
		return Vec2(first_rect.getMinX(), first_rect.getMinY());
		break;
	case GameFieldSide::DIRECTION::DOWN:
		return Vec2(first_rect.getMinX(), first_rect.getMaxY());
		break;
	case GameFieldSide::DIRECTION::LEFT:
		return Vec2(first_rect.getMaxX(), first_rect.getMaxY());
		break;
	default: break;
	}
}

void GameFieldSide::setParent(Node* parent)
{
	for (auto& part : m_parts)
	{
		part->setParent(parent);
	}
}

/// <summary>
/// GameField
/// </summary>

void GameField::setParent(Node* parent)
{
	m_ccParent = parent;
	// TODO: add all recursive parts of game table to parent layer
	m_ccParent->addChild(m_ccGameFieldNode, 1);
}

GameField::~GameField()
{
	m_ccGameFieldNode->removeFromParent();
}


/// <summary>
/// GameFieldBuilder
/// </summary>

void GameFieldBuilder::addPlayRect(const cocos2d::Rect& rect)
{
	m_field->m_playRect = rect;
}

void GameFieldBuilder::addSide(GameFieldSidePtr side)
{
	auto rect = m_field->m_playRect;

	switch (side->getDirection())
	{
	case GameFieldSide::DIRECTION::UP:
		side->moveTo(Vec2(rect.getMinX(), rect.getMinY()));
		break;
	case GameFieldSide::DIRECTION::RIGHT:
		side->moveTo(Vec2(rect.getMinX(), rect.getMaxY()));
		break;
	case GameFieldSide::DIRECTION::DOWN:
		side->moveTo(Vec2(rect.getMaxX(), rect.getMaxY()));
		break;
	case GameFieldSide::DIRECTION::LEFT:
		side->moveTo(Vec2(rect.getMaxX(), rect.getMinY()));
		break;
	default: break;
	}

	side->setParent(m_field->m_ccGameFieldNode);
	m_field->m_sides.push_back(std::move(side));
}

bool GameFieldBuilder::check() const
{
	return true;
}