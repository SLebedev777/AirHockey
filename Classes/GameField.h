#pragma once

#include "cocos2d.h"
#include <vector>

/*

����������� ����� ��� ����-������.
���� �������� � ���� ������.

���� - �������������.

���� - ��������� ������, ������� ���������� �� ������.
������������ �����:
	- ������������� ������� ����, ��� ����� ������������ ������� � �����
    - 2 ������� ����� (��������������)
	- 2 �������� ����� (��������������)
	- 2 ����� �� �������� ������
	- 4 ����, ����������� ����� (��������������, ���� ��������)
�������������� �����:
	- ����� �������� �� ������� ����
	- ����������� ����

*/


struct GoalGateSettings
{
	enum class GoalGateShapeType
	{
		HALF_CIRCLE,
		RECTANGLE
	};

	explicit GoalGateSettings(float gate_width, float line_width, const cocos2d::Color4F& fill_color, const cocos2d::Color4F& line_color,
		GoalGateShapeType goal_gate_shape_type, cocos2d::Texture2D* texture = nullptr) :
		gateWidth(gate_width),
		lineWidth(line_width),
		fillColor(fill_color),
		lineColor(line_color),
		goalGateShapeType(goal_gate_shape_type),
		texture(texture)
	{}

	float gateWidth;
	float lineWidth;
	cocos2d::Color4F fillColor;
	cocos2d::Color4F lineColor;
	GoalGateShapeType goalGateShapeType;
	cocos2d::Texture2D* texture = nullptr;
};

class GoalGate
{
public:
	GoalGate(const GoalGateSettings& settings) :
		m_settings(settings)
	{}
	~GoalGate() {}

private:
	GoalGateSettings m_settings;
	cocos2d::DrawNode* m_drawNode = nullptr;
	cocos2d::Sprite* m_sprite = nullptr;
};


struct CentralCircleSettings
{
	explicit CentralCircleSettings(float radius, float line_width, const cocos2d::Color4F& fill_color, const cocos2d::Color4F& line_color,
		cocos2d::Texture2D* texture = nullptr):
		radius(radius),
		lineWidth(line_width),
		fillColor(fill_color),
		lineColor(line_color),
		texture(texture)
	{}

	float radius;
	float lineWidth;
	cocos2d::Color4F fillColor;
	cocos2d::Color4F lineColor;
	cocos2d::Texture2D* texture = nullptr;
};

class CentralCircle
{
public:
	CentralCircle(const CentralCircleSettings& settings) :
		m_settings(settings)
	{}
	~CentralCircle() {}

private:
	CentralCircleSettings m_settings;
	cocos2d::DrawNode* m_drawNode = nullptr;
	cocos2d::Sprite* m_sprite = nullptr;
};


class GameFieldSidePart
{
public:
	GameFieldSidePart(const cocos2d::Size& size, cocos2d::Texture2D* texture);
	GameFieldSidePart(const cocos2d::Size& size, const cocos2d::Color4F& fill_color); // v
	GameFieldSidePart(const std::string& sprite_name);
	GameFieldSidePart(const GameFieldSidePart& other);
	GameFieldSidePart& operator=(const GameFieldSidePart& other);

	void moveBy(const cocos2d::Vec2& shift);  // v
	const cocos2d::Vec2& getPosition() const { return m_node->getPosition(); }  // v
	void setPosition(const cocos2d::Vec2& pos) { m_node->setPosition(pos); }  // v
	cocos2d::Rect getRect() const { return m_node->getBoundingBox(); }  // v
	void setParent(cocos2d::Node* parent);  // v
	void addPhysicsBody();

private:
	cocos2d::Node* m_node = nullptr;
	//cocos2d::DrawNode* m_drawNode = nullptr;
	//cocos2d::Sprite* m_sprite = nullptr;
};
typedef std::unique_ptr<GameFieldSidePart> GameFieldSidePartPtr;


class GameFieldSide
{
public:
	enum class DIRECTION
	{
		UP = 0,
		RIGHT,
		DOWN,
		LEFT
	};

public:
	GameFieldSide(const DIRECTION& direction) :
		m_direction(direction)
	{}
	GameFieldSide(const GameFieldSide& other);
	GameFieldSide& operator=(const GameFieldSide& other);

	void addSidePart(GameFieldSidePartPtr part);  // v
	void moveBy(const cocos2d::Vec2& shift);  // v
	void moveTo(const cocos2d::Vec2& pos);  // v
	const DIRECTION& getDirection() const { return m_direction; }  // v
	const cocos2d::Vec2& getOrigin() const;  // v ��������� ���������� ���� �����, � ����������� �� ����������� ����� 
	void setParent(cocos2d::Node* parent);  // v

private:
	std::vector<GameFieldSidePartPtr> m_parts;
	DIRECTION m_direction;
};
typedef std::unique_ptr<GameFieldSide> GameFieldSidePtr;

// ����� ����� �������� ��� �������, �.�. ��� ������ ������, �� ������� ���������� �������
// ������ ������ ���������� �� ������ ������� ���� ����� (�� ����� origin �����), ��� ������������� ����� (0, 0)
// ������ (0�) ������� � ������ ������ ��������� ������� �� ����� origin � �����, ������� ���������� � ��������.
// �.�. � �������� ���������� ����� �������, � ������ ���������� � ���������� ��������

// �������� ������ ���� �� ������� ������� � ������ ������������� ����� � ����� origin (0, 0)
// ���-�� �������� ������ ���� ������
// � ������ ������ ����������� ����� �������������� ������� (0, height), 

class GameFieldBuilder;

class GameField
{
	friend class GameFieldBuilder;

public:
	void setParent(cocos2d::Node* parent);
	//GameField(const GameField& other);
	//GameField& operator=(const GameField& other);
	~GameField(); // v


	GameField() = default;

private:
	std::vector<GameFieldSidePtr> m_sides;
	std::vector<GameFieldSidePartPtr> m_corners;
	//GoalGate m_gatePlayer1, m_gatePlayer2;
	//CentralCircle m_centralCircle;
	cocos2d::Vec2 m_center;
	cocos2d::Rect m_playRect;
	cocos2d::Node* m_ccGameFieldNode = nullptr;
	cocos2d::Node* m_ccParent = nullptr;
};

typedef std::unique_ptr<GameField> GameFieldPtr;


class GameFieldBuilder
{

public:
	GameFieldBuilder()
	{ 
		m_field = std::make_unique<GameField>();
		m_field->m_ccGameFieldNode = cocos2d::Node::create();
	}
	void addPlayRect(const cocos2d::Rect& rect); // v
	void addSide(GameFieldSidePtr side); // v
	void addCorner(GameFieldSidePartPtr corner);
	void addGoalGate(const GoalGateSettings& settings, const cocos2d::Vec2& pos);
	void addCentralCircle(const CentralCircleSettings& settings);
	GameFieldPtr getResult()
	{
		if (check())
			return std::move(m_field);
		else
			return nullptr;
	}
private:
	bool check() const;

private:
	GameFieldPtr m_field = nullptr;
};