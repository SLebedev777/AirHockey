#ifndef __AIRHOCKEY_GLOBALSETTINGS_H__
#define __AIRHOCKEY_GLOBALSETTINGS_H__

#include "cocos2d.h"

namespace airhockey
{
	namespace GlobalSettings
	{
		extern bool isAudioEnabled;
		extern bool isVFXEnabled;
		extern cocos2d::Color4F paddle1Color;
		extern cocos2d::Color4F paddle2Color;
		extern cocos2d::Color4F puckColor;
		//extern GameFieldTheme gameFieldTheme;  // TODO
	}

}

#endif