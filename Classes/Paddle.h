#pragma once

#include "cocos2d.h"
#include "cocos/math/CCGeometry.h"

USING_NS_CC;

class Paddle
{
public:
	Paddle(const std::string& image_name, float x, float y, float vel_x, float vel_y, float radius, Rect& field_rect);
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

	cocos2d::Sprite* getSprite() const { return m_ccSprite; }
	cocos2d::Vec2 getPosition() const { return cocos2d::Vec2(m_centerX, m_centerY); }
	void setPosition(cocos2d::Vec2 pos);
	cocos2d::Vec2 boundToFieldRect(cocos2d::Vec2 pos);
	float getRadius() const { return m_radius; }
	void setNormalizedVelocity(const cocos2d::Vec2& nvel) { m_velXnorm = nvel.x; m_velYnorm = nvel.y; }

private:
	Rect m_fieldRect;
	cocos2d::Sprite* m_ccSprite = nullptr;
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
};

typedef std::shared_ptr<Paddle> PaddlePtr;