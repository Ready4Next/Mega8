#ifndef MEGA8CONFiG_H
#define MEGA8CONFiG_H

#include <wx/config.h>
#include <wx/string.h>
// Keycodes definitions
#include <wx/defs.h>
#include "Joysticks.h"
#include "Chip8.h"

class Mega8Config
{
    public:
        static Mega8Config& getInstance();

        /** wxConfig */
        void resetConfig();
        void loadConfig(const wxString &profile);
        void reloadConfig(const wxString &profile);
        void saveConfig(const wxString &profile);
		void writeConfig();

        // Is the key related to joystick ?
        bool hasJoystick() { return _hasJoystick; }
        bool getKeyIsJoy(unsigned char OriginalKey) { return _Keys[OriginalKey] < 0; }
        int *getKeys() { return _Keys; }
        int getKey(unsigned char OriginalKey) { return (OriginalKey < 16) ? _Keys[OriginalKey]: 0; }
        void setKey(unsigned char OriginalKey, int NewKey) { if (OriginalKey < 16) _Keys[OriginalKey] = NewKey; }
        void resetKeyboard();

        wxString getLastFolder() { return _LastFolder; }
        void setLastFolder(const wxString &value) { _LastFolder = value; }
        bool getSound() { return _Sound; }
        void setSound(bool value) { _Sound = value; }
        bool getUseSleep() { return _UseSleep; }
        void setUseSleep(bool value) { _UseSleep = value; }
        bool getFullScreen() { return _FullScreen; }
        void setFullScreen(bool value) { _FullScreen = value; }
        bool getSpeedAuto() { return _SpeedAuto; }
        void setSpeedAuto(bool value) { _SpeedAuto = value; }
        bool getDisplayHUD() { return _DisplayHUD; }
        void setDisplayHUD(bool value) { _DisplayHUD = value; }
        bool getFiltered() { return _Filtered; }
        void setFiltered(bool value) { _Filtered = value; }
        bool getSyncClock() { return _SyncClock; }
        void setSyncClock(bool value) { _SyncClock = value; }
        long getFrequencyRatio(Chip8Types type) { return _FrequencyRatio[(int) type]; };
        void setFrequencyRatio(Chip8Types type, int value) { _FrequencyRatio[(int) type] = value; }
        Chip8ColorTheme getColorTheme() { return _ColorTheme; }
        void setColorTheme(Chip8ColorTheme value) { _ColorTheme = value; }
        bool getInverseColor() { return _InverseColor; }
        void setInverseColor(bool value) { _InverseColor = value; }
        wxConfig *getConfig() { return _config; }
    protected:
    private:

        Mega8Config& operator= (const Mega8Config&){}
        Mega8Config (const Mega8Config&){}
        // Instance
        static Mega8Config _Instance;

        // Properties
        wxString _LastFolder;
        wxString _currentProfile;
        int _Keys[16];
        bool _Sound;
        bool _UseSleep;
        bool _FullScreen;
        bool _SpeedAuto;
        bool _DisplayHUD;
        bool _Filtered;
        bool _SyncClock;
        bool _hasJoystick;
        long _FrequencyRatio[sizeof(Chip8Types) + 1];
        Chip8ColorTheme _ColorTheme;
        bool _InverseColor;

        wxConfig *_config;

        /** Default constructor */
        Mega8Config();
        /** Default destructor */
        virtual ~Mega8Config();

        // Private
        bool loadKeyboard(const wxString &profile);
        void saveKeyboard(const wxString &profile);
        wxString getMachineTypeStr(Chip8Types machineType);

        wxString readString(const wxString &KeyPath);
        bool readBool(const wxString &KeyPath);
        long readLong(const wxString &KeyPath);
        void writeString(const wxString &KeyPath, const wxString &value);
        void writeBool(const wxString &KeyPath, bool value);
        void writeLong(const wxString &KeyPath, long value);
};

#endif // MEGA8CONFiG_H
