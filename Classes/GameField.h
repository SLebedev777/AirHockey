#pragma once

#include "cocos2d.h"
#include <vector>

/*

Построитель стола для аэро-хоккея.
Стол строится в виде сверху.

Стол - прямоугольник.

Стол - составной объект, который собирается из частей.
Обязательные части:
	- 2 длинных борта (прямоугольники)
	- 2 коротких борта (прямоугольники)
	- 2 ворот на коротких бортах
	- 4 угла, соединяющих борты (прямоугольники, чаще квадраты)
	- прямоугольное игровое поле, где будут перемещаться ракетки и шайба
Необязательные части:
	- линии разметки на игровом поле
	- центральный круг

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
	GoalGate(const GoalGateSettings& settings);
	~GoalGate() {}

private:
	GoalGateSettings settings;
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
	CentralCircle(const CentralCircleSettings& settings);
	~CentralCircle() {}

private:
	CentralCircleSettings settings;
	cocos2d::DrawNode* m_drawNode = nullptr;
	cocos2d::Sprite* m_sprite = nullptr;
};


class GameFieldSidePart
{
public:
	GameFieldSidePart(const cocos2d::Vec2& pos, const cocos2d::Size& size, cocos2d::Texture2D* texture);
	GameFieldSidePart(const cocos2d::Vec2& pos, const std::string& sprite_name);
	GameFieldSidePart(const GameFieldSidePart& other);
	GameFieldSidePart& operator=(const GameFieldSidePart& other);

private:
	cocos2d::DrawNode* m_drawNode = nullptr;
	cocos2d::Sprite* m_sprite = nullptr;
};
typedef std::unique_ptr<GameFieldSidePart> GameFieldSidePartPtr;

class GameFieldSide
{
public:
	GameFieldSide();
	void addSidePart(GameFieldSidePartPtr part);

private:
	std::vector<GameFieldSidePartPtr> m_parts;
};
typedef std::shared_ptr<GameFieldSide> GameFieldSidePtr;

// борта стола задаются как ломаная, т.е. как список вершин, из которых получаются отрезки
// отсчет ВСЕГДА начинается от левого нижнего угла стола (от точки origin стола), что ссответствует точке (0, 0)
// первый (0й) элемент в списке вершин описывает отрезок из точки origin в точку, которая содержится в элементе.
// т.е. в элементе содержится КОНЕЦ отрезка, а НАЧАЛО содержится в предыдущем элементе

// описание вершин идет по часовой стрелке и должно заканчиваться также в точке origin (0, 0)
// кол-во отрезков должно быть четным
// в списке вершин обязательно дожны присутствовать вершины (0, height), 

class GameFieldBuilder;

class GameField
{
	friend class GameFieldBuilder;

public:
	void setParent(cocos2d::Node* parent);
	GameField(const GameField& other);
	GameField& operator=(const GameField& other);
	~GameField();

private:
	GameField() = default;

private:
	std::vector<GameFieldSidePtr> m_sides;
	std::vector<GameFieldSidePartPtr> m_corners;
	GoalGate m_gatePlayer1, m_gatePlayer2;
	CentralCircle m_centralCircle;
	cocos2d::Vec2 m_center;
	cocos2d::Rect m_playRect;
	cocos2d::Node* m_ccParent;
};

typedef std::unique_ptr<GameField> GameFieldPtr;


class GameFieldBuilder
{
public:
	GameFieldBuilder();
	void addPlayRect(const cocos2d::Rect& rect);
	void addSide();
	void addCorner();
	void addGoalGate(const GoalGateSettings& settings, const cocos2d::Vec2& pos);
	void addCentralCircle(const CentralCircleSettings& settings);
	GameFieldPtr getResult()
	{
		if (isBuilt())
			return std::move(m_field);
		else
			return nullptr;
	}
private:
	bool isBuilt() const;

private:
	GameFieldPtr m_field = nullptr;
	bool m_isBuilt = false;
};