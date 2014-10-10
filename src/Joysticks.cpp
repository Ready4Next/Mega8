#include "Joysticks.h"
#include <wx/tokenzr.h>

/************************* Class Joystick  **************************/

Joystick::Joystick(int Num)
{
    _number = Num;
    _joy = SDL_JoystickOpen(_number);
	_instance = SDL_JoystickInstanceID(_joy);

	// Rumble
	_hasRumble = false;
	if (SDL_JoystickIsHaptic(_joy)) {
		_hap = SDL_HapticOpenFromJoystick(_joy);
		if (_hap) {
			_hasRumble = SDL_HapticRumbleInit(_hap) == 0;
		}
	}

    _opened = (_joy != NULL);
	_button = 0;
    _persistantButton = 0;
    _threshold = 8000;
}

Joystick::~Joystick()
{
    //
}

void Joystick::close()
{
    if (_opened && _joy) {
		if(_hasRumble && _hap)
        {
            SDL_HapticClose(_hap);
            _hap = NULL;
			_hasRumble = false;
        }
        SDL_JoystickClose(_joy);
		_joy = NULL;
		_opened = false;
    }
}

void Joystick::updateDirections(SDL_Event &evt)
{
    // Is it for me ?
    if (evt.jaxis.which == _number) {
        switch(evt.type) {
            // Check X-Y and Right Stick
            case SDL_JOYAXISMOTION:
                switch(evt.jaxis.axis) {
                    case 0:  // X-Axis - Check Threshold
                       // _direction = Joystick::EJoystickDirections::JOY_NONE;
                        if ((evt.jaxis.value > (0-_threshold)) && (evt.jaxis.value < _threshold)) {
                            // No movement
                        } else {
                            if (evt.jaxis.value < 0) {
                                _direction = Joystick::EJoystickDirections::JOY_LEFT;
                                if (_persistantDirection != _direction)  {
                                    _persistantDirection = _direction;
                                }
                            } else {
                                _direction = Joystick::EJoystickDirections::JOY_RIGHT;
                                if (_persistantDirection != _direction)  {
                                    _persistantDirection = _direction;
                                }
                            }
                        }
                        break;

                    case 1: // Y-Axis
                        //_direction = Joystick::EJoystickDirections::JOY_NONE;
                        if ((evt.jaxis.value > (0-_threshold)) && (evt.jaxis.value < _threshold)) {
                            // No movement
                        } else {
                            if (evt.jaxis.value < 0) {
                                _direction = Joystick::EJoystickDirections::JOY_UP;
                                if (_persistantDirection != _direction)  {
                                    _persistantDirection = _direction;
                                }
                            } else {
                                _direction = Joystick::EJoystickDirections::JOY_DOWN;
                                if (_persistantDirection != _direction)  {
                                    _persistantDirection = _direction;
                                }
                            }
                        }
                        break;
                    case 2: // Second stick left/right
                        //_direction = Joystick::EJoystickDirections::JOY_NONE;
                        if ((evt.jaxis.value > (0-_threshold)) && (evt.jaxis.value < _threshold)) {
                            // No movement
                        } else {
                            if (evt.jaxis.value < 0) {
                                _direction = Joystick::EJoystickDirections::JOY_R_LEFT;
                                if (_persistantDirection != _direction)  {
                                    _persistantDirection = _direction;
                                }
                            } else {
                                _direction = Joystick::EJoystickDirections::JOY_R_RIGHT;
                                if (_persistantDirection != _direction)  {
                                    _persistantDirection = _direction;
                                }
                            }
                        }
                        break;
                    case 3: // Second srick up/down
                        //_direction = Joystick::EJoystickDirections::JOY_NONE;
                        if ((evt.jaxis.value > (0-_threshold)) && (evt.jaxis.value < _threshold)) {
                            // No movement
                        } else {
                            if (evt.jaxis.value < 0) {
                                _direction = Joystick::EJoystickDirections::JOY_R_UP;
                                if (_persistantDirection != _direction)  {
                                    _persistantDirection = _direction;
                                }
                            } else {
                                _direction = Joystick::EJoystickDirections::JOY_R_DOWN;
                                if (_persistantDirection != _direction)  {
                                    _persistantDirection = _direction;
                                }
                            }
                        }
                        break;
                }
                break;

            case SDL_JOYHATMOTION:
                // Check left stick
                //_direction = Joystick::EJoystickDirections::JOY_NONE;
                switch (evt.jhat.value) {
                    case SDL_HAT_CENTERED:
                        _direction = Joystick::EJoystickDirections::JOY_NONE;
                        break;
                    case SDL_HAT_DOWN:
                        _direction = Joystick::EJoystickDirections::JOY_H_DOWN;
                        if (_persistantDirection != _direction)  {
                            _persistantDirection = _direction;
                        }
                        break;
                    case SDL_HAT_UP:
                        _direction = Joystick::EJoystickDirections::JOY_H_UP;
                        if (_persistantDirection != _direction)  {
                            _persistantDirection = _direction;
                        }
                        break;
                    case SDL_HAT_LEFT:
                        _direction = Joystick::EJoystickDirections::JOY_H_LEFT;
                        if (_persistantDirection != _direction)  {
                            _persistantDirection = _direction;
                        }
                        break;
                    case SDL_HAT_RIGHT:
                        _direction = Joystick::EJoystickDirections::JOY_H_RIGHT;
                        if (_persistantDirection != _direction)  {
                            _persistantDirection = _direction;
                        }
                        break;

					// Angles
					case SDL_HAT_LEFTDOWN:
                        _direction = Joystick::EJoystickDirections::JOY_H_DOWN_LEFT;
                        if (_persistantDirection != _direction)  {
                            _persistantDirection = _direction;
                        }
                        break;
                    case SDL_HAT_LEFTUP:
                        _direction = Joystick::EJoystickDirections::JOY_H_UP_LEFT;
                        if (_persistantDirection != _direction)  {
                            _persistantDirection = _direction;
                        }
                        break;
                    case SDL_HAT_RIGHTUP:
                        _direction = Joystick::EJoystickDirections::JOY_H_UP_RIGHT;
                        if (_persistantDirection != _direction)  {
                            _persistantDirection = _direction;
                        }
                        break;
                    case SDL_HAT_RIGHTDOWN:
                        _direction = Joystick::EJoystickDirections::JOY_H_DOWN_RIGHT;
                        if (_persistantDirection != _direction)  {
                            _persistantDirection = _direction;
                        }
                        break;
                }
                break;
        }
    }
}

void Joystick::updateButton(SDL_Event &evt)
{
    if (evt.jbutton.which == _number) {
        if (evt.type == SDL_JOYBUTTONDOWN) {
            _button |= (1 << evt.jbutton.button);
            if (_persistantButton != _button) {
                _persistantButton = _button;
            }
        } else if (evt.type == SDL_JOYBUTTONUP) {
            _button &= ~(1 << evt.jbutton.button);
        }
    }
}

// Get actual pressed direction
wxString Joystick::getDirectionStr()
{
	return getDirectionStrFromCode(_direction);
}

// Get last pressed direction
wxString Joystick::getPersistantDirectionStr()
{
	return getDirectionStrFromCode(_persistantDirection);
}

// Translate code to string
wxString Joystick::getDirectionStrFromCode(Joystick::EJoystickDirections direction)
{
    std::map<Joystick::EJoystickDirections, wxString> m;
    m[Joystick::EJoystickDirections::JOY_LEFT] = wxT("LEFT");
    m[Joystick::EJoystickDirections::JOY_RIGHT] =  wxT("RIGHT");
    m[Joystick::EJoystickDirections::JOY_UP] =  wxT("UP");
    m[Joystick::EJoystickDirections::JOY_DOWN] =  wxT("DOWN");

    m[Joystick::EJoystickDirections::JOY_R_LEFT] =  wxT("R-LEFT");
    m[Joystick::EJoystickDirections::JOY_R_RIGHT] =  wxT("R-RIGHT");
    m[Joystick::EJoystickDirections::JOY_R_UP] =  wxT("R-UP");
    m[Joystick::EJoystickDirections::JOY_R_DOWN] =  wxT("R-DOWN");

    m[Joystick::EJoystickDirections::JOY_H_LEFT] =  wxT("H-LEFT");
    m[Joystick::EJoystickDirections::JOY_H_RIGHT] =  wxT("H-RIGHT");
    m[Joystick::EJoystickDirections::JOY_H_UP] = wxT("H-UP");
    m[Joystick::EJoystickDirections::JOY_H_DOWN] = wxT("H-DOWN");

	m[Joystick::EJoystickDirections::JOY_H_UP_LEFT] =  wxT("H-UP-LEFT");
    m[Joystick::EJoystickDirections::JOY_H_UP_RIGHT] =  wxT("H-UP-RIGHT");
    m[Joystick::EJoystickDirections::JOY_H_DOWN_LEFT] = wxT("H-DOWN-LEFT");
	m[Joystick::EJoystickDirections::JOY_H_DOWN_RIGHT] = wxT("H-DOWN-RIGHT");

    return m[direction];
}

Joystick::EJoystickDirections Joystick::getDirectionCodeFromStr(const wxString &direction)
{
    std::map<wxString, Joystick::EJoystickDirections> m;
    m[wxT("LEFT")] = Joystick::EJoystickDirections::JOY_LEFT;
    m[wxT("RIGHT")] =  Joystick::EJoystickDirections::JOY_RIGHT;
    m[wxT("UP")] =  Joystick::EJoystickDirections::JOY_UP;
    m[wxT("DOWN")] =  Joystick::EJoystickDirections::JOY_DOWN;

    m[wxT("R-LEFT")] =  Joystick::EJoystickDirections::JOY_R_LEFT;
    m[wxT("R-RIGHT")] =  Joystick::EJoystickDirections::JOY_R_RIGHT;
    m[wxT("R-UP")] =  Joystick::EJoystickDirections::JOY_R_UP;
    m[wxT("R-DOWN")] =  Joystick::EJoystickDirections::JOY_R_DOWN;

    m[wxT("H-LEFT")] =  Joystick::EJoystickDirections::JOY_H_LEFT;
    m[wxT("H-RIGHT")] =  Joystick::EJoystickDirections::JOY_H_RIGHT;
    m[wxT("H-UP")] =  Joystick::EJoystickDirections::JOY_H_UP;
    m[wxT("H-DOWN")] =  Joystick::EJoystickDirections::JOY_H_DOWN;

	m[wxT("H-UP-LEFT")] =  Joystick::EJoystickDirections::JOY_H_UP_LEFT;
    m[wxT("H-UP-RIGHT")] =  Joystick::EJoystickDirections::JOY_H_UP_RIGHT;
    m[wxT("H-DOWN-LEFT")] =  Joystick::EJoystickDirections::JOY_H_DOWN_LEFT;
    m[wxT("H-DOWN-RIGHT")] =  Joystick::EJoystickDirections::JOY_H_DOWN_RIGHT;

    return m[direction.Upper()];
}

// Button are mapped to bits now
wxString Joystick::getButtonStr()
{
	return Joystick::getButtonStrFromCode(_persistantButton);
}

wxString Joystick::getPersistantButtonStr()
{
	return Joystick::getButtonStrFromCode(_persistantButton);
}

// Static
long Joystick::getButtonCodeFromStr(const wxString &buttonStr) 
{
	long result = 0;

	if (buttonStr != wxEmptyString) {
		wxString btnStr = buttonStr;
		btnStr.Remove(0, 6);

		if (btnStr.Left(4).Upper() == "BTN_") {
			// BTN_
			wxStringTokenizer buttonTokenizer(btnStr, "+");
			while ( buttonTokenizer.HasMoreTokens() )
			{
				wxString token = buttonTokenizer.GetNextToken();
				// BTN_x
				wxStringTokenizer buttonNumTokenizer(token, "_");
				buttonNumTokenizer.GetNextToken();
				long btnNum = 0;
				buttonNumTokenizer.GetNextToken().ToLong(&btnNum);
				result |= (1 << btnNum);
			}
		} 
	}

	return result;
}

// Static 
wxString Joystick::getButtonStrFromCode(long buttonCode)
{
	wxString result = wxEmptyString;
	wxString glue = wxEmptyString;

	int i = 0;
	while (buttonCode > 0) {
		if ((buttonCode & 1) == 1) {
			result.Printf(wxT("%s%sBTN_%d"), result, glue, i);
			glue = "+";
		}
		buttonCode >>= 1;
		i++;
	}

    return result;
}

wxString Joystick::toString()
{
    wxString lbl = wxEmptyString;
    if (_opened) {
        lbl.Printf(wxT("Joystick %d: %s (%d buttons / %d axes / %d balls / %d hats)"),
                                            _number,
                                            SDL_JoystickName(_joy),
                                            SDL_JoystickNumButtons(_joy),
                                            SDL_JoystickNumAxes(_joy),
                                            SDL_JoystickNumBalls(_joy),
                                            SDL_JoystickNumHats(_joy));
    } else {
        lbl.Printf(wxT("Joystick %d not detected"), _number);
    }
    return lbl;
}

/************************* Class Joysticks **************************/

// Init Instance
Joysticks Joysticks::_Instance = Joysticks();

Joysticks::Joysticks()
{
    //ctor
    Joysticks::_Joystick[0] = NULL;
    Joysticks::_Joystick[1] = NULL;
	_isMoveEvent = false;
	_isButtonEvent = false;
}

Joysticks::~Joysticks()
{
    //dtor
}

void Joysticks::close()
{
    if (Joysticks::_Joystick[0]) {
        _Joystick[0]->close();
        delete Joysticks::_Joystick[0];
        Joysticks::_Joystick[0] = NULL;
    }

    if (Joysticks::_Joystick[1]) {
        _Joystick[1]->close();
        delete Joysticks::_Joystick[1];
        Joysticks::_Joystick[1] = NULL;
    }
}

Joysticks &Joysticks::getInstance()
{
    return _Instance;
}

void Joysticks::initJoysticks() {
    if (HasJoy1() && Joysticks::_Joystick[0] == NULL) {
        Joysticks::_Joystick[0] = new Joystick(0);
    }

    if (HasJoy2() && _Joystick[1] == NULL) {
        Joysticks::_Joystick[1] = new Joystick(1);
    }
}

long Joysticks::getCodeFromJoystick(Joystick *joy, bool persistant = false)
{
    int result = 0;

    if (joy) {

		// First the joystick number - Limit to 6
		result = joy->getMyNum() & 1;

        if (IsMoveEvent()) {
            result += ((persistant) ? joy->getPersistantDirection() : joy->getDirection());
        } else if (IsButtonEvent()) {
			result += ((persistant) ? joy->getPersistantButton() : joy->getButton()) << Joystick::JOY_BUTTON_0_SHIFT;
        }
    }

	// Return a negative value to differ from keyboard code
    return result * -1;
}

// Check if the code is in current state of the joystick
bool Joysticks::checkCodeFromJoystick(Joystick *joy, long code)
{
    int joyNum;

    // Get the joystick number
	code = abs(code);
    joyNum = (int)code & 0x01;
	code -= joyNum;
    
    // Check if it's the correct joystick
    if (joy->getMyNum() == joyNum) {
        // Get button state from joystick
		long btnCode = code >> Joystick::JOY_BUTTON_0_SHIFT;
		if (btnCode > 0) {
            // The code is in joy button state ?
			if (joy->getButton() == btnCode) {
                return true;
            }
        } else {
            // Get directional state from joystick
            if ( code == (int)joy->getDirection()) {
                return true;
            }
        }
    }
    // No keys
    return false;
}

long Joysticks::getCodeFromString(const wxString &str)
{
    long result = 0;

    if (str != wxEmptyString && str.Left(3).Upper() == "JOY") {
        wxStringTokenizer tokenizer(str, "_");
        wxString token = tokenizer.GetNextToken();
        token = tokenizer.GetNextToken();
        int joyNum = wxAtoi(token);
        token = tokenizer.GetNextToken();
        if (token.Upper() == "BTN") {
			long btn = Joystick::getButtonCodeFromStr(str) << Joystick::JOY_BUTTON_0_SHIFT;
            result = btn + joyNum;
        } else {
			result = Joystick::getDirectionCodeFromStr(token) + joyNum;
        }
    }

	// Return a negative value to differ from keyboard codes
    return result * -1;
}

wxString Joysticks::getStringFromCode(long code)
{
    wxString result = wxEmptyString;
    int joyNum;

    // Get the joystick number
	code = abs(code);
    joyNum = code & 0x01;
	code -= joyNum;
	long btnCode = code >> Joystick::JOY_BUTTON_0_SHIFT;
    if (btnCode > 0) {
		result.Printf("JOY_%d_%s", joyNum, Joystick::getButtonStrFromCode(btnCode));
    } else {
        result.Printf("JOY_%d_%s", joyNum, Joystick::getDirectionStrFromCode((Joystick::EJoystickDirections) code));
    }

    return result;
}

void Joystick::playRumble(float strength, int milli) {
	if (_hasRumble && _hap && strength < 1 && strength > 0)
		if (SDL_HapticRumblePlay(_hap, strength, milli) != 0) {
            printf( "Warning: Unable to play rumble! %s\n", SDL_GetError() );
        }
}

// Return the joystick who generated the event
Joystick *Joysticks::updateJoyState() {
    Joystick *result = NULL;
    SDL_Event evt;
    wxString keyStr;

    if (SDL_PollEvent(&evt) != 0) {
        _isMoveEvent = false;
        _isButtonEvent = false;
        switch (evt.type) {
            case SDL_JOYAXISMOTION:
            case SDL_JOYHATMOTION:
                // Which joystick ?
                if (HasJoy1()) {
                    Joysticks::_Joystick[0]->updateDirections(evt);
                    result = Joysticks::_Joystick[0];
                }

                if (HasJoy2()) {
                    Joysticks::_Joystick[1]->updateDirections(evt);
                    result = Joysticks::_Joystick[1];
                }
                _isMoveEvent = true;
                break;

            case SDL_JOYBUTTONDOWN:
            case SDL_JOYBUTTONUP:
                // Which joystick ?
                if (HasJoy1()) {
                    Joysticks::_Joystick[0]->updateButton(evt);
                    result = Joysticks::_Joystick[0];
                }

                if (HasJoy2()) {
                    Joysticks::_Joystick[1]->updateButton(evt);
                    result = Joysticks::_Joystick[1];
                }

				result->playRumble(0.75, 500);

                _isButtonEvent = true;
                break;

            case SDL_JOYDEVICEADDED:
                /*if (evt.jdevice.which == 0) {
                    if (Joysticks::_Joystick[0] == NULL) {
                        Joysticks::_Joystick[0] = new Joystick(evt.cdevice.which);
                    }
                } else if (evt.jdevice.which == 1) {
                    if (Joysticks::_Joystick[1] == NULL) {
                        Joysticks::_Joystick[1] = new Joystick(evt.cdevice.which);
                    }
                }*/
                // Close All
                Joysticks::close();
                //Reinit
                Joysticks::initJoysticks();
                break;

            case SDL_JOYDEVICEREMOVED:
                /*if (evt.jdevice.which == 0) {
                    Joysticks::_Joystick[0]->close();
                    delete Joysticks::_Joystick[0];
                    Joysticks::_Joystick[0] = NULL;
                } else if (evt.jdevice.which == 1) {
                    Joysticks::_Joystick[0]->close();
                    delete Joysticks::_Joystick[1];
                    Joysticks::_Joystick[1] = NULL;
                }*/
                // Close All
                Joysticks::close();
                //Reinit
                Joysticks::initJoysticks();
                break;
        }
    }

    // Return active joystick
    return result;
}
