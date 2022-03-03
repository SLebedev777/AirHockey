#include "GameScene_VFX.h"
#include "cocos/2d/CCParticleExamples.h"

USING_NS_CC;

namespace airhockey
{
    namespace VFX
    {

        ParticleSystemQuad* getParticleVFXPuckCollidePaddle(const cocos2d::Color4F& paddle_color)
        {
            auto emitter = ParticleExplosion::createWithTotalParticles(1000);
            emitter->setDuration(0.1f);
            emitter->setEmitterMode(cocos2d::ParticleSystem::Mode::RADIUS);
            emitter->setStartRadius(1.0f);
            emitter->setEndRadius(500.0f);
            emitter->setLife(0.1f);
            emitter->setStartSize(15.0f);
            emitter->setEndSize(2.0f);
            cocos2d::Color4F start_color(paddle_color);
            start_color += cocos2d::Color4F(0.4f, 0.4f, 0.4f, 1.0f);
            emitter->setStartColor(start_color);
            emitter->setStartColorVar(cocos2d::Color4F(0.1f, 0.1f, 0.1f, 0.0f));
            emitter->setEndColor(paddle_color);
            emitter->setEndColorVar(cocos2d::Color4F(0.0f, 0.0f, 0.0f, 0.0f));
            emitter->setEmissionRate(1000);
            emitter->setAngleVar(30);

            return emitter;
        }

        ParticleSystemQuad* getParticleVFXPuckCollideWalls()
        {
            auto emitter = ParticleExplosion::createWithTotalParticles(1000);
            emitter->setDuration(0.1f);
            emitter->setEmitterMode(cocos2d::ParticleSystem::Mode::RADIUS);
            emitter->setStartRadius(1.0f);
            emitter->setEndRadius(300.0f);
            emitter->setLife(0.2f);
            emitter->setStartSize(50.0f);
            emitter->setEndSize(10.0f);
            cocos2d::Color4F start_color(cocos2d::Color4F::WHITE);
            emitter->setStartColor(start_color);
            emitter->setStartColorVar(cocos2d::Color4F(0.1f, 0.1f, 0.1f, 0.0f));
            emitter->setEndColor(cocos2d::Color4F::YELLOW);
            emitter->setEndColorVar(cocos2d::Color4F(0.0f, 0.0f, 0.0f, 0.0f));
            emitter->setEmissionRate(1000);
            emitter->setAngleVar(45);

            return emitter;
        }

        cocos2d::ParticleSystemQuad* getParticleVFXGoalHit(bool is_upper_goal, float goal_width, const Color4F& color)
        {
            auto emitter = ParticleSnow::createWithTotalParticles(150);

            emitter->setDuration(1.0f);
            emitter->setLife(1.5f);
            emitter->setLifeVar(1);

            emitter->setEmissionRate(emitter->getTotalParticles() / emitter->getLife());

            // gravity
            float gravity = -10.f;
            float speed = 300.0f;
            if (!is_upper_goal)
            {
                gravity *= -1;
                speed *= -1;
            }

            emitter->setGravity(Vec2(0.0f, gravity));

            // speed of particles
            emitter->setSpeed(speed);
            emitter->setSpeedVar(100);

            emitter->setTangentialAccel(0);
            emitter->setTangentialAccelVar(70);

            emitter->setStartColor(color);

            emitter->setPosVar(Vec2(goal_width / 2, 0));

            emitter->setStartSize(50);
            emitter->setEndSize(30);

            auto texture = Director::getInstance()->getTextureCache()->addImage("vfx/star.png");
            emitter->setTexture(texture);

            return emitter;
        }
    }
}