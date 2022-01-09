#include "UISettings.h"
#include "Sound.h"

namespace airhockey
{
	const char* const FONT_FILENAME_MENU = "fonts/Abduction.ttf";
	const char* const FONT_FILENAME_GAME_HUD = "fonts/Abduction.ttf";

    USING_NS_CC;

	ui::Button* createUIButton(const std::string& title_text, bool fit_to_title)
	{
        auto button = ui::Button::create("HD/ui/button_frame.png", "HD/ui/button_frame.png");
        button->setScale9Enabled(true);
        Vec2 cap_inset_origin(25, 25);
        Size button_size = button->getContentSize();
        Rect cap_insets(cap_inset_origin, Size(button_size.width - 2 * cap_inset_origin.x, button_size.height - 2 * cap_inset_origin.y));
        button->setCapInsets(cap_insets);
        button->setTitleFontName(FONT_FILENAME_MENU);
        button->setTitleFontSize(50);
        button->setTitleText(title_text);
        auto fit_button_to_title = [](ui::Button* button, float coeff = 1.1f) {
            auto lbl_size = button->getTitleRenderer()->getContentSize();
            button->setContentSize(Size(lbl_size.width * coeff, lbl_size.height * coeff));
        };
        if (fit_to_title)
        {
            fit_button_to_title(button);
        }
        else
        {
            button->setContentSize(Size(550, 100));
        }
        // assign sound effect to event when button is clicked
        button->addTouchEventListener(airhockey::sound::uiButtonClickSoundCallback);
        return button;

	}
}
