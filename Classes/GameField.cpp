#include "GameField.h"

namespace airhockey
{

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
	/// GoalGateMarking
	/// </summary>

	GoalGateMarking::GoalGateMarking(const GoalGateMarkingSettings& settings, const GoalGateLocationType& location) :
		m_settings(settings),
		m_location(location),
		m_drawNode(DrawNode::create())
	{
		if (m_settings.goalGateMarkingShapeType == GoalGateMarkingSettings::GoalGateMarkingShapeType::HALF_CIRCLE)
		{
			throw std::invalid_argument("Not implemented!");
		}
		else
		{
			Vec2* vertices = nullptr;
			if (m_location == GoalGateLocationType::LOWER)
			{
				Vec2 vert[] = {
					Vec2(-m_settings.gateWidth / 2, 0),
					Vec2(-m_settings.gateWidth / 2, m_settings.gateWidth / 2),
					Vec2(m_settings.gateWidth / 2, settings.gateWidth / 2),
					Vec2(settings.gateWidth / 2, 0)
				};
				vertices = vert;
			}
			else if (m_location == GoalGateLocationType::UPPER)
			{
				Vec2 vert[] = {
					Vec2(-m_settings.gateWidth / 2, 0),
					Vec2(-m_settings.gateWidth / 2, -m_settings.gateWidth / 2),
					Vec2(m_settings.gateWidth / 2, -settings.gateWidth / 2),
					Vec2(settings.gateWidth / 2, 0)
				};
				vertices = vert;
			}
			else
				throw;

			m_drawNode->setAnchorPoint(Vec2(0.0, 0.0));
			m_drawNode->setLineWidth(m_settings.lineWidth);
			m_drawNode->drawPoly(vertices, 4, false, m_settings.lineColor);
			m_drawNode->setAnchorPoint(Vec2(0.5, 0.0));
		}
	}

	void GoalGateMarking::setPosition(const cocos2d::Vec2& pos)
	{
		m_drawNode->setPosition(pos);
	}

	void GoalGateMarking::setParent(Node* parent)
	{
		parent->addChild(m_drawNode, 1);
	}

	/// <summary>
	/// GameFieldSidePart
	/// </summary>

	GameFieldSidePart::GameFieldSidePart(const cocos2d::Size& size, const cocos2d::Color4F& fill_color, bool add_default_physics_body) :
		m_node(DrawNode::create())
	{
		static_cast<DrawNode*>(m_node)->drawSolidRect(Vec2::ZERO, Vec2(size.width, size.height), fill_color);
		m_node->setAnchorPoint(Vec2(0, 0));
		m_node->setPosition(Vec2::ZERO);
		m_node->setContentSize(size);
		if (add_default_physics_body)
		{
			addPhysicsBody();
		}
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

	void GameFieldSidePart::addPhysicsBody(const cocos2d::PhysicsMaterial& material)
	{
		PhysicsBody* box = PhysicsBody::createEdgeBox(m_node->getContentSize(), material);
		box->setDynamic(false);
		m_node->addComponent(box);
	}

	/// <summary>
	/// GoalGate
	/// </summary>

	GoalGate::GoalGate(const cocos2d::Size& size, const GoalGateLocationType& location_type) :
		m_node(Node::create()),
		m_locationType(location_type)
	{
		m_node->setContentSize(size);
		m_node->setAnchorPoint(Vec2(0.5f, 0.5f));
	}

	void GoalGate::setPosition(const cocos2d::Vec2& pos)
	{
		m_node->setPosition(pos);
	}

	void GoalGate::setParent(cocos2d::Node* parent)
	{
		parent->addChild(m_node, 1);
	}


	/// <summary>
	/// GameFieldSide
	/// </summary>

	void GameFieldSide::addSidePart(GameFieldSidePartPtr part, float gap)
	{
		Vec2 new_pos = Vec2::ZERO;
		Rect last_rect = Rect::ZERO;
		if (m_parts.empty())
		{
			start_gap = gap;
		}
		else
		{
			last_rect = m_parts.back()->getRect();
		}

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

		switch (m_direction)
		{
		case GameFieldSide::DIRECTION::UP:
			return Vec2(first_rect.getMaxX(), first_rect.getMinY() - start_gap);
			break;
		case GameFieldSide::DIRECTION::RIGHT:
			return Vec2(first_rect.getMinX() - start_gap, first_rect.getMinY());
			break;
		case GameFieldSide::DIRECTION::DOWN:
			return Vec2(first_rect.getMinX(), first_rect.getMaxY() + start_gap);
			break;
		case GameFieldSide::DIRECTION::LEFT:
			return Vec2(first_rect.getMaxX() + start_gap, first_rect.getMaxY());
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

	cocos2d::Vec2 GameField::getPlayRectCornerPoint(const GameFieldPlayRectCornerType& corner_type) const
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

	const GoalGate& GameField::getGoalGate(const GoalGateLocationType& location_type) const
	{
		if (location_type == GoalGateLocationType::LOWER)
			return *m_gateLower.get();
		else if (location_type == GoalGateLocationType::UPPER)
			return *m_gateUpper.get();
	}

	cocos2d::Rect GameField::getPlayRect(const GoalGateLocationType& location_type) const
	{
		Rect rect = getPlayRect();
		if (location_type == GoalGateLocationType::LOWER)
			return Rect(rect.getMinX(), rect.getMinY(), rect.size.width, rect.size.height / 2);
		else if (location_type == GoalGateLocationType::UPPER)
			return Rect(rect.getMinX(), rect.getMidY(), rect.size.width, rect.size.height / 2);
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

	void GameFieldBuilder::addGoalGate(GoalGatePtr gate)
	{
		if (m_field->m_playRect.equals(Rect::ZERO))
			throw std::runtime_error("Can't add goal gate to empty game field!");

		gate->setParent(m_field->m_ccGameFieldNode);
		// can add concrete type of gate only once
		if (gate->getLocationType() == GoalGateLocationType::LOWER)
		{
			if (m_field->m_gateLower)
				return;

			gate->setPosition(Vec2(m_field->m_playRect.getMidX(), m_field->m_playRect.getMinY() - gate->getRect().size.height / 2));
			m_field->m_gateLower = std::move(gate);
		}
		else if (gate->getLocationType() == GoalGateLocationType::UPPER)
		{
			if (m_field->m_gateUpper)
				return;

			gate->setPosition(Vec2(m_field->m_playRect.getMidX(), m_field->m_playRect.getMaxY() + gate->getRect().size.height / 2));
			m_field->m_gateUpper = std::move(gate);
		}
	}

	void GameFieldBuilder::addGoalGateMarking(const GoalGateMarkingSettings& settings, const cocos2d::Vec2& pos)
	{
		m_field->m_gateLowerMarking = std::make_unique<GoalGateMarking>(settings, GoalGateLocationType::LOWER);
		m_field->m_gateLowerMarking->setPosition(Vec2(m_field->getPlayRect().getMidX(), m_field->getPlayRect().getMinY()));
		m_field->m_gateLowerMarking->setParent(m_field->m_ccGameFieldNode);

		m_field->m_gateUpperMarking = std::make_unique<GoalGateMarking>(settings, GoalGateLocationType::UPPER);
		m_field->m_gateUpperMarking->setPosition(Vec2(m_field->getPlayRect().getMidX(), m_field->getPlayRect().getMaxY()));
		m_field->m_gateUpperMarking->setParent(m_field->m_ccGameFieldNode);

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

}