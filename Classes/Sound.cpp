#include "Sound.h"
#include "audio/include/AudioEngine.h"

namespace airhockey
{
	namespace sound
	{
		USING_NS_CC;

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
				AudioEngine::play2d("sound/button_click.mp3", false, 0.5);
			}
		}
	}
}
