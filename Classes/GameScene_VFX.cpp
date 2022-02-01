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
            emitter->setDuration(0.25f);
            emitter->setEmitterMode(cocos2d::ParticleSystem::Mode::RADIUS);
            emitter->setStartRadius(1.0f);
            emitter->setEndRadius(300.0f);
            emitter->setLife(0.5f);
            emitter->setStartSize(15.0f);
            emitter->setEndSize(2.0f);
            cocos2d::Color4F start_color(paddle_color);
            emitter->setStartColor(start_color);
            emitter->setStartColorVar(cocos2d::Color4F(0.1, 0.1, 0.1, 0));
            emitter->setEndColor(cocos2d::Color4F::BLACK);
            emitter->setEndColorVar(cocos2d::Color4F(0, 0, 0, 0));
            emitter->setEmissionRate(1000);

            return emitter;
        }

        ParticleSystemQuad* getParticleVFXPuckCollideWalls()
        {
            auto emitter = ParticleExplosion::createWithTotalParticles(1000);
            emitter->setDuration(0.25f);
            emitter->setEmitterMode(cocos2d::ParticleSystem::Mode::RADIUS);
            emitter->setStartRadius(1.0f);
            emitter->setEndRadius(300.0f);
            emitter->setLife(0.5f);
            emitter->setStartSize(15.0f);
            emitter->setEndSize(2.0f);
            cocos2d::Color4F start_color(cocos2d::Color4F::YELLOW);
            emitter->setStartColor(start_color);
            emitter->setStartColorVar(cocos2d::Color4F(0.1, 0.1, 0.1, 0));
            emitter->setEndColor(cocos2d::Color4F::BLACK);
            emitter->setEndColorVar(cocos2d::Color4F(0, 0, 0, 0));
            emitter->setEmissionRate(1000);

            return emitter;
        }

    }
}