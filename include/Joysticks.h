#ifndef JOYSTICKS_H
#define JOYSTICKS_H

#include <stdlib.h>
#include <wx/string.h>
#ifdef WIN32
	#include <SDL.h>
	#include <SDL_haptic.h>
#else
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_haptic.h>
#endif
#include <map>

class Joysticks;

// Define properties & functions for one joystick
class Joystick {
    public:
        Joystick(int Num);
        virtual ~Joystick();

        // All possible directions (maybe add JOY_UP_RIGHT,...)
        enum EJoystickDirections { JOY_NONE = 0,
								   JOY_LEFT   = 2, JOY_RIGHT   = 4,  JOY_UP   = 6,  JOY_DOWN   = 8,
                                   JOY_R_LEFT = 10, JOY_R_RIGHT = 12,  JOY_R_UP = 14,  JOY_R_DOWN = 16,
                                   JOY_H_LEFT = 18, JOY_H_RIGHT = 20, JOY_H_UP = 22, JOY_H_DOWN = 24,
									
								   JOY_UP_LEFT   = 26, JOY_UP_RIGHT   = 28,  JOY_DOWN_LEFT   = 30,  JOY_DOWN_RIGHT   = 32,
                                   JOY_R_UP_LEFT = 34, JOY_R_UP_RIGHT = 36,  JOY_R_DOWN_LEFT = 38,  JOY_R_DOWN_RIGHT = 40,
                                   JOY_H_UP_LEFT = 42, JOY_H_UP_RIGHT = 44, JOY_H_DOWN_LEFT = 46, JOY_H_DOWN_RIGHT = 48,
								 };

        static const int JOY_BUTTON_0_SHIFT = 7;

        // Get Enum by name
        static Joystick::EJoystickDirections getDirectionCodeFromStr(const wxString &direction);
        // Get name for Enum
        static wxString getDirectionStrFromCode(Joystick::EJoystickDirections direction);
		static long getButtonCodeFromStr(const wxString &buttonStr);
		static wxString getButtonStrFromCode(long buttonCode);

        bool isOpened() { return _opened; }
        int getMyNum() { return _number; }
        int getThreshold() { return _threshold; }
        void setThreshold(int value) { _threshold = value; }

        // Get actual direction (for easy reading / editing in config file)
        Joystick::EJoystickDirections getDirection() { return _direction; }
        // Get last pressed direction
        Joystick::EJoystickDirections getPersistantDirection() { return _persistantDirection; }

		// Get Joy InstanceID
		int getInstanceID() { return _instance; }
		// Rumble ?
		bool hasRumble() { return _hasRumble; }
        void playRumble(float strength, int milli);
		// Get enum name from Directions (for easy reading / editing in config file)
        wxString getDirectionStr();
        wxString getPersistantDirectionStr();

        // Get actual pressed buttons
        long getButton() { return _button; };
        // Get last pressed button
        long getPersistantButton() { return _persistantButton; }
        wxString getButtonStr();
        wxString getPersistantButtonStr();
		int getMaxButtons() { if (_joy) return SDL_JoystickNumButtons(_joy); else return 0; }

        // Update status from SDLEvent
        void updateDirections(SDL_Event &evt);
        void updateButton(SDL_Event &evt);

        // Close joystick
        void close();
        // Get info string from joystick
        wxString toString();
    protected:
    private:
        int _number;
		int _instance;
        bool _opened;
        Joystick::EJoystickDirections _direction;
        Joystick::EJoystickDirections _persistantDirection;
        long _button;
        long _persistantButton;
        int _threshold;
		bool _hasRumble;

        SDL_Joystick *_joy;
		SDL_Haptic *_hap;
};

class Joysticks
{
    public:
        static Joysticks& getInstance();

        static bool HasJoy1() { return SDL_NumJoysticks() >= 1; }
        static bool HasJoy2() { return SDL_NumJoysticks() >= 2; }
        static bool HasJoysticks() { return SDL_NumJoysticks() > 0; }

        // Example: Convert "JOY_1_LEFT" to integer code (signed)
        static long getCodeFromString(const wxString &str);
        // Convert -109 to "JOY_1_H-LEFT" (signed)
        static wxString getStringFromCode(long code);

        // Convert joystick actual or persistant status to integer code (signed)
        long getCodeFromJoystick(Joystick *joy, bool persistant);
        // Check if the given code is part of actual (not persistant) joystick status
        bool checkCodeFromJoystick(Joystick *joy, long code);

        void initJoysticks();
        void close();
        bool IsMoveEvent() { return _isMoveEvent; }
        bool IsButtonEvent() { return _isButtonEvent; }

        Joystick *getJoy1() {return _Joystick[0]; }
        Joystick *getJoy2() {return _Joystick[1]; }

        // Update all joysticks status
        Joystick *updateJoyState();

    protected:

    private:

        Joysticks& operator= (const Joysticks&){}
        Joysticks (const Joysticks&){}
        // Instance
        static Joysticks _Instance;

        bool _isMoveEvent;
        bool _isButtonEvent;

        // Can be more than 2 with little modifications
        Joystick *_Joystick[2];

        /** Default constructor */
        Joysticks();
        /** Default destructor */
        virtual ~Joysticks();
};

#endif // JOYSTICKS_H
