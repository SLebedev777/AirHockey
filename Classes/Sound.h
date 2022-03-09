#ifndef _AIRHOCKEY__SOUND_H__
#define _AIRHOCKEY__SOUND_H__

#include "cocos2d.h"
#include "cocos/ui/CocosGUI.h"

namespace airhockey
{
	namespace Sound
	{
		extern const char* AUDIOFILE_BUTTON_CLICK;
		extern const char* AUDIOFILE_BUZZ;
		extern const char* AUDIOFILE_COLLIDE_PUCK_PADDLE;
		extern const char* AUDIOFILE_COLLIDE_PUCK_WALLS;
		extern const char* AUDIOFILE_LOSE;
		extern const char* AUDIOFILE_ORGAN1;
		extern const char* AUDIOFILE_ORGAN2;
		extern const char* AUDIOFILE_ORGAN3;
		extern const char* AUDIOFILE_PUCK_FALL_GOAL;
		extern const char* AUDIOFILE_WIN;


		void onToggleAudioCallback(bool audio_enable);
		void uiButtonClickSoundCallback(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType event_type);
	}
}

#endif