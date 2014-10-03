#include "InputDialog.h"

//(*InternalHeaders(InputDialog)
#include <wx/string.h>
#include <wx/intl.h>
//*)

#include <wx/accel.h>       // Name of keys
#include "Mega8Config.h"
#include "kb-layout.xpm"
#include <wx/defs.h>

//(*IdInit(InputDialog)
const long InputDialog::ID_STATICBOX3 = wxNewId();
const long InputDialog::ID_STATICBOX1 = wxNewId();
const long InputDialog::ID_STATICBOX2 = wxNewId();
const long InputDialog::ID_BUTTON1 = wxNewId();
const long InputDialog::ID_BUTTON2 = wxNewId();
const long InputDialog::ID_BUTTON3 = wxNewId();
const long InputDialog::ID_STATICBITMAP1 = wxNewId();
const long InputDialog::ID_STATICTEXT1 = wxNewId();
const long InputDialog::ID_PANEL1 = wxNewId();
const long InputDialog::ID_STATICTEXT2 = wxNewId();
const long InputDialog::ID_STATICTEXT3 = wxNewId();
const long InputDialog::ID_STATICTEXT4 = wxNewId();
//*)

const long InputDialog::ID_TxtKeypad[] = { wxNewId(),  wxNewId(),  wxNewId(),  wxNewId(),
                                          wxNewId(),  wxNewId(),  wxNewId(),  wxNewId(),
                                          wxNewId(),  wxNewId(),  wxNewId(),  wxNewId(),
                                          wxNewId(),  wxNewId(),  wxNewId(),  wxNewId() };

// Corresponding index to the original Keypad
const byte KeypadCorrespondingIndex[] = { 1, 2, 3, 12, 4, 5, 6, 13, 7, 8, 9, 14, 10, 0, 11, 15 };

BEGIN_EVENT_TABLE(InputDialog,wxDialog)
	//(*EventTable(InputDialog)
	//*)
	EVT_IDLE(InputDialog::OnIdle)
END_EVENT_TABLE()

InputDialog::InputDialog(const wxString &CurrentProfile, wxWindow* parent,wxWindowID id, const wxPoint& pos, const wxSize& size)
{
	//(*Initialize(InputDialog)
	Create(parent, wxID_ANY, _("Edit configuration"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
	SetClientSize(wxSize(727,471));
	BoxKeypad = new wxStaticBox(this, ID_STATICBOX3, _(" Keyboard Mapping"), wxPoint(248,176), wxSize(464,248), 0, _T("ID_STATICBOX3"));
	StaticBox1 = new wxStaticBox(this, ID_STATICBOX1, _(" Keyboard Configuration "), wxPoint(8,8), wxSize(712,88), 0, _T("ID_STATICBOX1"));
	StaticBox2 = new wxStaticBox(this, ID_STATICBOX2, _(" Original keypad "), wxPoint(8,176), wxSize(224,248), 0, _T("ID_STATICBOX2"));
	BtnReset = new wxButton(this, ID_BUTTON1, _("Reset Configuration"), wxPoint(8,432), wxSize(176,32), 0, wxDefaultValidator, _T("ID_BUTTON1"));
	BtnCancel = new wxButton(this, ID_BUTTON2, _("Cancel"), wxPoint(496,432), wxSize(117,32), 0, wxDefaultValidator, _T("ID_BUTTON2"));
	BtnOk = new wxButton(this, ID_BUTTON3, _("Ok"), wxPoint(616,432), wxSize(101,32), 0, wxDefaultValidator, _T("ID_BUTTON3"));
	SBKeypad = new wxStaticBitmap(this, ID_STATICBITMAP1, wxNullBitmap, wxPoint(16,200), wxSize(208,216), wxSIMPLE_BORDER, _T("ID_STATICBITMAP1"));
	StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Choose your profile: "), wxPoint(64,48), wxSize(120,16), wxALIGN_RIGHT, _T("ID_STATICTEXT1"));
	PanelKeypad = new wxPanel(this, ID_PANEL1, wxPoint(256,200), wxSize(448,216), wxTRANSPARENT_WINDOW|wxTAB_TRAVERSAL, _T("ID_PANEL1"));
	LblJoy1 = new wxStaticText(this, ID_STATICTEXT2, _("Joystick 1"), wxPoint(8,104), wxSize(712,16), wxALIGN_CENTRE|wxTRANSPARENT_WINDOW, _T("ID_STATICTEXT2"));
	StaticText2 = new wxStaticText(this, ID_STATICTEXT3, _("To set another key, just mouse over the corresponding box you want to change and press the new key !"), wxPoint(8,152), wxSize(712,16), wxALIGN_CENTRE|wxTRANSPARENT_WINDOW, _T("ID_STATICTEXT3"));
	LblJoy2 = new wxStaticText(this, ID_STATICTEXT4, _("Joystick 2"), wxPoint(8,128), wxSize(712,16), wxALIGN_CENTRE|wxTRANSPARENT_WINDOW, _T("ID_STATICTEXT4"));

	Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&InputDialog::OnBtnResetClick);
	Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&InputDialog::OnBtnCancelClick);
	Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&InputDialog::OnBtnOkClick);
	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&InputDialog::OnClose);
	//*)

	// Text Defaut
    LblJoy1->SetForegroundColour(wxColour(0xFF4747));
    LblJoy1->SetLabel(wxT("Joystick 1 not detected !"));
    LblJoy2->SetForegroundColour(wxColour(0xFF4747));
    LblJoy2->SetLabel(wxT("Joystick 2 not detected !"));

	_CurrentProfile = CurrentProfile;
	_Dirty = false;

	// Set Picture
	SBKeypad->SetBitmap(wxBitmap(kb_layout_xpm));

	// Set Keypad Layout
	GSKeypad = new wxGridSizer(4, 4, 3, 3);
	PanelKeypad->SetSizer(GSKeypad);
	PanelKeypad->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

	for  (int i = 0; i < 4*4; i++) {
        _TxtKeypad[i] = new wxTextCtrl(PanelKeypad, ID_TxtKeypad[i], wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, wxControlNameStr);
        _TxtKeypad[i]->Bind(wxEVT_KEY_DOWN, &InputDialog::OnTxtKeypadOnKeydown, this);
        _TxtKeypad[i]->Bind(wxEVT_ENTER_WINDOW, &InputDialog::OnTxtKeypadEnterWindow, this);
        _TxtKeypad[i]->Bind(wxEVT_LEAVE_WINDOW, &InputDialog::OnTxtKeypadLeaveWindow, this);
        //_TxtKeypad[i]->Bind(wxEVT_SET_FOCUS, &InputDialog::OnTxtKeypadSetFocus, this);
        //_TxtKeypad[i]->Bind(wxEVT_KILL_FOCUS, &InputDialog::OnTxtKeypadKillFocus, this);
        _TxtKeypad[i]->SetFont(wxFont(10, wxFONTFAMILY_MODERN, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        GSKeypad->Add(_TxtKeypad[i], 0, wxEXPAND);
 	}

 	PanelKeypad->Layout();

 	LoadConfig();

 	CenterOnScreen();
}

void InputDialog::OnIdle(wxIdleEvent &event)
{
    wxTextCtrl *currentKeypad;
    int currentKeypadIndex;

    if (Joysticks::getInstance().HasJoysticks()) {

        // Initialize joysticks
        Joysticks::getInstance().initJoysticks();

        // Testing joystick 1
        if (Joysticks::getInstance().HasJoy1() && Joysticks::getInstance().getJoy1()->toString() != LblJoy1->GetLabel()) {
            LblJoy1->SetForegroundColour(wxColour(0x3E8100));
            LblJoy1->SetLabel(Joysticks::getInstance().getJoy1()->toString());
        } else if (!Joysticks::getInstance().HasJoy1() && LblJoy1->GetForegroundColour() != wxColour(0xFF4747)) {
            // Text Default
            LblJoy1->SetForegroundColour(wxColour(0xFF4747));
            LblJoy1->SetLabel(wxT("Joystick 1 not detected !"));
        }

        // Testing Joystick 2
        if (Joysticks::getInstance().HasJoy2() && Joysticks::getInstance().getJoy2()->toString() != LblJoy1->GetLabel()) {
            LblJoy2->SetForegroundColour(wxColour(0x3E8100));
            LblJoy2->SetLabel(Joysticks::getInstance().getJoy2()->toString());
        } else if (!Joysticks::getInstance().HasJoy2() && LblJoy2->GetForegroundColour() != wxColour(0xFF4747)) {
            // Text Default
            LblJoy2->SetForegroundColour(wxColour(0xFF4747));
            LblJoy2->SetLabel(wxT("Joystick 2 not detected !"));
        }


        // Detect current key
        currentKeypad = NULL;
        for (int i = 0; i < 4*4; i++) {
            if (_TxtKeypad[i]->HasFocus()) {
                currentKeypad = _TxtKeypad[i];
                currentKeypadIndex = i;
                break;
            }
        }

        // Don't poll event if no key is selected
        if (currentKeypad) {
            // Poll the joysticks
            Joystick *joy = Joysticks::getInstance().updateJoyState();
            if (joy) {
                wxString keyStr = wxEmptyString;

                // Check if joystick event
                if (Joysticks::getInstance().IsMoveEvent()) {
                    if (joy->getPersistantDirectionStr() != wxEmptyString)
                        keyStr.Printf(wxT("JOY_%d_%s"), joy->getMyNum(), joy->getPersistantDirectionStr());
                } else if (Joysticks::getInstance().IsButtonEvent()) {
                    if (joy->getPersistantButtonStr() != wxEmptyString)
                        keyStr.Printf(wxT("JOY_%d_%s"), joy->getMyNum(), joy->getPersistantButtonStr());
                }

                // Update text with new key
                if (keyStr != wxEmptyString) {
                    currentKeypad->SetValue(keyStr);
                    Mega8Config::getInstance().setKey(KeypadCorrespondingIndex[currentKeypadIndex], Joysticks::getInstance().getCodeFromJoystick(joy, true));
                }
            }
        }
    } else if (LblJoy2->GetForegroundColour() != wxColour(0xFF4747)){ // Testing the colour to avoid flickering in windows
        LblJoy1->SetForegroundColour(wxColour(0xFF4747));
        LblJoy1->SetLabel(wxT("Joystick 1 not detected !"));
        LblJoy2->SetForegroundColour(wxColour(0xFF4747));
        LblJoy2->SetLabel(wxT("Joystick 2 not detected !"));
    }
}

InputDialog::~InputDialog()
{
	//(*Destroy(InputDialog)
	//*)
}

void InputDialog::LoadConfig()
{
    wxString profileName, path;
    wxArrayString profiles;
    long index;
    int currentProfileIndex, cnt;
    bool finished;

    // Insert profile Names
    path = Mega8Config::getInstance().getConfig()->GetPath();
    Mega8Config::getInstance().getConfig()->SetPath("/Keys");

    // Read all saved profiles
    finished = Mega8Config::getInstance().getConfig()->GetFirstGroup(profileName, index);
    currentProfileIndex = -1;
    cnt = 0;
    while ( finished ) {

        if (profileName != wxEmptyString) {
            profiles.Add(profileName);
        }
        // Keep the index of the Current profile
        if (profileName == _CurrentProfile) {
            currentProfileIndex = cnt;
        }
        cnt++;
        finished = Mega8Config::getInstance().getConfig()->GetNextGroup(profileName, index);
    }
    Mega8Config::getInstance().getConfig()->SetPath(path);

    // Create the Choice Box
    _ChxProfile = new wxChoice(this, wxNewId(), wxPoint(184,40), wxSize(480,32), profiles, 0, wxDefaultValidator, _T("ID_ChxProfile"));
    _ChxProfile->Bind(wxEVT_CHOICE, &InputDialog::OnChxProfileSelect, this);

    if (currentProfileIndex != -1)
        _ChxProfile->SetSelection((int)currentProfileIndex);
    else
        _ChxProfile->SetSelection(0);

    LoadKeypad(_CurrentProfile);

}

void InputDialog::SaveConfig()
{
    // Save & reload
    Mega8Config::getInstance().saveConfig(_CurrentProfile);
}

bool InputDialog::CheckDirty() {
    bool res = true;

    if (_Dirty) {
        switch (wxMessageBox(_("The profile '") + _CurrentProfile + _("' has been modified.\nShould we save it ?"), _("Profile modified"), wxYES_NO | wxCANCEL | wxCENTRE, this)) {
            case wxYES:
                SaveConfig();
                res = true;
                _Dirty = false;
                break;

            case wxNO:
                res = true;
                _Dirty = false;
                break;

            case wxCANCEL:
            default:
                res = false;
        }
    }

    return res;
}

void InputDialog::LoadKeypad(const wxString &profile)
{
    wxString path;

    if (profile != wxEmptyString && CheckDirty()) {
        // Load the profile
        _CurrentProfile = profile;
        Mega8Config::getInstance().loadConfig(_CurrentProfile);

        // Load the keys
        wxAcceleratorEntry *ae = new wxAcceleratorEntry();

        for (int i = 0; i < 16; i++) {
            // Convert this string representation to keycode
            int key = Mega8Config::getInstance().getKey(KeypadCorrespondingIndex[i]);
            if (key > 0) {
                ae->Set(wxACCEL_NORMAL, key, 0);
                _TxtKeypad[i]->SetValue(ae->ToString());
            } else {
                _TxtKeypad[i]->SetValue(Joysticks::getStringFromCode(key));
            }
        }

        delete ae;
    }
}

void InputDialog::OnBtnCancelClick(wxCommandEvent& event)
{
    if (CheckDirty()) {
        EndModal(wxID_CANCEL);
    }
}

void InputDialog::OnClose(wxCloseEvent& event)
{
    // Detach Objects
    _ChxProfile->Destroy();
    GSKeypad->Clear(true);
    Destroy();
}

void InputDialog::OnChxProfileSelect(wxCommandEvent& event)
{
    wxChoice *Chx = (wxChoice*) event.GetEventObject();
    LoadKeypad(Chx->GetString(Chx->GetSelection()));
}

void InputDialog::OnTxtKeypadEnterWindow(wxMouseEvent &event)
{
    wxTextCtrl *txt = (wxTextCtrl*) event.GetEventObject();
    HighlightCurrentKey(txt, true);
    txt->SetFocus();
}

void InputDialog::OnTxtKeypadLeaveWindow(wxMouseEvent &event)
{
    wxTextCtrl *txt = (wxTextCtrl*) event.GetEventObject();
    HighlightCurrentKey(txt, false);
}

void InputDialog::HighlightCurrentKey(wxTextCtrl *txt, bool value)
{
    if (value) {
        txt->SetBackgroundColour(wxColour(0x5ee884));
        txt->SetForegroundColour(wxColour(0x6b7770));
    }
    else {
        txt->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
        txt->SetForegroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOWTEXT));
    }
}

void InputDialog::OnTxtKeypadOnKeydown(wxKeyEvent& event)
{
    int KeyCode = event.GetKeyCode();
    wxString keyCodeStr = wxEmptyString;

    // Check if the key is not binded to another key
    for (int i = 0; i < 16; i++) {
        if (event.GetKeyCode() == Mega8Config::getInstance().getKey(i)) {
            wxMessageBox(_("This key is already binded to another key.\nPlease choose another one."), _("Key in use"));
            return;
        }
    }

    wxTextCtrl *txt = NULL;
    int index;

    // Find object index
    for (int i = 0; i < 16; i++) {
        if (_TxtKeypad[i] == (wxTextCtrl*)event.GetEventObject()) {
            txt = (wxTextCtrl*)event.GetEventObject();
            index = i;
            break;
        }
    }

    if (txt) {
        // Load the keys
        wxAcceleratorEntry *ae = new wxAcceleratorEntry();

        // Not ok for wxAccelerator entry
        ae->Set(wxACCEL_NORMAL, KeyCode, 0);
        if (!ae->IsOk()) {
            wxMessageBox(_("This key is not supported.\nPlease choose another one."), _("Key not supported"));
            return;
        }

        // Last Test - Can keycode be converted to string ?
        try {
            keyCodeStr = ae->ToString();
        } catch (int e) {
            wxMessageBox(_("This key cannot be converted by wxAcceleratorEntry.\nPlease choose another one."), _("Key not supported"));
            return;
        }

        // All is finally ok. Set dirty flag
        if (keyCodeStr != wxEmptyString) {
            _Dirty = true;
            Mega8Config::getInstance().setKey(KeypadCorrespondingIndex[index], KeyCode);
            txt->SetValue(keyCodeStr);
        }

        delete ae;
    }
}


void InputDialog::OnBtnOkClick(wxCommandEvent& event)
{
    if (CheckDirty()) {
        SaveConfig();
        EndModal(wxID_OK);
    }
}

void InputDialog::OnBtnResetClick(wxCommandEvent& event)
{
    if (wxMessageBox(_("Are you sure you want to reset to default key configuration ?"), _("Reset key configuration"), wxYES_NO) == wxYES) {
        Mega8Config::getInstance().resetKeyboard();
        Mega8Config::getInstance().saveConfig(_CurrentProfile);
        _Dirty = false;
        LoadConfig();
    }
}
