#ifndef JOYSTICKS_H
#define JOYSTICKS_H

#include <wx/string.h>
#include <SDL2/SDL.h>
#include <map>

class Joysticks;

// Define properties & functions for one joystick
class Joystick {
    public:
        Joystick(int Num);
        virtual ~Joystick();

        // All possible directions (maybe add JOY_UP_RIGHT,...)
        enum EJoystickDirections { JOY_NONE   = 0,
                                   JOY_LEFT   = -1, JOY_RIGHT   = -2,  JOY_UP   = -3,  JOY_DOWN   = -4,
                                   JOY_R_LEFT = -5, JOY_R_RIGHT = -6,  JOY_R_UP = -7,  JOY_R_DOWN = -8,
                                   JOY_H_LEFT = -9, JOY_H_RIGHT = -10, JOY_H_UP = -11, JOY_H_DOWN = -12 };

        static const int JOY_BUTTON_0 = -30;

        // Get Enum by name
        static Joystick::EJoystickDirections getDirectionFromStr(const wxString &direction);
        // Get name for Enum
        static wxString getDirectionStrFromCode(Joystick::EJoystickDirections direction);

        bool isOpened() { return _opened; }
        int getMyNum() { return _number; }
        int getThreshold() { return _threshold; }
        void setThreshold(int value) { _threshold = value; }

        // Get actual direction (for easy reading / editing in config file)
        Joystick::EJoystickDirections getDirection() { return _direction; }
        // Get last pressed direction
        Joystick::EJoystickDirections getPersistantDirection() { return _persistantDirection; }

        // Get enum name from Directions (for easy reading / editing in config file)
        wxString getDirectionStr();
        wxString getPersistantDirectionStr();

        // Get actual pressed button
        int getButton() { return _button; };
        // Get last pressed button
        int getPersistantButton() { return _persistantButton; }
        wxString getButtonStr();
        wxString getPersistantButtonStr();

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
        bool _opened;
        Joystick::EJoystickDirections _direction;
        Joystick::EJoystickDirections _persistantDirection;
        int _button = -1;
        int _persistantButton = -1;
        int _threshold = 8000;

        SDL_Joystick *_joy;
};

class Joysticks
{
    public:
        static Joysticks& getInstance();

        static bool HasJoy1() { return SDL_NumJoysticks() >= 1; }
        static bool HasJoy2() { return SDL_NumJoysticks() >= 2; }
        static bool HasJoysticks() { return SDL_NumJoysticks() > 0; }

        // Example: Convert "JOY_1_LEFT" to integer code (signed)
        static int getCodeFromString(const wxString &str);
        // Convert -109 to "JOY_1_H-LEFT" (signed)
        static wxString getStringFromCode(int code);
        // Convert joystick actual or persistant status to integer code (signed)
        int getCodeFromJoystick(Joystick *joy, bool persistant);
        // Check if the given code is part of actual (not persistant) joystick status
        bool checkCodeFromJoystick(Joystick *joy, int code);

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

        bool _isMoveEvent = false;
        bool _isButtonEvent = false;

        // Can be more than 2 with little modifications
        Joystick *_Joystick[2];

        /** Default constructor */
        Joysticks();
        /** Default destructor */
        virtual ~Joysticks();
};

#endif // JOYSTICKS_H
