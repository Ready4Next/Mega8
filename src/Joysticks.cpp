#include "Joysticks.h"
#include <wx/tokenzr.h>

/************************* Class Joystick  **************************/

Joystick::Joystick(int Num)
{
    _number = Num;
    _joy = SDL_JoystickOpen(_number);
    _opened = (_joy != NULL);
}

Joystick::~Joystick()
{
    //
}

void Joystick::close()
{
    if (_opened && _joy) {
        SDL_JoystickClose(_joy);
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
                }
                break;
        }
    }
}

void Joystick::updateButton(SDL_Event &evt)
{
    // Is it for me ?
    _button = -1;
    if (evt.jbutton.which == _number) {
        if (evt.type == SDL_JOYBUTTONDOWN) {
            _button = evt.jbutton.button;
            if (_persistantButton != _button) {
                _persistantButton = _button;
            }
        } else if (evt.type == SDL_JOYBUTTONUP) {
            _button = -1;
        }
    }
}

wxString Joystick::getDirectionStr()
{
    wxString result = wxEmptyString;

    switch (_direction) {
        // Pad
        case Joystick::EJoystickDirections::JOY_DOWN:
            result = wxT("DOWN");
            break;
        case Joystick::EJoystickDirections::JOY_UP:
            result = wxT("UP");
            break;
        case Joystick::EJoystickDirections::JOY_LEFT:
            result = wxT("LEFT");
            break;
        case Joystick::EJoystickDirections::JOY_RIGHT:
            result = wxT("RIGHT");
            break;

        // Left Stick
        case Joystick::EJoystickDirections::JOY_H_DOWN:
            result = wxT("H-DOWN");
            break;
        case Joystick::EJoystickDirections::JOY_H_UP:
            result = wxT("H-UP");
            break;
        case Joystick::EJoystickDirections::JOY_H_LEFT:
            result = wxT("H-LEFT");
            break;
        case Joystick::EJoystickDirections::JOY_H_RIGHT:
            result = wxT("H-RIGHT");
            break;

        // Right Stick
        case Joystick::EJoystickDirections::JOY_R_DOWN:
            result = wxT("R-DOWN");
            break;
        case Joystick::EJoystickDirections::JOY_R_UP:
            result = wxT("R-UP");
            break;
        case Joystick::EJoystickDirections::JOY_R_LEFT:
            result = wxT("R-LEFT");
            break;
        case Joystick::EJoystickDirections::JOY_R_RIGHT:
            result = wxT("R-RIGHT");
            break;
    }

    return result;
}

wxString Joystick::getPersistantDirectionStr()
{
    wxString result = wxEmptyString;

    switch (_persistantDirection) {
        // Pad
        case Joystick::EJoystickDirections::JOY_DOWN:
            result = wxT("DOWN");
            break;
        case Joystick::EJoystickDirections::JOY_UP:
            result = wxT("UP");
            break;
        case Joystick::EJoystickDirections::JOY_LEFT:
            result = wxT("LEFT");
            break;
        case Joystick::EJoystickDirections::JOY_RIGHT:
            result = wxT("RIGHT");
            break;

        // Left Stick
        case Joystick::EJoystickDirections::JOY_H_DOWN:
            result = wxT("H-DOWN");
            break;
        case Joystick::EJoystickDirections::JOY_H_UP:
            result = wxT("H-UP");
            break;
        case Joystick::EJoystickDirections::JOY_H_LEFT:
            result = wxT("H-LEFT");
            break;
        case Joystick::EJoystickDirections::JOY_H_RIGHT:
            result = wxT("H-RIGHT");
            break;

        // Right Stick
        case Joystick::EJoystickDirections::JOY_R_DOWN:
            result = wxT("R-DOWN");
            break;
        case Joystick::EJoystickDirections::JOY_R_UP:
            result = wxT("R-UP");
            break;
        case Joystick::EJoystickDirections::JOY_R_LEFT:
            result = wxT("R-LEFT");
            break;
        case Joystick::EJoystickDirections::JOY_R_RIGHT:
            result = wxT("R-RIGHT");
            break;
    }

    return result;
}

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

    return m[direction];
}

Joystick::EJoystickDirections Joystick::getDirectionFromStr(const wxString &direction)
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

    return m[direction.Upper()];
}

wxString Joystick::getButtonStr()
{
    wxString result = wxEmptyString;

    if (_button != -1) {
        result.Printf(wxT("BTN_%d"), _button);
    }
    return result;
}

wxString Joystick::getPersistantButtonStr()
{
    wxString result = wxEmptyString;

    if (_persistantButton != -1) {
        result.Printf(wxT("BTN_%d"), _persistantButton);
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

int Joysticks::getCodeFromJoystick(Joystick *joy, bool persistant = false)
{
    int result = -1;

    if (joy) {
        if (IsMoveEvent()) {
            result = ((persistant) ? joy->getPersistantDirection() : joy->getDirection()) - (joy->getMyNum() * 100);
        } else if (IsButtonEvent()) {
            result = Joystick::JOY_BUTTON_0 - ((persistant) ? joy->getPersistantButton() : joy->getButton()) - (joy->getMyNum() * 100);
        }
    }

    return result;
}

// Check if the code is in current state of the joystick
bool Joysticks::checkCodeFromJoystick(Joystick *joy, int code)
{
    int joyNum;

    // Get the joystick number
    joyNum = (int)abs(floor(code / 100));
    code += joyNum * 100;

    // Check if it's the correct joystick
    if (joy->getMyNum() == joyNum) {
        // Get button state from joystick
        if (code <= Joystick::JOY_BUTTON_0) {
            // The code is in joy button state ?
            if (joy->getButton() == abs(code - Joystick::JOY_BUTTON_0)) {
                return true;
            }
        } else if (code < 0) {
            // Get directional state from joystick
            if ( code == (int)joy->getDirection()) {
                return true;
            }
        }
    }
    // No keys
    return false;
}

int Joysticks::getCodeFromString(const wxString &str)
{
    int result = -1;

    if (str != wxEmptyString && str.Left(3).Upper() == "JOY") {
        wxStringTokenizer tokenizer(str, "_");
        wxString token = tokenizer.GetNextToken();
        token = tokenizer.GetNextToken();
        int joyNum = wxAtoi(token);
        token = tokenizer.GetNextToken();
        if (token.Upper() == "BTN") {
            int btn = Joystick::JOY_BUTTON_0 - wxAtoi(tokenizer.GetNextToken());
            result = btn - (joyNum * 100);
        } else {
            result = Joystick::getDirectionFromStr(token) - (joyNum * 100);
        }
    }

    return result;
}

wxString Joysticks::getStringFromCode(int code)
{
    wxString result = wxEmptyString;
    int joyNum;

    // Get the joystick number
    joyNum = (int)abs(floor(code / 100));
    code += joyNum * 100;
    if (code <= Joystick::JOY_BUTTON_0) {
        int btn = abs(code - Joystick::JOY_BUTTON_0);
        result.Printf("JOY_%d_BTN_%d", joyNum, btn);
    } else {
        result.Printf("JOY_%d_%s", joyNum, Joystick::getDirectionStrFromCode((Joystick::EJoystickDirections) code));
    }

    return result;
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
