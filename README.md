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

COMPILE
=======

After adding the required libs to your system, you can compile with Code::Block or simply
type in your shell

`
`
`
./configure
make
cd bin/Release
./Mega8
`
`
`

Makefile created by cbp2mak & autotools.

KEYBOARD
========

Keyboard is binded like this :

Original CHIP-8 Keypad              PC Numpad

    1 | 2 | 3 | C                7  | 8 |   9   | DEL
    --+---+---+--               ----+---+-------+--
    4 | 5 | 6 | D                4  | 5 |   6   | ADD
    --+---+---+--               ----+---+-------+--
    7 | 8 | 9 | E                1  | 2 |   3   | SUB
    --+---+---+--               ----+---+-------+--
    A | 0 | B | F               MUL | 0 | ENTER | DIV

This can't be changed right now, that can make
some games horrible to play, because some games don't use the same
keys to move... Some uses 4-8-6-2 (Astrododge), others 1-5-3-2, ...
You can check the keyboard at anytime by pressing 'K'.

GRAPHICS
========

It uses OpenGL for rendering, it's quite fast. You can use texture filtering
to make it look less like 'Minecraft' by pressing 'L' ;-).
You can change the display size freely or you can make it x1, x2,... of the
original resolution.
To ENTER fullscreen mode, just press 'F'.
To display some informations, just type 'H' (the HUD can make a huge drop down
to the actual FPS... at least on my old system).

SPEED
=====

Since all documents I've found are unclear about the real frequency of the chip,
I'm basing on the clock speed from the COSMAC VIP, about 1.76 Khz.
But you can press CTRL+/- to change the speed from 44 Hz to 1.8 Mhz (from 1/20x up to 1024x).
When you change the frequency, the base clock stays still @60Hz (Delay and sound),
but you can make it follow the actual frequency you've set by pressing 'S'.

SOUND
=====

Not much right now, just a beep...

NOTES
=====

This emulator is still in development because I want to add some
more features to it, like:

    - Mega-Chip support;
    - CDP1802 opcodes;
    - Support for hybrid Chip-8 roms;
    - Input configuration;
    - Better sound support (actually a wav file is played for beep).
    - Internationalization support;
    - Load/Save State;
    - Screenshot or video.

NOTES 2
=======

Sorry for the french comments in source files ! I'll change them if I have time.
For more credits check the About menu !
