/***************************************************************
 * Name:      Mega8Main.cpp
 * Purpose:   Code for Application Frame
 * Author:    Junta (junta@hotmail.be)
 * Created:   2014-07-24
 * Copyright: Junta (http://www.ready4next.be)
 * License:
 **************************************************************/

#include "Mega8Main.h"
#include "include/bell.h"

#include <wx/msgdlg.h>
#include <wx/dcmemory.h>
#include <wx/dcclient.h>
#include <wx/dcbuffer.h>
#include <wx/bitmap.h>
#include <wx/filedlg.h>
#include <wx/image.h>
#include <wx/thread.h>
#include <wx/stdpaths.h>

#ifndef __WIN32__
    #include "mega8.xpm"
#else
    #include <wx/wxicon.h>
#endif // __WIN32__

#include <chrono>

//(*InternalHeaders(Mega8Frame)
#include <wx/string.h>
#include <wx/intl.h>
//*)

#define APP_TITLE "Mega/Super/HiRes/Chip-8 Emulator"
#define APP_VERSION "1.1"
#define APP_AUTHOR "Ready4Next/Junta"

//helper functions
enum wxbuildinfoformat {
    short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(APP_TITLE);

    wxbuild << _T(" - Version ") << APP_VERSION << _T("\n\nUsing OpenGL & ");
    wxbuild << wxVERSION_STRING << "\n";

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("\t* Windows");
#elif defined(__UNIX__)
        wxbuild << _T("\t* Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build *");
#else
        wxbuild << _T("-ANSI build *");
#endif // wxUSE_UNICODE
    }

    wxbuild << _T("\n\n\t(C) 2014 ") << APP_AUTHOR;
    wxbuild << _T("\n\nTTF \"Game Over\" by Pedro Muñoz Pastor");
    wxbuild << _T("\nKeypad Test [Hap, 2006]");
    wxbuild << _T("\nChip8 emulator Logo [Garstyciuks]");
    wxbuild << _T("\n\nShortcuts:\n\n");
    wxbuild << _T("ESC: Quit\t\tO: Open\nF: Fullscreen\tH: Display HUD\nL: Filter\t\tR: Reset\nC: Close\t\tSpace: Pause / Resume\nS: Sync Clock @60 Hz\nCTRL +/-: Increase / Decrease frequency\nChip-8 Keypad (0-F): Numpad (0-9.+-/*ENTER)\nK: Check Keypad Test");

    return wxbuild;
}

Mix_Chunk *gSound = NULL;
Mix_Chunk *gMusic = NULL;
const long Mega8Frame::idChipGLCanvas = wxNewId();

const int Mega8Frame::StatusBarFieldsStatus = 1;
const int Mega8Frame::StatusBarFieldsMode = 2;
const int Mega8Frame::StatusBarFieldsResolution = 3;
const int Mega8Frame::StatusBarFieldsFrequency = 4;
const int Mega8Frame::StatusBarFieldsSound = 5;
const int Mega8Frame::StatusBarFieldsFramePerSec = 6;

//(*IdInit(Mega8Frame)
const long Mega8Frame::idMenuOpen = wxNewId();
const long Mega8Frame::idMenuClose = wxNewId();
const long Mega8Frame::idMenuQuit = wxNewId();
const long Mega8Frame::idMenuReset = wxNewId();
const long Mega8Frame::idMenuPause = wxNewId();
const long Mega8Frame::idMenuCTDefault = wxNewId();
const long Mega8Frame::idMenuCTC64 = wxNewId();
const long Mega8Frame::idMenuCTGameBoy = wxNewId();
const long Mega8Frame::idMenuCTRed = wxNewId();
const long Mega8Frame::idMenuCTGreen = wxNewId();
const long Mega8Frame::idMenuCTBlue = wxNewId();
const long Mega8Frame::idMenuCTInverse = wxNewId();
const long Mega8Frame::ID_MENUITEM3 = wxNewId();
const long Mega8Frame::idMenuSpeedAuto = wxNewId();
const long Mega8Frame::idMenuSpeedDiv40 = wxNewId();
const long Mega8Frame::idMenuSpeedDiv20 = wxNewId();
const long Mega8Frame::idMenuSpeedDiv4 = wxNewId();
const long Mega8Frame::idMenuSpeed1 = wxNewId();
const long Mega8Frame::idMenuSpeed2 = wxNewId();
const long Mega8Frame::idMenuSpeed4 = wxNewId();
const long Mega8Frame::idMenuSpeed8 = wxNewId();
const long Mega8Frame::idMenuSpeed16 = wxNewId();
const long Mega8Frame::idMenuSpeed32 = wxNewId();
const long Mega8Frame::idMenuSpeed256 = wxNewId();
const long Mega8Frame::idMenuSpeed1024 = wxNewId();
const long Mega8Frame::idSyncToBase = wxNewId();
const long Mega8Frame::idMenuSpeed = wxNewId();
const long Mega8Frame::idMenuDisplayHUD = wxNewId();
const long Mega8Frame::idMenuFiltered = wxNewId();
const long Mega8Frame::idMenuSize1x1 = wxNewId();
const long Mega8Frame::idMenuSize2x2 = wxNewId();
const long Mega8Frame::idMenuSize3x3 = wxNewId();
const long Mega8Frame::idMenuSize4x4 = wxNewId();
const long Mega8Frame::idMenuSize5x5 = wxNewId();
const long Mega8Frame::idMenuSize10x10 = wxNewId();
const long Mega8Frame::idMenuFullScreen = wxNewId();
const long Mega8Frame::ID_MENUITEM1 = wxNewId();
const long Mega8Frame::ID_MENUITEM2 = wxNewId();
const long Mega8Frame::idMenuAbout = wxNewId();
const long Mega8Frame::ID_STATUSBAR1 = wxNewId();
//*)

wxDEFINE_EVENT(wxEVT_COMMAND_CPUTHREAD_COMPLETED, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_CPUTHREAD_UPDATE, wxThreadEvent);

BEGIN_EVENT_TABLE(Mega8Frame,wxFrame)
    //(*EventTable(Mega8Frame)
    //*)
END_EVENT_TABLE()

Mega8Frame::Mega8Frame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(Mega8Frame)
    wxMenuItem* MenuItem2;
    wxMenuItem* MenuItem1;
    wxMenu* Menu1;
    wxMenu* Menu2;

    Create(parent, id, _("Chip 8 Emulator"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_FRAME_STYLE|wxWANTS_CHARS, _T("id"));
    MenuBar1 = new wxMenuBar();
    Menu1 = new wxMenu();
    MenuItem3 = new wxMenuItem(Menu1, idMenuOpen, _("Open\tO"), _("Open ROM"), wxITEM_NORMAL);
    Menu1->Append(MenuItem3);
    MenuItem7 = new wxMenuItem(Menu1, idMenuClose, _("Close\tC"), _("Close current ROM"), wxITEM_NORMAL);
    Menu1->Append(MenuItem7);
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit\tESC"), _("Quit emulator"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu3 = new wxMenu();
    MenuItem4 = new wxMenuItem(Menu3, idMenuReset, _("Reset\tALT-R"), _("Reset Emulator"), wxITEM_NORMAL);
    Menu3->Append(MenuItem4);
    MenuItem6 = new wxMenuItem(Menu3, idMenuPause, _("Start / Pause"), _("Start / Pause emulation"), wxITEM_NORMAL);
    Menu3->Append(MenuItem6);
    Menu3->AppendSeparator();
    MenuItem5 = new wxMenu();
    MenuItem9 = new wxMenuItem(MenuItem5, idMenuCTDefault, _("Default"), wxEmptyString, wxITEM_RADIO);
    MenuItem5->Append(MenuItem9);
    MenuItem17 = new wxMenuItem(MenuItem5, idMenuCTC64, _("C64"), wxEmptyString, wxITEM_RADIO);
    MenuItem5->Append(MenuItem17);
    MenuItem18 = new wxMenuItem(MenuItem5, idMenuCTGameBoy, _("Game Boy"), wxEmptyString, wxITEM_RADIO);
    MenuItem5->Append(MenuItem18);
    MenuItem19 = new wxMenuItem(MenuItem5, idMenuCTRed, _("Red"), wxEmptyString, wxITEM_RADIO);
    MenuItem5->Append(MenuItem19);
    MenuItem20 = new wxMenuItem(MenuItem5, idMenuCTGreen, _("Green"), wxEmptyString, wxITEM_RADIO);
    MenuItem5->Append(MenuItem20);
    MenuItem21 = new wxMenuItem(MenuItem5, idMenuCTBlue, _("Blue"), wxEmptyString, wxITEM_RADIO);
    MenuItem5->Append(MenuItem21);
    MenuItem5->AppendSeparator();
    MenuItem16 = new wxMenuItem(MenuItem5, idMenuCTInverse, _("Inverse"), wxEmptyString, wxITEM_CHECK);
    MenuItem5->Append(MenuItem16);
    Menu3->Append(ID_MENUITEM3, _("Color Theme"), MenuItem5, wxEmptyString);
    Menu3->AppendSeparator();
    MenuItem8 = new wxMenu();
    MenuSpeedAuto = new wxMenuItem(MenuItem8, idMenuSpeedAuto, _("Automatic"), wxEmptyString, wxITEM_CHECK);
    MenuItem8->Append(MenuSpeedAuto);
    MenuSpeedAuto->Check(true);
    MenuItem8->AppendSeparator();
    MenuSpeedDiv40 = new wxMenuItem(MenuItem8, idMenuSpeedDiv40, _("/40"), wxEmptyString, wxITEM_RADIO);
    MenuItem8->Append(MenuSpeedDiv40);
    MenuSpeedDiv20 = new wxMenuItem(MenuItem8, idMenuSpeedDiv20, _("/20"), wxEmptyString, wxITEM_RADIO);
    MenuItem8->Append(MenuSpeedDiv20);
    MenuSpeedDiv4 = new wxMenuItem(MenuItem8, idMenuSpeedDiv4, _("/4"), wxEmptyString, wxITEM_RADIO);
    MenuItem8->Append(MenuSpeedDiv4);
    MenuSpeed1 = new wxMenuItem(MenuItem8, idMenuSpeed1, _("1x"), wxEmptyString, wxITEM_RADIO);
    MenuItem8->Append(MenuSpeed1);
    MenuSpeed2 = new wxMenuItem(MenuItem8, idMenuSpeed2, _("2x"), wxEmptyString, wxITEM_RADIO);
    MenuItem8->Append(MenuSpeed2);
    MenuSpeed4 = new wxMenuItem(MenuItem8, idMenuSpeed4, _("4x"), wxEmptyString, wxITEM_RADIO);
    MenuItem8->Append(MenuSpeed4);
    MenuSpeed8 = new wxMenuItem(MenuItem8, idMenuSpeed8, _("8x"), wxEmptyString, wxITEM_RADIO);
    MenuItem8->Append(MenuSpeed8);
    MenuSpeed16 = new wxMenuItem(MenuItem8, idMenuSpeed16, _("16x"), wxEmptyString, wxITEM_RADIO);
    MenuItem8->Append(MenuSpeed16);
    MenuSpeed32 = new wxMenuItem(MenuItem8, idMenuSpeed32, _("32x"), wxEmptyString, wxITEM_RADIO);
    MenuItem8->Append(MenuSpeed32);
    MenuSpeed256 = new wxMenuItem(MenuItem8, idMenuSpeed256, _("256x"), wxEmptyString, wxITEM_RADIO);
    MenuItem8->Append(MenuSpeed256);
    MenuSpeed1024 = new wxMenuItem(MenuItem8, idMenuSpeed1024, _("1024x"), wxEmptyString, wxITEM_RADIO);
    MenuItem8->Append(MenuSpeed1024);
    MenuItem8->AppendSeparator();
    MnuSyncClock = new wxMenuItem(MenuItem8, idSyncToBase, _("Sync clock @60Hz"), wxEmptyString, wxITEM_CHECK);
    MenuItem8->Append(MnuSyncClock);
    MnuSyncClock->Check(true);
    Menu3->Append(idMenuSpeed, _("Speed"), MenuItem8, wxEmptyString);
    Menu4 = new wxMenu();
    MenuDisplayHUD = new wxMenuItem(Menu4, idMenuDisplayHUD, _("Display HUD\tH"), _("Display on screen informations"), wxITEM_CHECK);
    Menu4->Append(MenuDisplayHUD);
    MnuFilter = new wxMenuItem(Menu4, idMenuFiltered, _("Filter Texture"), _("Apply OpenGL linear filtering"), wxITEM_CHECK);
    Menu4->Append(MnuFilter);
    Menu4->AppendSeparator();
    MenuSize1 = new wxMenuItem(Menu4, idMenuSize1x1, _("1x1"), _("Set base size (Chip-8: 64x32, SChip-8: 128x64)"), wxITEM_NORMAL);
    Menu4->Append(MenuSize1);
    MenuItem11 = new wxMenuItem(Menu4, idMenuSize2x2, _("2x2"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItem11);
    MenuItem12 = new wxMenuItem(Menu4, idMenuSize3x3, _("3x3"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItem12);
    MenuItem13 = new wxMenuItem(Menu4, idMenuSize4x4, _("4x4"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItem13);
    MenuItem10 = new wxMenuItem(Menu4, idMenuSize5x5, _("5x5"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItem10);
    MenuItem15 = new wxMenuItem(Menu4, idMenuSize10x10, _("10x10"), wxEmptyString, wxITEM_NORMAL);
    Menu4->Append(MenuItem15);
    Menu4->AppendSeparator();
    MenuItem14 = new wxMenuItem(Menu4, idMenuFullScreen, _("FullScreen\tF"), _("Set emulator fullscreen"), wxITEM_NORMAL);
    Menu4->Append(MenuItem14);
    Menu3->Append(ID_MENUITEM1, _("Graphics"), Menu4, wxEmptyString);
    MenuCheckKeypad = new wxMenuItem(Menu3, ID_MENUITEM2, _("Input"), _("Check keypad"), wxITEM_NORMAL);
    Menu3->Append(MenuCheckKeypad);
    MenuBar1->Append(Menu3, _("&Emulation"));
    Menu2 = new wxMenu();
    MenuItem2 = new wxMenuItem(Menu2, idMenuAbout, _("About\tF1"), _("Show info about this application"), wxITEM_NORMAL);
    Menu2->Append(MenuItem2);
    MenuBar1->Append(Menu2, _("&Help"));
    SetMenuBar(MenuBar1);
    StatusBar = new wxStatusBar(this, ID_STATUSBAR1, 0, _T("ID_STATUSBAR1"));
    int __wxStatusBarWidths_1[7] = { -1, -10, -10, -10, -10, -10, -10 };
    int __wxStatusBarStyles_1[7] = { wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL, wxSB_NORMAL };
    StatusBar->SetFieldsCount(7,__wxStatusBarWidths_1);
    StatusBar->SetStatusStyles(7,__wxStatusBarStyles_1);
    SetStatusBar(StatusBar);

    Connect(idMenuOpen,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnOpen);
    Connect(idMenuClose,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuClose);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnQuit);
    Connect(idMenuReset,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnReset);
    Connect(idMenuPause,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnPause);
    Connect(idMenuCTDefault,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnChooseColorTheme);
    Connect(idMenuCTC64,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnChooseColorTheme);
    Connect(idMenuCTGameBoy,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnChooseColorTheme);
    Connect(idMenuCTRed,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnChooseColorTheme);
    Connect(idMenuCTGreen,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnChooseColorTheme);
    Connect(idMenuCTBlue,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnChooseColorTheme);
    Connect(idMenuCTInverse,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuColorInverse);
    Connect(idMenuSpeedAuto,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSpeedSelected);
    Connect(idMenuSpeedDiv40,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSpeedSelected);
    Connect(idMenuSpeedDiv20,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSpeedSelected);
    Connect(idMenuSpeedDiv4,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSpeedSelected);
    Connect(idMenuSpeed1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSpeedSelected);
    Connect(idMenuSpeed2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSpeedSelected);
    Connect(idMenuSpeed4,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSpeedSelected);
    Connect(idMenuSpeed8,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSpeedSelected);
    Connect(idMenuSpeed16,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSpeedSelected);
    Connect(idMenuSpeed32,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSpeedSelected);
    Connect(idMenuSpeed256,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSpeedSelected);
    Connect(idMenuSpeed1024,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSpeedSelected);
    Connect(idSyncToBase,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSyncClock);
    Connect(idMenuDisplayHUD,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuDisplayHUDSelected);
    Connect(idMenuFiltered,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnFilter);
    Connect(idMenuSize1x1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSizeSelected);
    Connect(idMenuSize2x2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSizeSelected);
    Connect(idMenuSize3x3,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSizeSelected);
    Connect(idMenuSize4x4,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSizeSelected);
    Connect(idMenuSize5x5,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSizeSelected);
    Connect(idMenuSize10x10,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSizeSelected);
    Connect(idMenuFullScreen,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSizeSelected);
    Connect(ID_MENUITEM2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuCheckKeypadSelected);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnAbout);
    Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&Mega8Frame::OnClose);
    //*)

#ifdef __WIN32__
    // Application icon
    SetIcon(wxIcon(AppIcon));
#else
    wxIcon AppIcon(mega8_xpm);
    SetIcon(AppIcon);
#endif // __WIN32__

    // Create Panel
    int GLCanvasAttributes[] = {
    	WX_GL_RGBA,
    	WX_GL_DOUBLEBUFFER,
    	WX_GL_DEPTH_SIZE,      16,
    	WX_GL_STENCIL_SIZE,    0,
    	0, 0 };
    GLDisplay = new Chip8GL(this, idChipGLCanvas, GLCanvasAttributes);
    GLDisplay->setFiltered(false);
    GLDisplay->setDisplayHUD(false);

    // Update Graphics
    Connect(wxID_ANY,wxEVT_IDLE,(wxObjectEventFunction)&Mega8Frame::onIdle);
    // CPU Thread
    Connect(wxID_ANY,wxEVT_COMMAND_CPUTHREAD_COMPLETED,(wxObjectEventFunction)&Mega8Frame::OnCPUThreadCompletion);
    Connect(wxID_ANY,wxEVT_COMMAND_CPUTHREAD_UPDATE,(wxObjectEventFunction)&Mega8Frame::OnCPUThreadUpdate);

    // Init SDL
    SDL_Init(SDL_INIT_AUDIO);
    // Init SDL_Mixer
    if( Mix_OpenAudio( 11025, AUDIO_U8, 1, 1024 ) < 0 ) {
        printf("Unable to open audio! SDL_mixer Error: %s\n", Mix_GetError());
        exit(1);
    }

    // Init Audio
    gSound = NULL;
    gMusic = NULL;

    // Get Application Directory
    wxString exePath = wxPathOnly(::wxStandardPaths::Get().GetExecutablePath());

    // Set base clock
    _frequency = BASE_FREQ;

    // Load "BIOS" ;-)
    CurrentRomDir = exePath;
    CurrentRomPath = wxEmptyString;
    _frequency = -1;
    _machine = new Chip8();
    SetSyncClock(MnuSyncClock->IsChecked());
    SetFiltered(MnuFilter->IsChecked());
    GLDisplay->setFiltered(MnuFilter->IsChecked());

    _machine->loadBios();                   // Reminds me Nintendo Logo when power up...
    updateFrequency(_machine->getFrequencyMultiplicator());
    SetTitle(APP_TITLE);

    DoStartCPUThread();
}

Mega8Frame::~Mega8Frame()
{
    if (gMusic != NULL) {
        Mix_FreeChunk(gMusic);
        gMusic = NULL;
    }

    if (gSound != NULL) {
        Mix_FreeChunk(gSound);
        gMusic = NULL;
    }

    delete _machine;
    //(*Destroy(Mega8Frame)
    //*)
}

void Mega8Frame::OnOpen(wxCommandEvent& event)
{
    DoOpen();
}

void Mega8Frame::DoOpen() {
    wxFileDialog* OpenDialog = new wxFileDialog(
		this, _("Open a M/S/Chip-8 Rom"), CurrentRomDir, wxEmptyString,
		_("All Chip-8 Roms (*.ch8;*.sc8;*.mc8)|*.ch8;*.sc8;*.mc8|All files (*.*)|*"),
		wxFD_OPEN, wxDefaultPosition);

	// Creates a "open file" dialog with 4 file types
	if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
	{
	    _frequency = -1;
		CurrentRomPath = OpenDialog->GetPath();
		CurrentRomDir = OpenDialog->GetDirectory();

        if (gMusic != NULL) {
            Mix_FreeChunk(gMusic);
            gMusic = NULL;
        }

		// Sets our current document to the file the user selected
		// Delete CPU Thread
		if (_CPUThread) {
            if (_Paused)
                DoResumeCPUThread();

            DeleteCPUThread();
            delete _CPUThread;
            _CPUThread = NULL;
		}

        delete _machine;
        _machine = new Chip8();
		_machine->initialize(CHIP8);
		SetSyncClock(MnuSyncClock->IsChecked());
		SetFiltered(MnuFilter->IsChecked());
		_machine->loadGame(CurrentRomPath.mb_str()); //Opens that file
        wxString title = APP_TITLE;
        title << " " << OpenDialog->GetFilename();
		SetTitle(title); // Set the Title to reflect the file open

        DoStartCPUThread();
	}

	// Clean up after ourselves
	OpenDialog->Destroy();
}

void Mega8Frame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void Mega8Frame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, APP_TITLE);
}

void Mega8Frame::OnClose(wxCloseEvent&)
{
    /* This is the cleaning up part */
    Mix_CloseAudio();
	Mix_Quit();

    DeleteCPUThread();
    Destroy();
}

/**************** CPUThread *******************/

void Mega8Frame::DoStartCPUThread()
{
    if (_machine) {
        if (_machine->loaded()) {
            _CPUThread = new CPUThreadHandler(this);
            // Create the thread
            if ( _CPUThread->Run() != wxTHREAD_NO_ERROR ) {
                wxLogError("Can't create thread!");
                delete _CPUThread;
                _machine = NULL;
            }
        }
    }
}

void Mega8Frame::DoPauseCPUThread()
{
    wxCriticalSectionLocker enter(_CPUThreadCS);
    if (_CPUThread)         // does the thread still exist?
    {
        // without a critical section, once reached this point it may happen
        // that the OS scheduler gives control to the MyThread::Entry() function,
        // which in turn may return (because it completes its work) making
        // invalid the m_pThread pointer
        if (_CPUThread->Pause() != wxTHREAD_NO_ERROR )
            wxLogError("Can't pause the thread!");
    }
    _Paused = true;
}

// a resume routine would be nearly identic to DoPauseThread()
void Mega8Frame::DoResumeCPUThread()
{
    wxCriticalSectionLocker enter(_CPUThreadCS);
    if (_CPUThread)         // does the thread still exist?
    {
        // without a critical section, once reached this point it may happen
        // that the OS scheduler gives control to the MyThread::Entry() function,
        // which in turn may return (because it completes its work) making
        // invalid the m_pThread pointer
        if (_CPUThread->Resume() != wxTHREAD_NO_ERROR )
            wxLogError("Can't pause the thread!");
    }
    _Paused = false;
}

void Mega8Frame::OnCPUThreadUpdate(wxThreadEvent&)
{
    static bool isPlaying;

    // Check if we must play a sound
    if (_machine->timerSound() != 0 && _machine->getType() != MCHIP8) {
        //Load sound effects
        if (gSound == NULL) {
            SDL_RWops *chunk = SDL_RWFromConstMem(bell_wav, bell_wav_len);
            gSound = Mix_LoadWAV_RW(chunk, 0);
            if( gSound == NULL )
            {
                printf( "Failed to load Beep Sound Effect! SDL_mixer Error: %s\n", Mix_GetError() );
                SDL_RWclose(chunk);
                gSound = NULL;
            }
            SDL_RWclose(chunk);
        }

        if (gSound != NULL)
            Mix_PlayChannel( -1, gSound, 0 );
    } else {
        if (_machine->getSoundBuffer() != NULL && !isPlaying) {
            // Load RAW Data from Chip8 (8bit mono 11025Hz)
            gMusic = Mix_QuickLoad_RAW(_machine->getSoundBuffer(), _machine->getSoundBufferSize());
            if( gMusic == NULL )
            {
                printf( "Failed to load Mega-Chip Music! SDL_mixer Error: %s\n", Mix_GetError() );
                gMusic = NULL;
            }
            isPlaying = true;

            if (gMusic != NULL)
                Mix_FadeInChannel( -1, gMusic, _machine->getSoundRepeat(), 3000 );

        } else if (_machine->getSoundBuffer() == NULL && isPlaying) {
            Mix_FreeChunk(gMusic);
            gMusic = NULL;
            isPlaying = false;
        }
    }
}

string getFrequencyStr(float frequency) {
    string freqStr;
    float fDiv;
    int cnt;
    char *endStr[] = {"Mhz","Khz","Hz"};

    fDiv = 1000000;
    cnt = 0;
    while (fDiv != 1) {
        if ((int)floorf(frequency / fDiv) > 0) {
            char buf[25];
            sprintf(buf, "%.2f %s", (frequency / fDiv), endStr[cnt]);
            freqStr = buf;
            return freqStr;
        }
        cnt++;
        fDiv /= 1000;
    }
    char buf[25];
    sprintf(buf, "%.2f %s", (float)frequency, endStr[2]);
    freqStr = buf;
    return freqStr;
}

void Mega8Frame::updateFrequency(int Multiplicator) {
    // Get the base frequency set by the CPU
    long baseFrequency = BASE_FREQ * _machine->getFrequencyMultiplicator();

    if (baseFrequency != _frequency) {

        // If _frequency is not set on automatic
        if (_frequency != -1) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[_freqCoeff];
            baseFrequency = _frequency;
        }

        // Adjust Menu's texts { 1, 2, 10, 40, 80, 160, 320, 640 }
        MenuSpeed1024->SetItemLabel(wxString::Format(_("1024x - %s"), getFrequencyStr(BASE_FREQ * 40960).c_str()));
        if (Multiplicator == (40 * 1024)) {
            _freqCoeff = 10;
            MenuSpeed1024->Check(true);
        }

        MenuSpeed256->SetItemLabel(wxString::Format(_("256x - %s"), getFrequencyStr(BASE_FREQ * 10240).c_str()));
        if (Multiplicator == (40 * 256)) {
            _freqCoeff = 9;
            MenuSpeed256->Check(true);
        }

        MenuSpeed32->SetItemLabel(wxString::Format(_("32x - %s"), getFrequencyStr(BASE_FREQ * 1280).c_str()));
        if (Multiplicator == (40 * 32)) {
            _freqCoeff = 8;
            MenuSpeed32->Check(true);
        }

        MenuSpeed16->SetItemLabel(wxString::Format(_("16x - %s"), getFrequencyStr(BASE_FREQ * 640).c_str()));
        if (Multiplicator == (40 * 16)) {
            _freqCoeff = 7;
            MenuSpeed16->Check(true);
        }

        MenuSpeed8->SetItemLabel(wxString::Format(_("8x - %s"), getFrequencyStr(BASE_FREQ * 320).c_str()));
        if (Multiplicator == (40 * 8)) {
            _freqCoeff = 6;
            MenuSpeed8->Check(true);
        }
        MenuSpeed4->SetItemLabel(wxString::Format(_("4x - %s"), getFrequencyStr(BASE_FREQ * 160).c_str()));
        if (Multiplicator == (40 * 4)) {
            _freqCoeff = 5;
            MenuSpeed4->Check(true);
        }
        MenuSpeed2->SetItemLabel(wxString::Format(_("2x - %s"), getFrequencyStr(BASE_FREQ * 80).c_str()));
        if (Multiplicator == (40 * 2)) {
            _freqCoeff = 4;
            MenuSpeed2->Check(true);
        }
        MenuSpeed1->SetItemLabel(wxString::Format(_("1x - %s"), getFrequencyStr(BASE_FREQ * 40).c_str()));
        if (Multiplicator == 40) {
            _freqCoeff = 3;
            MenuSpeed1->Check(true);
        }

        MenuSpeedDiv4->SetItemLabel(wxString::Format(_("1/4x - %s"), getFrequencyStr(BASE_FREQ * 10).c_str()));
        if (Multiplicator == (40 / 4)) {
            _freqCoeff = 2;
            MenuSpeedDiv4->Check(true);
        }
        MenuSpeedDiv20->SetItemLabel(wxString::Format(_("1/20x - %s"), getFrequencyStr(BASE_FREQ * 2).c_str()));
        if (Multiplicator == (40 / 20)) {
            _freqCoeff = 1;
            MenuSpeedDiv20->Check(true);
        }
        MenuSpeedDiv40->SetItemLabel(wxString::Format(_("1/40x - %s"), getFrequencyStr(BASE_FREQ).c_str()));
        if (Multiplicator == (40 / 40)) {
            _freqCoeff = 0;
            MenuSpeedDiv40->Check(true);
        }
    }
}

void Mega8Frame::OnCPUThreadCompletion(wxThreadEvent&)
{
    // Nothing for now...
}

void Mega8Frame::DeleteCPUThread()
{
    if (_Paused)
        DoResumeCPUThread();

    {
        wxCriticalSectionLocker enter(_CPUThreadCS);
        if (_CPUThread)         // does the thread still exist?
        {
            wxMessageOutputDebug().Printf("Chip8CPUFrame: deleting thread");
            if (_CPUThread->Delete() != wxTHREAD_NO_ERROR )
                wxLogError("Can't delete the thread!");
        }
    }       // exit from the critical section to give the thread
            // the possibility to enter its destructor
            // (which is guarded with m_pThreadCS critical section!)
    while (1)
    {
        { // was the ~MyThread() function executed?
            wxCriticalSectionLocker enter(_CPUThreadCS);
            if (!_CPUThread)
            {
                //wxThread::This->Kill();
                break;
            }
            wxMessageOutputDebug().Printf("Chip8CPUFrame: Waiting thread to delete...");
        }
        // wait for thread completion
        wxThread::This()->Sleep(1);
    }
}

void Mega8Frame::onIdle(wxIdleEvent &event) {

    if (_machine) {
        //
        if (_machine->loaded() ) {
                // && _machine->needToRefresh()
            if (_machine->getScreen() != NULL) {
                GLDisplay->Render(_machine->getScreen(), _machine->getWidth(), _machine->getHeight());
                GLDisplay->Flip();
            }
        }
    }

    updateStatus();

    event.RequestMore(); // render continuously, not only once on idle
}

void Mega8Frame::OnPaint(wxPaintEvent &event) {
    if (_machine) {
        if (_machine->loaded()) {
            if (_machine->getScreen() != NULL) {
                unsigned char *buf = (unsigned char*) malloc(_machine->getScreenSizeOf());
                memcpy(buf, _machine->getScreen(), _machine->getScreenSizeOf());

                GLDisplay->Render(buf, _machine->getWidth(), _machine->getHeight());
                GLDisplay->Flip();
                free(buf);
            }
        }
    }
}

/******** CPU Thread Class ********/

CPUThreadHandler::~CPUThreadHandler() {
    wxCriticalSectionLocker enter(_handler->_CPUThreadCS);
    // the thread is being destroyed; make sure not to leave dangling pointers around
    _handler->_CPUThread = NULL;
}

wxThread::ExitCode CPUThreadHandler::Entry()
{
    // This need C++11 compilator setting
    auto t_start = chrono::high_resolution_clock::now();
    auto t_end = chrono::high_resolution_clock::now();
    double t;
    int currentFrequency;

    t_start = chrono::high_resolution_clock::now();

    // TestDestroy is called to ensure that calls Pause(), Delete(),...
    while (!TestDestroy()) {

        // Calc elapsed time
        t_end = chrono::high_resolution_clock::now();
        t = chrono::duration_cast<chrono::microseconds>(t_end - t_start).count();

        // Get actual frequency
        if (_handler->_frequency != -1)
            // User sets the frequency
            currentFrequency = _handler->_frequency;
        else {
            // Frequency is set on auto
            currentFrequency = _handler->_machine->getFrequencyMultiplicator() * BASE_FREQ;
        }

        // Can we execute the next instruction ?
        if (t >= getMicroFromHertz(currentFrequency)) {
            if (_handler->_machine) {
                if (_handler->_machine->loaded()) {
                    _handler->_machine->execute(getMilliFromHertz(_handler->_frequency));
                }
            }

            // If frequency is in auto mode and currentFrequency have changed
            if (_handler->_frequency == -1 && (currentFrequency != _handler->_machine->getFrequencyMultiplicator() * BASE_FREQ)){
                // Search for frequency new multiplicator
                for (int i = 0; i < sizeof(FREQ_MENU_INDEX); i++)
                    if (FREQ_MENU_INDEX[i] == _handler->_machine->getFrequencyMultiplicator())
                        _handler->_freqCoeff = i;

                // Update CPU Frequency and GUI
                _handler->updateFrequency(FREQ_MENU_INDEX[_handler->_freqCoeff]);
            }

            wxQueueEvent(_handler, new wxThreadEvent(wxEVT_COMMAND_CPUTHREAD_UPDATE));

            // Restart Counter
            t_start = chrono::high_resolution_clock::now();
        }
    }
    // signal the event handler that this thread is going to be destroyed
    // NOTE: here we assume that using the m_pHandler pointer is safe,
    //       (in this case this is assured by the MyFrame destructor)
    wxQueueEvent(_handler, new wxThreadEvent(wxEVT_COMMAND_CPUTHREAD_COMPLETED));

    return (wxThread::ExitCode)0;    // success
}

void Mega8Frame::OnFilter(wxCommandEvent& event)
{
    GLDisplay->setFiltered(!GLDisplay->getFiltered());
}

void Mega8Frame::OnReset(wxCommandEvent& event)
{
    Reset();
}

void Mega8Frame::Reset() {
    if (_machine) {
        // Hard Reset
        if (gMusic != NULL) {
            Mix_FreeChunk(gMusic);
            gMusic = NULL;
        }

		/*// Delete CPU Thread
		if (_CPUThread) {
            if (_Paused)
                DoResumeCPUThread();

            DeleteCPUThread();
            delete _CPUThread;
            _CPUThread = NULL;
		}

        delete _machine;
        _machine = new Chip8();*/
		_machine->initialize(CHIP8);
		SetSyncClock(MnuSyncClock->IsChecked());
		SetFiltered(MnuFilter->IsChecked());
        _machine->initialize(CHIP8);
        if (CurrentRomPath == wxEmptyString)
            _machine->loadBios();
        else
            _machine->loadGame(CurrentRomPath);
    }
}

void Mega8Frame::OnPause(wxCommandEvent& event)
{
    if (!_Paused) {
        DoPauseCPUThread();
    } else {
        DoResumeCPUThread();
    }
}

/*

Chip8 Keyboard

1 | 2 | 3 | C
--+---+---+--
4 | 5 | 6 | D
--+---+---+--
7 | 8 | 9 | E
--+---+---+--
A | 0 | B | F

*/

void Mega8Frame::CloseRom() {
    delete _machine;
    _machine = new Chip8();
    SetSyncClock(MnuSyncClock->IsChecked());
    SetFiltered(MnuFilter->IsChecked());
    _machine->loadBios();
}

void Mega8Frame::OnMenuClose(wxCommandEvent& event)
{
    CloseRom();
}

void Mega8Frame::OnMenuSpeedSelected(wxCommandEvent& event)
{
    if (_machine) {
        if (!_Paused && _machine->isRunning()) {
            DoPauseCPUThread();
            _Paused = true;
        }

        long baseFrequency = _machine->getFrequencyMultiplicator() * BASE_FREQ;

        if (event.GetId() == Mega8Frame::idMenuSpeedAuto) {
            _frequency = -1;
            _freqCoeff = 0;
        } else {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[3];
            _freqCoeff = 3;
            MenuSpeedAuto->Check(false);
        }

        if (event.GetId() == Mega8Frame::idMenuSpeedDiv40) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[0];
            _freqCoeff = 0;
        }

        if (event.GetId() == Mega8Frame::idMenuSpeedDiv20) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[1];
            _freqCoeff = 1;
        }

        if (event.GetId() == Mega8Frame::idMenuSpeedDiv4) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[2];
            _freqCoeff = 2;
        }

        if (event.GetId() == Mega8Frame::idMenuSpeed1) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[3];
            _freqCoeff = 3;
        }

        if (event.GetId() == Mega8Frame::idMenuSpeed2) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[4];
            _freqCoeff = 4;
        }

        if (event.GetId() == Mega8Frame::idMenuSpeed4) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[5];
            _freqCoeff = 5;
        }

        if (event.GetId() == Mega8Frame::idMenuSpeed8) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[6];
            _freqCoeff = 6;
        }

        if (event.GetId() == Mega8Frame::idMenuSpeed16) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[7];
            _freqCoeff = 7;
        }

        if (event.GetId() == Mega8Frame::idMenuSpeed32) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[8];
            _freqCoeff = 8;
        }

        if (event.GetId() == Mega8Frame::idMenuSpeed256) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[9];
            _freqCoeff = 9;
        }

        if (event.GetId() == Mega8Frame::idMenuSpeed1024) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[10];
            _freqCoeff = 10;
        }

        // Update CPU Frequency
        updateFrequency(FREQ_MENU_INDEX[_freqCoeff]);

        if (_Paused) {
            DoStartCPUThread();
            _Paused = false;
        }
    }
}

void Mega8Frame::OnMenuSizeSelected(wxCommandEvent& event)
{
    if (_machine) {
        if (event.GetId() == Mega8Frame::idMenuFullScreen) {
            _FullScreen = !_FullScreen;
            ShowFullScreen(_FullScreen);
        }

        if (event.GetId() == Mega8Frame::idMenuSize10x10) {
            wxSize size(_machine->getWidth() * SIZE_MENU_INDEX[5], _machine->getHeight() * SIZE_MENU_INDEX[5]);
            this->SetClientSize(size);

        }

        if (event.GetId() == Mega8Frame::idMenuSize5x5) {
            wxSize size(_machine->getWidth() * SIZE_MENU_INDEX[4], _machine->getHeight() * SIZE_MENU_INDEX[4]);
            this->SetClientSize(size);

        }

        if (event.GetId() == Mega8Frame::idMenuSize4x4) {
            wxSize size(_machine->getWidth() * SIZE_MENU_INDEX[3], _machine->getHeight() * SIZE_MENU_INDEX[3]);
            this->SetClientSize(size);

        }

        if (event.GetId() == Mega8Frame::idMenuSize3x3) {
            wxSize size(_machine->getWidth() * SIZE_MENU_INDEX[2], _machine->getHeight() * SIZE_MENU_INDEX[2]);
            this->SetClientSize(size);
        }

        if (event.GetId() == Mega8Frame::idMenuSize2x2) {
            wxSize size(_machine->getWidth() * SIZE_MENU_INDEX[1], _machine->getHeight() * SIZE_MENU_INDEX[1]);
            this->SetClientSize(size);
        }

        if (event.GetId() == Mega8Frame::idMenuSize1x1) {
            wxSize size(_machine->getWidth() * SIZE_MENU_INDEX[0], _machine->getHeight() * SIZE_MENU_INDEX[0]);
            this->SetClientSize(size);
        }
    }
}

void Mega8Frame::updateStatus() {
    wxString status, mode, resolution, FPS;

    if (_machine) {
        if (_machine->loaded()) {
            if (_Paused) {
                status = _("Paused");
            }
            else if (_machine->isRunning()) {
                status = _("Running");
            } else {
                status = _("Loaded");
            }
        }

        switch (_machine->getType()) {
            case CHIP8:
                mode = "Chip-8";
                break;

            case SCHIP8:
                mode = "SChip-8";
                break;

            case MCHIP8:
                mode = "MChip-8";
                break;

            case CHIP8_HiRes:
                mode = "HiRes C-8";
                break;

            default:
                mode = _("Not supported yet");
        }

        resolution.Printf("%dx%d", _machine->getWidth(), _machine->getHeight());
        FPS.Printf("%d FPS", GLDisplay->getFPS());

        // Update Status Bar
        // Base frequency
        long baseFrequency = BASE_FREQ * _machine->getFrequencyMultiplicator();
        if (_frequency != -1)
            baseFrequency = _frequency;

        StatusBar->SetStatusText(wxString(getFrequencyStr(baseFrequency)), StatusBarFieldsFrequency);
        StatusBar->SetStatusText(status, StatusBarFieldsStatus);
        StatusBar->SetStatusText(resolution, StatusBarFieldsResolution);
        StatusBar->SetStatusText(FPS, StatusBarFieldsFramePerSec);
        StatusBar->SetStatusText(mode, StatusBarFieldsMode);

        if (GLDisplay->getDisplayHUD()) {
            mode.Printf("@ %s", mode);
            GLDisplay->PrintGLInfos(5, 0x00FF00, (const char*) mode.mb_str());
            GLDisplay->PrintGLInfos(4, 0x00FF00, (const char*) resolution.mb_str());
            GLDisplay->PrintGLInfos(3, 0x00FF00, (const char*) wxString(getFrequencyStr(baseFrequency)).mb_str());
            GLDisplay->PrintGLInfos(2, 0x00FF00, GLDisplay->getFiltered() ? _("Filter on"): _("Filter off"));
            GLDisplay->PrintGLInfos(1, 0x00FF00, (const char*) status.mb_str());
        }

    } else {
        StatusBar->SetStatusText(_("Waiting"), StatusBarFieldsStatus);
    }
}

void Mega8Frame::OnKeyDown(wxKeyEvent& event)
{
    // Numpad numérics
    switch (event.GetKeyCode()) {
    case WXK_NUMPAD0:
        this->_machine->setKey(0x0, true);
        break;

    case WXK_NUMPAD1:
        this->_machine->setKey(0x7, true);
        break;

    case WXK_NUMPAD2:
        this->_machine->setKey(0x8, true);
        break;

    case WXK_NUMPAD3:
        this->_machine->setKey(0x9, true);
        break;

    case WXK_NUMPAD4:
        this->_machine->setKey(0x4, true);
        break;

    case WXK_NUMPAD5:
        this->_machine->setKey(0x5, true);
        break;

    case WXK_NUMPAD6:
        this->_machine->setKey(0x6, true);
        break;

    case WXK_NUMPAD7:
        this->_machine->setKey(0x1, true);
        break;

    case WXK_NUMPAD8:
        this->_machine->setKey(0x2, true);
        break;

    case WXK_NUMPAD9:
        this->_machine->setKey(0x3, true);
        break;

    // Other keys
    case WXK_NUMPAD_ADD:
        if (wxGetKeyState(WXK_CONTROL)) {
            // Pause the thread
            if (!_Paused && _machine->isRunning()) {
                DoPauseCPUThread();
                _Paused = true;
            }
            MenuSpeedAuto->Check(false);
            _frequency = 0;
            if (_freqCoeff >= sizeof(FREQ_MENU_INDEX))
                _freqCoeff = 0;
            else
                _freqCoeff++;

            updateFrequency(FREQ_MENU_INDEX[_freqCoeff]);
            if (_Paused) {
                DoResumeCPUThread();
                _Paused = false;
            }
        } else
            this->_machine->setKey(0xD, true);
        break;

    case WXK_NUMPAD_DECIMAL:
        this->_machine->setKey(0xC, true);
        break;

    case WXK_NUMPAD_DIVIDE:
        this->_machine->setKey(0xF, true);
        break;

    case WXK_NUMPAD_MULTIPLY:
        this->_machine->setKey(0xA, true);
        break;

    case WXK_NUMPAD_SUBTRACT:
        if (wxGetKeyState(WXK_CONTROL)) {
            if (!_Paused && _machine->isRunning()) {
                DoPauseCPUThread();
                _Paused = true;
            }
            MenuSpeedAuto->Check(false);
            _frequency = 0;
            if (_freqCoeff == 0)
                _freqCoeff = sizeof(FREQ_MENU_INDEX) - 1;
            else
                _freqCoeff--;

            updateFrequency(FREQ_MENU_INDEX[_freqCoeff]);

            if (_Paused) {
                DoResumeCPUThread();
                _Paused = false;
            }
        } else
            this->_machine->setKey(0xE, true);
        break;

    case WXK_NUMPAD_ENTER:
        this->_machine->setKey(0xB, true);
        break;

    // Commands
    case WXK_SPACE:
        if (!_Paused) {
            DoPauseCPUThread();
            _Paused = true;
        } else {
            DoResumeCPUThread();
            _Paused = false;
        }
        break;

    case wxKeyCode('O'):
        DoOpen();
        break;

    case wxKeyCode('C'):
        CloseRom();
        break;

    case wxKeyCode('F'):
        _FullScreen = !_FullScreen;
        ShowFullScreen(_FullScreen);
        break;

    case WXK_ESCAPE:
        Close();
        break;

    case wxKeyCode('R'):
        Reset();
        break;

    case wxKeyCode('L'):
        SetFiltered(!MnuFilter->IsChecked());
        break;

    case wxKeyCode('S'):
        SetSyncClock(!MnuSyncClock->IsChecked());
        break;

    case wxKeyCode('K'):
        _machine->initialize(_machine->getType());
        _machine->loadKeypad();
        break;

    case wxKeyCode('H'):
        GLDisplay->setDisplayHUD(!GLDisplay->getDisplayHUD());
        break;
    }
}

void Mega8Frame::OnKeyUp(wxKeyEvent& event)
{
    switch (event.GetKeyCode()) {
    case WXK_NUMPAD0:
        _machine->setKey(0x0, false);
        break;

    case WXK_NUMPAD1:
        _machine->setKey(0x7, false);
        break;

    case WXK_NUMPAD2:
        _machine->setKey(0x8, false);
        break;

    case WXK_NUMPAD3:
        _machine->setKey(0x9, false);
        break;

    case WXK_NUMPAD4:
        _machine->setKey(0x4, false);
        break;

    case WXK_NUMPAD5:
        _machine->setKey(0x5, false);
        break;

    case WXK_NUMPAD6:
        _machine->setKey(0x6, false);
        break;

    case WXK_NUMPAD7:
        _machine->setKey(0x1, false);
        break;

    case WXK_NUMPAD8:
        _machine->setKey(0x2, false);
        break;

    case WXK_NUMPAD9:
        _machine->setKey(0x3, false);
        break;

    // Other keys
    case WXK_NUMPAD_ADD:
        _machine->setKey(0xD, false);
        break;

    case WXK_NUMPAD_DECIMAL:
        _machine->setKey(0xC, false);
        break;

    case WXK_NUMPAD_DIVIDE:
        _machine->setKey(0xF, false);
        break;

    case WXK_NUMPAD_MULTIPLY:
        _machine->setKey(0xA, false);
        break;

    case WXK_NUMPAD_SUBTRACT:
        _machine->setKey(0xE, false);
        break;

    case WXK_NUMPAD_ENTER:
        _machine->setKey(0xB, false);
        break;
    }
}

void Mega8Frame::SetFiltered(bool value) {
    if (_machine) {
        GLDisplay->setFiltered(value);
        MnuFilter->Check(value);
    }
}

void Mega8Frame::SetSyncClock(bool value) {
    if (_machine) {
        _machine->SetSyncClockToOriginal(value);
        MnuSyncClock->Check(value);
    }
}

void Mega8Frame::OnMenuSyncClock(wxCommandEvent& event)
{
    SetSyncClock(MnuSyncClock->IsChecked());
}

void Mega8Frame::OnChooseColorTheme(wxCommandEvent& event)
{
    if (_machine) {
        if (event.GetId() == Mega8Frame::idMenuCTInverse) {
            _machine->setColorTheme(INVERSE);
        }

        if (event.GetId() == Mega8Frame::idMenuCTC64) {
                _machine->setColorTheme(C64);
        }

        if (event.GetId() == Mega8Frame::idMenuCTGameBoy) {
                _machine->setColorTheme(GAMEBOY);
        }

        if (event.GetId() == Mega8Frame::idMenuCTDefault) {
            _machine->setColorTheme(DEFAULT);
        }

        if (event.GetId() == Mega8Frame::idMenuCTRed) {
            _machine->setColorTheme(RED);
        }

        if (event.GetId() == Mega8Frame::idMenuCTGreen) {
            _machine->setColorTheme(GREEN);
        }

        if (event.GetId() == Mega8Frame::idMenuCTBlue) {
            _machine->setColorTheme(BLUE);
        }
    }
}

void Mega8Frame::OnMenuColorInverse(wxCommandEvent& event)
{
    if (_machine) {
        _machine->setInverseColor(event.IsChecked());
    }
}

void Mega8Frame::OnMenuDisplayHUDSelected(wxCommandEvent& event)
{
    GLDisplay->setDisplayHUD(!GLDisplay->getDisplayHUD());
}

void Mega8Frame::OnMenuCheckKeypadSelected(wxCommandEvent& event)
{
    _machine->initialize(_machine->getType());
    _machine->loadKeypad();
}
