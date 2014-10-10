<<<<<<< HEAD
#include <wx/dcclient.h>
#include <vector>
#include <sstream>

#include "Chip8GL.h"
// Font file
#include "game_over.h"

Chip8GL::Chip8GL(wxWindow *parent, wxWindowID id, int GLCanvasAttributes[])
            :wxGLCanvas(parent, id,  wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS, wxT("Chip8GL"), GLCanvasAttributes)
{
    //ctor
    _Context = new wxGLContext(this);
    _font = new FTPixmapFont(game_over_ttf, game_over_ttf_len);
    _FPS = 0;
	_stopRender = false;
}

Chip8GL::~Chip8GL()
{
    //dtor
    delete _font;
    delete _Context;
}

void Chip8GL::Render(unsigned char *screen, int w, int h) {
    if (!_stopRender)
        updateGfx(screen, w, h);
}

void Chip8GL::PrintGL(float x, float y, float fontSize, int fontColor, const char *text, ...) {

    char buffer[256];

    if (_font) {
        va_list args;
        va_start(args, text);
        vsprintf(buffer, text, args);
        va_end(args);

        if (!_font->Error()) {
            // Set the font size and render a small text.
            _font->FaceSize(fontSize);

            // Set text color
            glColor3f(getR(fontColor) / 255.0, getG(fontColor) / 255.0, getB(fontColor) / 255.0);
            // Move to top left corner and add some offset (font size = 72)
            FTBBox box = _font->BBox(buffer);
            glRasterPos2f(x, y + _font->Ascender());
            // Display text
            _font->Render(buffer);
            // Clear Color
            glColor3f(1.0, 1.0, 1.0);
        }
    }
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

void Chip8GL::addOrReplaceHUDLine(T_TEXT line) {
    int index = -1;

    // Check if the vector is not empty and scan for that line
    if (!_HUDLines.empty()) {
        for (vector<T_TEXT>::iterator it = _HUDLines.begin(); it < _HUDLines.end(); ++it) {
            if (it->index == line.index) {
                index = it - _HUDLines.begin();
                break;
            }
        }
    }

    // Or else, we push it back
    if (index == -1) {
        _HUDLines.push_back(line);
        // Max 5 lines - Remove first element
        if (_HUDLines.size() > 6)
            _HUDLines.erase(_HUDLines.begin());
    } else {
        _HUDLines[index] = line;
    }
}

void Chip8GL::PrintGLInfosF(int index, int fontColor, const char *text, ...) {

    char buffer[128];
    T_TEXT buf;

    va_list args;
    va_start(args, text);
    vsprintf(buffer, text, args);
    va_end(args);

    buf.fontColor = fontColor;
    buf.index = index;
    strncpy(buf.text, buffer, sizeof(buf.text));

    addOrReplaceHUDLine(buf);
}

int Chip8GL::GetHeight() {
    // GetViewPort Size
    wxClientDC dc(this);
    wxSize size = dc.GetSize();
    return size.GetHeight();
}

int Chip8GL::GetWidth() {
    // GetViewPort Size
    wxClientDC dc(this);
    wxSize size = dc.GetSize();
    return size.GetWidth();
}

void Chip8GL::PrintGLHUD() {
    if (_font && !_HUDLines.empty()) {
        if (!_font->Error()) {
            // Set the font size and render a small text.
            _font->FaceSize(36);

            for (vector<T_TEXT>::iterator it = _HUDLines.begin(); it < _HUDLines.end(); ++it) {
                glColor3f(getR(it->fontColor) / 255.0, getG(it->fontColor) / 255.0, getB(it->fontColor) / 255.0);

                // Move to Bottom Right & add some offset (font size = 12)
                FTBBox box = _font->BBox(it->text);
                glRasterPos2f(GetWidth() - box.Upper().Xf() - 5.0f, GetHeight() - (_font->Ascender() * it->index));

                // Display text
                _font->Render(it->text);
            }
            // Clear Color
            glColor3f(1.0, 1.0, 1.0);
        }
    }
}

void Chip8GL::PrintGLInfos(int index, int fontColor, const char *text) {
    T_TEXT buf;

    buf.fontColor = fontColor;
    buf.index = index;
    strncpy(buf.text, text, sizeof(buf.text));

    addOrReplaceHUDLine(buf);
}

void Chip8GL::Clear() {
    this->SetCurrent();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
    prepare2DViewport(0, 0, GetWidth(), GetHeight());
    glFlush();
    this->SwapBuffers();
}

/** Inits the OpenGL viewport for drawing in 2D. */
void Chip8GL::prepare2DViewport(int topleft_x, int topleft_y, int bottomright_x, int bottomright_y)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
    glEnable(GL_TEXTURE_2D);   // textures
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glViewport(topleft_x, topleft_y, bottomright_x-topleft_x, bottomright_y-topleft_y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(topleft_x, bottomright_x, bottomright_y, topleft_y, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Chip8GL::updateGfx(unsigned char *pixels, int sourceW, int sourceH) {
    static clock_t startTime = clock();
    static int incFPS = 0;
    clock_t elapsedTime;
    //wxString textFPS;

    GLuint texture;

    incFPS++;

    _Rendering = true;

    // Select current GLDisplay
    this->SetCurrent();

    texture = GenerateGLTextureFromChip8(pixels, sourceW, sourceH);

    glBindTexture( GL_TEXTURE_2D, texture );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ------------- draw some 2D ----------------
    prepare2DViewport(0, 0, GetWidth(), GetHeight());
    glLoadIdentity();

    glBegin( GL_QUADS );
        //Bottom-left vertex (corner)
        glTexCoord2i( 0, 0 );
        glVertex3f( 0, 0, 0 );

        //Bottom-right vertex (corner)
        glTexCoord2i( 1, 0 );
        glVertex3f( GetWidth(), 0, 0.f );

        //Top-right vertex (corner)
        glTexCoord2i( 1, 1 );
        glVertex3f( GetWidth(), GetHeight(), 0.f );

        //Top-left vertex (corner)
        glTexCoord2i( 0, 1 );
        glVertex3f( 0, GetHeight(), 0.f );

    glEnd();

    // Free Created Texture
    glDeleteTextures( 1, &texture );

    // Print OnScreenInformations
    if (_displayHUD)
        PrintGLHUD();

    // Calc how many time has passed since clock init
    elapsedTime = clock() - startTime;
    if (((float)elapsedTime/CLOCKS_PER_SEC) >= 1.0) {
        // Restart Clock
        startTime = clock();
        // Set current FPS counter to property and reset counter
        _FPS = incFPS;
        incFPS = 0;

        //textFPS.Printf("%d FPS", getFPS());
        //PrintGLInfos(0, 0xFF0000, textFPS.mb_str());
        //_FPS = 0;
    }
}

void Chip8GL::Flip()
{
    // Flush & swap
    glFlush();
    this->SwapBuffers();

    _Rendering = false;
}

GLuint Chip8GL::GenerateGLTextureFromChip8(unsigned char *pixels, int w, int h)
{
    GLuint texture;

    // Have OpenGL generate a texture object handle for us
	glGenTextures( 1, &texture );

	// Bind the texture object
	glBindTexture( GL_TEXTURE_2D, texture );

	// Set the texture's stretching properties
	if (_Filtered) {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

	// Edit the texture object's image data using the information SDL_Surface gives us
	glTexImage2D( GL_TEXTURE_2D, 0, 4, w, h, 0,
                      GL_RGBA, GL_UNSIGNED_BYTE, pixels );

    return texture;
}

wxBitmap Chip8GL::getScreenshot()
{
    // Read the OpenGL image into a pixel array
    GLint view[4];
    glGetIntegerv(GL_VIEWPORT, view);
    void* pixels = malloc(3 * view[2] * view[3]); // must use malloc
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer( GL_BACK_LEFT );
    glReadPixels(0, 0, view[2], view[3], GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // Put the image into a wxImage
    wxImage image((int) view[2], (int) view[3]);
    image.SetData((unsigned char*) pixels);
    image = image.Mirror(false);
    return wxBitmap(image);
}
=======
#include <wx/dcclient.h>
#include <vector>
#include <sstream>

#include "Chip8GL.h"
// Font file
#include "game_over.h"

Chip8GL::Chip8GL(wxWindow *parent, wxWindowID id, int GLCanvasAttributes[])
            :wxGLCanvas(parent, id,  wxDefaultPosition, wxDefaultSize, wxWANTS_CHARS, wxT("Chip8GL"), GLCanvasAttributes)
{
    //ctor
    _Context = new wxGLContext(this);
    _font = new FTPixmapFont(game_over_ttf, game_over_ttf_len);
    _FPS = 0;
	_stopRender = false;
}

Chip8GL::~Chip8GL()
{
    //dtor
    delete _font;
    delete _Context;
}

void Chip8GL::Render(unsigned char *screen, int w, int h) {
    if (!_stopRender)
        updateGfx(screen, w, h);
}

void Chip8GL::PrintGL(float x, float y, float fontSize, int fontColor, const char *text, ...) {

    char buffer[256];

    if (_font) {
        va_list args;
        va_start(args, text);
        vsprintf(buffer, text, args);
        va_end(args);

        if (!_font->Error()) {
            // Set the font size and render a small text.
            _font->FaceSize(fontSize);

            // Set text color
            glColor3f(getR(fontColor) / 255.0, getG(fontColor) / 255.0, getB(fontColor) / 255.0);
            // Move to top left corner and add some offset (font size = 72)
            FTBBox box = _font->BBox(buffer);
            glRasterPos2f(x, y + _font->Ascender());
            // Display text
            _font->Render(buffer);
            // Clear Color
            glColor3f(1.0, 1.0, 1.0);
        }
    }
}

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

void Chip8GL::addOrReplaceHUDLine(T_TEXT line) {
    int index = -1;

    // Check if the vector is not empty and scan for that line
    if (!_HUDLines.empty()) {
        for (vector<T_TEXT>::iterator it = _HUDLines.begin(); it < _HUDLines.end(); ++it) {
            if (it->index == line.index) {
                index = it - _HUDLines.begin();
                break;
            }
        }
    }

    // Or else, we push it back
    if (index == -1) {
        _HUDLines.push_back(line);
        // Max 5 lines - Remove first element
        if (_HUDLines.size() > 6)
            _HUDLines.erase(_HUDLines.begin());
    } else {
        _HUDLines[index] = line;
    }
}

void Chip8GL::PrintGLInfosF(int index, int fontColor, const char *text, ...) {

    char buffer[128];
    T_TEXT buf;

    va_list args;
    va_start(args, text);
    vsprintf(buffer, text, args);
    va_end(args);

    buf.fontColor = fontColor;
    buf.index = index;
    strncpy(buf.text, buffer, sizeof(buf.text));

    addOrReplaceHUDLine(buf);
}

int Chip8GL::GetHeight() {
    // GetViewPort Size
    wxClientDC dc(this);
    wxSize size = dc.GetSize();
    return size.GetHeight();
}

int Chip8GL::GetWidth() {
    // GetViewPort Size
    wxClientDC dc(this);
    wxSize size = dc.GetSize();
    return size.GetWidth();
}

void Chip8GL::PrintGLHUD() {
    if (_font && !_HUDLines.empty()) {
        if (!_font->Error()) {
            // Set the font size and render a small text.
            _font->FaceSize(36);

            for (vector<T_TEXT>::iterator it = _HUDLines.begin(); it < _HUDLines.end(); ++it) {
                glColor3f(getR(it->fontColor) / 255.0, getG(it->fontColor) / 255.0, getB(it->fontColor) / 255.0);

                // Move to Bottom Right & add some offset (font size = 12)
                FTBBox box = _font->BBox(it->text);
                glRasterPos2f(GetWidth() - box.Upper().Xf() - 5.0f, GetHeight() - (_font->Ascender() * it->index));

                // Display text
                _font->Render(it->text);
            }
            // Clear Color
            glColor3f(1.0, 1.0, 1.0);
        }
    }
}

void Chip8GL::PrintGLInfos(int index, int fontColor, const char *text) {
    T_TEXT buf;

    buf.fontColor = fontColor;
    buf.index = index;
    strncpy(buf.text, text, sizeof(buf.text));

    addOrReplaceHUDLine(buf);
}

void Chip8GL::Clear() {
    this->SetCurrent();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
    prepare2DViewport(0, 0, GetWidth(), GetHeight());
    glFlush();
    this->SwapBuffers();
}

/** Inits the OpenGL viewport for drawing in 2D. */
void Chip8GL::prepare2DViewport(int topleft_x, int topleft_y, int bottomright_x, int bottomright_y)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Black Background
    glEnable(GL_TEXTURE_2D);   // textures
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glViewport(topleft_x, topleft_y, bottomright_x-topleft_x, bottomright_y-topleft_y);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(topleft_x, bottomright_x, bottomright_y, topleft_y, -1.0f, 1.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Chip8GL::updateGfx(unsigned char *pixels, int sourceW, int sourceH) {
    static clock_t startTime = clock();
    static int incFPS = 0;
    clock_t elapsedTime;
    //wxString textFPS;

    GLuint texture;

    incFPS++;

    _Rendering = true;

    // Select current GLDisplay
    this->SetCurrent();

    texture = GenerateGLTextureFromChip8(pixels, sourceW, sourceH);

    glBindTexture( GL_TEXTURE_2D, texture );

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // ------------- draw some 2D ----------------
    prepare2DViewport(0, 0, GetWidth(), GetHeight());
    glLoadIdentity();

    glBegin( GL_QUADS );
        //Bottom-left vertex (corner)
        glTexCoord2i( 0, 0 );
        glVertex3f( 0, 0, 0 );

        //Bottom-right vertex (corner)
        glTexCoord2i( 1, 0 );
        glVertex3f( GetWidth(), 0, 0.f );

        //Top-right vertex (corner)
        glTexCoord2i( 1, 1 );
        glVertex3f( GetWidth(), GetHeight(), 0.f );

        //Top-left vertex (corner)
        glTexCoord2i( 0, 1 );
        glVertex3f( 0, GetHeight(), 0.f );

    glEnd();

    // Free Created Texture
    glDeleteTextures( 1, &texture );

    // Print OnScreenInformations
    if (_displayHUD)
        PrintGLHUD();

    // Calc how many time has passed since clock init
    elapsedTime = clock() - startTime;
    if (((float)elapsedTime/CLOCKS_PER_SEC) >= 1.0) {
        // Restart Clock
        startTime = clock();
        // Set current FPS counter to property and reset counter
        _FPS = incFPS;
        incFPS = 0;

        //textFPS.Printf("%d FPS", getFPS());
        //PrintGLInfos(0, 0xFF0000, textFPS.mb_str());
        //_FPS = 0;
    }
}

void Chip8GL::Flip()
{
    // Flush & swap
    glFlush();
    this->SwapBuffers();

    _Rendering = false;
}

GLuint Chip8GL::GenerateGLTextureFromChip8(unsigned char *pixels, int w, int h)
{
    GLuint texture;

    // Have OpenGL generate a texture object handle for us
	glGenTextures( 1, &texture );

	// Bind the texture object
	glBindTexture( GL_TEXTURE_2D, texture );

	// Set the texture's stretching properties
	if (_Filtered) {
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

	// Edit the texture object's image data using the information SDL_Surface gives us
	glTexImage2D( GL_TEXTURE_2D, 0, 4, w, h, 0,
                      GL_RGBA, GL_UNSIGNED_BYTE, pixels );

    return texture;
}

wxBitmap Chip8GL::getScreenshot()
{
    // Read the OpenGL image into a pixel array
    GLint view[4];
    glGetIntegerv(GL_VIEWPORT, view);
    void* pixels = malloc(3 * view[2] * view[3]); // must use malloc
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer( GL_BACK_LEFT );
    glReadPixels(0, 0, view[2], view[3], GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // Put the image into a wxImage
    wxImage image((int) view[2], (int) view[3]);
    image.SetData((unsigned char*) pixels);
    image = image.Mirror(false);
    return wxBitmap(image);
}
>>>>>>> 4dfbb63e5631206625c60576f26a61ed8632ee15
