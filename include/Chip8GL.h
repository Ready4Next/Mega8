#ifndef CHIP8GL_H
#define CHIP8GL_H

#include <wx/wx.h>
#include <wx/glcanvas.h>
#include <time.h>
#include <FTGL/ftgl.h>
#include <cstdarg>

#include "../Chip8/Chip8.h"

#include <vector>
#include <string>

struct T_TEXT {
    int index;
    int fontColor;
    char text[128];
};

class Chip8GL : public wxGLCanvas
{
    public:
        /** Default constructor */
        Chip8GL(wxWindow *parent, wxWindowID id, int GLCanvasAttributes[]);
        /** Default destructor */
        virtual ~Chip8GL();

        int getFPS() { return _FPS; };
        int GetHeight();
        int GetWidth();
        void PrintGLHUD();
        void PrintGL(float x, float y, float fontSize, int fontColor, const char *text, ...);
        void PrintGLInfosF(int index, int fontColor, const char *text, ...);
        void PrintGLInfos(int index, int fontColor, const char *text);
        void Render(unsigned char *screen, int w, int h);
        void Flip();
        void Clear();
        bool getIsRendering() { return _Rendering; }
        bool getFiltered() { return _Filtered; }
        void setFiltered(bool value) { _Filtered = value; }
        bool getStopRender() { return _stopRender; }
        void setStopRender(bool value) { _stopRender = value; }
        void setDisplayHUD(bool value) { _displayHUD = value; }
        bool getDisplayHUD() { return _displayHUD; }
        wxBitmap getScreenshot();
    protected:

    private:

        FTPixmapFont *_font;
        bool _Rendering;
        bool _Filtered;
        wxGLContext *_Context;
        //clock_t _startTime;
        int _FPS;
        vector<T_TEXT> _HUDLines;
        bool _displayHUD;
        bool _stopRender;

        void prepare2DViewport(int topleft_x, int topleft_y, int bottomright_x, int bottomright_y);
        void updateGfx(unsigned char *pixels, int sourceW, int sourceH);
        GLuint GenerateGLTextureFromChip8(unsigned char *pixels, int w, int h);
        void addOrReplaceHUDLine(T_TEXT line);
};

#endif // CHIP8GL_H
