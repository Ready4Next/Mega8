/***************************************************************
 * Name:      Chip8EmuApp.cpp
 * Purpose:   Code for Application Class
 * Author:    Junta (junta@hotmail.be)
 * Created:   2014-07-24
 * Copyright: Junta (http://www.ready4next.be)
 * License:
 **************************************************************/

#include "Chip8EmuApp.h"

//(*AppHeaders
#include "Chip8EmuMain.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(Chip8EmuApp);

bool Chip8EmuApp::OnInit()
{
    /***************** ATTENTION *******************

    Ne pas oublier de retirer la redéclaration de Frame
    si sauvegarde avec wxSmith !!!!

    ************************************************/

    //(*AppInitialize
    bool wxsOK = true;
    wxInitAllImageHandlers();
    if ( wxsOK )
    {
    	Frame = new Chip8EmuFrame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}

int Chip8EmuApp::FilterEvent(wxEvent& event)
{
    if (event.GetEventType() == wxEVT_KEY_DOWN) {
        Frame->OnKeyDown( (wxKeyEvent&)event );
        return true;
    }

    if (event.GetEventType() == wxEVT_KEY_UP) {
        Frame->OnKeyUp( (wxKeyEvent&)event );
        return true;
    }

    return -1;
}

