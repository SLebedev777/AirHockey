# AirHockey

Simple "AirHockey" cross-platform video game, made with Cocos2d-x engine.

Play vs AI to hit 7 goals to win the match!

## Supported platforms:
PC: Windows 8+

Mobile: Android (built against Android 11+, Android API 30+, but actually works okay on Android 8 and higher)

Download Win32 release: [msi](https://github.com/SLebedev777/AirHockey/raw/master/airhockey.msi) or [zip](https://github.com/SLebedev777/AirHockey/raw/master/airhockey.zip) 

## Some interesting features:
- Works correctly on variety of screen resolutions (game window is always vertical 16:9)
- Friendly input - use drag or tap/click for touch/mouse, or keyboard arrows/WASD. Input controller is auto-selected depending in platform
- AI:
	- Finite State Machine with states for attack, defense, and idle
	- different sub-strategies within attack state and defense state, depending on game objects properties
	- prediction where the puck will be to hit it to the gate
	- AI is able to target specific corner of player's gate when hitting targeted shot
	- "bricole" targeted shot from side 
	- very rarely stucks when the puck stops in some corner (well-known bug for many air hockey games)
- Chipmunk physics engine
- Particle VFX and sounds use physics information (particles direction and sound volume depend on objects' collision)
- Motion streak VFX for flying puck in main menu screen
