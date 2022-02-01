#pragma once

//#include "cocos/2d/CCParticleSystemQuad.h"
#include "cocos2d.h"

namespace airhockey
{
	namespace VFX
	{
		cocos2d::ParticleSystemQuad* getParticleVFXPuckCollideWalls();
		cocos2d::ParticleSystemQuad* getParticleVFXPuckCollidePaddle(const cocos2d::Color4F& paddle_color);
	}
}