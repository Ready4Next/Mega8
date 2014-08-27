/***************************************************************
 * Name:      Mega8App.cpp
 * Purpose:   Code for Application Class
 * Author:    Junta (junta@hotmail.be)
 * Created:   2014-07-24
 * Copyright: Junta (http://www.ready4next.be)
 * License:
 **************************************************************/

#include "Mega8App.h"

//(*AppHeaders
#include "Mega8Main.h"
#include <wx/image.h>
//*)

IMPLEMENT_APP(Mega8App);

bool Mega8App::OnInit()
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
    	Frame = new Mega8Frame(0);
    	Frame->Show();
    	SetTopWindow(Frame);
    }
    //*)
    return wxsOK;

}

int Mega8App::FilterEvent(wxEvent& event)
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

