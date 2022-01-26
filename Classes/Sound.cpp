#include "Sound.h"
#include "audio/include/AudioEngine.h"

namespace airhockey
{
	namespace Sound
	{
		USING_NS_CC;

		const char* AUDIOFILE_BUTTON_CLICK = "sound/button_click.mp3";
		const char* AUDIOFILE_BUZZ = "sound/buzz.mp3";
		const char* AUDIOFILE_COLLIDE_PUCK_PADDLE = "sound/collide_puck_paddle3.mp3";
		const char* AUDIOFILE_COLLIDE_PUCK_WALLS = "sound/collide_puck_walls3.mp3";
		const char* AUDIOFILE_LOSE = "sound/lose.mp3";
		const char* AUDIOFILE_ORGAN1 = "sound/organ1.mp3";
		const char* AUDIOFILE_ORGAN2 = "sound/organ2.mp3";
		const char* AUDIOFILE_ORGAN3 = "sound/organ3.mp3";
		const char* AUDIOFILE_PUCK_FALL_GOAL = "sound/puck_fall_goal.mp3";
		const char* AUDIOFILE_WIN = "sound/win.mp3";


		void onToggleAudioCallback(bool audio_enable)
		{
			if (audio_enable)
			{
				AudioEngine::setEnabled(true);
			}
			else
			{
				AudioEngine::setEnabled(false);
			}
		}

		void uiButtonClickSoundCallback(cocos2d::Ref* ref, cocos2d::ui::Widget::TouchEventType event_type)
		{
			if (event_type == ui::Widget::TouchEventType::ENDED)
			{
				AudioEngine::play2d(AUDIOFILE_BUTTON_CLICK, false, 0.5);
			}
		}
	}
}
