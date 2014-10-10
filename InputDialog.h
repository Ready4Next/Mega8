<<<<<<< HEAD
#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

//(*Headers(InputDialog)
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/statbmp.h>
//*)

#include <wx/textctrl.h>
#include <wx/wx.h>
#include "Joysticks.h"

class InputDialog: public wxDialog
{
	public:

		InputDialog(const wxString &CurrentProfile, wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~InputDialog();

		//(*Declarations(InputDialog)
		wxStaticText* LblJoy2;
		wxStaticText* LblJoy1;
		wxButton* BtnCancel;
		wxStaticText* StaticText1;
		wxStaticBox* StaticBox2;
		wxStaticBitmap* SBKeypad;
		wxPanel* PanelKeypad;
		wxStaticBox* StaticBox1;
		wxButton* BtnReset;
		wxButton* BtnOk;
		wxStaticBox* BoxKeypad;
		wxStaticText* StaticText2;
		//*)

		wxTextCtrl *_TxtKeypad[16];
		wxGridSizer *GSKeypad;
		wxString _CurrentProfile;
		wxChoice *_ChxProfile;

		void LoadConfig();
		void SaveConfig();
        bool CheckDirty();
		void LoadKeypad(const wxString &profile);

	protected:

		//(*Identifiers(InputDialog)
		static const long ID_STATICBOX3;
		static const long ID_STATICBOX1;
		static const long ID_STATICBOX2;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_BUTTON3;
		static const long ID_STATICBITMAP1;
		static const long ID_STATICTEXT1;
		static const long ID_PANEL1;
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT4;
		//*)

		static const long ID_TxtKeypad[16];

		void OnIdle(wxIdleEvent &event);

	private:

	    bool _Dirty;

		//(*Handlers(InputDialog)
		void OnBtnCancelClick(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnBtnOkClick(wxCommandEvent& event);
		void OnBtnResetClick(wxCommandEvent& event);
		//*)

		void OnTxtKeypadOnKeydown(wxKeyEvent& event);
		void OnTxtKeypadEnterWindow(wxMouseEvent &event);
		void OnTxtKeypadLeaveWindow(wxMouseEvent &event);

		void OnChxProfileSelect(wxCommandEvent& event);
		void HighlightCurrentKey(wxTextCtrl *txt, bool value);

		DECLARE_EVENT_TABLE()
};

#endif
=======
#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

//(*Headers(InputDialog)
#include <wx/dialog.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/statbmp.h>
//*)

#include <wx/textctrl.h>
#include <wx/wx.h>
#include "Joysticks.h"

class InputDialog: public wxDialog
{
	public:

		InputDialog(const wxString &CurrentProfile, wxWindow* parent,wxWindowID id=wxID_ANY,const wxPoint& pos=wxDefaultPosition,const wxSize& size=wxDefaultSize);
		virtual ~InputDialog();

		//(*Declarations(InputDialog)
		wxStaticText* LblJoy2;
		wxStaticText* LblJoy1;
		wxButton* BtnCancel;
		wxStaticText* StaticText1;
		wxStaticBox* StaticBox2;
		wxStaticBitmap* SBKeypad;
		wxPanel* PanelKeypad;
		wxStaticBox* StaticBox1;
		wxButton* BtnReset;
		wxButton* BtnOk;
		wxStaticBox* BoxKeypad;
		wxStaticText* StaticText2;
		//*)

		wxTextCtrl *_TxtKeypad[16];
		wxGridSizer *GSKeypad;
		wxString _CurrentProfile;
		wxChoice *_ChxProfile;

		void LoadConfig();
		void SaveConfig();
        bool CheckDirty();
		void LoadKeypad(const wxString &profile);

	protected:

		//(*Identifiers(InputDialog)
		static const long ID_STATICBOX3;
		static const long ID_STATICBOX1;
		static const long ID_STATICBOX2;
		static const long ID_BUTTON1;
		static const long ID_BUTTON2;
		static const long ID_BUTTON3;
		static const long ID_STATICBITMAP1;
		static const long ID_STATICTEXT1;
		static const long ID_PANEL1;
		static const long ID_STATICTEXT2;
		static const long ID_STATICTEXT3;
		static const long ID_STATICTEXT4;
		//*)

		static const long ID_TxtKeypad[16];

		void OnIdle(wxIdleEvent &event);

	private:

	    bool _Dirty;

		//(*Handlers(InputDialog)
		void OnBtnCancelClick(wxCommandEvent& event);
		void OnClose(wxCloseEvent& event);
		void OnBtnOkClick(wxCommandEvent& event);
		void OnBtnResetClick(wxCommandEvent& event);
		//*)

		void OnTxtKeypadOnKeydown(wxKeyEvent& event);
		void OnTxtKeypadEnterWindow(wxMouseEvent &event);
		void OnTxtKeypadLeaveWindow(wxMouseEvent &event);

		void OnChxProfileSelect(wxCommandEvent& event);
		void HighlightCurrentKey(wxTextCtrl *txt, bool value);

		DECLARE_EVENT_TABLE()
};

#endif
>>>>>>> 4dfbb63e5631206625c60576f26a61ed8632ee15
