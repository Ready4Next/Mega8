/***************************************************************
 * Name:      Chip8EmuMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Junta (junta@hotmail.be)
 * Created:   2014-07-24
 * Copyright: Junta (http://www.ready4next.be)
 * License:
 **************************************************************/

#include "Chip8EmuMain.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <wx/msgdlg.h>
#include <wx/dcmemory.h>
#include <wx/dcclient.h>
#include <wx/dcbuffer.h>
#include <wx/bitmap.h>
#include <wx/filedlg.h>
#include <wx/image.h>
#include <wx/thread.h>
#include <wx/stdpaths.h>

#include <chrono>

//(*InternalHeaders(Chip8EmuFrame)
#include <wx/string.h>
#include <wx/intl.h>
//*)

#define APP_TITLE "S/HiRes/Chip-8 Emulator"
#define APP_VERSION "1.0"
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
    wxbuild << _T("\nTTF \"Game Over\" by Pedro Muñoz Pastor\n");
    wxbuild << _T("\nKeypad Test [Hap, 2006]\n");
    wxbuild << _T("\nChip8 emulator Logo [Garstyciuks]");
    wxbuild << _T("\n\nShortcuts:\n");
    wxbuild << _T("ESC: Quit\t\tO: Open\nF: Fullscreen\tH: Display HUD\nL: Filter\t\tR: Reset\nC: Close\t\tSpace: Pause / Resume\nS: Sync Clock @60 Hz\nCTRL +/-: Increase / Decrease frequency\nChip-8 Keypad (0-F): Numpad (0-9.+-/*ENTER)\nK: Check Keypad Test");

    return wxbuild;
}

Mix_Chunk *gBeep = NULL;
const long Chip8EmuFrame::idChipGLCanvas = wxNewId();

const int Chip8EmuFrame::StatusBarFieldsStatus = 1;
const int Chip8EmuFrame::StatusBarFieldsMode = 2;
const int Chip8EmuFrame::StatusBarFieldsResolution = 3;
const int Chip8EmuFrame::StatusBarFieldsFrequency = 4;
const int Chip8EmuFrame::StatusBarFieldsSound = 5;
const int Chip8EmuFrame::StatusBarFieldsFramePerSec = 6;

//(*IdInit(Chip8EmuFrame)
const long Chip8EmuFrame::idMenuOpen = wxNewId();
const long Chip8EmuFrame::idMenuClose = wxNewId();
const long Chip8EmuFrame::idMenuQuit = wxNewId();
const long Chip8EmuFrame::idMenuReset = wxNewId();
const long Chip8EmuFrame::idMenuPause = wxNewId();
const long Chip8EmuFrame::idMenuCTDefault = wxNewId();
const long Chip8EmuFrame::idMenuCTC64 = wxNewId();
const long Chip8EmuFrame::idMenuCTGameBoy = wxNewId();
const long Chip8EmuFrame::idMenuCTRed = wxNewId();
const long Chip8EmuFrame::idMenuCTGreen = wxNewId();
const long Chip8EmuFrame::idMenuCTBlue = wxNewId();
const long Chip8EmuFrame::idMenuCTInverse = wxNewId();
const long Chip8EmuFrame::ID_MENUITEM3 = wxNewId();
const long Chip8EmuFrame::idMenuSpeedDiv20 = wxNewId();
const long Chip8EmuFrame::idMenuSpeedDiv10 = wxNewId();
const long Chip8EmuFrame::idMenuSpeedDiv2 = wxNewId();
const long Chip8EmuFrame::idMenuSpeed1 = wxNewId();
const long Chip8EmuFrame::idMenuSpeed2 = wxNewId();
const long Chip8EmuFrame::idMenuSpeed5 = wxNewId();
const long Chip8EmuFrame::idMenuSpeed10 = wxNewId();
const long Chip8EmuFrame::idSyncToBase = wxNewId();
const long Chip8EmuFrame::idMenuSpeed = wxNewId();
const long Chip8EmuFrame::idMenuDisplayHUD = wxNewId();
const long Chip8EmuFrame::idMenuFiltered = wxNewId();
const long Chip8EmuFrame::idMenuSize1x1 = wxNewId();
const long Chip8EmuFrame::idMenuSize2x2 = wxNewId();
const long Chip8EmuFrame::idMenuSize3x3 = wxNewId();
const long Chip8EmuFrame::idMenuSize4x4 = wxNewId();
const long Chip8EmuFrame::idMenuSize5x5 = wxNewId();
const long Chip8EmuFrame::idMenuSize10x10 = wxNewId();
const long Chip8EmuFrame::idMenuFullScreen = wxNewId();
const long Chip8EmuFrame::ID_MENUITEM1 = wxNewId();
const long Chip8EmuFrame::ID_MENUITEM2 = wxNewId();
const long Chip8EmuFrame::idMenuAbout = wxNewId();
const long Chip8EmuFrame::ID_STATUSBAR1 = wxNewId();
//*)

wxDEFINE_EVENT(wxEVT_COMMAND_CPUTHREAD_COMPLETED, wxThreadEvent);
wxDEFINE_EVENT(wxEVT_COMMAND_CPUTHREAD_UPDATE, wxThreadEvent);

BEGIN_EVENT_TABLE(Chip8EmuFrame,wxFrame)
    //(*EventTable(Chip8EmuFrame)
    //*)
END_EVENT_TABLE()

Chip8EmuFrame::Chip8EmuFrame(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(Chip8EmuFrame)
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
    MenuSpeedDiv20 = new wxMenuItem(MenuItem8, idMenuSpeedDiv20, _("/20"), wxEmptyString, wxITEM_RADIO);
    MenuItem8->Append(MenuSpeedDiv20);
    MenuSpeedDiv10 = new wxMenuItem(MenuItem8, idMenuSpeedDiv10, _("/10"), wxEmptyString, wxITEM_RADIO);
    MenuItem8->Append(MenuSpeedDiv10);
    MenuSpeedDiv2 = new wxMenuItem(MenuItem8, idMenuSpeedDiv2, _("/2"), wxEmptyString, wxITEM_RADIO);
    MenuItem8->Append(MenuSpeedDiv2);
    MenuSpeed1 = new wxMenuItem(MenuItem8, idMenuSpeed1, _("1x"), wxEmptyString, wxITEM_RADIO);
    MenuItem8->Append(MenuSpeed1);
    MenuSpeed2 = new wxMenuItem(MenuItem8, idMenuSpeed2, _("2x"), wxEmptyString, wxITEM_RADIO);
    MenuItem8->Append(MenuSpeed2);
    MenuSpeed5 = new wxMenuItem(MenuItem8, idMenuSpeed5, _("5x"), wxEmptyString, wxITEM_RADIO);
    MenuItem8->Append(MenuSpeed5);
    MenuSpeed10 = new wxMenuItem(MenuItem8, idMenuSpeed10, _("10x"), wxEmptyString, wxITEM_RADIO);
    MenuItem8->Append(MenuSpeed10);
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

    Connect(idMenuOpen,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnOpen);
    Connect(idMenuClose,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnMenuClose);
    Connect(idMenuQuit,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnQuit);
    Connect(idMenuReset,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnReset);
    Connect(idMenuPause,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnPause);
    Connect(idMenuCTDefault,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnChooseColorTheme);
    Connect(idMenuCTC64,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnChooseColorTheme);
    Connect(idMenuCTGameBoy,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnChooseColorTheme);
    Connect(idMenuCTRed,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnChooseColorTheme);
    Connect(idMenuCTGreen,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnChooseColorTheme);
    Connect(idMenuCTBlue,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnChooseColorTheme);
    Connect(idMenuCTInverse,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnMenuColorInverse);
    Connect(idMenuSpeedDiv20,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnMenuSpeedSelected);
    Connect(idMenuSpeedDiv10,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnMenuSpeedSelected);
    Connect(idMenuSpeedDiv2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnMenuSpeedSelected);
    Connect(idMenuSpeed1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnMenuSpeedSelected);
    Connect(idMenuSpeed2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnMenuSpeedSelected);
    Connect(idMenuSpeed5,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnMenuSpeedSelected);
    Connect(idMenuSpeed10,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnMenuSpeedSelected);
    Connect(idSyncToBase,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnMenuSyncClock);
    Connect(idMenuDisplayHUD,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnMenuDisplayHUDSelected);
    Connect(idMenuFiltered,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnFilter);
    Connect(idMenuSize1x1,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnMenuSizeSelected);
    Connect(idMenuSize2x2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnMenuSizeSelected);
    Connect(idMenuSize3x3,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnMenuSizeSelected);
    Connect(idMenuSize4x4,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnMenuSizeSelected);
    Connect(idMenuSize5x5,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnMenuSizeSelected);
    Connect(idMenuSize10x10,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnMenuSizeSelected);
    Connect(idMenuFullScreen,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnMenuSizeSelected);
    Connect(ID_MENUITEM2,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnMenuCheckKeypadSelected);
    Connect(idMenuAbout,wxEVT_COMMAND_MENU_SELECTED,(wxObjectEventFunction)&Chip8EmuFrame::OnAbout);
    Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&Chip8EmuFrame::OnClose);
    //*)

    // Adjust Menu's texts
    MenuSpeed1->SetItemLabel(wxString::Format(_("1x - %.2f Khz"), (BASE_FREQ / 1000)));
    MenuSpeed1->Check(true);
    MenuSpeed2->SetItemLabel(wxString::Format(_("2x - %.2f Khz"), (BASE_FREQ * 2) / 1000));
    MenuSpeed5->SetItemLabel(wxString::Format(_("5x - %.2f Khz"), (BASE_FREQ * 5) / 1000));
    MenuSpeed10->SetItemLabel(wxString::Format(_("10x - %.2f Khz"), (BASE_FREQ * 10) / 1000));
    MenuSpeedDiv20->SetItemLabel(wxString::Format(_("1/20x - %.2f Hz"), BASE_FREQ / 20));
    MenuSpeedDiv10->SetItemLabel(wxString::Format(_("1/10x - %.2f Hz"), BASE_FREQ / 10));
    MenuSpeedDiv2->SetItemLabel(wxString::Format(_("1/2x - %.2f Hz"), BASE_FREQ / 2));

    // Create Panel
    int GLCanvasAttributes[] = {
    	WX_GL_RGBA,
    	WX_GL_DOUBLEBUFFER,
    	WX_GL_DEPTH_SIZE,      16,
    	WX_GL_STENCIL_SIZE,    0,
    	0, 0 };
    GLDisplay = new Chip8GL(this, idChipGLCanvas, GLCanvasAttributes);
    GLDisplay->setFiltered(false);

    // Update Graphics
    Connect(wxID_ANY,wxEVT_IDLE,(wxObjectEventFunction)&Chip8EmuFrame::onIdle);
    // CPU Thread
    Connect(wxID_ANY,wxEVT_COMMAND_CPUTHREAD_COMPLETED,(wxObjectEventFunction)&Chip8EmuFrame::OnCPUThreadCompletion);
    Connect(wxID_ANY,wxEVT_COMMAND_CPUTHREAD_UPDATE,(wxObjectEventFunction)&Chip8EmuFrame::OnCPUThreadUpdate);

    // Init SDL
    SDL_Init(SDL_INIT_AUDIO);
    // Init SDL_Mixer
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ) {
        printf("Unable to open audio! SDL_mixer Error: %s\n", Mix_GetError());
        exit(1);
    }

    // Get Application Directory
    wxString exePath = wxPathOnly(::wxStandardPaths::Get().GetExecutablePath());

    //Load sound effects
    gBeep = Mix_LoadWAV( exePath + "/bell.wav" );
    if( gBeep == NULL )
    {
        printf( "Failed to load beep sound effect! SDL_mixer Error: %s\n", Mix_GetError() );
        exit(1);
    }

    // Set base clock
    _frequency = BASE_FREQ;

    // Load "BIOS" ;-)
    CurrentRomDir = exePath;
    CurrentRomPath = wxEmptyString;
    _machine = new Chip8();
    SetSyncClock(MnuSyncClock->IsChecked());
    SetFiltered(MnuFilter->IsChecked());
    GLDisplay->setFiltered(MnuFilter->IsChecked());

    _machine->loadBios();                   // Reminds me Nintendo Logo when power up...
    SetTitle(APP_TITLE);

    DoStartCPUThread();
}

Chip8EmuFrame::~Chip8EmuFrame()
{
    delete _machine;
    //(*Destroy(Chip8EmuFrame)
    //*)
}

void Chip8EmuFrame::OnOpen(wxCommandEvent& event)
{
    DoOpen();
}

void Chip8EmuFrame::DoOpen() {
    wxFileDialog* OpenDialog = new wxFileDialog(
		this, _("Open a S/Chip-8 Rom"), CurrentRomDir, wxEmptyString,
		_("S/Chip-8 Roms (*.ch8)|*.ch8|All files (*.*)|*"),
		wxFD_OPEN, wxDefaultPosition);

	// Creates a "open file" dialog with 4 file types
	if (OpenDialog->ShowModal() == wxID_OK) // if the user click "Open" instead of "Cancel"
	{
		CurrentRomPath = OpenDialog->GetPath();
		CurrentRomDir = OpenDialog->GetDirectory();

		// Exit renderer
		while (GLDisplay->getIsRendering()) { _ExitRender = true; }

		// Sets our current document to the file the user selected
		// Delete CPU Thread
		if (_CPUThread) {
            if (_Paused)
                DoResumeCPUThread();

            DeleteCPUThread();
            delete _CPUThread;
            _CPUThread = NULL;
		}

		/*delete _machine;

		_machine = new Chip8();*/
		_machine->initialize(CHIP8);
		SetSyncClock(MnuSyncClock->IsChecked());
		SetFiltered(MnuFilter->IsChecked());
		_machine->loadGame(CurrentRomPath.mb_str()); //Opens that file
        wxString title = APP_TITLE;
        title << " " << OpenDialog->GetFilename();
		SetTitle(title); // Set the Title to reflect the file open

        DoStartCPUThread();
        _ExitRender = false;
	}

	// Clean up after ourselves
	OpenDialog->Destroy();
}

void Chip8EmuFrame::OnQuit(wxCommandEvent& event)
{
    Close();
}

void Chip8EmuFrame::OnAbout(wxCommandEvent& event)
{
    wxString msg = wxbuildinfo(long_f);
    wxMessageBox(msg, APP_TITLE);
}

void Chip8EmuFrame::OnClose(wxCloseEvent&)
{
    /* This is the cleaning up part */
	Mix_FreeChunk(gBeep);
	gBeep = NULL;
    Mix_Quit();

    DeleteCPUThread();
    Destroy();
}

/**************** CPUThread *******************/

void Chip8EmuFrame::DoStartCPUThread()
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

void Chip8EmuFrame::DoPauseCPUThread()
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
void Chip8EmuFrame::DoResumeCPUThread()
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

void Chip8EmuFrame::OnCPUThreadUpdate(wxThreadEvent&)
{
    if (_machine->timerSound() != 0) {
        Mix_PlayChannel( -1, gBeep, 0 );
    }
}

void Chip8EmuFrame::OnCPUThreadCompletion(wxThreadEvent&)
{
    // Nothing for now...
}

void Chip8EmuFrame::DeleteCPUThread()
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

void Chip8EmuFrame::onIdle(wxIdleEvent &event) {

    if (_machine) {
        if (_machine->loaded() && !_ExitRender) {
            if (_machine->getScreen() != NULL) {
                GLDisplay->Render(_machine->getScreen(), _machine->getWidth(), _machine->getHeight());
                GLDisplay->Flip();
            }
        }
    }

    updateStatus();

    event.RequestMore(); // render continuously, not only once on idle
}

void Chip8EmuFrame::OnPaint(wxPaintEvent &event) {
    if (_machine) {
        if (_machine->loaded() && !_ExitRender) {
            if (_machine->getScreen() != NULL) {
                GLDisplay->Render(_machine->getScreen(), _machine->getWidth(), _machine->getHeight());
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

    t_start = chrono::high_resolution_clock::now();

    // TestDestroy is called to ensure that calls Pause(), Delete(),...
    while (!TestDestroy()) {

        // Calc elapsed time
        t_end = chrono::high_resolution_clock::now();
        t = chrono::duration_cast<chrono::microseconds>(t_end - t_start).count();
        if (t >= getMicroFromHertz(_handler->_frequency)) {
            if (_handler->_machine) {
                if (_handler->_machine->loaded()) {
                    _handler->_machine->execute(getMilliFromHertz(_handler->_frequency));
                }
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

void Chip8EmuFrame::OnFilter(wxCommandEvent& event)
{
    GLDisplay->setFiltered(!GLDisplay->getFiltered());
}

void Chip8EmuFrame::OnReset(wxCommandEvent& event)
{
    Reset();
}

void Chip8EmuFrame::Reset() {
    _machine->initialize(_machine->getType());
    if (CurrentRomPath == wxEmptyString)
        _machine->loadBios();
    else
        _machine->loadGame(CurrentRomPath);
}

void Chip8EmuFrame::OnPause(wxCommandEvent& event)
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

void Chip8EmuFrame::CloseRom() {
    delete _machine;
    _machine = new Chip8();
    SetSyncClock(MnuSyncClock->IsChecked());
    SetFiltered(MnuFilter->IsChecked());
    _machine->loadBios();
}

void Chip8EmuFrame::OnMenuClose(wxCommandEvent& event)
{
    CloseRom();
}

void Chip8EmuFrame::OnMenuSpeedSelected(wxCommandEvent& event)
{
    _frequency = BASE_FREQ;

    if (event.GetId() == Chip8EmuFrame::idMenuSpeedDiv20) {
        _frequency = BASE_FREQ * FREQ_COEFF[6];
    }

    if (event.GetId() == Chip8EmuFrame::idMenuSpeedDiv10) {
        _frequency = BASE_FREQ * FREQ_COEFF[5];
    }

    if (event.GetId() == Chip8EmuFrame::idMenuSpeedDiv2) {
        _frequency = BASE_FREQ * FREQ_COEFF[4];
    }

    if (event.GetId() == Chip8EmuFrame::idMenuSpeed1) {
        _frequency = BASE_FREQ * FREQ_COEFF[3];
    }

    if (event.GetId() == Chip8EmuFrame::idMenuSpeed2) {
        _frequency = BASE_FREQ * FREQ_COEFF[2];
    }

    if (event.GetId() == Chip8EmuFrame::idMenuSpeed5) {
        _frequency = BASE_FREQ * FREQ_COEFF[1];
    }

    if (event.GetId() == Chip8EmuFrame::idMenuSpeed10) {
        _frequency = BASE_FREQ * FREQ_COEFF[0];
    }
}

void Chip8EmuFrame::OnMenuSizeSelected(wxCommandEvent& event)
{
    if (_machine) {
        if (event.GetId() == Chip8EmuFrame::idMenuFullScreen) {
            _FullScreen = !_FullScreen;
            ShowFullScreen(_FullScreen);
        }

        if (event.GetId() == Chip8EmuFrame::idMenuSize10x10) {
            wxSize size(_machine->getWidth() * SIZE_COEFF[5], _machine->getHeight() * SIZE_COEFF[5]);
            this->SetClientSize(size);

        }

        if (event.GetId() == Chip8EmuFrame::idMenuSize5x5) {
            wxSize size(_machine->getWidth() * SIZE_COEFF[4], _machine->getHeight() * SIZE_COEFF[4]);
            this->SetClientSize(size);

        }

        if (event.GetId() == Chip8EmuFrame::idMenuSize4x4) {
            wxSize size(_machine->getWidth() * SIZE_COEFF[3], _machine->getHeight() * SIZE_COEFF[3]);
            this->SetClientSize(size);

        }

        if (event.GetId() == Chip8EmuFrame::idMenuSize3x3) {
            wxSize size(_machine->getWidth() * SIZE_COEFF[2], _machine->getHeight() * SIZE_COEFF[2]);
            this->SetClientSize(size);
        }

        if (event.GetId() == Chip8EmuFrame::idMenuSize2x2) {
            wxSize size(_machine->getWidth() * SIZE_COEFF[1], _machine->getHeight() * SIZE_COEFF[1]);
            this->SetClientSize(size);
        }

        if (event.GetId() == Chip8EmuFrame::idMenuSize1x1) {
            wxSize size(_machine->getWidth() * SIZE_COEFF[0], _machine->getHeight() * SIZE_COEFF[0]);
            this->SetClientSize(size);
        }
    }
}

void Chip8EmuFrame::updateStatus() {
    wxString freq, status, mode, resolution, FPS;

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

        if (_frequency < 1000)
            freq.Printf("%d Hz", _frequency);
        else
            freq.Printf("%.2f Khz", _frequency / 1000.0);

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
        StatusBar->SetStatusText(freq, StatusBarFieldsFrequency);
        StatusBar->SetStatusText(status, StatusBarFieldsStatus);
        StatusBar->SetStatusText(resolution, StatusBarFieldsResolution);
        StatusBar->SetStatusText(FPS, StatusBarFieldsFramePerSec);
        StatusBar->SetStatusText(mode, StatusBarFieldsMode);

        if (GLDisplay->getDisplayHUD()) {
            mode.Printf("@ %s", mode);
            GLDisplay->PrintGLInfos(5, 0x00FF00, (const char*) mode.mb_str());
            GLDisplay->PrintGLInfos(4, 0x00FF00, (const char*) resolution.mb_str());
            GLDisplay->PrintGLInfos(3, 0x00FF00, (const char*) freq.mb_str());
            GLDisplay->PrintGLInfos(2, 0x00FF00, GLDisplay->getFiltered() ? _("Filter on"): _("Filter off"));
            GLDisplay->PrintGLInfos(1, 0x00FF00, (const char*) status.mb_str());
        }

    } else {
        StatusBar->SetStatusText(_("Waiting"), StatusBarFieldsStatus);
    }
}

void Chip8EmuFrame::OnKeyDown(wxKeyEvent& event)
{
    static int sizeCoeff = 0;
    static int freqCoeff = 0;

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
            if (freqCoeff == 0)
                freqCoeff = 5;
            else
                freqCoeff--;

            _frequency = BASE_FREQ * FREQ_COEFF[freqCoeff];
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
            if (freqCoeff == 5)
                freqCoeff = 0;
            else
                freqCoeff++;

            _frequency = BASE_FREQ * FREQ_COEFF[freqCoeff];
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
    }
}

void Chip8EmuFrame::OnKeyUp(wxKeyEvent& event)
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

void Chip8EmuFrame::SetFiltered(bool value) {
    if (_machine) {
        GLDisplay->setFiltered(value);
        MnuFilter->Check(value);
    }
}

void Chip8EmuFrame::SetSyncClock(bool value) {
    if (_machine) {
        _machine->SetSyncClockToOriginal(value);
        MnuSyncClock->Check(value);
    }
}

void Chip8EmuFrame::OnMenuSyncClock(wxCommandEvent& event)
{
    SetSyncClock(MnuSyncClock->IsChecked());
}

void Chip8EmuFrame::OnChooseColorTheme(wxCommandEvent& event)
{
    if (_machine) {
        if (event.GetId() == Chip8EmuFrame::idMenuCTInverse) {
            _machine->setColorTheme(INVERSE);
        }

        if (event.GetId() == Chip8EmuFrame::idMenuCTC64) {
                _machine->setColorTheme(C64);
        }

        if (event.GetId() == Chip8EmuFrame::idMenuCTGameBoy) {
                _machine->setColorTheme(GAMEBOY);
        }

        if (event.GetId() == Chip8EmuFrame::idMenuCTDefault) {
            _machine->setColorTheme(DEFAULT);
        }

        if (event.GetId() == Chip8EmuFrame::idMenuCTRed) {
            _machine->setColorTheme(RED);
        }

        if (event.GetId() == Chip8EmuFrame::idMenuCTGreen) {
            _machine->setColorTheme(GREEN);
        }

        if (event.GetId() == Chip8EmuFrame::idMenuCTBlue) {
            _machine->setColorTheme(BLUE);
        }
    }
}

void Chip8EmuFrame::OnMenuColorInverse(wxCommandEvent& event)
{
    if (_machine) {
        _machine->setInverseColor(event.IsChecked());
    }
}

void Chip8EmuFrame::OnMenuDisplayHUDSelected(wxCommandEvent& event)
{
    GLDisplay->setDisplayHUD(!GLDisplay->getDisplayHUD());
}

void Chip8EmuFrame::OnMenuCheckKeypadSelected(wxCommandEvent& event)
{
    _machine->initialize(_machine->getType());
    _machine->loadKeypad();
}
