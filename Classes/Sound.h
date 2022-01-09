#ifndef _AIRHOCKEY__SOUND_H__
#define _AIRHOCKEY__SOUND_H__

#include "cocos2d.h"
#include "cocos/ui/CocosGUI.h"

namespace airhockey
{
	namespace sound
	{
		void onToggleAudioCallback(bool audio_enable);
		void uiButtonClickSoundCallback(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType event_type);
	}
}

#endif