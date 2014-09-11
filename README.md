Mega8
=====

Mega/Super/HiRes/Chip-8 Emulator in C/C++, OpenGL, SDL, wxWidgets.
I made it just to see if I was capable of doing an emulator and to learn how
OpenGL/wxWidgets works. You can use the source as you want, but if you use the source,
it would be kind to add a reference to me somewhere in your project ;-). It has been coded
with Code::Blocks 13.12, gcc 4.8.1 on Linux Mint 16, it should compile as well
on other systems, be aware that you must have:

    - Code::Blocks, since it has a .cbp project file, but you can choose
      the IDE you like;
    - wxWidgets 3.0.1 development files;
    - OpenGL Support in wxWidget (wxGLCanvas);
    - SDL2 and SDL_Mixer;
    - C++11 std support(I use Chrono from std, not from libboost)

HOW TO USE
==========

## Quick Manual

# Shortcuts

CTRL+O: Open a ROM (File | Open)
CTRL+C: Close a ROM (File | Close)
CTRL+R: Reset ROM (Emulation | Reset)
CTRL+SPACE: Starts / Pause (Emulation | Start / Pause)
ESC: Exit application (File | Quit)
CTRL+F: Set fullscreen mode (Emulation | Graphics | Fullscreen)
CTRL+L: Set opengl filter (Emulation | Graphics | Filter Texture)
CTRL+H: Display some informations on screen (Emulation | Graphics | Display HUD)
CTRL+A: Set automatic mode frequency  (Emulation | Speed | Automatic)
CTRL+Y: Set clock timer to 60 Hz, else follow current frequency

# Other Menus

File | Color Themes: Replace black / white colors with some funny ones
File | Graphics: You can also change here the size of the window regarding the original resolution
File | Speed: You can choose from 1/40x to 1024x the original COSMAC VIP base frequency (1760 Hz), useful for debugging purpose and smoothness of Twisterbar Demo ;-)
File | Input: There you can change the keyboard mapping in relation to the ROM you play
File | About: Some build infos, shortcuts list and some credits

## Changes

Mega8 1.2
---------

Windows / Linux
- Added key configuration editor (Can be set by profile/ROM)
- Corrected big mistake in sound !!! (length and frequency)
- It supports from 11025 Hz up to 44100 Hz (still in 8bits mono)
- With that correction, you can have more than one sound :-P
- Now supports saving settings like Filtering activated, Fullscreen,...
- Changed shortcuts (All "command" shortcuts must be set with CTRL+shortcut)
- Size of main window defaults to 5x the sizeof Chip-8 resolution
- Main window appear at the center of the screen
- Correction with std::chrono precision. In Linux it delivers nano and microseconds as well, in Windows, only milliseconds (Aaargh !)
  Now wxStopWatch is used, in Windows it make use of QueryPerformanceCounter
- Small corrections on scrolling parts
- Corrections when changing frequencies
- Correction when calculating FPS
- ...

Windows Only
- Binaries finally released !!
- Both in 32 and 64 bits

Linux Only:
- Binaries are in 64bits mode only
- Command line compilation broken :-(

## COMPILE

After adding the required libs to your system, you can compile with Code::Blocks. Here are the required libs:

- wxWidgets 3.0.1
- FTGL
- SDL 2
- SDL_Mixer 2
- Freetype 2

Command line compilation (Using Makefile) is broken with 1.2 due to added Windows (i686 and x86_64) builds. 
I'll fix that soon.

## KEYBOARD

Keyboard is binded like this by default :

    Original CHIP-8 Keypad              PC Numpad

    1 | 2 | 3 | C                7  | 8 |   9   | DEL
    --+---+---+--               ----+---+-------+--
    4 | 5 | 6 | D                4  | 5 |   6   | ADD
    --+---+---+--               ----+---+-------+--
    7 | 8 | 9 | E                1  | 2 |   3   | SUB
    --+---+---+--               ----+---+-------+--
    A | 0 | B | F               MUL | 0 | ENTER | DIV

This can be changed with 1.2 simply go to Emulation | Input and set the keys you want. 
It is saved by profile / game.

## GRAPHICS

It uses OpenGL for rendering, it's quite fast. It supports texture filtering
to make it look less like 'Minecraft'. It has also a little on screen display with some
informations regarding the system (the HUD can make a huge drop down
to the actual FPS... at least on my old system).

## SPEED

Since all documents I've found are unclear about the real frequency of the chip,
I'm basing on the clock speed from the COSMAC VIP, about 1.76 Khz.
But you can press CTRL+/- to change the speed from 44 Hz to 1.8 Mhz (from 1/20x up to 1024x).
When you change the frequency, the base clock stays still @60Hz (Delay and sound),
but you can make it follow the actual frequency you've set by pressing 'S'.

## SOUND

**Chip-8/SChip-8**: SDL_Mixer is used to play a simple beep at 440Hz
**MChip8**: Full support for sound, check Twister Demo ;-)

## NOTES

This emulator is still in development because I want to add some
more features to it, like:

    - ~~Mega-Chip support~~ Done !;
    - Mega-Chip small modifications I have in mind;
    - CDP1802 opcodes;
    - Support for hybrid Chip-8 roms;
    - ~~Input configuration~~ Done (1.2) !;
    - ~~Better sound support (actually a wav file is played for beep)~~ Done !;
    - Internationalization support;
    - Load/Save State;
    - Screenshot or video.

## NOTES 2

Sorry for the french comments in source files ! I'll change them if I have time.
For more credits check the About menu !
