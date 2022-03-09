#ifndef __GAMESCENE_VFX_H__
#define __GAMESCENE_VFX_H__

#include "cocos2d.h"

namespace airhockey
{
	namespace VFX
	{
		cocos2d::ParticleSystemQuad* getParticleVFXPuckCollideWalls();
		cocos2d::ParticleSystemQuad* getParticleVFXPuckCollidePaddle(const cocos2d::Color4F& paddle_color);
		cocos2d::ParticleSystemQuad* getParticleVFXGoalHit(bool is_upper_goal, float goal_width, const cocos2d::Color4F& color);
	}
}

#endif