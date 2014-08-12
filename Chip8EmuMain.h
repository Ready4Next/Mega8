/***************************************************************
 * Name:      Chip8EmuMain.h
 * Purpose:   Defines Application Frame
 * Author:    Junta (junta@hotmail.be)
 * Created:   2014-07-24
 * Copyright: Junta (http://www.ready4next.be)
 * License:
 **************************************************************/

#ifndef CHIP8EMUMAIN_H
#define CHIP8EMUMAIN_H

//(*Headers(Chip8EmuFrame)
#include <wx/menu.h>
#include <wx/statusbr.h>
#include <wx/frame.h>
//*)

#include <SDL2/SDL.h>
#include <wx/wx.h>
#include <wx/string.h>
#include "Chip8/Chip8.h"
#include "include/Chip8GL.h"

// declare a new type of event, to be used by our CpuThread class:
wxDECLARE_EVENT(wxEVT_COMMAND_CPUTHREAD_COMPLETED, wxThreadEvent);
wxDECLARE_EVENT(wxEVT_COMMAND_CPUTHREAD_UPDATE, wxThreadEvent);

const float FREQ_COEFF[] = { 10, 5, 2, 1, 0.5, 0.1, 0.05  };
const int   SIZE_COEFF[] = { 1, 2, 2, 4, 5, 10 };

class Chip8EmuFrame;

class CPUThreadHandler: public wxThread
{
public:
    CPUThreadHandler(Chip8EmuFrame *handler): wxThread(wxTHREAD_DETACHED) { _handler = handler; }
    ~CPUThreadHandler();
protected:
    virtual ExitCode Entry();
    Chip8EmuFrame *_handler;
};

class Chip8EmuFrame: public wxFrame
{
    friend class CPUThreadHandler;

    public:

        Chip8EmuFrame(wxWindow* parent,wxWindowID id = -1);
        virtual ~Chip8EmuFrame();

        void OnKeyDown(wxKeyEvent& event);
        void OnKeyUp(wxKeyEvent& event);

    protected:

        Chip8 *_machine;
        int _frequency;
        CPUThreadHandler *_CPUThread;
        wxCriticalSection _CPUThreadCS;       // Protect the thread
        bool _ExitRender;

        void DoStartCPUThread();
        void DoPauseCPUThread();
        void DoResumeCPUThread();
        void OnCPUThreadUpdate(wxThreadEvent&);
        void OnCPUThreadCompletion(wxThreadEvent&);
        void DeleteCPUThread();

        void DoOpen();
        void Reset();
        void CloseRom();
        void updateStatus();
        void SetSyncClock(bool value);
        void SetFiltered(bool value);

        void onIdle(wxIdleEvent &event);
        void OnPaint(wxPaintEvent &event);

    private:

        wxString CurrentRomPath;
        wxString CurrentRomDir;
        bool _Paused;
        bool _FullScreen;
        wxWindow *_parent;

        //(*Handlers(Chip8EmuFrame)
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
        //*)



        //(*Identifiers(Chip8EmuFrame)
        static const long idMenuOpen;
        static const long idMenuClose;
        static const long idMenuQuit;
        static const long idMenuReset;
        static const long idMenuPause;
        static const long idMenuCTDefault;
        static const long idMenuCTC64;
        static const long idMenuCTGameBoy;
        static const long idMenuCTRed;
        static const long idMenuCTGreen;
        static const long idMenuCTBlue;
        static const long idMenuCTInverse;
        static const long ID_MENUITEM3;
        static const long idMenuSpeedDiv20;
        static const long idMenuSpeedDiv10;
        static const long idMenuSpeedDiv2;
        static const long idMenuSpeed1;
        static const long idMenuSpeed2;
        static const long idMenuSpeed5;
        static const long idMenuSpeed10;
        static const long idSyncToBase;
        static const long idMenuSpeed;
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

        //(*Declarations(Chip8EmuFrame)
        wxMenuItem* MenuSpeedDiv20;
        wxMenuItem* MenuItem16;
        wxMenuItem* MenuItem12;
        wxMenu* Menu3;
        wxMenuItem* MenuItem19;
        wxMenuItem* MenuItem20;
        wxMenuItem* MenuItem15;
        wxMenuItem* MnuSyncClock;
        wxMenuItem* MenuItem21;
        wxMenuItem* MenuItem17;
        wxMenuItem* MenuSpeed2;
        wxMenuItem* MenuSpeedDiv10;
        wxMenuItem* MenuItem3;
        wxMenuItem* MenuItem9;
        wxMenu* Menu4;
        wxMenu* MenuItem5;
        wxMenuItem* MenuSize1;
        wxMenuItem* MenuItem11;
        wxMenuItem* MenuSpeed10;
        wxMenuItem* MenuSpeedDiv2;
        wxMenuItem* MenuSpeed5;
        wxMenuItem* MnuFilter;
        wxMenuItem* MenuDisplayHUD;
        wxMenuBar* MenuBar1;
        wxMenuItem* MenuItem10;
        wxMenuItem* MenuItem18;
        wxMenuItem* MenuItem6;
        wxMenuItem* MenuItem4;
        wxMenuItem* MenuItem7;
        wxStatusBar* StatusBar;
        wxMenuItem* MenuItem13;
        wxMenuItem* MenuCheckKeypad;
        wxMenuItem* MenuSpeed1;
        wxMenu* MenuItem8;
        wxMenuItem* MenuItem14;
        //*)

        Chip8GL *GLDisplay;

        DECLARE_EVENT_TABLE()
};

#endif // CHIP8EMUMAIN_H
