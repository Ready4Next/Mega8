/***************************************************************
 * Name:      Mega8App.h
 * Purpose:   Defines Application Class
 * Author:    Junta (junta@hotmail.be)
 * Created:   2014-07-24
 * Copyright: Junta (http://www.ready4next.be)
 * License:
 **************************************************************/

#ifndef Mega8APP_H
#define Mega8APP_H

#include <wx/app.h>
#include "Mega8Main.h"

class Mega8App : public wxApp
{
    public:
        virtual bool OnInit();

    protected:
        Mega8Frame* Frame;

        int FilterEvent(wxEvent& event);
};

#endif // Mega8APP_H
