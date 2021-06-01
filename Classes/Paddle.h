#pragma once

#include "cocos2d.h"
#include "PlayerInputController.h"

USING_NS_CC;

class Paddle
{
public:
	Paddle(const std::string& image_name, float x, float y, float vel_x, float vel_y, float radius);
	~Paddle();

	void attachInputController(IPlayerInputControllerPtr controller);

	void onMovingLeft();
	void onMovingRight();
	void onMovingUp();
	void onMovingDown();
	void onStopMovingLeft();
	void onStopMovingRight();
	void onStopMovingUp();
	void onStopMovingDown();

	bool move();

	cocos2d::Sprite* getSprite() const { return m_ccSprite; }
	cocos2d::Vec2 getPosition() const { return cocos2d::Vec2(m_centerX, m_centerY); }
	float getRadius() const { return m_radius; }

private:
	cocos2d::Sprite* m_ccSprite = nullptr;
	IPlayerInputControllerPtr m_pController = nullptr;
	float m_centerX = 0.0f;
	float m_centerY = 0.0f;
	float m_velX = 0.0f;
	float m_velY = 0.0f;
	float m_accX = 0.0f;
	float m_accY = 0.0f;
	float m_radius = 100;
	int m_movingUp = 0;  // -1 (down), 0, +1 (up)
	int m_movingRight = 0;  // -1 (left), 0, +1 (right)
};

typedef std::shared_ptr<Paddle> PaddlePtr;