#include "Mega8Config.h"
#include <wx/accel.h>
#include <wx/msgdlg.h>

// Init Instance
Mega8Config Mega8Config::_Instance = Mega8Config();

Mega8Config::Mega8Config()
{
    //ctor
    _config = NULL;
}

Mega8Config::~Mega8Config()
{
    //dtor
    if (_config) {
        delete _config;
        _config = NULL;
    }
}

Mega8Config &Mega8Config::getInstance()
{
    return _Instance;
}

void Mega8Config::resetKeyboard()
{
    // Keyboard
    setKey(0x0, WXK_NUMPAD0);
    setKey(0x1, WXK_NUMPAD7);
    setKey(0x2, WXK_NUMPAD8);
    setKey(0x3, WXK_NUMPAD9);
    setKey(0x4, WXK_NUMPAD4);
    setKey(0x5, WXK_NUMPAD5);
    setKey(0x6, WXK_NUMPAD6);
    setKey(0x7, WXK_NUMPAD1);
    setKey(0x8, WXK_NUMPAD2);
    setKey(0x9, WXK_NUMPAD3);
    setKey(0xA, WXK_NUMPAD_MULTIPLY);
    setKey(0xB, WXK_NUMPAD_ENTER);
    setKey(0xC, WXK_NUMPAD_DECIMAL);
    setKey(0xD, WXK_NUMPAD_ADD);
    setKey(0xE, WXK_NUMPAD_SUBTRACT);
    setKey(0xF, WXK_NUMPAD_DIVIDE);
}

void Mega8Config::resetConfig()
{
    _currentProfile = wxT("General");
    resetKeyboard();

    // Graphic
    _LastFolder = wxEmptyString;
    setColorTheme(DEFAULT);
    setSound(true);
    setUseSleep(true);
    setFullScreen(false);
    setSpeedAuto(true);
    setDisplayHUD(false);
    setFiltered(false);
    setSyncClock(true);
    setFrequencyRatio(CHIP8, 40);
    setFrequencyRatio(CHIP8_HiRes, 40);
    setFrequencyRatio(CHIP8_HiRes2, 40);
    setFrequencyRatio(SCHIP8, 80);
    setFrequencyRatio(MCHIP8, 640);
}

void Mega8Config::loadConfig(const wxString &profile)
{
    bool isNew;

    if (_config == NULL) {
        _config = new wxConfig(wxT("Mega8"), wxT("Ready4Next"));
        if (!readBool(wxT("FirstInit"))) {
            resetConfig();
            writeBool(wxT("FirstInit"), true);
            saveConfig(profile);
        }
    }
    _currentProfile = profile;
    isNew = loadKeyboard(profile);
    _LastFolder = readString(wxT("LastFolder"));

    _FullScreen = readBool(wxT("FullScreen"));
    _SpeedAuto = readBool(wxT("SpeedAuto"));
    _DisplayHUD = readBool(wxT("DisplayHUD"));
    _Filtered = readBool(wxT("Filtered"));
    _Sound = readBool(wxT("Sound"));
    _UseSleep = readBool(wxT("UseSleep"));
    _SyncClock = readBool(wxT("SyncClock"));
    _ColorTheme = (Chip8ColorTheme)readLong(wxT("ColorTheme"));
    _InverseColor = readBool(wxT("InverseColor"));
    for (int i = 0; i <= sizeof(Chip8Types); i++) {
        _FrequencyRatio[i] = readLong(wxT("FrequencyRatio/") + getMachineTypeStr((Chip8Types) i));

        // Removed 1024x
        _FrequencyRatio[i] = (_FrequencyRatio[i] == 10) ? 9 : _FrequencyRatio[i];
    }

    // Save this profile if new
    if (isNew) {
        saveConfig(profile);
    }
}

void Mega8Config::saveConfig(const wxString &profile)
{
    if (profile == wxEmptyString) {
        _currentProfile = wxT("General");
    } else {
        _currentProfile = profile;
    }

    saveKeyboard(_currentProfile);
    writeString(wxT("LastFolder"), _LastFolder);

    writeBool(wxT("FullScreen"), _FullScreen);
    writeBool(wxT("SpeedAuto"), _SpeedAuto);
    writeBool(wxT("DisplayHUD"), _DisplayHUD);
    writeBool(wxT("Filtered"), _Filtered);
    writeBool(wxT("SyncClock"), _SyncClock);
    writeLong(wxT("ColorTheme"), (int)_ColorTheme);
    writeBool(wxT("InverseColor"), _InverseColor);
    writeBool(wxT("Sound"), _Sound);
    writeBool(wxT("UseSleep"), _UseSleep);

    for (int i = 0; i <= sizeof(Chip8Types); i++) {
        writeLong(wxT("FrequencyRatio/") + getMachineTypeStr((Chip8Types)i), _FrequencyRatio[i]);
    }

    // Really save config
    if (_config != NULL) {
        reloadConfig(profile);
    }
}

void Mega8Config::reloadConfig(const wxString &profile) {
    // Saves config to disk
    delete _config;
    _config = new wxConfig(wxT("Mega8"), wxT("Ready4Next"));
    // Reload
    loadConfig(profile);
}

bool Mega8Config::loadKeyboard(const wxString &profile)
{
    bool isNew = false;
    wxString keyPath = wxEmptyString;
    wxString keyStr = wxEmptyString;
    wxAcceleratorEntry *ae = new wxAcceleratorEntry();

    _hasJoystick = false;
    for (int i =0; i < 16; i++) {
        keyPath << wxT("Keys/") << profile << wxT("/") << i;
        keyStr = readString(keyPath);
        // If no default key config for this profile found
        if (keyStr == wxEmptyString) {
            // Take from General
            isNew = true;
            keyPath = wxEmptyString;
            keyPath << wxT("Keys/") << wxT("General") << wxT("/") << i;
            keyStr = readString(keyPath);
        }

        // Convert this string representation to keycode or joystick code
        if (keyStr.Left(3).Upper() != wxT("JOY")) {
            // Can keycode be converted to string ?
            try {
                ae->FromString(keyStr);
            } catch (int e) {
                wxMessageBox(_("This key cannot be converted by wxAcceleratorEntry.\nI will fall back to default key configuration."), _("Key not supported"));
                resetKeyboard();
                return false;
            }

            // It's ok ? Are you sure ?
            if (!ae->IsOk()) {
                wxMessageBox(_("This key is not recognized.\nI will fall back to default key configuration."), _("Error reading key"));
                resetKeyboard();
                return false;
            }

            _Keys[i] = ae->GetKeyCode();
        } else {
            // Key joystick value
            _Keys[i] = Joysticks::getCodeFromString(keyStr);
            _hasJoystick = true;
        }
        keyPath = wxEmptyString;
    }

    delete ae;
    return isNew;
}

void Mega8Config::saveKeyboard(const wxString &profile)
{
    wxString keyPath = wxEmptyString;
    wxString key;
    wxAcceleratorEntry *ae = new wxAcceleratorEntry();

    for (int i =0; i < 16; i++) {
        keyPath << wxT("Keys/") << profile << wxT("/") << i;
        if (!getKeyIsJoy(i)) {
            ae->Set(wxACCEL_NORMAL, _Keys[i], 0);
            key = ae->ToString();
        } else {
            key = Joysticks::getStringFromCode(_Keys[i]);
        }
        writeString(keyPath, key);
        keyPath = wxEmptyString;
    }

    delete ae;
}

wxString Mega8Config::getMachineTypeStr(Chip8Types machineType)
{
    switch (machineType) {
        case CHIP8:
            return wxT("Chip-8");

        case SCHIP8:
            return wxT("SChip-8");

        case MCHIP8:
            return wxT("MChip-8");

        case CHIP8_HiRes:
        case CHIP8_HiRes2:
            return wxT("HiRes Chip-8");
    }

    // Never goes here
    return "Unknown";
}

wxString Mega8Config::readString(const wxString &KeyPath) {
    wxString result;

    if (_config->Read(KeyPath, &result)) {
        return result;
    } else {
        return wxEmptyString;
    }
}

bool Mega8Config::readBool(const wxString &KeyPath) {
    bool result = false;

    if (_config->Read(KeyPath, &result)) {
        return result;
    } else {
        return false;
    }
}

long Mega8Config::readLong(const wxString &KeyPath) {
    long result = 0;

    if (_config->Read(KeyPath, &result)) {
        return result;
    } else {
        return 0;
    }
}

void Mega8Config::writeString(const wxString &KeyPath, const wxString &value)
{
    _config->Write(KeyPath, value);
}

void Mega8Config::writeBool(const wxString &KeyPath, bool value)
{
    _config->Write(KeyPath, value);
}

void Mega8Config::writeLong(const wxString &KeyPath, long value)
{
    _config->Write(KeyPath, value);
}
