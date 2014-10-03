#ifndef CHIP8_H
#define CHIP8_H

#include <stdlib.h>     /* srand, rand */
#include <math.h>

/* Timeval & gettimeofday */
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(WIN32)
	#include <time.h>
	#include <winsock.h>
#else
	#include <sys/time.h>
#endif

#include <cstdio>
#include <string.h>     // memcpy
#include <math.h>

#include <fstream>
#include <stack>

#include <chrono>

using namespace std;

enum Chip8Types { CHIP8 = 0, CHIP8_HiRes, CHIP8_HiRes2, SCHIP8, MCHIP8 };
enum Chip8ColorTheme { DEFAULT = 0, INVERSE, GAMEBOY, C64, RED, GREEN, BLUE };
enum Chip8BlendModes { BLEND_NORMAL = 0, BLEND_25 = 1, BLEND_50 = 2, BLEND_75 = 3, BLEND_ADD = 4, BLEND_MUL = 5};

struct TCOLOR {
    unsigned int foreColor;
    unsigned int backColor;
};

#ifndef RPCNDR_H
	typedef unsigned char byte;
#endif

#define getNanoFromHertz(h)         (((double)1.0/h) * 1000) * 1000000)
#define getMicroFromHertz(h)        (((double)1.0/h) * 1000000)
#define getMilliFromHertz(h)        (((double)1.0/h) * 1000)
#define getSecondsFromHertz(h)      ((double)1.0/h)
#define getR(color)                 ((color >> 24) & 0xFF)
#define getG(color)                 ((color >> 16) & 0xFF)
#define getB(color)                 ((color >> 8) & 0xFF)
#define getAlpha(color)             (color & 0xFF)
#define getRGB(R, G, B)             (unsigned int)((R << 16) | (G << 8) | B)
#define getRGBA(R, G, B, Alpha)     (unsigned int)((R << 24) | (G << 16) | (B << 8) | Alpha)

// Original Chip-8 Frequencies according to various docs
// CPU Frequency (COSMAC VIP 1.76 Khz)
const float    BASE_FREQ     = 44;      // 1760 div 40;

// Clock Frequency 60Hz
const float    BASE_CLOCK    = 60;
const double   BASE_CLOCK_MS = (1/BASE_CLOCK) * 1000;
const double   BASE_CLOCK_NS = BASE_CLOCK_MS * 1000000;

enum SoundState { OPEN = 1, CLOSE };

// Chip-8 & SChip-8 font set
const byte Chip8Font[240] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80, // F

    // S-Chip-8
    0xF0, 0xF0, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0xF0, 0xF0, // 0
    0x20, 0x20, 0x60, 0x60, 0x20, 0x20, 0x20, 0x20, 0x70, 0x70, // 1
    0xF0, 0xF0, 0x10, 0x10, 0xF0, 0xF0, 0x80, 0x80, 0xF0, 0xF0, // 2
    0xF0, 0xF0, 0x10, 0x10, 0xF0, 0xF0, 0x10, 0x10, 0xF0, 0xF0, // 3
    0x90, 0x90, 0x90, 0x90, 0xF0, 0xF0, 0x10, 0x10, 0x10, 0x10, // 4
    0xF0, 0xF0, 0x80, 0x80, 0xF0, 0xF0, 0x10, 0x10, 0xF0, 0xF0, // 5
    0xF0, 0xF0, 0x80, 0x80, 0xF0, 0xF0, 0x90, 0x90, 0xF0, 0xF0, // 6
    0xF0, 0xF0, 0x10, 0x10, 0x20, 0x20, 0x40, 0x40, 0x40, 0x40, // 7
    0xF0, 0xF0, 0x90, 0x90, 0xF0, 0xF0, 0x90, 0x90, 0xF0, 0xF0, // 8
    0xF0, 0xF0, 0x90, 0x90, 0xF0, 0xF0, 0x10, 0x10, 0xF0, 0xF0, // 9
    0xF0, 0xF0, 0x90, 0x90, 0xF0, 0xF0, 0x90, 0x90, 0x90, 0x90, // A
    0xE0, 0xE0, 0x90, 0x90, 0xE0, 0xE0, 0x90, 0x90, 0xE0, 0xE0, // B
    0xF0, 0xF0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0xF0, 0xF0, // C
    0xE0, 0xE0, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0xE0, 0xE0, // D
    0xF0, 0xF0, 0x80, 0x80, 0xF0, 0xF0, 0x80, 0x80, 0xF0, 0xF0, // E
    0xF0, 0xF0, 0x80, 0x80, 0xF0, 0xF0, 0x80, 0x80, 0x80, 0x80  // F
};

// FakeBios - Chip8 emulator Logo [Garstyciuks] (Cmd: xxd -i "file" > "file.h")
const unsigned char bios_ch8[] = {
  0x00, 0xe0, 0x60, 0x00, 0x61, 0x00, 0x62, 0x08, 0xa2, 0x20, 0x40, 0x40,
  0x22, 0x1a, 0x41, 0x20, 0x12, 0x10, 0xd0, 0x18, 0xf2, 0x1e, 0x70, 0x08,
  0x12, 0x0a, 0x60, 0x00, 0x71, 0x08, 0x00, 0xee, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x7f, 0x40, 0x5f, 0x50, 0x57, 0x54, 0x54, 0x00, 0xfc, 0x04, 0xf4,
  0x14, 0xd4, 0x54, 0x54, 0x00, 0x3f, 0x20, 0x2f, 0x28, 0x2b, 0x2a, 0x2a,
  0x00, 0xfe, 0x02, 0xfa, 0x0a, 0xea, 0x2a, 0x2a, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x54, 0x54, 0x54, 0x54, 0x54, 0x54, 0x74, 0x00,
  0x54, 0x54, 0x54, 0x54, 0x74, 0x00, 0x00, 0x00, 0x2a, 0x2a, 0x2a, 0x2a,
  0x2a, 0x2a, 0x3b, 0x00, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0xee, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x74, 0x54, 0x54, 0x54,
  0x54, 0x54, 0x54, 0x54, 0x00, 0x00, 0x74, 0x54, 0x54, 0x54, 0x54, 0x54,
  0x3b, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0x2a, 0xee, 0x2a, 0x2a, 0x2a,
  0x2a, 0x2a, 0x2a, 0x2a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x54, 0x54, 0x57, 0x50, 0x5f, 0x40, 0x7f, 0x00, 0x54, 0x54, 0xd4, 0x14,
  0xf4, 0x04, 0xfc, 0x00, 0x2a, 0x2a, 0x2b, 0x28, 0x2f, 0x20, 0x3f, 0x00,
  0x2a, 0x2a, 0xea, 0x0a, 0xfa, 0x02, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
const unsigned int bios_ch8_len = 288;

const unsigned char keypad_ch8[] = {
  0x12, 0x4e, 0x08, 0x19, 0x01, 0x01, 0x08, 0x01, 0x0f, 0x01, 0x01, 0x09,
  0x08, 0x09, 0x0f, 0x09, 0x01, 0x11, 0x08, 0x11, 0x0f, 0x11, 0x01, 0x19,
  0x0f, 0x19, 0x16, 0x01, 0x16, 0x09, 0x16, 0x11, 0x16, 0x19, 0xfc, 0xfc,
  0xfc, 0xfc, 0xfc, 0xfc, 0xfc, 0x00, 0xa2, 0x02, 0x82, 0x0e, 0xf2, 0x1e,
  0x82, 0x06, 0xf1, 0x65, 0x00, 0xee, 0xa2, 0x02, 0x82, 0x0e, 0xf2, 0x1e,
  0x82, 0x06, 0xf1, 0x55, 0x00, 0xee, 0x6f, 0x10, 0xff, 0x15, 0xff, 0x07,
  0x3f, 0x00, 0x12, 0x46, 0x00, 0xee, 0x00, 0xe0, 0x62, 0x00, 0x22, 0x2a,
  0xf2, 0x29, 0xd0, 0x15, 0x70, 0xff, 0x71, 0xff, 0x22, 0x36, 0x72, 0x01,
  0x32, 0x10, 0x12, 0x52, 0xf2, 0x0a, 0x22, 0x2a, 0xa2, 0x22, 0xd0, 0x17,
  0x22, 0x42, 0xd0, 0x17, 0x12, 0x64
};
const unsigned int keypad_ch8_len = 114;


// Some size constants depending on machine
class BaseCHIP8 {
    public:
        void BaseChip8() {
            //setType(type);
            _colorTheme = DEFAULT;
            _inverseColor  = false;
        };

        // Chips base frequency
        int getFrequencyMultiplicator() {
            switch (_type) {
                case MCHIP8:
                    return 640;
                case SCHIP8:
                    return 80;
                default:
                    return 40;
            }
        }

        // Mega Chip-8
        void setCollisionColorIndex(byte value) { _collisionColorIndex = value; }
        byte getCollisionColorIndex() { return _collisionColorIndex; }
        void setSpriteBlendMode(Chip8BlendModes value) { _spriteBlendMode = value; }
        Chip8BlendModes getSpriteBlendMode() { return _spriteBlendMode; }
        void setExtendedMode(bool value) { _extendedMode = value; }
        bool getExtendedMode() { return _extendedMode; }
        unsigned short getSpriteWidth() { return _spriteWidth; }
        unsigned short getSpriteHeight() { return _spriteHeight; }
        void setSpriteWidth(unsigned short value) { _spriteWidth = value; }
        void setSpriteHeight(unsigned short value) { _spriteHeight = value; }

        bool getInverseColor() { return _inverseColor; }
        Chip8ColorTheme getColorTheme() { return _colorTheme; }
        Chip8Types getType() { return _type; }
        const char *getTypeStr() {
            switch (getType()) {
                case CHIP8:
                    return "Chip-8";
                    break;

                case SCHIP8:
                    return "SChip-8";
                    break;

                case MCHIP8:
                    return "MChip-8";
                    break;

                case CHIP8_HiRes:
                    return "HiRes Chip-8";
                    break;

                default:
                    return "Not supported yet";
            }
        }
        unsigned short getWidth() { return _width; }
        unsigned short getHeight() { return _height; }
        unsigned short getBytesPerPixel() { return _bytesPerPixel; }
        unsigned long getScreenSizeOf() { return (_width * _height * _bytesPerPixel); }

    protected:

        void setColorTheme(Chip8ColorTheme value) { _colorTheme = value; }
        void setInverseColor(bool value) { _inverseColor = value; }

        TCOLOR getColor(Chip8ColorTheme color) {
            TCOLOR tCol;

            switch (color) {
            case INVERSE:
                tCol.foreColor = 0;
                tCol.backColor = 0xFFFFFFFF;
                break;

            case GAMEBOY:
                tCol.foreColor = 0x103F10FF;
                tCol.backColor = 0x9CB916FF;
                break;

            case C64:
                tCol.foreColor = 0x11BCFFFF;
                tCol.backColor = 0x0019FFFF;
                break;

            case RED:
                tCol.foreColor = 0xFF0000FF;
                tCol.backColor = 0xFF;
                break;

            case GREEN:
                tCol.foreColor = 0xFF00FF;
                tCol.backColor = 0xFF;
                break;

            case BLUE:
                tCol.foreColor = 0xFFFF;
                tCol.backColor = 0xFF;
                break;

            case DEFAULT:
            default:
                tCol.foreColor = 0xFFFFFFFF;
                tCol.backColor = 0xFF;
                break;
            }
            if (_inverseColor) {
                tCol.foreColor = tCol.foreColor + tCol.backColor;
                tCol.backColor = tCol.foreColor - tCol.backColor;
                tCol.foreColor = tCol.foreColor - tCol.backColor;
            }
            return tCol;
        }

        void setType(Chip8Types Type) {
            _bytesPerPixel = 4;
            _type = Type;
            switch (Type) {
                case CHIP8:
                    _width = 64;
                    _height = 32;
                    break;
                case CHIP8_HiRes2:
                    _width = 64;
                    _height = 48;
                    break;
                case CHIP8_HiRes:
                    _width = 64;
                    _height = 64;
                    break;
                case SCHIP8:
                    if (_extendedMode) {
                        _width = 128;
                        _height = 64;
                    } else {
                        _width = 64;
                        _height = 32;
                    }
                    break;
                case MCHIP8:
                    if (_extendedMode) {
                        _width = 256;
                        _height = 192;
                    } else {
                        /* According to some docs (and SCHIP behavior to switch to a lower resolution when extended mode is off
                        _width = 128;
                        _height = 64;

                        In fact, it can cause some demos (Megamaze, MegaSirpinsky) not to work
                        */
                        _width = 256;
                        _height = 192;
                    }
                    break;
            }
        }

    private:
        unsigned short _width;
        unsigned short _height;

        // Mega-Chip8
        unsigned short _spriteWidth;
        unsigned short _spriteHeight;
        Chip8BlendModes _spriteBlendMode;
        byte _collisionColorIndex;

        bool _extendedMode;

        byte  _bytesPerPixel;
        Chip8Types _type;
        Chip8ColorTheme _colorTheme;
        bool _inverseColor;
};

class Chip8: public BaseCHIP8
{
    public:

        Chip8();
        virtual ~Chip8();

        // Accesseurs - Display
        unsigned char *getScreen() { return _gfx; };

        // Keyboard
        bool getKey(unsigned char key) { return _key[key]; }
        void setKey(unsigned char key, bool value) { _key[key] = value; }

        // Timers
        unsigned char timerSound() { return _timerSound; }
        unsigned char timerDelay() { return _timerDelay; }

        // Sound
        unsigned char *getSoundBuffer() { return _soundBuffer; }
        unsigned long getSoundBufferSize() { return _soundBufferSize; }
        unsigned int getSoundBufferFrequency() { return _soundBufferFrequency; }
        unsigned char getSoundRepeat() { return _soundRepeat; }
        // Read once property
        bool getSoundRefresh() { if (_soundRefresh) { _soundRefresh = false; return true; } return _soundRefresh; }
        SoundState getSoundState() { return _soundState; }

        // Others
        void setColorTheme(Chip8ColorTheme value);
        void setInverseColor(bool value);
        bool getSyncClockToOriginal() { return _syncClockToOriginal; }
        void SetSyncClockToOriginal(bool value) { _syncClockToOriginal = value; }
        void ChangeMachineType(Chip8Types Type, bool extMode);
        bool loaded() { return _loaded; }
        void initialize(Chip8Types Type);
        void reset();
        bool loadGame(const char *filename);
        void loadBios();
        void loadKeypad() ;
        void execute(double frequencyInMs);
        bool isRunning() { return _isRunning; }
		void destroy();

    private:

        bool _loaded;
        bool _isRunning;
        bool _needToRefresh;
        bool _soundRefresh;
        SoundState _soundState;

        // Does the clock follow the CPU Frequency or it stick to the base frequency of 60 Hz ?
        bool _syncClockToOriginal;

        // Current instruction
        unsigned short _opcode;

        /* Memory Map
            0x000-0x1FF - Chip 8 interpreter (contains font set in emu)
            0x050-0x0A0 - Used for the built in 4x5 pixel font set (0-F)
            0x200-0xFFF - Program ROM and work RAM
        */
        /* Chip 8 4096 - Mega More */
        unsigned char *_memory;
        unsigned int _memorySize;

        // 16 registers
        unsigned char _V[16];
        // HP48 Registers (8) - Opcodes FX75 & FX85 - Compatibility
        unsigned char _V48[8];

        // Index register
        /* Chip 8 short - MChip8 int */
        unsigned int _I;

        // Program Counter [0x000 .. 0xFFF]
        /* Chip 8 short - MChip8 int */
        unsigned int _pc;

        // Graphic 64 * 32 pixels
        unsigned char *_gfx;
        unsigned char *_gfxBuffer;
        unsigned char *_soundBuffer;
        unsigned long _soundBufferSize;
        unsigned int _soundBufferFrequency;
        unsigned char _soundRepeat;
        // Mega Chip-8 - 255 Color palette + alpha
        unsigned int _palette[256];

        // Timers at 60 Hz;
        unsigned char _timerDelay;
        unsigned char _timerSound;

        // Stack
        stack<unsigned int> _callStack;
        deque<string> _opcodeTrace;
        //unsigned short _stack[16];
        //unsigned short _sp;

        // Keys
        bool _key[16];

        // Function used by opcodes
        // Graphics
        void drawScreen(byte coordX, byte coordY, byte K);
        void flip();
        void clearGfx();
        void clearScreen();
        bool opcodesMega(byte Code, byte KK,  byte K);
        void opcodesSuper(byte Code, byte K);
        void opcodesCalc(byte Code, byte X, byte Y);
        void opcodesKeyboard(byte Code, byte X);
        void opcodesOther(byte Code, byte X);

        // Get/Set pixel ** Buffer Only **
        void setPixel(byte X, byte Y, byte R, byte G, byte B, byte Alpha);
        void setPixel(byte X, byte Y, unsigned int color);
        void setPixel(unsigned int adr, byte R, byte G, byte B, byte Alpha);
        void setPixel(unsigned int adr, unsigned int color);

        unsigned int getPixelAdr(byte X, byte Y);
        unsigned int getPixel(byte X, byte Y);
        unsigned int getPixel(unsigned int adr);
        unsigned int getMegaColor(int spriteX, int spriteY, int destX, int destY);

        // Animations
        void setScreenAlpha(float alpha);
        void scrollDown(byte lines);
        void scrollUp(byte lines);
        void scrollLeft();
        void scrollRight();

        // Sound
        void playSound(byte repeat);
        void clearSound();

        // Memory
        void writeMemB(unsigned int adr, byte toWrite);
        void writeMemS(unsigned int adr, unsigned short int toWrite);
        byte readMemB(unsigned int adr);
        unsigned short int readMemS(unsigned int adr);

        // functions
        void refreshScreen(TCOLOR colorFrom, TCOLOR colorTo);
        void loadFont();
        void load(char *rom, unsigned int size);

        // Logs
        void backtrace(unsigned short int Code, unsigned short int pc);
        void backtrace(string trace);
        void printBacktrace();
};

#endif // CHIP8_H
