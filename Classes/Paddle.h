#ifndef __PADDLE_H__
#define __PADDLE_H__

#include "cocos2d.h"
#include "cocos/math/CCGeometry.h"

namespace airhockey
{
	class Paddle
	{
	public:
		Paddle(const std::string& image_name, float start_x, float start_y, float vel_x, float vel_y, float radius, 
			cocos2d::Rect& field_rect, cocos2d::Node* parent, cocos2d::PhysicsWorld* physics_world);
		~Paddle();

		void onMovingLeft();
		void onMovingRight();
		void onMovingUp();
		void onMovingDown();
		void onStopMovingLeft();
		void onStopMovingRight();
		void onStopMovingUp();
		void onStopMovingDown();

		bool move(float dt);

		cocos2d::Sprite* getSprite() const { return m_ccsSprite; }
		cocos2d::Vec2 getPosition() const { return cocos2d::Vec2(m_centerX, m_centerY); }
		void setPosition(cocos2d::Vec2 pos);
		cocos2d::Vec2 boundToFieldRect(cocos2d::Vec2 pos);
		float getRadius() const { return m_radius; }
		void setNormalizedVelocity(const cocos2d::Vec2& nvel) { m_velXnorm = nvel.x; m_velYnorm = nvel.y; }
		void setStartPosition(const cocos2d::Vec2& pos) { m_startPos = pos; }
		const cocos2d::Vec2& getStartPosition() const { return m_startPos; }
		cocos2d::PhysicsBody* getPhysicsBody() const { return static_cast<cocos2d::PhysicsBody*>(m_ccsSprite->getComponent(m_physicsBodyName)); }

	private:
		cocos2d::Rect m_fieldRect;
		cocos2d::Sprite* m_ccsSprite = nullptr;
		float m_centerX = 0.0f;
		float m_centerY = 0.0f;
		float m_velX = 0.0f;
		float m_velY = 0.0f;
		float m_velXnorm = 0.0f;  // normalized velocity vector in range(0, 0) - (1, 1)
		float m_velYnorm = 0.0f;
		float m_accX = 0.0f;
		float m_accY = 0.0f;
		float m_radius = 100;
		int m_movingUp = 0;  // -1 (down), 0, +1 (up)
		int m_movingRight = 0;  // -1 (left), 0, +1 (right)

		cocos2d::PhysicsWorld* m_ccPhysicsWorld = nullptr;
		cocos2d::Node* m_ccnParent = nullptr;
		cocos2d::Node* m_ccnStick = nullptr;
		const char* m_physicsBodyName = "paddle_body";

		cocos2d::Vec2 m_startPos = cocos2d::Vec2::ZERO;
	};

	typedef std::shared_ptr<Paddle> PaddlePtr;

}

#endif // __PADDLE_H__