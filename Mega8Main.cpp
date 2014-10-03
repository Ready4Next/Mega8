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
#include <wx/stopwatch.h>
#include <wx/cmdline.h>

#ifndef __WIN32__
    #include "mega8.xpm"
	#include <unistd.h>
#else
   // Already included in res file
#endif // __WIN32__

//(*InternalHeaders(Mega8Frame)
#include <wx/string.h>
#include <wx/intl.h>
//*)

#define APP_TITLE "Mega/Super/HiRes/Chip-8 Emulator"
#define APP_SHORTNAME "Mega8"
#define APP_VERSION "1.3"
#define APP_AUTHOR "Ready4Next/Junta"

//helper functions
enum wxbuildinfoformat { short_f, long_f };

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(APP_TITLE);

    wxbuild << _T(" - Version ") << APP_VERSION << _T("\n\t\t\t** ") << APP_SHORTNAME << _T(" **\n\n\tUsing OpenGL & ");
    wxbuild << wxVERSION_STRING << "\n";

    if (format == long_f )
    {

// Platform
#if defined(__WXMSW__)
        wxbuild << _T("\t* Windows");
#elif defined(__UNIX__)
        wxbuild << _T("\t* Unix/Linux");
#endif

// Architecture
#if defined(__x86_64__)||defined(__LP64__)
    wxbuild << _T("-64bits");
#else
    wxbuild << _T("-32bits");
#endif // defined

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build *");
#else
        wxbuild << _T("-ANSI build *");
#endif // wxUSE_UNICODE
    }

    wxbuild << _T("\n\nClock Precision: ")  << ((double) std::chrono::high_resolution_clock::period::num / std::chrono::high_resolution_clock::period::den);
	wxbuild << _T("\nOpenGL Version: ")  << glGetString(GL_VERSION);
	wxString glExt = glGetString(GL_EXTENSIONS);
	glExt.Replace(" ", "\n");
	wxbuild << _T("\nOpenGL Extensions: ")  << glExt;

    wxbuild << _T("\n\n\t\t(C) 2014 ") << APP_AUTHOR;
    wxbuild << _T("\n\nTTF \"Game Over\" by Pedro Muñoz Pastor");
    wxbuild << _T("\nKeypad Test [Hap, 2006]");
    wxbuild << _T("\nChip8 emulator Logo [Garstyciuks]");
    wxbuild << _T("\n\nShortcuts:\n\n");
    wxbuild << _T("ESC: Quit\t\t\t\tCTRL+O: Open\nCTRL+F: Fullscreen\t\tCTRL+H: Display HUD\nCTRL+L: Filter\t\t\tCTRL+R: Reset\nCTRL+C: Close Rom\t\tCTRL+Space: Pause / Resume\nCTRL+Y: Sync Clock @60 Hz");
    wxbuild << _T("\nCTRL +/-: Increase / Decrease frequency\nChip-8 Keypad (0-F): Default Numpad (0-9.+-/*ENTER)\nCTRL+K: Check Keypad Test\nCTRL+M: Mute sound\t\tCTRL+S: Take Screenshot");

    return wxbuild;
}

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
const long Mega8Frame::idMenuUseSleep = wxNewId();
const long Mega8Frame::idMenuReset = wxNewId();
const long Mega8Frame::idMenuPause = wxNewId();
const long Mega8Frame::idMenuSound = wxNewId();
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
const long Mega8Frame::idSyncToBase = wxNewId();
const long Mega8Frame::idMenuSpeed = wxNewId();
const long Mega8Frame::idMenuScreenshot = wxNewId();
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
    MenuItem3 = new wxMenuItem(Menu1, idMenuOpen, _("Open"), _("Open ROM"), wxITEM_NORMAL);
    Menu1->Append(MenuItem3);
    MenuItem7 = new wxMenuItem(Menu1, idMenuClose, _("Close"), _("Close current ROM"), wxITEM_NORMAL);
    Menu1->Append(MenuItem7);
    MenuItem1 = new wxMenuItem(Menu1, idMenuQuit, _("Quit"), _("Quit emulator"), wxITEM_NORMAL);
    Menu1->Append(MenuItem1);
    MenuBar1->Append(Menu1, _("&File"));
    Menu3 = new wxMenu();
    MenuUseSleep = new wxMenuItem(Menu3, idMenuUseSleep, _("Use Sleep"), _("Enable sleep in thread (recommended)"), wxITEM_CHECK);
    Menu3->Append(MenuUseSleep);
    Menu3->AppendSeparator();
    MenuItem4 = new wxMenuItem(Menu3, idMenuReset, _("Reset"), _("Reset Emulator"), wxITEM_NORMAL);
    Menu3->Append(MenuItem4);
    MenuItem6 = new wxMenuItem(Menu3, idMenuPause, _("Start / Pause"), _("Start / Pause emulation"), wxITEM_NORMAL);
    Menu3->Append(MenuItem6);
    Menu3->AppendSeparator();
    MenuSound = new wxMenuItem(Menu3, idMenuSound, _("Sound"), _("Set mute on sound"), wxITEM_CHECK);
    Menu3->Append(MenuSound);
    Menu3->AppendSeparator();
    MenuItem5 = new wxMenu();
    MnuCTDefault = new wxMenuItem(MenuItem5, idMenuCTDefault, _("Default"), wxEmptyString, wxITEM_RADIO);
    MenuItem5->Append(MnuCTDefault);
    MnuCTC64 = new wxMenuItem(MenuItem5, idMenuCTC64, _("C64"), wxEmptyString, wxITEM_RADIO);
    MenuItem5->Append(MnuCTC64);
    MnuCTGameBoy = new wxMenuItem(MenuItem5, idMenuCTGameBoy, _("Game Boy"), wxEmptyString, wxITEM_RADIO);
    MenuItem5->Append(MnuCTGameBoy);
    MnuCTRed = new wxMenuItem(MenuItem5, idMenuCTRed, _("Red"), wxEmptyString, wxITEM_RADIO);
    MenuItem5->Append(MnuCTRed);
    MnuCTGreen = new wxMenuItem(MenuItem5, idMenuCTGreen, _("Green"), wxEmptyString, wxITEM_RADIO);
    MenuItem5->Append(MnuCTGreen);
    MnuCTBlue = new wxMenuItem(MenuItem5, idMenuCTBlue, _("Blue"), wxEmptyString, wxITEM_RADIO);
    MenuItem5->Append(MnuCTBlue);
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
    MenuItem8->AppendSeparator();
    MnuSyncClock = new wxMenuItem(MenuItem8, idSyncToBase, _("Sync clock @60Hz"), wxEmptyString, wxITEM_CHECK);
    MenuItem8->Append(MnuSyncClock);
    MnuSyncClock->Check(true);
    Menu3->Append(idMenuSpeed, _("Speed"), MenuItem8, wxEmptyString);
    Menu4 = new wxMenu();
    MenuScreenshot = new wxMenuItem(Menu4, idMenuScreenshot, _("Screenshot"), _("Take a screenshot from OpenGL"), wxITEM_NORMAL);
    Menu4->Append(MenuScreenshot);
    Menu4->AppendSeparator();
    MenuDisplayHUD = new wxMenuItem(Menu4, idMenuDisplayHUD, _("Display HUD"), _("Display on screen informations"), wxITEM_CHECK);
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
    MnuFullScreen = new wxMenuItem(Menu4, idMenuFullScreen, _("FullScreen"), _("Set emulator fullscreen"), wxITEM_CHECK);
    Menu4->Append(MnuFullScreen);
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
    Connect(idMenuUseSleep,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuUseSleepSelected);
    Connect(idMenuReset,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnReset);
    Connect(idMenuPause,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnPause);
    Connect(idMenuSound,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSoundSelected);
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
    Connect(idSyncToBase,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuSyncClock);
    Connect(idMenuScreenshot,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Mega8Frame::OnMenuScreenshotSelected);
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

	_exit = false;

    // Set Application icon
    wxIcon icon = wxICON(mega8);
    SetIcon(icon);

    // Create Panel
    int GLCanvasAttributes[] = {
    	WX_GL_RGBA,
    	WX_GL_DOUBLEBUFFER,
    	WX_GL_DEPTH_SIZE,      16,
    	WX_GL_STENCIL_SIZE,    0,
    	0, 0 };
    GLDisplay = new Chip8GL(this, idChipGLCanvas, GLCanvasAttributes);
    GLDisplay->setFiltered(Mega8Config::getInstance().getFiltered());
    GLDisplay->setDisplayHUD(Mega8Config::getInstance().getDisplayHUD());

    // Update Graphics
    Connect(wxID_ANY,wxEVT_IDLE,(wxObjectEventFunction)&Mega8Frame::onIdle);
    // CPU Thread
    Connect(wxID_ANY,wxEVT_COMMAND_CPUTHREAD_COMPLETED,(wxObjectEventFunction)&Mega8Frame::OnCPUThreadCompletion);
    Connect(wxID_ANY,wxEVT_COMMAND_CPUTHREAD_UPDATE,(wxObjectEventFunction)&Mega8Frame::OnCPUThreadUpdate);

    // Init SDL
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_JOYSTICK);
    SDL_JoystickEventState(SDL_ENABLE);

    // Init SDL_Mixer
    if( Mix_OpenAudio( 44100, AUDIO_U8, 1, 1024 ) < 0 ) {
        wxPrintf("Unable to open audio! SDL_mixer Error: %s\n", Mix_GetError());
        exit(1);
    }

    // Init Audio - Load 'beep' sound effect - Tries to get at least 16 channels
    _maxChannels = Mix_AllocateChannels(16);
    for (int i = 0; i < _maxChannels; i++) {
        if (i == 0) {
            // Load the beep
            SDL_RWops *chunk = SDL_RWFromConstMem(bell_wav, bell_wav_len);
            _mcSound[i] = Mix_LoadWAV_RW(chunk, 0);
            if( _mcSound[i] == NULL )
            {
                wxPrintf( "Failed to load Beep Sound Effect! SDL_mixer Error: %s\n", Mix_GetError() );
                SDL_RWclose(chunk);
                _mcSound[i] = NULL;
            }
            SDL_RWclose(chunk);
        } else {
            _mcSound[i] = NULL;
        }
    }

    // One channel loaded
    _currentSound = 1;

    // Get Application Directory
    wxString exePath = wxPathOnly(::wxStandardPaths::Get().GetExecutablePath());

    // Set Last Folder to exePath if not set
    Mega8Config::getInstance().loadConfig("General");
    if (Mega8Config::getInstance().getLastFolder() == wxEmptyString)
        Mega8Config::getInstance().setLastFolder(exePath);

    _machine = new Chip8();

    // Set some default settings
    CurrentRomDir = Mega8Config::getInstance().getLastFolder();
    CurrentRomPath = CurrentRomName = wxEmptyString;
    SetSyncClock(Mega8Config::getInstance().getSyncClock());
    SetFiltered(Mega8Config::getInstance().getFiltered());
    SetFullScreenMode(Mega8Config::getInstance().getFullScreen());
    SetColorTheme(Mega8Config::getInstance().getColorTheme());
    SetSound(Mega8Config::getInstance().getSound());
    SetUseSleep(Mega8Config::getInstance().getUseSleep());
    SetDisplayHUD(Mega8Config::getInstance().getDisplayHUD());

    // Load "BIOS" ;-)
    _machine->loadBios();                   // Reminds me Nintendo Logo when power up...

    // Set Status widths
    const int widths[7] = {-1, 85, 105, 68, 65, 65, 75};
    StatusBar->SetStatusWidths(7, widths);
    StatusBar->SetStatusText(wxT("Running"), StatusBarFieldsStatus);

    // Set default size
    if (!Mega8Config::getInstance().getFullScreen()) {
        wxSize size(_machine->getWidth() * SIZE_MENU_INDEX[5], _machine->getHeight() * SIZE_MENU_INDEX[5]);
        this->SetClientSize(size);
    }

    // Set Auto frequency or saved frequency
    if (Mega8Config::getInstance().getSpeedAuto()) {
        updateFrequency(_machine->getFrequencyMultiplicator());
    } else {
        updateFrequency(FREQ_MENU_INDEX[Mega8Config::getInstance().getFrequencyRatio(_machine->getType())]);
    }

    // Start the whole thing
    SetTitle(APP_TITLE);
    DoStartCPUThread();

    CenterOnScreen();
}

Mega8Frame::~Mega8Frame()
{
    //(*Destroy(Mega8Frame)
    //*)
}

void Mega8Frame::OnOpen(wxCommandEvent& event)
{
    DoOpen();
}

void Mega8Frame::DoOpen() {
    wxFileDialog* OpenDialog = new wxFileDialog(
		this, _("Open a M/S/Hi-Res/Chip-8 Rom"), CurrentRomDir, wxEmptyString,
		_("All Chip-8 Roms (*.ch8;*.sc8;*.mc8)|*.ch8;*.sc8;*.mc8|All files (*)|*"),
		wxFD_OPEN, wxDefaultPosition);

	// Creates a "open file" dialog with 4 file types
	if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
	{
	    hardReset();

	    CurrentRomPath = OpenDialog->GetPath();
		CurrentRomDir = OpenDialog->GetDirectory();
		CurrentRomName = OpenDialog->GetFilename();

		// Load config for this ROM (still necessary for Keyboard / Joystick)
	    Mega8Config::getInstance().loadConfig(CurrentRomName);

		// Save it for future use
		Mega8Config::getInstance().setLastFolder(CurrentRomDir);

		if (_machine->loadGame(CurrentRomPath.mb_str())) { //Opens that file

            wxString title = APP_SHORTNAME;
            title << " - " << CurrentRomName;
            SetTitle(title);

            // Starts it
            DoStartCPUThread();
		}
	}

	// Clean up
	OpenDialog->Destroy();
}

void Mega8Frame::DoScreenshot()
{
    SetPause(true);
    wxFileDialog* SaveDialog = new wxFileDialog(
		this, _("Save screenshot to..."), wxPathOnly(::wxStandardPaths::Get().GetExecutablePath()), wxEmptyString,
		_("PNG File (*.png)|*.png|JPEG File (*.jpg)|*.jpg|Bitmap File (*.bmp)|*.bmp"),
		wxFD_SAVE | wxFD_OVERWRITE_PROMPT, wxDefaultPosition);

	// Creates a "open file" dialog with 4 file types
	if (SaveDialog->ShowModal() == wxID_OK && SaveDialog->GetFilename() != wxEmptyString) // if the user click "Open" instead of "Cancel"
	{
	    // Take the screenshot from OpenL's context
	    wxBitmap screenShot = GLDisplay->getScreenshot();

	    wxString fileName;

	    // Save file to chosen format
	    switch (SaveDialog->GetFilterIndex()) {
            case 0: // PNG
                fileName = (SaveDialog->GetFilename().find('.')) ?SaveDialog->GetFilename() : SaveDialog->GetFilename() + wxT(".png");
                screenShot.SaveFile(fileName, wxBITMAP_TYPE_PNG);
                break;
            case 1: // JPEG
                fileName = (SaveDialog->GetFilename().find('.')) ?SaveDialog->GetFilename() : SaveDialog->GetFilename() + wxT(".jpg");
                screenShot.SaveFile(fileName, wxBITMAP_TYPE_JPEG);
                break;
            case 2: // Bitmap
                fileName = (SaveDialog->GetFilename().find('.')) ?SaveDialog->GetFilename() : SaveDialog->GetFilename() + wxT(".bmp");
                screenShot.SaveFile(fileName, wxBITMAP_TYPE_BMP);
                break;
	    }
	    wxMessageBox(_("Screenshot saved."), APP_SHORTNAME, wxOK);
	}

	// Clean up
    SaveDialog->Destroy();
    SetPause(false);
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
   exitApplication();
}

void Mega8Frame::exitApplication()
{
    _exit = true;
    GLDisplay->setStopRender(true);
    if (_CPUThread) {
        DeleteCPUThread();
        _CPUThread = NULL;
    }

    // Free beep & Musics
    for (int i = 0; i < _currentSound + 1; i++) {
        if (_mcSound[i] != NULL) {
            Mix_FreeChunk(_mcSound[i]);
            _mcSound[i] = NULL;
        }
    }

    // Save Config
    Mega8Config::getInstance().saveConfig(CurrentRomName);

    // Close All Joysticks before quitting SDL
    Joysticks::getInstance().close();

    /* This is the cleaning up part */
    Mix_CloseAudio();
    Mix_Quit();
	SDL_Quit();

    if (_machine) {
		_machine->destroy();
        delete _machine;
        _machine = NULL;
    }

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
                    wxMessageBox("Error", "test");
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

    if (_MusicIsPlaying) {
        Mix_Resume(-1);
    }
    _Paused = false;
}

void Mega8Frame::OnCPUThreadUpdate(wxThreadEvent&)
{
    int freq, channels;
    Uint16 format;
    int freqRatio;
    unsigned char *convertedSoundBuffer;

    if (_machine) {
        // Set Status type
        if (_machine->getTypeStr() != StatusBar->GetStatusText(StatusBarFieldsMode)) {

            if (_machine->getType() == MCHIP8) {
                SetColorTheme(DEFAULT, false);
                SetInverseColor(false, false);
            }

            StatusBar->SetStatusText(_machine->getTypeStr(), StatusBarFieldsMode);
            wxString res;
            res.Printf("%dx%d", _machine->getWidth(), _machine->getHeight());
            StatusBar->SetStatusText(res, StatusBarFieldsResolution);
        }

        // Check if we must play a sound
        if (_machine->timerSound() != 0 && _machine->getType() != MCHIP8) {
            // Play beep
            if (_mcSound != NULL)
                if (_mcSound[0] != NULL) {
                    Mix_PlayChannel( -1, _mcSound[0], 0 );
                }
        } else {
            if (_machine->getSoundRefresh() && _machine->getSoundState() == OPEN) {

                // Get output specs
                Mix_QuerySpec(&freq, &format, &channels);
                freqRatio = (freq / _machine->getSoundBufferFrequency());

                // Initialize the buffer
                convertedSoundBuffer = (unsigned char*) malloc(_machine->getSoundBufferSize() * freqRatio);
                if (freqRatio == 1) {
                    memcpy(convertedSoundBuffer, _machine->getSoundBuffer(), _machine->getSoundBufferSize());
                } else {
                    // Convert frequency
                    unsigned char *buf = _machine->getSoundBuffer();
                    for (unsigned int i = 0; i < (_machine->getSoundBufferSize() * freqRatio); i++) {
                        int index = (int)floor(i / freqRatio);
                        convertedSoundBuffer[i] = buf[index];
                    }
                }

                // Load our music
                _mcSound[_currentSound] = Mix_QuickLoad_RAW(convertedSoundBuffer, _machine->getSoundBufferSize() * freqRatio);
                if( _mcSound[_currentSound] == NULL )
                {
                    wxPrintf( "Failed to load Mega-Chip Music! SDL_mixer Error: %s\n", Mix_GetError() );
                    _mcSound[_currentSound] = NULL;
                }
                _MusicIsPlaying = true;

                if (_mcSound[_currentSound] != NULL) {
                    int nextChannel = Mix_PlayChannel( -1, _mcSound[_currentSound], _machine->getSoundRepeat() );
                    _channels.push(nextChannel);
                    if (_currentSound < 16) {
                        _currentSound++;
                    } else {
                        _currentSound = 1;
                    }
                }

            } else if (_MusicIsPlaying && _machine->getSoundState() == CLOSE) {
                if (_currentSound > 1) {
                    int lastChannel = --_currentSound;
                    if (_mcSound[lastChannel]) {
                        Mix_Pause(_channels.top());
                        _channels.pop();
                        Mix_FreeChunk(_mcSound[lastChannel]);
                        _mcSound[lastChannel] = NULL;
                    }
                } else
                    _MusicIsPlaying = false;
            }
        }
    }
}

string getFrequencyStr(long frequency) {
    string freqStr;
    float fDiv;
    int cnt;
    static const char *endStr[] = {"Mhz","Khz","Hz"};

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
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[Mega8Config::getInstance().getFrequencyRatio(_machine->getType())];
            baseFrequency = _frequency;
        }

        // Set Status Frequency
        StatusBar->SetStatusText(getFrequencyStr(baseFrequency).c_str(), StatusBarFieldsFrequency);

        // Adjust Menu's texts { 1, 2, 10, 40, 80, 160, 320, 640 }
        /*MenuSpeed1024->SetItemLabel(wxString::Format(_("1024x - %s"), getFrequencyStr(BASE_FREQ * 40960).c_str()));
        if (Multiplicator == (40 * 1024)) {
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 10);
            MenuSpeed1024->Check(true);
        }*/

        MenuSpeed256->SetItemLabel(wxString::Format(_("256x - %s"), getFrequencyStr(BASE_FREQ * 10240).c_str()));
        if (Multiplicator == (40 * 256)) {
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 9);
            MenuSpeed256->Check(true);
        }

        MenuSpeed32->SetItemLabel(wxString::Format(_("32x - %s"), getFrequencyStr(BASE_FREQ * 1280).c_str()));
        if (Multiplicator == (40 * 32)) {
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 8);
            MenuSpeed32->Check(true);
        }

        MenuSpeed16->SetItemLabel(wxString::Format(_("16x - %s"), getFrequencyStr(BASE_FREQ * 640).c_str()));
        if (Multiplicator == (40 * 16)) {
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 7);
            MenuSpeed16->Check(true);
        }

        MenuSpeed8->SetItemLabel(wxString::Format(_("8x - %s"), getFrequencyStr(BASE_FREQ * 320).c_str()));
        if (Multiplicator == (40 * 8)) {
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 6);
            MenuSpeed8->Check(true);
        }
        MenuSpeed4->SetItemLabel(wxString::Format(_("4x - %s"), getFrequencyStr(BASE_FREQ * 160).c_str()));
        if (Multiplicator == (40 * 4)) {
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 5);
            MenuSpeed4->Check(true);
        }
        MenuSpeed2->SetItemLabel(wxString::Format(_("2x - %s"), getFrequencyStr(BASE_FREQ * 80).c_str()));
        if (Multiplicator == (40 * 2)) {
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 4);
            MenuSpeed2->Check(true);
        }
        MenuSpeed1->SetItemLabel(wxString::Format(_("1x - %s"), getFrequencyStr(BASE_FREQ * 40).c_str()));
        if (Multiplicator == 40) {
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 3);
            MenuSpeed1->Check(true);
        }

        MenuSpeedDiv4->SetItemLabel(wxString::Format(_("1/4x - %s"), getFrequencyStr(BASE_FREQ * 10).c_str()));
        if (Multiplicator == (40 / 4)) {
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 2);
            MenuSpeedDiv4->Check(true);
        }
        MenuSpeedDiv20->SetItemLabel(wxString::Format(_("1/20x - %s"), getFrequencyStr(BASE_FREQ * 2).c_str()));
        if (Multiplicator == (40 / 20)) {
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 1);
            MenuSpeedDiv20->Check(true);
        }
        MenuSpeedDiv40->SetItemLabel(wxString::Format(_("1/40x - %s"), getFrequencyStr(BASE_FREQ).c_str()));
        if (Multiplicator == (40 / 40)) {
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 0);
            MenuSpeedDiv40->Check(true);
        }

        // Save
        if (_frequency != -1) {
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), Mega8Config::getInstance().getFrequencyRatio(_machine->getType()) );
            SetSpeedAuto(false);
        }
        else
            SetSpeedAuto(true);
    }
}

void Mega8Frame::SetSpeedAuto(bool value)
{
    Mega8Config::getInstance().setSpeedAuto(value);
    MenuSpeedAuto->Check(value);
    if (value) {
        _frequency = -1;
    } else {
        _frequency = FREQ_MENU_INDEX[Mega8Config::getInstance().getFrequencyRatio(_machine->getType())] * BASE_FREQ;
    }
}

void Mega8Frame::OnCPUThreadCompletion(wxThreadEvent&)
{
    // Free sound if not already done...
    if (_machine) {
        if (_machine->getSoundBuffer() == NULL) {
            for (int i = BEEP_CHANNEL + 1; i < _maxChannels; i++) {
                if (_mcSound[i] != NULL) {
                    Mix_FreeChunk(_mcSound[i]);
                    _mcSound[i] = NULL;
                }
            }
            _MusicIsPlaying = false;
        }
    }
}

void Mega8Frame::DeleteCPUThread()
{
    if (_Paused)
        DoResumeCPUThread();

    {
        wxCriticalSectionLocker enter(_CPUThreadCS);
        if (_CPUThread)         // does the thread still exist?
        {
            wxPrintf("Chip8CPUFrame: deleting thread\n");
            if (_CPUThread->Delete() != wxTHREAD_NO_ERROR ) {
                wxLogError("Can't delete the thread!");
            }
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
                break;
            }
            _CPUThread->Delete();
            wxPrintf("Chip8CPUFrame: Waiting thread to delete...\n");
        }
        // wait for thread completion
        wxThread::This()->Sleep(1);
    }
}

void Mega8Frame::onIdle(wxIdleEvent &event) {
    // Frame Limit
    static wxStopWatch chrono;
    static wxLongLong t_start = chrono.TimeInMicro();
    static wxLongLong t_end = chrono.TimeInMicro();
    wxLongLong t;

    if (!_exit) {
        if (_machine) {
            if (_machine->loaded() ) {
                // Determine if we use sleep (May slow down emulation but may the CPU be cooler)
                bool useSleep = Mega8Config::getInstance().getUseSleep();

                t_end = chrono.TimeInMicro();
                t =  t_end - t_start;

                // Frame Limit: 300 FPS
                if (_machine->getScreen() != NULL && t >= 3333.33) {
                    GLDisplay->Render(_machine->getScreen(), _machine->getWidth(), _machine->getHeight());
                    GLDisplay->Flip();

                    chrono.Start(0);
                    t_start = chrono.TimeInMicro();

                    updateStatusFPS();
                }

                // Update joystick if we have
                if (Mega8Config::getInstance().hasJoystick() && !_Paused) {
                    Joysticks::getInstance().initJoysticks();
                    Joystick *joy = Joysticks::getInstance().updateJoyState();
                    // If there is a state update in one of the joysticks
                    if (joy) {
                        int *keys = Mega8Config::getInstance().getKeys();
                        for (int i = 0; i < 16; i++) {
                            if (Mega8Config::getInstance().getKeyIsJoy(i)) {
                                // Convert joy value to code
                                _machine->setKey(i, Joysticks::getInstance().checkCodeFromJoystick(joy, keys[i]));
                            }
                        }
                    }
                }
            }
        }

        // render continuously, not only once on idle
        event.RequestMore();
    }
}

void Mega8Frame::OnPaint(wxPaintEvent &event) {
    if (_machine) {
        if (_machine->loaded()) {
            if (_machine->getScreen() != NULL) {
                GLDisplay->Render(_machine->getScreen(), _machine->getWidth(), _machine->getHeight());
                GLDisplay->Flip();
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
    wxStopWatch chrono;

    chrono.Start(0);
    wxLongLong t_start = chrono.TimeInMicro();
    wxLongLong t_end = chrono.TimeInMicro();
    wxLongLong t;
    unsigned long baseFrequency, currentFrequency;

    // TestDestroy is called to ensure that calls Pause(), Delete(),...
    while (!TestDestroy()) {

        // Determine if we use sleep (May slow down emulation but may the CPU be cooler)
        bool useSleep = Mega8Config::getInstance().getUseSleep();

        // Calc elapsed time
        //if (!useSleep) {
        t_end = chrono.TimeInMicro();
        t = (t_end - t_start);
        //}

        // Base frequency for this type of chip
        baseFrequency = _handler->_machine->getFrequencyMultiplicator() * BASE_FREQ;

        // Get actual frequency
        if (_handler->_frequency != -1)
            // User sets the frequency
            currentFrequency = _handler->_frequency;
        else {
            // Frequency is set on auto
            currentFrequency = _handler->_machine->getFrequencyMultiplicator() * BASE_FREQ;
        }

        /*wxPrintf("Start: %ld - End: %ld - T: %ld - Current frequency: %.5f\n", t_start,
                                                                                t_end,
                                                                                t,
                                                                                getMicroFromHertz(currentFrequency));*/

        // Can we execute the next instruction ?
        if (t >= getMicroFromHertz(currentFrequency)) {
            if (_handler->_machine) {
                if (_handler->_machine->loaded()) {
                    // Execute one opcode and give the actual frequency so the chip can know when he must decrease the clocks
                    _handler->_machine->execute(getMilliFromHertz(currentFrequency));
                }
            }

            // If base frequency changed, we changed Chip-8's type
            if (baseFrequency != _handler->_machine->getFrequencyMultiplicator() * BASE_FREQ) {
                if (Mega8Config::getInstance().getSpeedAuto()) {
                    // Search for new frequency multiplicator
                    for (int i = 0; i < (int)(sizeof(FREQ_MENU_INDEX) / sizeof(float)); i++)
                        if (FREQ_MENU_INDEX[i] == _handler->_machine->getFrequencyMultiplicator()) {
                            Mega8Config::getInstance().setFrequencyRatio(_handler->_machine->getType(), i);
                        }
                }

                // Adjust frequency with new type of Chip based upon what the user has chosen for it
                // Or if SpeedAuto is set, it's the Chip that decides
                _handler->updateFrequency(FREQ_MENU_INDEX[Mega8Config::getInstance().getFrequencyRatio(_handler->_machine->getType())]);
            }

            wxQueueEvent(_handler, new wxThreadEvent(wxEVT_COMMAND_CPUTHREAD_UPDATE));

            // Restart Counter
            //if (!useSleep) {
            chrono.Start(0);
            t_start = chrono.TimeInMicro();
            //}
        }

        // Sleep some micro-seconds (half of waiting time)
        if (useSleep) {
            //wxPrintf("Sleeping for %ld microseconds...\n", (unsigned long)getMicroFromHertz(currentFrequency));
            wxMicroSleep((unsigned long)getMicroFromHertz(currentFrequency) / 2);
            //usleep((unsigned long)getMicroFromHertz(currentFrequency));
        }
    }
    // signal the event handler that this thread is going to be destroyed
    // NOTE: here we assume that using the Handler pointer is safe,
    //       (in this case this is assured by the Frame destructor)

    if (_handler && _handler->_machine)
        if (!_handler->_exit)
            wxQueueEvent(_handler, new wxThreadEvent(wxEVT_COMMAND_CPUTHREAD_COMPLETED));

    return (wxThread::ExitCode)0;    // success
}

void Mega8Frame::OnFilter(wxCommandEvent& event)
{
    SetFiltered(!Mega8Config::getInstance().getFiltered());
}

void Mega8Frame::OnReset(wxCommandEvent& event)
{
    Reset();
}

void Mega8Frame::Reset() {
    if (_machine) {
        hardReset();

        if (CurrentRomPath == wxEmptyString)
            _machine->loadBios();
        else
            _machine->loadGame(CurrentRomPath.mb_str());

        // Restart the thread
        DoStartCPUThread();
    }
}

void Mega8Frame::OnPause(wxCommandEvent& event)
{
    SetPause(!_Paused);
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

void Mega8Frame::softReset() {
    // Soft reset
    DoPauseCPUThread();

    // Free music if not already freed
    for (int i = BEEP_CHANNEL + 1; i < _maxChannels; i ++) {
        if (_mcSound[i] != NULL) {
            Mix_FreeChunk(_mcSound[i]);
            _mcSound[i] = NULL;
        }
    }
    _MusicIsPlaying = false;

    _machine->initialize(_machine->getType());

    // Initialize machine
    SetSyncClock(Mega8Config::getInstance().getSyncClock());
    SetFiltered(Mega8Config::getInstance().getFiltered());
    SetFullScreenMode(Mega8Config::getInstance().getFullScreen());
    SetColorTheme(Mega8Config::getInstance().getColorTheme());
}

void Mega8Frame::hardReset() {
    // Free some resources
    GLDisplay->setStopRender(true);

    wxString title = APP_TITLE;
    SetTitle(title);

    // Free music if not already freed
    for (int i = BEEP_CHANNEL + 1; i < _currentSound + 1; i ++) {
        if (_mcSound[i] != NULL) {
            Mix_FreeChunk(_mcSound[i]);
            _mcSound[i] = NULL;
        }
    }
    _MusicIsPlaying = false;

    // Delete CPU Thread
    if (_CPUThread) {
        if (_Paused)
            DoResumeCPUThread();

        DeleteCPUThread();
        delete _CPUThread;
        _CPUThread = NULL;
    }

    // Make a "hard" reset
	_machine->destroy();
    delete _machine;
    _machine = new Chip8();
    _machine->initialize(CHIP8);

    // Initialize machine
    GLDisplay->setStopRender(false);
    //Mega8Config::getInstance().saveConfig(CurrentRomName);
    Mega8Config::getInstance().reloadConfig(CurrentRomName);
    SetSyncClock(Mega8Config::getInstance().getSyncClock());
    SetFiltered(Mega8Config::getInstance().getFiltered());
    SetFullScreenMode(Mega8Config::getInstance().getFullScreen());
    SetColorTheme(Mega8Config::getInstance().getColorTheme());
    SetSound(Mega8Config::getInstance().getSound());
    SetUseSleep(Mega8Config::getInstance().getUseSleep());
}

void Mega8Frame::CloseRom() {
    hardReset();
    _machine->loadBios();
    //CurrentRomName = CurrentRomPath = wxEmptyString;
    DoStartCPUThread();
}

void Mega8Frame::OnMenuClose(wxCommandEvent& event)
{
    CloseRom();
}

void Mega8Frame::OnMenuSpeedSelected(wxCommandEvent& event)
{
    if (_machine) {
        if (!_Paused && _machine->isRunning()) {
            SetPause(true);
        }

        long baseFrequency = _machine->getFrequencyMultiplicator() * BASE_FREQ;

        if (event.GetId() == Mega8Frame::idMenuSpeedAuto) {
            _frequency = -1;
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 0);
            SetSpeedAuto(true);
        } else {
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), Mega8Config::getInstance().getFrequencyRatio(_machine->getType()));
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[Mega8Config::getInstance().getFrequencyRatio(_machine->getType()) ];
            SetSpeedAuto(false);
        }

        if (event.GetId() == Mega8Frame::idMenuSpeedDiv40) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[0];
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 0);
        }

        if (event.GetId() == Mega8Frame::idMenuSpeedDiv20) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[1];
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 1);
        }

        if (event.GetId() == Mega8Frame::idMenuSpeedDiv4) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[2];
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 2);
        }

        if (event.GetId() == Mega8Frame::idMenuSpeed1) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[3];
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 3);
        }

        if (event.GetId() == Mega8Frame::idMenuSpeed2) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[4];
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 4);
        }

        if (event.GetId() == Mega8Frame::idMenuSpeed4) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[5];
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 5);
        }

        if (event.GetId() == Mega8Frame::idMenuSpeed8) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[6];
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 6);
        }

        if (event.GetId() == Mega8Frame::idMenuSpeed16) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[7];
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 7);
        }

        if (event.GetId() == Mega8Frame::idMenuSpeed32) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[8];
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 8);
        }

        if (event.GetId() == Mega8Frame::idMenuSpeed256) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[9];
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 9);
        }

        /*if (event.GetId() == Mega8Frame::idMenuSpeed1024) {
            _frequency = BASE_FREQ * FREQ_MENU_INDEX[10];
            Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), 10);
        }*/

        // Update CPU Frequency
        updateFrequency(FREQ_MENU_INDEX[Mega8Config::getInstance().getFrequencyRatio(_machine->getType()) ]);

        if (_Paused) {
            SetPause(false);
        }
    }
}

void Mega8Frame::OnMenuSizeSelected(wxCommandEvent& event)
{
    if (_machine) {

        if (event.GetId() == Mega8Frame::idMenuFullScreen) {
            SetFullScreenMode(MnuFullScreen->IsChecked());
            return;
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

void Mega8Frame::updateStatusFPS() {
    wxString FPS;

    static wxStopWatch sw;
    static wxLongLong start = sw.Time();
    wxLongLong end = 0;
    wxLongLong t;

    if (_machine) {
        if (_machine->loaded()) {
            // Refresh every seconds
            end = sw.Time();
            t = end - start;
            if (t >= 1000) {
                FPS.Printf("%d FPS", GLDisplay->getFPS());

                if (FPS != StatusBar->GetStatusText(StatusBarFieldsFramePerSec))
                    StatusBar->SetStatusText(FPS, StatusBarFieldsFramePerSec);

                if (Mega8Config::getInstance().getDisplayHUD()) {
                    wxString  mode = wxT("@ ");
                    mode << StatusBar->GetStatusText(StatusBarFieldsMode);
                    GLDisplay->PrintGLInfos(5, 0x00FF0000, (const char*) mode.mb_str());
                    GLDisplay->PrintGLInfos(4, 0x00FF0000, (const char*) StatusBar->GetStatusText(StatusBarFieldsResolution).mb_str());
                    //GLDisplay->PrintGLInfos(4, 0x00FF00, (const char*) StatusBar->GetStatusText(StatusBarFieldsFramePerSec).mb_str());
                    GLDisplay->PrintGLInfos(3, 0x00FF0000, GLDisplay->getFiltered() ? _("Filter on").mb_str(): _("Filter off").mb_str());
                    GLDisplay->PrintGLInfos(2, 0x00FF0000, (const char*) StatusBar->GetStatusText(StatusBarFieldsStatus).mb_str());
                    GLDisplay->PrintGLInfos(1, 0xFF000000, (const char*) FPS.mb_str());
                }
                sw.Start(0);
                start = sw.Time();
            }
        }
    } else {
        StatusBar->SetStatusText(_("Waiting"), StatusBarFieldsStatus);
    }
}

void Mega8Frame::OnKeyDown(wxKeyEvent& event)
{
    int *keys;
    int keyCode = event.GetKeyCode();
    int freqRatio;

    // Check Machine Keys
    if (!wxGetKeyState(WXK_CONTROL)) {
        keys = Mega8Config::getInstance().getKeys();
        for (int i = 0; i < 16; i++) {
            // If it's a code for joystick, bypass it
            if (!Mega8Config::getInstance().getKeyIsJoy(i)) {
                // Check lowercase
                if (keys[i] < 128 && keys[i] > 96) {
                    keys[i] -= 0x20;
                }
                if (keys[i] == keyCode) {
                    // We have pushed a key
                    _machine->setKey(i, true);
                }
            }
        }

        // Exit application
        if (event.GetKeyCode() == WXK_ESCAPE) {
            exitApplication();
        }

    } else {
        // Mega8 Commands
        switch (event.GetKeyCode()) {
            case WXK_NUMPAD_ADD:
                if (wxGetKeyState(WXK_CONTROL)) {
                    // Pause the thread
                    if (!_Paused && _machine->isRunning()) {
                        SetPause(true);
                    }
                    MenuSpeedAuto->Check(false);
                    _frequency = 0;

                    freqRatio = Mega8Config::getInstance().getFrequencyRatio(_machine->getType());
                    if (freqRatio >= (sizeof(FREQ_MENU_INDEX)/sizeof(float)) - 1)
                        freqRatio = 0;
                    else
                        freqRatio++;
                    Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), freqRatio);

                    updateFrequency(FREQ_MENU_INDEX[Mega8Config::getInstance().getFrequencyRatio(_machine->getType())]);
                    if (_Paused) {
                        SetPause(false);
                    }
                }
                break;

            case WXK_NUMPAD_SUBTRACT:
                if (wxGetKeyState(WXK_CONTROL)) {
                    if (!_Paused && _machine->isRunning()) {
                        SetPause(true);
                    }
                    MenuSpeedAuto->Check(false);
                    _frequency = 0;

                    freqRatio = Mega8Config::getInstance().getFrequencyRatio(_machine->getType());
                    if (freqRatio > 0)
                        freqRatio--;
                    else
                        freqRatio = (sizeof(FREQ_MENU_INDEX) / sizeof(float) ) -1;
                    Mega8Config::getInstance().setFrequencyRatio(_machine->getType(), freqRatio);

                    updateFrequency(FREQ_MENU_INDEX[Mega8Config::getInstance().getFrequencyRatio(_machine->getType())]);

                    if (_Paused) {
                        SetPause(false);
                    }
                }
                break;

            // Commands
            case WXK_SPACE:
                SetPause(!_Paused);
                break;

            case wxKeyCode('O'):
                DoOpen();
                break;

            case wxKeyCode('C'):
                CloseRom();
                break;

            case wxKeyCode('A'):
                SetSpeedAuto(!Mega8Config::getInstance().getSpeedAuto());
                break;

            case wxKeyCode('F'):
                SetFullScreenMode(!Mega8Config::getInstance().getFullScreen());
                break;

            case wxKeyCode('R'):
                Reset();
                break;

            case wxKeyCode('L'):
                SetFiltered(!Mega8Config::getInstance().getFiltered());
                break;

            case wxKeyCode('Y'):
                SetSyncClock(!Mega8Config::getInstance().getSyncClock());
                break;

            case wxKeyCode('K'):
                _machine->initialize(_machine->getType());
                _machine->loadKeypad();
                break;

            case wxKeyCode('H'):
                SetDisplayHUD(!GLDisplay->getDisplayHUD());
                break;

            case wxKeyCode('M'):
                SetSound(!Mega8Config::getInstance().getSound());
                break;

            case wxKeyCode('S'):
                // Screen capture
                DoScreenshot();
                break;
        }
    }
}

void Mega8Frame::OnKeyUp(wxKeyEvent& event)
{
    int *keys;

    if (!wxGetKeyState(WXK_CONTROL)) {
        keys = Mega8Config::getInstance().getKeys();
        for (int i = 0; i < 16; i++) {
            if (keys[i] == event.GetKeyCode()) {
                // We have pushed a key
                _machine->setKey(i, false);
            }
        }
    }
}

void Mega8Frame::SetSound(bool value) {
    MenuSound->Check(value);
    Mega8Config::getInstance().setSound(value);

    // Set Sound Status
    StatusBar->SetStatusText((value) ? wxT("On") : wxT("Off"), StatusBarFieldsSound);

    // Set Volume
    Mix_Volume(-1, value ? 128 : 0);
}

void Mega8Frame::SetUseSleep(bool value) {
    MenuUseSleep->Check(value);
    Mega8Config::getInstance().setUseSleep(value);
}

void Mega8Frame::SetDisplayHUD(bool value) {
    MenuDisplayHUD->Check(value);
    GLDisplay->setDisplayHUD(value);
    Mega8Config::getInstance().setDisplayHUD(value);
}

void Mega8Frame::SetPause(bool value) {
    if (value) {
        DoPauseCPUThread();
        if (_MusicIsPlaying) {
            Mix_Pause(-1);
        }
        _Paused = value;
        StatusBar->SetStatusText(wxT("Paused"), StatusBarFieldsStatus);
    } else {
        DoResumeCPUThread();
        if (_MusicIsPlaying) {
            Mix_Resume(-1);
        }
        _Paused = value;
        StatusBar->SetStatusText(wxT("Running"), StatusBarFieldsStatus);
    }
}

void Mega8Frame::SetFiltered(bool value) {
    if (_machine) {
        GLDisplay->setFiltered(value);
        MnuFilter->Check(value);
        Mega8Config::getInstance().setFiltered(value);
    }
}

void Mega8Frame::SetSyncClock(bool value) {
    if (_machine) {
        _machine->SetSyncClockToOriginal(value);
        MnuSyncClock->Check(value);
        Mega8Config::getInstance().setSyncClock(value);
    }
}

void Mega8Frame::SetFullScreenMode(bool value) {
    if (_machine) {
        MnuFullScreen->Check(value);
        ShowFullScreen(value);
        Mega8Config::getInstance().setFullScreen(value);
    }
}

void Mega8Frame::SetColorTheme(Chip8ColorTheme value, bool updateCfg)
{
    if (_machine) {
        // No need to change if we are already in that color theme
        if (_machine->getColorTheme() != value) {
            _machine->setColorTheme(value);
            switch (value) {
                case BLUE:
                    MnuCTBlue->Check(true);
                    break;
                case RED:
                    MnuCTRed->Check(true);
                    break;
                case GREEN:
                    MnuCTGreen->Check(true);
                    break;
                case C64:
                    MnuCTC64->Check(true);
                    break;
                case GAMEBOY:
                    MnuCTGameBoy->Check(true);
                    break;
                case DEFAULT:
                default:
                    MnuCTDefault->Check(true);
                    break;
            }

            // In case I disabled it for Mega emulation, don't save
            if (updateCfg)
                Mega8Config::getInstance().setColorTheme(value);
        }
    }
}

void Mega8Frame::SetInverseColor(bool value, bool updateCfg)
{
    if (_machine) {
        if (_machine->getInverseColor() != value) {
            _machine->setInverseColor(value);

            // In case I disabled it for Mega emulation, don't save
            if (updateCfg)
                Mega8Config::getInstance().setInverseColor(value);
        }
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
            SetColorTheme(INVERSE);
        }

        if (event.GetId() == Mega8Frame::idMenuCTC64) {
            SetColorTheme(C64);
        }

        if (event.GetId() == Mega8Frame::idMenuCTGameBoy) {
            SetColorTheme(GAMEBOY);
        }

        if (event.GetId() == Mega8Frame::idMenuCTDefault) {
            SetColorTheme(DEFAULT);
        }

        if (event.GetId() == Mega8Frame::idMenuCTRed) {
            SetColorTheme(RED);
        }

        if (event.GetId() == Mega8Frame::idMenuCTGreen) {
            SetColorTheme(GREEN);
        }

        if (event.GetId() == Mega8Frame::idMenuCTBlue) {
            SetColorTheme(BLUE);
        }
    }
}

void Mega8Frame::OnMenuColorInverse(wxCommandEvent& event)
{
    SetInverseColor(event.IsChecked());
}

void Mega8Frame::OnMenuDisplayHUDSelected(wxCommandEvent& event)
{
    SetDisplayHUD(MenuDisplayHUD->IsChecked());
}

void Mega8Frame::OnMenuCheckKeypadSelected(wxCommandEvent& event)
{
    SetPause(true);
    if (CurrentRomName != wxEmptyString)
        dlgInput = new InputDialog(CurrentRomName, this);
    else
        dlgInput = new InputDialog(wxT("General"), this);

    wxTheApp->SetTopWindow(dlgInput);

    if (dlgInput->ShowModal() == wxID_OK) {
        // Reload Config - Just to be sure we're up to date
        Mega8Config::getInstance().reloadConfig(CurrentRomName);
    }
    dlgInput->Destroy();
    SetPause(false);
}

void Mega8Frame::OnMenuUseSleepSelected(wxCommandEvent& event)
{
    SetUseSleep(!Mega8Config::getInstance().getUseSleep());
}

void Mega8Frame::OnMenuScreenshotSelected(wxCommandEvent& event)
{
    DoScreenshot();
}

void Mega8Frame::OnMenuSoundSelected(wxCommandEvent& event)
{
    SetSound(!Mega8Config::getInstance().getSound());
}
