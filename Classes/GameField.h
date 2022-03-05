#ifndef __GAMEFIELD_H__
#define __GAMEFIELD_H__

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

namespace airhockey
{

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

	/*
	* Разметка ворот
	*/
	class GoalGateMarking
	{
	public:
		GoalGateMarking(const GoalGateMarkingSettings& settings, const GoalGateLocationType& location);
		~GoalGateMarking() {}

		void setPosition(const cocos2d::Vec2& pos);
		void setParent(cocos2d::Node* parent);
		const GoalGateMarkingSettings& getSettings() const { return m_settings; }

	private:
		GoalGateMarkingSettings m_settings;
		GoalGateLocationType m_location;
		cocos2d::DrawNode* m_drawNode = nullptr;
		cocos2d::Sprite* m_sprite = nullptr;
	};
	typedef std::unique_ptr<GoalGateMarking> GoalGateMarkingPtr;

	/*
	* Настройки разметки центрального круга
	*/
	struct CentralCircleMarkingSettings
	{
		explicit CentralCircleMarkingSettings(float radius, float line_width, const cocos2d::Color4F& fill_color, const cocos2d::Color4F& line_color,
			cocos2d::Texture2D* texture = nullptr) :
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

	/*
	* Разметка центрального круга
	*/
	class CentralCircleMarking
	{
	public:
		CentralCircleMarking(const CentralCircleMarkingSettings& settings);
		~CentralCircleMarking() {}
		void setPosition(const cocos2d::Vec2& pos);
		void setParent(cocos2d::Node* parent);
		const CentralCircleMarkingSettings& getSettings() const { return m_settings; }

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
		GameFieldSidePart(const cocos2d::Size& size, const cocos2d::Color4F& fill_color, bool add_default_physics_body = true);
		// TODO - not implemented now
		/*
		GameFieldSidePart(const cocos2d::Size& size, cocos2d::Texture2D* texture, bool add_default_physics_body = true);
		GameFieldSidePart(const std::string& sprite_name, bool add_default_physics_body = true);
		GameFieldSidePart(const GameFieldSidePart& other);
		GameFieldSidePart& operator=(const GameFieldSidePart& other);
		*/
		void moveBy(const cocos2d::Vec2& shift);
		const cocos2d::Vec2& getPosition() const { return m_node->getPosition(); }
		void setPosition(const cocos2d::Vec2& pos) { m_node->setPosition(pos); }
		cocos2d::Rect getRect() const { return m_node->getBoundingBox(); }
		void setAnchorPoint(const cocos2d::Vec2& anchor_point);
		void setParent(cocos2d::Node* parent);
		void addPhysicsBody(cocos2d::PhysicsBody* body = nullptr, 
			const cocos2d::PhysicsMaterial& material = cocos2d::PhysicsMaterial(0.1f, 1.0f, 0.0f));

	private:
		cocos2d::Node* m_node = nullptr;

	};
	typedef std::unique_ptr<GameFieldSidePart> GameFieldSidePartPtr;


	/*
	* Ворота. Пока - просто прямоугольник, на который будет срабатывать расположение шайбы, чтобы засчитать гол.
	*/
	class GoalGate
	{
	public:
		GoalGate(const cocos2d::Size& size, const GoalGateLocationType& location_type);

		const GoalGateLocationType& getLocationType() const { return m_locationType; }
		cocos2d::Rect getRect() const { return m_node->getBoundingBox(); }
		void setPosition(const cocos2d::Vec2& pos);
		void setParent(cocos2d::Node* parent);

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

		void addSidePart(GameFieldSidePartPtr part, float gap = 0.0f);
		void moveBy(const cocos2d::Vec2& shift);
		void moveTo(const cocos2d::Vec2& pos);
		const DIRECTION& getDirection() const { return m_direction; }
		cocos2d::Vec2 getOrigin() const;  // стартовый внутренний угол борта, в зависимости от направления борта 
		void setParent(cocos2d::Node* parent);

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
		~GameField();

		cocos2d::Vec2 getPlayRectCornerPoint(const GameFieldPlayRectCornerType& corner_type) const;
		const cocos2d::Vec2& getCenter() const { return m_center; }
		cocos2d::Rect getPlayRect() const { return m_playRect; }
		cocos2d::Rect getPlayRect(const GoalGateLocationType& location_type) const;  // get play rect for each player
		void setParent(cocos2d::Node* parent);
		const GoalGate& getGoalGate(const GoalGateLocationType& location_type) const;
		const CentralCircleMarking& getCentralCircleMarking() const { return *m_centralCircleMarking; }

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
		GoalGateMarkingPtr m_gateLowerMarking = nullptr;
		GoalGateMarkingPtr m_gateUpperMarking = nullptr;
	};

	typedef std::unique_ptr<GameField> GameFieldPtr;


	/*
	* Класс-построитель игрового поля.
	*/
	class GameFieldBuilder
	{

	public:
		GameFieldBuilder();
		void addPlayRect(const cocos2d::Rect& rect, cocos2d::Node* background = nullptr);
		void addSide(GameFieldSidePtr side);
		void addCorner(GameFieldSidePartPtr corner, const GameField::GameFieldPlayRectCornerType& play_rect_corner_type);
		void addGoalGate(GoalGatePtr gate);
		void addGoalGateMarking(const GoalGateMarkingSettings& settings, const cocos2d::Vec2& pos);
		void addCentralCircleMarking(const CentralCircleMarkingSettings& settings);
		void addCentralLine(const cocos2d::Color4F& color, float width);
		GameFieldPtr getResult();

	private:
		bool check() const;

	private:
		GameFieldPtr m_field = nullptr;
	};

}

#endif  // __GAMEFIELD_H__