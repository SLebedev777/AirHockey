#ifndef __UISETTINGS_H__
#define __UISETTINGS_H__

#include "cocos2d.h"
#include "cocos/ui/CocosGUI.h"

namespace airhockey
{
	extern const char* const FONT_FILENAME_MENU;
	extern const char* const FONT_FILENAME_GAME_HUD;

	cocos2d::ui::Button* createUIButton(const std::string& title_text, bool fit_to_title = false);
}

#endif  // __UISETTINGS_H__
