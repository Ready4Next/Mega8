/***************************************************************
 * Name:      Mega8Main.h
 * Purpose:   Defines Application Frame
 * Author:    Junta (junta@hotmail.be)
 * Created:   2014-07-24
 * Copyright: Junta (http://www.ready4next.be)
 * License:
 **************************************************************/

#ifndef Mega8MAIN_H
#define Mega8MAIN_H

//(*Headers(Mega8Frame)
#include <wx/menu.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
//*)

#ifdef WIN32
	#include <SDL.h>
	#include <SDL_mixer.h>
#else
	#include <SDL2/SDL.h>
	#include <SDL2/SDL_mixer.h>
#endif

#include <wx/wx.h>
#include <wx/string.h>
#include <iostream>     // std::cout, std::fixed
#include <iomanip>      // std::setprecision
#include <math.h>

#include "Chip8.h"
#include "Chip8GL.h"
#include "Mega8Config.h"
#include "InputDialog.h"

// declare a new type of event, to be used by our CpuThread class:
wxDECLARE_EVENT(wxEVT_COMMAND_CPUTHREAD_COMPLETED, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_CPUTHREAD_UPDATE, wxThreadEvent);

const float FREQ_MENU_INDEX[] = { 1, 2, 10, 40, 80, 160, 320, 640, 1280, 10240 }; // , 40960 1024x Causes problems in Windows
const int   SIZE_MENU_INDEX[] = { 1, 2, 3, 4, 5, 10 };
const int   BEEP_CHANNEL = 0;

class Mega8Frame;

class CPUThreadHandler: public wxThread
{
public:
    CPUThreadHandler(Mega8Frame *handler): wxThread(wxTHREAD_DETACHED) { _handler = handler; }
    ~CPUThreadHandler();
protected:
    virtual ExitCode Entry();
    Mega8Frame *_handler;
};

class Mega8Frame: public wxFrame
{
    friend class CPUThreadHandler;

    public:

        Mega8Frame(wxWindow* parent,wxWindowID id = -1);
        virtual ~Mega8Frame();

        void OnKeyDown(wxKeyEvent& event);
        void OnKeyUp(wxKeyEvent& event);

    protected:
        bool _exit;
        bool _MusicIsPlaying;
        Chip8 *_machine;
        long _frequency;
        CPUThreadHandler *_CPUThread;
        wxCriticalSection _CPUThreadCS;         // Protect the thread
        InputDialog *dlgInput;                  // Input Configuration
        Mix_Chunk *_mcSound[16];                // Maximum sounds at the same time
        int _currentSound;
        stack<int> _channels;
        int _maxChannels;                       // Maximum channels allocated by SDL_Mixer

        void DoStartCPUThread();
        void DoPauseCPUThread();
        void DoResumeCPUThread();
        void OnCPUThreadUpdate(wxThreadEvent&);
        void OnCPUThreadCompletion(wxThreadEvent&);
        void DeleteCPUThread();
        void updateFrequency(int Multiplicator);

        void DoOpen();
        void DoScreenshot();
        void Reset();
        void softReset();
        void hardReset();
        void CloseRom();
        void updateStatusFPS();
        void SetUseSleep(bool value);
        void SetSound(bool value);
        void SetPause(bool value);
        void SetSyncClock(bool value);
        void SetFiltered(bool value);
        void SetDisplayHUD(bool value);
        void SetSpeedAuto(bool value);
        void SetFullScreenMode(bool value);
        void SetColorTheme(Chip8ColorTheme value, bool updateCfg = true);
        void SetInverseColor(bool value, bool updateCfg = true);

        void onIdle(wxIdleEvent &event);
        void OnPaint(wxPaintEvent &event);
        void exitApplication();

    private:

        wxString CurrentRomPath;
        wxString CurrentRomDir;
        wxString CurrentRomName;
        bool _Paused;
        wxWindow *_parent;

        //(*Handlers(Mega8Frame)
        void OnQuit(wxCommandEvent& event);
        void OnAbout(wxCommandEvent& event);
        void OnOpen(wxCommandEvent& event);
        void OnClose(wxCloseEvent& event);
        void OnCPUCycleTrigger(wxTimerEvent& event);
        void OnRendererTrigger(wxTimerEvent& event);
        void OnFilter(wxCommandEvent& event);
        void OnReset(wxCommandEvent& event);
        void OnPause(wxCommandEvent& event);
        void OnMenuClose(wxCommandEvent& event);
        void OnMenuSpeedSelected(wxCommandEvent& event);
        void OnMenuSizeSelected(wxCommandEvent& event);
        void OnMenuSyncClock(wxCommandEvent& event);
        void OnChooseColorTheme(wxCommandEvent& event);
        void OnMenuColorInverse(wxCommandEvent& event);
        void OnMenuDisplayHUDSelected(wxCommandEvent& event);
        void OnMenuCheckKeypadSelected(wxCommandEvent& event);
        void OnMenuUseSleepSelected(wxCommandEvent& event);
        void OnMenuScreenshotSelected(wxCommandEvent& event);
        void OnMenuSoundSelected(wxCommandEvent& event);
        //*)



        //(*Identifiers(Mega8Frame)
        static const long idMenuOpen;
        static const long idMenuClose;
        static const long idMenuQuit;
        static const long idMenuUseSleep;
        static const long idMenuReset;
        static const long idMenuPause;
        static const long idMenuSound;
        static const long idMenuCTDefault;
        static const long idMenuCTC64;
        static const long idMenuCTGameBoy;
        static const long idMenuCTRed;
        static const long idMenuCTGreen;
        static const long idMenuCTBlue;
        static const long idMenuCTInverse;
        static const long ID_MENUITEM3;
        static const long idMenuSpeedAuto;
        static const long idMenuSpeedDiv40;
        static const long idMenuSpeedDiv20;
        static const long idMenuSpeedDiv4;
        static const long idMenuSpeed1;
        static const long idMenuSpeed2;
        static const long idMenuSpeed4;
        static const long idMenuSpeed8;
        static const long idMenuSpeed16;
        static const long idMenuSpeed32;
        static const long idMenuSpeed256;
        static const long idSyncToBase;
        static const long idMenuSpeed;
        static const long idMenuScreenshot;
        static const long idMenuDisplayHUD;
        static const long idMenuFiltered;
        static const long idMenuSize1x1;
        static const long idMenuSize2x2;
        static const long idMenuSize3x3;
        static const long idMenuSize4x4;
        static const long idMenuSize5x5;
        static const long idMenuSize10x10;
        static const long idMenuFullScreen;
        static const long ID_MENUITEM1;
        static const long ID_MENUITEM2;
        static const long idMenuAbout;
        static const long ID_STATUSBAR1;
        //*)

        static const long idChipGLCanvas;
        static const int StatusBarFieldsStatus;
        static const int StatusBarFieldsMode;
        static const int StatusBarFieldsResolution;
        static const int StatusBarFieldsFrequency;
        static const int StatusBarFieldsSound;
        static const int StatusBarFieldsFramePerSec;

        //(*Declarations(Mega8Frame)
        wxMenuItem* MenuSpeed256;
        wxMenuItem* MnuCTC64;
        wxMenuItem* MenuSpeedDiv20;
        wxMenuItem* MenuScreenshot;
        wxMenuItem* MenuItem16;
        wxMenuItem* MenuItem12;
        wxMenuItem* MenuSpeedDiv4;
        wxMenu* Menu3;
        wxMenuItem* MnuFullScreen;
        wxMenuItem* MenuSpeedDiv40;
        wxMenuItem* MnuCTDefault;
        wxMenuItem* MenuItem15;
        wxMenuItem* MnuSyncClock;
        wxMenuItem* MnuCTRed;
        wxMenuItem* MenuSpeed2;
        wxMenuItem* MenuItem3;
        wxMenuItem* MnuCTBlue;
        wxMenu* Menu4;
        wxMenu* MenuItem5;
        wxMenuItem* MenuSpeed8;
        wxMenuItem* MenuSize1;
        wxMenuItem* MenuItem11;
        wxMenuItem* MenuSpeedAuto;
        wxMenuItem* MnuFilter;
        wxMenuItem* MenuDisplayHUD;
        wxMenuBar* MenuBar1;
        wxMenuItem* MenuItem10;
        wxMenuItem* MenuSound;
        wxMenuItem* MenuItem6;
        wxMenuItem* MenuItem4;
        wxMenuItem* MenuItem7;
        wxMenuItem* MenuSpeed32;
        wxStatusBar* StatusBar;
        wxMenuItem* MenuItem13;
        wxMenuItem* MnuCTGameBoy;
        wxMenuItem* MenuCheckKeypad;
        wxMenuItem* MenuSpeed1;
        wxMenuItem* MnuCTGreen;
        wxMenu* MenuItem8;
        wxMenuItem* MenuUseSleep;
        wxMenuItem* MenuSpeed4;
        wxMenuItem* MenuSpeed16;
        //*)

        Chip8GL *GLDisplay;

        DECLARE_EVENT_TABLE()
};

#endif // Mega8MAIN_H
