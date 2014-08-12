/***************************************************************
 * Name:      Chip8EmuApp.h
 * Purpose:   Defines Application Class
 * Author:    Junta (junta@hotmail.be)
 * Created:   2014-07-24
 * Copyright: Junta (http://www.ready4next.be)
 * License:
 **************************************************************/

#ifndef CHIP8EMUAPP_H
#define CHIP8EMUAPP_H

#include <wx/app.h>
#include "Chip8EmuMain.h"

class Chip8EmuApp : public wxApp
{
    public:
        virtual bool OnInit();

    protected:
        Chip8EmuFrame* Frame;

        int FilterEvent(wxEvent& event);
};

#endif // CHIP8EMUAPP_H
