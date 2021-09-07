#pragma once

#include "cocos2d.h"
#include <vector>

/*

Построитель стола для аэро-хоккея.
Стол строится в виде сверху.

Стол - прямоугольник.

Стол - составной объект, который собирается из частей.
Обязательные части:
	- прямоугольное игровое поле, где будут перемещаться ракетки и шайба
    - 2 длинных борта (прямоугольники)
	- 2 коротких борта (прямоугольники)
	- 2 ворот на коротких бортах
	- 4 угла, соединяющих борты (прямоугольники, чаще квадраты)
Необязательные части:
	- линии разметки на игровом поле
	- центральный круг

*/


enum class GoalGateLocationType
{
	LOWER = 0,
	UPPER
};

/*
* Параметры разметки поля у игровых ворот на короткому борту.
*/
struct GoalGateMarkingSettings
{
	enum class GoalGateMarkingShapeType
	{
		HALF_CIRCLE,
		RECTANGLE
	};

	explicit GoalGateMarkingSettings(float gate_width, float line_width, const cocos2d::Color4F& fill_color, const cocos2d::Color4F& line_color,
		GoalGateMarkingShapeType goal_gate_marking_shape_type, cocos2d::Texture2D* texture = nullptr) :
		gateWidth(gate_width),
		lineWidth(line_width),
		fillColor(fill_color),
		lineColor(line_color),
		goalGateMarkingShapeType(goal_gate_marking_shape_type),
		texture(texture)
	{}

	float gateWidth;
	float lineWidth;
	cocos2d::Color4F fillColor;
	cocos2d::Color4F lineColor;
	GoalGateMarkingShapeType goalGateMarkingShapeType;
	cocos2d::Texture2D* texture = nullptr;
};

class GoalGateMarking
{
public:
	GoalGateMarking(const GoalGateMarkingSettings& settings) :
		m_settings(settings)
	{}
	~GoalGateMarking() {}

private:
	GoalGateMarkingSettings m_settings;
	cocos2d::DrawNode* m_drawNode = nullptr;
	cocos2d::Sprite* m_sprite = nullptr;
};
typedef std::unique_ptr<GoalGateMarking> GoalGateMarkingPtr;


struct CentralCircleMarkingSettings
{
	explicit CentralCircleMarkingSettings(float radius, float line_width, const cocos2d::Color4F& fill_color, const cocos2d::Color4F& line_color,
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

class CentralCircleMarking
{
public:
	CentralCircleMarking(const CentralCircleMarkingSettings& settings);  // v
	~CentralCircleMarking() {}
	void setPosition(const cocos2d::Vec2& pos);  // v
	void setParent(cocos2d::Node* parent);  // v

private:
	CentralCircleMarkingSettings m_settings;
	cocos2d::DrawNode* m_drawNode = nullptr;
	cocos2d::Sprite* m_sprite = nullptr;
};
typedef std::unique_ptr<CentralCircleMarking> CentralCircleMarkingPtr;

/* Часть борта игрового стола - элемент, из которых состоит борт.
*  Функционально - это графический нод с физическим телом.
*/
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
	void setAnchorPoint(const cocos2d::Vec2& anchor_point);  // v
	void setParent(cocos2d::Node* parent);  // v
	void addPhysicsBody();

private:
	cocos2d::Node* m_node = nullptr;
	//cocos2d::DrawNode* m_drawNode = nullptr;
	//cocos2d::Sprite* m_sprite = nullptr;
};
typedef std::unique_ptr<GameFieldSidePart> GameFieldSidePartPtr;


/*
* Ворота. Пока -просто прямоугольник, на который будет срабатывать расположение шайбы, чтобы засчитать гол.
*/
class GoalGate
{
public:
	GoalGate(const cocos2d::Size& size, const GoalGateLocationType& location_type);  // v

	const GoalGateLocationType& getLocationType() const { return m_locationType; }  // v
	cocos2d::Rect getRect() const { return m_node->getBoundingBox(); }
	void setPosition(const cocos2d::Vec2& pos);  // v

private:
	cocos2d::Sprite* m_sprite = nullptr;
	cocos2d::Node* m_node = nullptr;
	GoalGateLocationType m_locationType;
};
typedef std::unique_ptr<GoalGate> GoalGatePtr;

/*
  Борт - ориентированная боковая сторона игрового стола. Состоит из частей-объектов GameFieldSidePart.
*/
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

	void addSidePart(GameFieldSidePartPtr part, float gap = 0.0f);  // v
	void moveBy(const cocos2d::Vec2& shift);  // v
	void moveTo(const cocos2d::Vec2& pos);  // v
	const DIRECTION& getDirection() const { return m_direction; }  // v
	cocos2d::Vec2 getOrigin() const;  // v стартовый внутренний угол борта, в зависимости от направления борта 
	void setParent(cocos2d::Node* parent);  // v

private:
	std::vector<GameFieldSidePartPtr> m_parts;
	DIRECTION m_direction;
	float start_gap = 0.0f;
};
typedef std::unique_ptr<GameFieldSide> GameFieldSidePtr;


// fwd declaration
class GameFieldBuilder;


/*
* Игровое поле (игровой стол в виде сверху).
* Создать можно только через билдер.
*/
class GameField
{
	friend class GameFieldBuilder;

public:
	enum class GameFieldPlayRectCornerType
	{
		BOTTOM_LEFT = 0,
		TOP_LEFT,
		TOP_RIGHT,
		BOTTOM_RIGHT
	};

public:
	GameField() = default;
	GameField(const GameField& other) = delete;
	GameField& operator=(const GameField& other) = delete;
	~GameField(); // v

	cocos2d::Vec2 getPlayRectCornerPoint(const GameFieldPlayRectCornerType& corner_type);  // v
	const cocos2d::Vec2& getCenter() { return m_center; }  // v
	void setParent(cocos2d::Node* parent);  // v

private:
	std::vector<GameFieldSidePtr> m_sides;
	std::vector<GameFieldSidePartPtr> m_corners;
	GoalGatePtr m_gateLower = nullptr;
	GoalGatePtr m_gateUpper = nullptr;
	cocos2d::Vec2 m_center = cocos2d::Vec2::ZERO;
	cocos2d::Rect m_playRect = cocos2d::Rect::ZERO;
	cocos2d::Node* m_ccGameFieldNode = nullptr;
	cocos2d::Node* m_ccParent = nullptr;
	CentralCircleMarkingPtr m_centralCircleMarking = nullptr;
};

typedef std::unique_ptr<GameField> GameFieldPtr;


/*
* Класс-построитель игрового поля.
*/
class GameFieldBuilder
{

public:
	GameFieldBuilder(); // v
	void addPlayRect(const cocos2d::Rect& rect); // v
	void addSide(GameFieldSidePtr side); // v
	void addCorner(GameFieldSidePartPtr corner, const GameField::GameFieldPlayRectCornerType& play_rect_corner_type);  // v
	void addGoalGate(GoalGatePtr gate);
	void addGoalGateMarking(const GoalGateMarkingSettings& settings, const cocos2d::Vec2& pos);
	void addCentralCircleMarking(const CentralCircleMarkingSettings& settings);  // v
	GameFieldPtr getResult();  // v

private:
	bool check() const;

private:
	GameFieldPtr m_field = nullptr;
};