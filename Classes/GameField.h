#pragma once

#include "cocos2d.h"
#include <vector>

/*

����������� ����� ��� ����-������.
���� �������� � ���� ������.

���� - �������������.

���� - ��������� ������, ������� ���������� �� ������.
������������ �����:
	- 2 ������� ����� (��������������)
	- 2 �������� ����� (��������������)
	- 2 ����� �� �������� ������
	- 4 ����, ����������� ����� (��������������, ���� ��������)
	- ������������� ������� ����, ��� ����� ������������ ������� � �����
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