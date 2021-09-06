#include "GameField.h"

USING_NS_CC;

CentralCircleMarking::CentralCircleMarking(const CentralCircleMarkingSettings& settings) :
	m_settings(settings),
	m_drawNode(DrawNode::create())
{
	m_drawNode->setAnchorPoint(Vec2(0.5, 0.5));
	m_drawNode->drawSolidCircle(Vec2::ZERO, m_settings.radius, 0.0, 64, m_settings.fillColor);
	m_drawNode->setLineWidth(m_settings.lineWidth);
	m_drawNode->drawCircle(Vec2::ZERO, m_settings.radius, 0.0, 64, false, m_settings.lineColor);
}

void CentralCircleMarking::setPosition(const cocos2d::Vec2& pos)
{
	m_drawNode->setPosition(pos);
}

void CentralCircleMarking::setParent(Node* parent)
{
	parent->addChild(m_drawNode, 1);
}

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

void GameFieldSidePart::setAnchorPoint(const cocos2d::Vec2& anchor_point)
{
	m_node->setAnchorPoint(anchor_point);
}

/// <summary>
/// GameFieldSide
/// </summary>

void GameFieldSide::addSidePart(GameFieldSidePartPtr part, float gap)
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
		new_pos = Vec2(last_rect.getMinX(), last_rect.getMaxY() + gap);
		break;
	case GameFieldSide::DIRECTION::RIGHT:
		new_pos = Vec2(last_rect.getMaxX() + gap, last_rect.getMinY());
		break;
	case GameFieldSide::DIRECTION::DOWN:
		new_pos = Vec2(last_rect.getMinX(), last_rect.getMinY() - part->getRect().size.height - gap);
		break;
	case GameFieldSide::DIRECTION::LEFT:
		new_pos = Vec2(last_rect.getMinX() - part->getRect().size.width - gap, last_rect.getMinY());
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

cocos2d::Vec2 GameFieldSide::getOrigin() const
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
	default:
		throw;
		break;
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
	// adding all recursive parts of game table to parent layer is done in Builder
	m_ccParent->addChild(m_ccGameFieldNode, 1);
}

GameField::~GameField()
{
	m_ccGameFieldNode->removeFromParent();
}

cocos2d::Vec2 GameField::getPlayRectCornerPoint(const GameFieldPlayRectCornerType& corner_type)
{
	switch (corner_type)
	{
	case GameFieldPlayRectCornerType::BOTTOM_LEFT:
		return m_playRect.origin;
		break;
	case GameFieldPlayRectCornerType::TOP_LEFT:
		return Vec2(m_playRect.getMinX(), m_playRect.getMaxY());
		break;
	case GameFieldPlayRectCornerType::TOP_RIGHT:
		return Vec2(m_playRect.getMaxX(), m_playRect.getMaxY());
		break;
	case GameFieldPlayRectCornerType::BOTTOM_RIGHT:
		return Vec2(m_playRect.getMaxX(), m_playRect.getMinY());
		break;
	default:
		throw;
		break;
	}
}

/// <summary>
/// GameFieldBuilder
/// </summary>

GameFieldBuilder::GameFieldBuilder() :
	m_field(std::make_unique<GameField>())
{
	m_field->m_ccGameFieldNode = cocos2d::Node::create();
}

void GameFieldBuilder::addPlayRect(const cocos2d::Rect& rect)
{
	m_field->m_playRect = rect;
	m_field->m_center = Vec2(rect.getMidX(), rect.getMidY());
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

void GameFieldBuilder::addCorner(GameFieldSidePartPtr corner, const GameField::GameFieldPlayRectCornerType& play_rect_corner_type)
{
	// bottom-left corner
	switch (play_rect_corner_type)
	{
	case GameField::GameFieldPlayRectCornerType::BOTTOM_LEFT:
		corner->setAnchorPoint(Vec2(1.0, 1.0));
		break;
	case GameField::GameFieldPlayRectCornerType::TOP_LEFT:
		corner->setAnchorPoint(Vec2(1.0, 0.0));
		break;
	case GameField::GameFieldPlayRectCornerType::TOP_RIGHT:
		corner->setAnchorPoint(Vec2(0.0, 0.0));
		break;
	case GameField::GameFieldPlayRectCornerType::BOTTOM_RIGHT:
		corner->setAnchorPoint(Vec2(0.0, 1.0));
		break;
	default:
		return;
		break;
	}
	corner->setPosition(m_field->getPlayRectCornerPoint(play_rect_corner_type));

	corner->setParent(m_field->m_ccGameFieldNode);
	m_field->m_corners.push_back(std::move(corner));
}

void GameFieldBuilder::addCentralCircleMarking(const CentralCircleMarkingSettings& settings)
{
	m_field->m_centralCircleMarking = std::make_unique<CentralCircleMarking>(settings);
	m_field->m_centralCircleMarking->setPosition(m_field->getCenter());
	m_field->m_centralCircleMarking->setParent(m_field->m_ccGameFieldNode);
}

GameFieldPtr GameFieldBuilder::getResult()
{
	if (check())
		return std::move(m_field);
	else
		return nullptr;
}

bool GameFieldBuilder::check() const
{
	return true;
}