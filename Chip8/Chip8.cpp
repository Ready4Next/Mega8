<<<<<<< HEAD
#include "Chip8.h"
#ifndef WIN32
	#include <unistd.h>
#endif

#if defined(_MSC_VER) || defined(__MINGW32__)  || defined(WIN32)
int gettimeofday(struct timeval* tp, void* tzp) {
	typedef std::chrono::high_resolution_clock Clock;

	auto duration = Clock::now().time_since_epoch();
	tp->tv_sec = (long)chrono::duration_cast<chrono::seconds>(duration).count();
    tp->tv_usec = (long)chrono::duration_cast<chrono::microseconds>(duration).count();
    // Success ;-)
    return 0;
}
#endif

Chip8::Chip8(): BaseCHIP8()
{
    //ctor
    _gfx = NULL;
    _gfxBuffer = NULL;
    _soundBuffer = NULL;
    _soundRepeat = 0;
    _memory = NULL;
    _memorySize = 0;
    _syncClockToOriginal = true;
    this->initialize(CHIP8);
}

Chip8::~Chip8() {
    //dtor
}

void Chip8::destroy() {
	_loaded = false;
    _isRunning = false;
    if (_soundBuffer != NULL)
        free(_soundBuffer);
    if (_gfx != NULL)
        free(_gfx);
    if (_gfxBuffer != NULL)
        free(_gfxBuffer);
    if (_memory != NULL)
        free(_memory);
    _soundBuffer = NULL;
    _gfx = NULL;
    _gfxBuffer = NULL;
    _memory = NULL;
}

void Chip8::initialize(Chip8Types Type = CHIP8) {
    // Init
    unsigned int i;

    ChangeMachineType(Type, getExtendedMode());

    if (_memory != NULL)
        free(_memory);
    // Default for Chip8
    _memorySize = 4096;
    _memory = (unsigned char *) malloc(_memorySize);

    for (i = 0x200; i < _memorySize; i++)
        _memory[i] = 0;

    reset();

    _loaded = false;
    _isRunning = false;
}

void Chip8::reset() {
    // reset rom
    unsigned int i;

    // Mega-Chip Sound
	if (_soundBuffer != NULL)
		free(_soundBuffer);
    _soundBuffer = NULL;
    _soundRepeat = 0;

	ChangeMachineType(CHIP8, false);

	_loaded = false;
	_pc = 0x200;
	_timerDelay = 0;
	_timerSound = 0;
	_I = 0;

	for (i = 0; i < 16; i++) {
		_key[i] = false;
		_V[i] = 0;
	}

	_isRunning = false;
}

/*
 * Change the machine type
 * Type: CHIP8, SCHIP8, CHIP8_HiRes, CHIP8_HiRes2, MCHIP8
 * extMode: Determine machine's extended mode (SCHIP, MCHIP)
 */

void Chip8::ChangeMachineType(Chip8Types Type, bool extMode) {
    setExtendedMode(extMode);
    setType(Type);

    // Allocate the screen
    if (_gfx != NULL)
        free(_gfx);
    _gfx = (unsigned char*)malloc(getScreenSizeOf());
	
	if (_gfx != NULL) {
		if (_gfxBuffer != NULL)
			free(_gfxBuffer);
		_gfxBuffer = (unsigned char *)malloc(getScreenSizeOf());

		if (_gfxBuffer != NULL) {
			// Clear screen & gfx
			clearScreen();
			clearGfx();
		} else {
			printf("Can't allocate screenBuffer (%dx%dx%d) !\n", getWidth(), getHeight(), getBytesPerPixel());
			if (_gfx != NULL)
				free(_gfx);

			_gfx = NULL;
			_gfxBuffer = NULL;
		}
	} else {
		if (_gfxBuffer != NULL)
			free(_gfxBuffer);
		printf("Can't allocate screen (%dx%dx%d) !\n", getWidth(), getHeight(), getBytesPerPixel());

		_gfx = NULL;
		_gfxBuffer = NULL;
	}
}

/********************************** Graphics **********************************/

/*
 * Get/Set a pixel on screen buffer
 * adr: Direct Access to buffer (Usually: ((Y * ScreenWidth) + X) * BytesPerPixels)
 * X, Y: Coordinates in screen resolution
 * R, G, B, Alpha: The color's components
 * color: Color to write (RGBA Format)
 */

unsigned int Chip8::getPixelAdr(byte X, byte Y) {
    // Screen overflow
    if (getType() != MCHIP8) {
        X %= getWidth();
        Y %= getHeight();
    } else {
        if (X > getWidth() || Y > getHeight())
            return 0;
    }
    // Calc adr
    unsigned int adr = ((Y * getWidth()) + X) * getBytesPerPixel();
    if (adr < getScreenSizeOf()) {
        return adr;
    } else {
        printf("Chip8::getPixelAdr(X,Y): Out of screen (%d x %d)", X, Y);
        return 0;
    }
}

void Chip8::setPixel(byte X, byte Y, byte R, byte G, byte B, byte Alpha, bool onBuffer = true) {
    
	byte *curGfx = (onBuffer ? _gfxBuffer : _gfx);

	if (curGfx != NULL) {
        unsigned int adr = getPixelAdr(X, Y);
        setPixel(adr, R, G, B, Alpha, onBuffer);
    } else {
        printf("CHIP8::setPixel(X,Y,RGBA): gfxBuffer is NULL !\n");
    }
}

void Chip8::setPixel(byte X, byte Y, unsigned int color, bool onBuffer = true) {
    
	byte *curGfx = (onBuffer ? _gfxBuffer : _gfx);

	if (curGfx != NULL) {
        unsigned int adr = getPixelAdr(X, Y);
        setPixel(adr, color, onBuffer);
    } else {
        printf("CHIP8::setPixel(X,Y,color): gfxBuffer is NULL !\n");
    }
}

unsigned int Chip8::getPixel(byte X, byte Y, bool onBuffer = true) {

	byte *curGfx = (onBuffer ? _gfxBuffer : _gfx);
	
	if (curGfx != NULL) {
        unsigned int adr = getPixelAdr(X, Y);
        return getPixel(adr, onBuffer);
    } else {
        printf("CHIP8::getPixel(X,Y): gfxBuffer is NULL !\n");
        return 0;
    }
}

void Chip8::setPixel(unsigned int adr, byte R, byte G, byte B, byte Alpha, bool onBuffer = true) {

	byte *curGfx = (onBuffer ? _gfxBuffer : _gfx);

	if (curGfx != NULL) {
        if (adr < getScreenSizeOf()) {
            // Colors are in RGBA
            curGfx[adr    ] = R;
			curGfx[adr + 1] = G;
			curGfx[adr + 2] = B;
			curGfx[adr + 3] = Alpha;
        } else {
            printf("CHIP8::setPixel(adr,RGBA): Trying to write from outside the screen ! Adress: %04X.\n", adr);
        }
    } else {
        printf("CHIP8::setPixel(adr,RGBA): gfxBuffer is NULL !\n");
    }
}

void Chip8::setPixel(unsigned int adr, unsigned int color, bool onBuffer = true) {
    
	byte *curGfx = (onBuffer ? _gfxBuffer : _gfx);
	
	if (curGfx != NULL) {
        // Colors are in RGBA
        byte R = getR(color);
        byte G = getG(color);
        byte B = getB(color);
        byte Alpha = getAlpha(color);

        setPixel(adr, R, G, B, Alpha, onBuffer);
    } else {
        printf("CHIP8::setPixel(adr, color): gfxBuffer is NULL !\n");
    }
}

unsigned int Chip8::getPixel(unsigned int adr, bool onBuffer = true) {
   
	byte *curGfx = (onBuffer ? _gfxBuffer : _gfx);
	
	if (curGfx != NULL) {
        // Ensure we're on screen
        if (adr < getScreenSizeOf()) {
            // Array Address
			return getRGBA(curGfx[adr    ],
						   curGfx[adr + 1],
                           curGfx[adr + 2],
                           curGfx[adr + 3]);
        } else {
            printf("CHIP8::getPixel(adr): Trying to read from outside of the screen ! Adress: %04X.\n", adr);
            return 0;
        }
    } else {
        printf("CHIP8::getPixel(adr): gfxBuffer is NULL !\n");
        return 0;
    }
}

/*
 * Get Mega Color
 * spriteX: Current pixel position X of the sprite to draw
 * spriteY: Current pixel position Y of the sprite to draw
 * destX  : Current pixel destination in screen coordinates of the sprite to draw
 * destY  : Current pixel destination in screen coordinates of the sprite to draw
 * Todo   : Pixel blending mode(0=Normal,1=25%,2=50%,3=75%,4=Additive,5=Multiply)
 * Return the pixel color to be drawn
 */

unsigned int getColorAlpha(unsigned int color, float alpha) {
    int R = (int)(alpha * getR(color));
    int G = (int)(alpha * getG(color));
    int B = (int)(alpha * getB(color));

    return getRGBA(R, G, B, 255);
}

unsigned int BlendAlpha(unsigned int color, unsigned int newColor, float alpha) {
    byte R = (byte)(alpha * getR(color)) + ((1 - alpha) * getR(newColor));
    byte G = (byte)(alpha * getG(color)) + ((1 - alpha) * getG(newColor));
    byte B = (byte)(alpha * getB(color)) + ((1 - alpha) * getB(newColor));

    return getRGBA(R, G, B, 255);
}

// Not used but can be cool
/*
unsigned int BlendAverage(unsigned int color, unsigned int newColor, float alpha) {
    byte R = (byte)((getR(color) + getR(newColor)) / 2);
    byte G = (byte)((getG(color) + getG(newColor)) / 2);
    byte B = (byte)((getB(color) + getB(newColor)) / 2);

    return getRGBA(R, G, B, 255);
}*/

unsigned int BlendAdd(unsigned int color, unsigned int newColor, float alpha) {
    byte R = (byte)min((int)(getR(color) + getR(newColor)), 255);
    byte G = (byte)min((int)(getG(color) + getG(newColor)), 255);
    byte B = (byte)min((int)(getB(color) + getB(newColor)), 255);

    return getRGBA(R, G, B, 255);
}

unsigned int BlendMul(unsigned int color, unsigned int newColor) {
    int RMul = (getR(color) * getR(newColor));
    int GMul = (getG(color) * getG(newColor));
    int BMul = (getB(color) * getB(newColor));
    byte R = (byte)(RMul / 0xFF);
    byte G = (byte)(GMul / 0xFF);
    byte B = (byte)(BMul / 0xFF);

    return getRGBA(R, G, B, 255);
}

void Chip8::setScreenAlpha(float alpha) {
    if (alpha < 1 && alpha > 0) {
        for (unsigned int i = 0; i < getScreenSizeOf(); i += getBytesPerPixel()) {
            // Calculate all pixels new alpha value from Screen
			unsigned int color = getPixel(i % 4, false);

            // Calculate already defined color alpha
            if (getAlpha(color) != 255)
                color = getColorAlpha(color, (float)getAlpha(color) / 255.0);

            // Calculate the new alpha set by the instruction & blend it with the buffer
            color = BlendAlpha(getPixel(i), color, alpha);

            // Set the new pixel directly on screen
			setPixel(i % 4, color, false);
        }
    }
}

unsigned int Chip8::getMegaColor(int spriteX, int spriteY, int destX, int destY) {
    // In the palette, colors are already in RGBA
    unsigned int color = _palette[readMemB(_I + spriteX + spriteY)];
    unsigned int oldColor = getPixel(destX, destY, true);

    // if color == 0, draw transparent
    if ((color >> 8) == 0) {
        //color = oldColor;
        return oldColor;
    } else if (getAlpha(color) != 255) {
        // Draw Alpha
        int alpha = getAlpha(color) / 255.0;

        // Calculate Alpha Component
        color = getColorAlpha(color, alpha);
    }

    // Mega Chip-8 Blend Modes - http://fr.wikibooks.org/wiki/Programmation_avec_la_SDL/La_transparence
    switch (getSpriteBlendMode()) {
        case BLEND_ADD:
            color = BlendAdd(color, oldColor, getAlpha(color) / 255.0);
            break;

        case BLEND_MUL:
            color = BlendMul(color, oldColor);
            break;

        case BLEND_25:
            color = BlendAlpha(color, oldColor, 0.25);
            break;

        case BLEND_50:
            color = BlendAlpha(color, oldColor, 0.50);
            break;

        case BLEND_75:
            color = BlendAlpha(color, oldColor, 0.75);
            break;

        case BLEND_NORMAL:
        default:
            // Nothing todo, color is the same
            break;
    }

    return color;
}

/*
 * Clear graphic buffer
 */

void Chip8::clearGfx() {
    int color;

    if (_gfxBuffer != NULL) {
        for (unsigned int i = 0; i < getScreenSizeOf(); i += getBytesPerPixel()) {
            color = getColor(getColorTheme()).backColor;
            setPixel(i, color);
        }
    }
}

// Private set all pixel to black non transparent
void Chip8::clearScreen() {
    int color;

    if (_gfx != NULL) {
        for (unsigned int i = 0; i < getScreenSizeOf(); i += getBytesPerPixel()) {
            color = getColor(getColorTheme()).backColor;
            setPixel(i, color, false);
        }
    }
}

/*
 * CHIP & SCHIP Color Themes
 */

void Chip8::setInverseColor(bool value) {
    TCOLOR col = getColor(getColorTheme());
    BaseCHIP8::setInverseColor(value);
    refreshScreen(col, getColor(getColorTheme()));
}

void Chip8::setColorTheme(Chip8ColorTheme value) {
    refreshScreen(getColor(getColorTheme()), getColor(value));
    BaseCHIP8::setColorTheme(value);
}

// If I used a bit-map instead of color-map, I wouldn't have to use that ...
void Chip8::refreshScreen(TCOLOR colorFrom, TCOLOR colorTo) {
    unsigned int actualColor, replaceColor;

    if (_gfxBuffer != NULL && getType() != MCHIP8) {
        for (unsigned int i = 0; i < getScreenSizeOf(); i += getBytesPerPixel()) {
            actualColor = getPixel(i);
            if (actualColor == colorFrom.foreColor) {
                replaceColor = colorTo.foreColor;
            } else {
                replaceColor = colorTo.backColor;
            }

            setPixel(i, replaceColor);
        }

        // Show changes
        flip();
    }
}

/* 0xDXYK
 * Draw K lines from Memory to GFX
 * Draws an 8xK (width x height) sprite at the coordinates given in register X and Y.
 * Chip8: If height specified is 0, draws a 16x16 sprite.
 * Mega : If height and width are 0, draws a 256x256 sprite.
 */

void Chip8::drawScreen(unsigned char coordX, unsigned char coordY, unsigned char K) {
    // Place l'index au début d'un caractère
    // Lit K lignes de pixels à partir de I et les affiche aux coordonnées (V[X], V[Y])
    // Reset flag
    _V[0xF] = 0;
    // Color Theme
    unsigned int color;

    // Mega Chip-8
    if (getType() == MCHIP8) {
        // The size of the sprite is set by an opcode (If height or width = 0 => 256)
        setSpriteHeight( (getSpriteHeight() == 0) ? 256 : getSpriteHeight() );
        setSpriteWidth ( (getSpriteWidth() == 0 ) ? 256 : getSpriteWidth() );
    } else {
        setSpriteWidth(8);
        setSpriteHeight(K);

        if (K == 0) {
            setSpriteHeight(16);
            // In extendedMode sprites are 16x16
            if (getExtendedMode())
                setSpriteWidth(16);
        }
    }

    // Count the bytes to load - Sprite may be less than 8 pixels ;-)
    int offset = max(1, (int)ceil( getSpriteWidth() ) / 8);

    // Pour chaque ligne
    for (unsigned int Y = 0; Y < getSpriteHeight(); Y++) {
        // Load bytes 1 by 1
        for (int iByte = 0; iByte < offset; iByte++) {
            // Get the first byte of the line to draw + bytes already readed
            byte byteToDraw = readMemB(_I + (Y * offset) + iByte);
            if (byteToDraw > 0) {
                byteToDraw = byteToDraw;
            }
            // Pour chaque pixels de la ligne - Une ligne = 8 pixels, sauf si K == 0
            int spriteWidth = min((int) getSpriteWidth(), 8);
            for (byte bit = 0; bit < spriteWidth; bit++) {
                // Calculate proper X coord for the sprite
                int X = bit + (iByte * spriteWidth);

                // Extrait le pixel à écrire (0 ou 1)
                unsigned char pixelToWrite = ((byteToDraw << bit) >> (spriteWidth - 1)) & 1;

                // On calcule l'adresse (On récupère tout byte par byte, ne pas oublier d'inclure le décalage) !
                unsigned int pixelAdr = getPixelAdr(coordX + X, coordY + Y);

                // get old pixel color (for collision detection) => RGB
                unsigned int oldPixelColor = getPixel(pixelAdr);

                // Si le pixel != noir, l'ancien pixel est 1
                unsigned char oldPixel;

                // Mega Chip8 le pixel est toujours le même suivant la palette
                if (getType() == MCHIP8) {
                    color = getMegaColor(X, Y * getSpriteWidth(), coordX + X, coordY + Y);

                    // Undocumented
                    oldPixel = ((_palette[getCollisionColorIndex()] == oldPixelColor) ? 1 : 0);

                    // Collision Detection - New pixel not black and pixel in buffer == ColorCollision
                    if (((color >> 8) != 0) && oldPixel != 0)
                        _V[0xF] = 1;
                } else {
                    // CHIP8, SCHIP8, HIRES-CHIP8: Si le pixel à écrire est différent du pixel présent
                    oldPixel = (oldPixelColor == getColor(getColorTheme()).foreColor) ? 1 : 0;
                    if ((pixelToWrite ^ oldPixel) == 1) {
                        // Draw theme foreColor
                        color = getColor(getColorTheme()).foreColor;
                    } else {
                        // Draw theme backColor
                        color = getColor(getColorTheme()).backColor;
                    }

                    // Collision Detection
                    //_V[0xF] = (byte)(oldPixel & pixelToWrite);
                    if (pixelToWrite != 0 && oldPixel != 0)
                        _V[0xF] = 1;
                }

                // Finally draw the pixel
                setPixel(pixelAdr, color);
            }
        }
    }
}

/*
 * 0x00CK
 * Scroll down K lines: Note that this opcode will scroll down the entire screen N lines.
 * /!\ Anything that gets outside the screen is erased (***)
 */

 // Mega: Now acts directly on the screen and takes missing pixels on buffer (Cool effect)

void Chip8::scrollDown(byte lines) {
    int nbBytesToMove;
    int start;
    unsigned int color;

    if (getType() == SCHIP8 || getType() == MCHIP8)
        nbBytesToMove = (getWidth() * getBytesPerPixel()) * lines;
    else
        nbBytesToMove = (int)ceil(lines / 2.0f) * ((getWidth() * getBytesPerPixel())/2);

    start = getScreenSizeOf() - nbBytesToMove;

    for (int i = getScreenSizeOf(); i >= 0; i -= 4) {
        // On bouge nos bytes
        if (start > 0) {
            // If it's transparent Color
            color = getRGB(_gfx[start    ], _gfx[start + 1], _gfx[start + 2]);

            if ((_gfx[start + 3] == 254 && getType() == MCHIP8) || color == 0) {
				// Take pixel from buffer
				unsigned int col = (getPixel(i) & 0xFFFFFF00) | 0xFE;
				// Set it to screen
				setPixel(i, col, false);
            } else {
                // else we take pixel on the screen
				setPixel(i, getPixel(start, false), false);
            }

            // Do the same in buffer if we're not in Mega Mode
            if (getType() != MCHIP8) {
                setPixel(i, getPixel(start));
            }
        } else {
            // We replace the missing pixels by
            if (getType() == MCHIP8) {
                // Transparent color in MChip mode
                color = getPixel(i) & 0xFFFFFF00;
            } else {
                // Selected background color in theme
                color = getColor(getColorTheme()).backColor;
            }

            // not buffer anymore
			setPixel(i, (color & 0xFFFFFF00) | 0xFE, false);

            // Do the same in buffer if we're not in Mega Mode
            if (getType() != MCHIP8) {
                setPixel(i, color);
            }
        }

        start -= 4;
    }
}

void Chip8::scrollUp(byte lines) {
    int nbBytesToMove, start;
    unsigned int color;
    int colorPos;

    if (getType() == SCHIP8 || getType() == MCHIP8)
        nbBytesToMove = (getWidth() * getBytesPerPixel()) * lines;
    else
        nbBytesToMove = (int)ceil(lines / 2.0f) * (getWidth() * getBytesPerPixel());

    start = nbBytesToMove;

    for (unsigned int i = 0; i < getScreenSizeOf(); i += 4) {
        // We move the color on the screen
        if (start < (int)(getScreenSizeOf() - nbBytesToMove)) {
            // If it's transparent Color
            color = getRGB(_gfx[start    ], _gfx[start + 1], _gfx[start + 2]);
            if ((_gfx[start + 3] == 254 && getType() == MCHIP8) || color == 0) {
                // Take pixel from buffer
				unsigned int col = (getPixel(i) & 0xFFFFFF00) | 0xFE;
				// Set it to screen
				setPixel(i, col, false);
            } else {
                // else we take pixel on the screen
				setPixel(i, getPixel(start, false), false);
            }

            // Do the same in buffer if we're not in Mega Mode
            if (getType() != MCHIP8) {
                setPixel(i, getPixel(start));
            }
        } else {
            // We replace the missing pixels by
            if (getType() == MCHIP8) {
                // Transparent color in MChip mode
                color = getPixel(i) & 0xFFFFFF00;
            } else {
                // Selected background color in theme
                color = getColor(getColorTheme()).backColor;
            }

            // not buffer anymore
            setPixel(i, (color & 0xFFFFFF00) | 0xFE, false);

            // Do the same in buffer if we're not in Mega Mode
            if (getType() != MCHIP8) {
                setPixel(i, color);
            }
        }

        start += 4;
    }
}

/*
 * Scroll 4 pixels right: Note that this opcode will scroll down the entire screen N lines.
 * /!\ Anything that gets outside the screen is erased (***)
 * /!\ 2 pixels in CHIP8 mode
 */

void Chip8::scrollRight() {
    byte shift;
    unsigned int realWidth;
    unsigned int color;

    shift = ((getType() != CHIP8) ? 4 * getBytesPerPixel() : 2 * getBytesPerPixel());

    realWidth = getWidth() * getBytesPerPixel();
    // For each lines
    for (int i = 0; i < getHeight(); i++) {
        // For each col
        for (int j = (realWidth - shift); j >= 0  ; j -= 4 ) {
            // Ce byte
            int current =  (i * realWidth) + j ;
            int newPosition = current + shift;

            if (getType() == MCHIP8) {
                // If it's transparent Color
                color = getRGB(_gfx[current    ], _gfx[current + 1], _gfx[current + 2]);
                if (_gfx[current + 3] == 254 && getType() == MCHIP8) {
					setPixel(newPosition, (getPixel(newPosition) & 0xFFFFFF00) | 0xFE, false);
                } else {
                    // else we take pixel on the screen
					setPixel(newPosition, getPixel(current, false), false);
                }
            } else {
                // And if we're not in MegaMode...
                // Do the same in buffer if we're not in Mega Mode
                setPixel(newPosition, getPixel(current));
            }

            if (j <= shift) {
                // We replace the missing pixels by
                if (getType() == MCHIP8) {
                    // Transparent color in MChip mode
                    color = getPixel(current) & 0xFFFFFF00;
                } else {
                    // Selected background color in theme
                    color = getColor(getColorTheme()).backColor;
                }

                // not buffer anymore
				setPixel(current, (color & 0xFFFFFF00) | 0xFE, false);

                // Do the same in buffer if we're not in Mega Mode
                if (getType() != MCHIP8) {
                    setPixel(current, color);
                }
            }
        }
    }
}

/*
 * Scroll 4 pixels left: Note that this opcode will scroll down the entire screen N lines.
 * /!\ Anything that gets outside the screen is erased (***)
 * /!\ 2 pixels in CHIP8 mode
 */

void Chip8::scrollLeft() {
    byte shift;
    int realWidth;
    unsigned int color;

    shift = ((getType() != CHIP8) ? 4 * getBytesPerPixel() : 2 * getBytesPerPixel());

    realWidth = getWidth() * getBytesPerPixel();
    // For each lines
    for (int i = 0; i < getHeight(); i++) {
        // For each col
        for (int j = shift; j < realWidth; j += 4 ) {
            // This byte
            int current =  (i * realWidth) + j ;
            int newPosition = current - shift;

            if (getType() == MCHIP8) {
                // If it's transparent Color
                color = getRGB(_gfx[current    ], _gfx[current + 1], _gfx[current + 2]);
                if ((_gfx[current + 3    ] == 254 && getType() == MCHIP8) || color == 0) {
					setPixel(newPosition, (getPixel(newPosition) & 0xFFFFFF00) | 0xFE, false);
                } else {
                    // else we take pixel on the screen
					setPixel(newPosition, getPixel(current, false), false);
                }
            } else {
                // And if we're not in MegaMode...
                // Do the same in buffer if we're not in Mega Mode
                setPixel(newPosition, getPixel(current));
            }

            // If we are over the old screen
            if (j >= (realWidth - shift)) {
                // We replace the missing pixels by
                if (getType() == MCHIP8) {
                    // Transparent color in MChip mode
                    color = getPixel(current) & 0xFFFFFF00;
                } else {
                    // Selected background color in theme
                    color = getColor(getColorTheme()).backColor;
                }

                // not buffer anymore
				setPixel(current, (color & 0xFFFFFF00) | 0xFE, false);

                // Do the same in buffer if we're not in Mega Mode
                if (getType() != MCHIP8) {
                    setPixel(current, color);
                }
            }
        }
    }
}

/********************************** Sound **********************************/

/*
 * Put wav sound mono 8bits into buffer
 */

void Chip8::playSound(byte repeat) {
    // Copy to soundBuffer
    if (_soundBuffer != NULL)
        free(_soundBuffer);
    // Apparently, the first two bytes are for the frequency
    // The next 3 bytes is the size
    //unsigned long bufSize = (_memorySize - _I - 4096);
    int BkpI = _I;
    _soundBufferFrequency = readMemS(BkpI);
    BkpI += 2;
    _soundBufferSize = (readMemS(BkpI) << 8) | readMemB(BkpI + 2); //bufSize;
    BkpI += 8;
    _soundBuffer = (unsigned char*)malloc(_soundBufferSize);
    // K = 0 => infinite; -1 for SDL_Mixer
    _soundRepeat = repeat - 1;

    for (unsigned int i = 0; i < _soundBufferSize; i++) {
        _soundBuffer[i] = readMemB(BkpI + i);
    }
    _soundRefresh = true;
    _soundState = OPEN;
}

void Chip8::clearSound() {
    if (_soundBuffer != NULL)
        free(_soundBuffer);
    _soundBuffer = NULL;
    _soundBufferSize = 0;
    _soundRepeat = 0;
    _soundRefresh = true;
    _soundState = CLOSE;
}

/********************************** Memory **********************************/

/*
 * Write byte in _memory
 */

void Chip8::writeMemB(unsigned int adr, byte toWrite) {
    if (_memory != NULL) {
        if (adr < _memorySize) {
            _memory[adr] = toWrite;
        }  else {
            printBacktrace();
            printf("CHIP8::writeMemB(adr, byte): adr out of bounds ! (Adr: %04X > MemorySize: %08X)\n", adr, _memorySize);
        }
    } else {
        printBacktrace();
        printf("CHIP8::writeMemB(adr, byte): _memory is NULL !\n");
    }
}

/*
 * Read byte in _memory
 */

byte Chip8::readMemB(unsigned int adr) {
    if (_memory != NULL) {
        if (adr < _memorySize) {
            return _memory[adr];
        }  else {
            printBacktrace();
            printf("CHIP8::ReadMemB(adr): adr out of bounds ! (Adr: %04X > MemorySize: %08X)\n", adr, _memorySize);
        }
    } else {
        printBacktrace();
        printf("CHIP8::ReadMemB(adr): _memory is NULL !\n");
    }
    return 0;
}

/*
 * Write short int in _memory
 */

unsigned short int Chip8::readMemS(unsigned int adr) {
    if (_memory != NULL) {
        if ((adr < _memorySize) && ((adr + 1) < _memorySize)) {
            return (unsigned short int) (_memory[adr] << 8) | _memory[adr + 1];
        }  else {
            printf("CHIP8::ReadMemS(adr): adr out of bounds ! (Adr/Adr+1: %04X(%04X) > MemorySize: %08X)\n", adr, adr + 1, _memorySize);
        }
    } else {
        printf("CHIP8::ReadMemS(adr): _memory is NULL !\n");
    }
    return 0;
}

/********************************** Opcodes **********************************/

bool Chip8::opcodesMega(byte Code, byte KK,  byte K) {
    string str;
    int to;
    // Mega-Chip8 0x0XNN (NNNN) Opcodes
    switch (Code) {
        case 0x0:
            // Not Mega-Chip8 (ClearScreen (NN: 0xE0) and return from routine (NN: 0xEE)
            break;
        case 0x1:
            // Mega-Chip8 0x01NN NNNN
            to = readMemS(_pc);
            _I = (KK << 16) + readMemS(_pc);
            char buf[50];
            sprintf(buf, "%04X: LDI I, %04X %04X", _pc-2, KK, readMemS(_pc));
            str = buf;
            backtrace(str);
            _pc += 2;
            return true;
            break;

        case 0x2:
            // 0x02KK -  Load nn-colors palette at I - Colors are in ARGB Format, ours are in RGBA
            int currentPALAddress;

            // Black / Transparent
            _palette[0] = 0;

            for (int i = 0; i <= KK; i++) {
                // La couleur 0 se trouve à _I - bytesPerPixels, la couleur 1 est à _I
                currentPALAddress = _I + ( i * getBytesPerPixel());

                _palette[i + 1] = getRGBA(readMemB(currentPALAddress + 1),
                                          readMemB(currentPALAddress + 2),
                                          readMemB(currentPALAddress + 3),
                                          readMemB(currentPALAddress));
            }
            return true;
            break;

        case 0x3:
            // 03KK 	 Set Sprite-width to nn
            setSpriteWidth(KK);
            return true;
            break;

        case 0x4:
            // 04nn+ 	 Set Sprite-height to nn	(SPRH  nn)
            setSpriteHeight(KK);
            return true;
            break;

        case 0x05:
            //05nn+ 	 Set Screenalpha to nn
            printf("TODO: ScreenAlpha - %04X\n", _opcode);
            setScreenAlpha((float)KK / 255.0);
            return true;
            break;

        case 0x06:
            // 060n+ 	 Play digitised sound at I	(DIGISND), will add n for loop/noloop
            printBacktrace();
            printf("%04X: DGISND, play sound at: %08X , Loop: %d\n", _pc-2, _I, K);
            playSound(K);
            return true;
            break;

        case 0x07:
            // 0700+ 	 Stop digitised sound 		(STOPSND)
            printf("%04X: STOPSND, stop sound\n", _pc-2);
            clearSound();
            return true;
            break;

        case 0x08:
            // 080n+ 	 Set sprite blendmode 		(BMODE n) (0=normal,1=25%,2=50%,3=75%,4=addative,5=multiply)
            switch (K) {
                case 1:
                    setSpriteBlendMode(BLEND_25);
                    break;
                case 2:
                    setSpriteBlendMode(BLEND_50);
                    break;
                case 3:
                    setSpriteBlendMode(BLEND_75);
                    break;
                case 4:
                    setSpriteBlendMode(BLEND_ADD);
                    break;
                case 5:
                    setSpriteBlendMode(BLEND_MUL);
                    break;
                case 0:
                default:
                    setSpriteBlendMode(BLEND_NORMAL);
            }
            return true;
            break;

        case 0x09:
            // Undocumented - MCHIP8 Set collision color index
            setCollisionColorIndex(K);
            return true;
            break;

        default:
            printBacktrace();
            printf("Unknown MEGA opcode: %04X (%02X) at address %04X.\n", _opcode, Code, _pc-2);
    }
    return false;
}

void Chip8::opcodesSuper(byte Code, byte K) {
    switch (Code) {
        case 0x10:
            // Mega-Chip8 Resolution 256x192
            ChangeMachineType(MCHIP8, false);
            break;
        case 0x11:
            // Standard SCHIP8 Resolution (Mega-Chip8)
            ChangeMachineType(MCHIP8, true);
            break;
        case 0xE0:
			// Force VBlank everytime we draw in chip8 modes
			_VBlank = getType() != MCHIP8;
            // Clear screen - ATT in MCHIP8 mode this is used to update the screen
            if (getType() == MCHIP8) {
                //TODO: screenbuffer
                flip();
				_VBlank = true;
            }
            clearGfx();
            break;
        case 0xEE:
            // Return from subroutine
            if (_callStack.size() > 0) {
                _pc = _callStack.top();
                _callStack.pop();
            } else {
                printf("Invalid return from subroutine (Code: %04X), no returning address from stack ! (@%04X)\n", _opcode, _pc-2);
                _pc -= 2;
            }
            break;
        // SCHIP8
        case 0xFB:
			_VBlank = true;
            scrollRight();
            break;
        case 0xFC:
			_VBlank = true;
            scrollLeft();
            break;
        case 0xFD:
            // Quit the emulator
            printf("Rom ask to exit emulation.\n");
            break;
        case 0xFE:
            // Set CHIP8 Graphic Mode
            ChangeMachineType(SCHIP8, false);
            break;
        case 0xFF:
            // Set SCHIP8, MCHIP8 Extended Graphic Mode
            if (getType() != MCHIP8)
                ChangeMachineType(SCHIP8, true);
            else
                ChangeMachineType(MCHIP8, false);
            break;
        default:
            printBacktrace();
            printf("Unknown Super opcode: %04X (%02X) at address %04X.\n", _opcode, Code, _pc-2);
    }
}

/*
 * Opcodes for calculation (bitwise ops, add,...)
 */

void Chip8::opcodesCalc(byte Code, byte X, byte Y) {
    unsigned int tmp;

    switch (Code) {
    case 0x0:
        // V[X] = _V[Y]
        _V[X] = _V[Y];
        break;
    case 0x1:
        // V[X] = V[X] or V[Y]
        _V[X] |= _V[Y];
        break;
    case 0x2:
        // V[X] = V[X] and V[Y]
        _V[X] &= _V[Y];
        break;
    case 0x3:
        // V[X] = V[X] xor V[Y]
        _V[X] ^= _V[Y];
        break;
    case 0x4:
        // V[X] = V[X] + V[Y]
        // Si V[X] > 255, flag de report (v[0xF]) mis à 1
        // Result = 8 derniers bits
        tmp  = _V [X] + _V [Y];
        _V [0xF] = (unsigned char)((tmp > 255) ? 1 : 0);
        _V [X] = (unsigned char)((tmp % 256) & 0XFF);
        break;
    case 0x5:
        // V[X] = V[X] - V[Y]
        // Si V[X] >= V[Y], flag de report mis à 1  /!\ Si le résultat est à 0, VF = 12 !
        _V[0xF] = (unsigned char)((_V[X] >= _V[Y]) ? 1 : 0);
        _V[X] -= _V[Y];
        break;
    case 0x6:
        // Shift Right V[X]
        // Si le dernier bit de V[X] = 1, le flag de report est mis à 1
        _V[0xF] = (unsigned char)(_V[X] & 1);
        _V[X] = (unsigned char)(_V[X] >> 1);
        break;
    case 0x7:
        // V[X] = V[Y] - V[X]
        // Si V[Y] > V[X], flag de report mis à 1
        _V[0xF] = (unsigned char)((_V[Y] >= _V[X]) ? 1 : 0);
        _V[X] = (unsigned char)(_V[Y] - _V[X]);
        break;
    case 0xE:
        // Shift left V[X]
        // Si le dernier bit de V[X] = 1, le flag de report est mis à 1
        _V[0xF] = (unsigned char)((_V[X] & 0x80) >> 7);
        _V[X] = (unsigned char)(_V[X] << 1);
    break;
    default:
        printBacktrace();
        printf("Unkown Calc opcode: %04X (%02X) at address %04X.\n", _opcode, Code, _pc-2);
    }
}

/*
 * Keyboard opcodes
 */

void Chip8::opcodesKeyboard(byte Code, byte X) {
    switch (Code) {
        case 0x9E:
            // Passe l'instruction suivante si la touche V[X] est pressée
            _pc += (unsigned short)((_key[_V[X]]) ? 2 : 0);
            break;
        case 0xA1:
            // Passe l'instruction suivante si la touche V[X] est relâchée
            _pc += (unsigned short)((!_key[_V[X]]) ? 2 : 0);
            break;
        default:
            printBacktrace();
            printf("Unkown Keyboard opcode: %04X (%02X) at address %4X.\n", _opcode, Code, _pc-2);
    }
}

/*
 * Other opcodes
 */

void Chip8::opcodesOther(byte Code, byte X) {
    unsigned int tmp, maxOverflow, i;

    switch (Code) {
    case 0x07:
        // La valeur du timer delay est mise dans _V[X]
        _V[X] = _timerDelay;
        break;
    case 0x0A:
        // Attends qu'une touche soit pressée
        // Place le code de la touche dans V[X]
        for (i = 0; i < sizeof(_key); i++) {
            if (_key[i] == true) {
                _V[X] = i;
                return;
            }
        }
        // Not found - Redo the same opcode
        _pc -= 2;
        break;
    case 0x15:
        // Set timer delay
        _timerDelay = _V[X];
        break;
    case 0x18:
        // Set timer sound
        _timerSound = _V[X];
        break;
    case 0x1E:
        // Add V[X] to Index register
        // /!\ VF is set on overflow, undocumented feature, used on Spacefight 2019!
        tmp = _V[X] + _I;
        // Max overflow on MChip8 is 0xFFFFFF
        maxOverflow = ((getType() == MCHIP8) ? 0xFFFFFF : 0xFFF);
        _V[0xF] = ((tmp > maxOverflow) ? 1 : 0);
        _I = tmp & maxOverflow;
        break;
    case 0x29:
        // I reçoit la position du sprite V[X];
        // Les sprites ont une taille de 5 bytes et se trouvent à
        // _memory[0x0 à 0x1FF]
        // In CHIP8, _I is maximum 0xFFF !
        _I = (unsigned short)((_V[X] * 5) & 0xFFF);

        // ATT: MegaChip Sprite Size must be accorded - It overwrites sprite size
        setSpriteWidth(8);
        setSpriteHeight(5);
        break;
    // SCHIP8
    case 0x30:
        // I reçoit la position du sprite V[X];
        // Les sprites ont une taille de 10 bytes et se trouvent à
        // _memory[0x0 à 0x1FF]
        // Je les ai placés à partir de la position 80... Doivent-ils être lu depuis 0 ?
        _I = (unsigned short)(((_V[X] * 10) + 80) & 0xFFF);

        setSpriteWidth(8);
        setSpriteHeight(10);
        break;
    case 0x33:
        // Place les décimales de V[X] en mémoire
        // en commençant par l'adresse I
        writeMemB(_I    , (unsigned char)((_V[X] / 100)));
        writeMemB(_I + 1, (unsigned char)(((_V[X] % 100) / 10)));
        writeMemB(_I + 2, (unsigned char)(((_V[X] % 100) % 10)));
        break;
    case 0x55:
        // Place les valeurs des registres V[0] à V[X] en mémoire à partir de I
        // /!\ Selon certaines docs, le registre I doit être incrémenté... En pratique NE PAS LE FAIRE !
        for (i = 0; i <= X; i++) {
            writeMemB(_I + i, _V[i]);
        }
        break;
    case 0x65:
        // Récupère les valeurs des registres V[0] à V[X] en mémoire à partir de I
        // /!\ Selon certaines docs, le registre I doit être incrémenté... En pratique NE PAS LE FAIRE !
        for (i = 0; i <= X; i++) {
            _V[i] = readMemB(_I + i);
        }
        break;
    // HP48 registers
    case 0x75:
        // Place les valeurs des registres HP48 V[0] à V[7] en mémoire à partir de I
        // /!\ Selon certaines docs, le registre I doit être incrémenté... En pratique NE PAS LE FAIRE !
        for (i = 0; i <= X; i++) {
            _V48[i] = _V[i];
        }
        break;
    case 0x85:
        // Place les valeurs des registres HP48 V[0] à V[7] en mémoire à partir de I
        // /!\ Selon certaines docs, le registre I doit être incrémenté... En pratique NE PAS LE FAIRE !
        for (i = 0; i <= X; i++) {
            _V[i] = _V48[i];
        }
        break;
    default:
        printBacktrace();
        printf("Unkown Other opcode: %04X (%02X) at address %04X.\n", _opcode, Code, _pc-2);
    }
}

void Chip8::backtrace(string trace) {
    _opcodeTrace.push_back(trace);
    if (_opcodeTrace.size() > 30) {
        _opcodeTrace.pop_front();
    }
}

void Chip8::backtrace(unsigned short int Code, unsigned short int pc) {
    char buffer[20];
    sprintf(buffer, "%04X: %04X", pc, Code);
    string trace = buffer;
    _opcodeTrace.push_back(trace);
    if (_opcodeTrace.size() > 30) {
        _opcodeTrace.pop_front();
    }
}

void Chip8::printBacktrace() {
    std::deque<string>::iterator it = _opcodeTrace.begin();

    while (it != _opcodeTrace.end()) {
        string line = *(it++);
        printf("Trace %02d: %s\n", (int)((it - _opcodeTrace.begin())-30),  line.c_str());
    }
}

void Chip8::execute(double frequencyInMs = BASE_CLOCK_MS) {
    // Paramètres possibles - Décompose les possibilités de l'opcode
    unsigned short code;	    // 1er byte
    unsigned short NNN;		    // Une adresse - 3 derniers bytes
    unsigned char KK;		    // 2 derniers bytes
    unsigned char K;			// dernier byte
    unsigned char X;			// 2ème
    unsigned char Y;			// 3ème

    // This need C++11 compilator setting - ATT: In windows, the precision is in milliseconds ! For our timers @60hthat's no big deal...
    static auto t_start = chrono::high_resolution_clock::now();
    auto t_end = chrono::high_resolution_clock::now();
    double t;

    if (!_loaded)
        return;

    _isRunning = true;

    // Comme chaque emplacement est 1 byte et que le code opération est de 2
    // On shift de 8 bits le byte actuel
    _opcode = readMemS(_pc); // (unsigned short) (_memory [_pc] << 8 | _memory [_pc + 1]);
    // Log
    backtrace(_opcode, _pc);

    code = (unsigned short)(_opcode & 0xF000);
    NNN = (unsigned short)(_opcode & 0x0FFF);
    KK = (unsigned char)(_opcode & 0x00FF);
    K = (unsigned char)(_opcode & 0x000F);
    X = (unsigned char)((_opcode & 0x0F00) >> 8);
    Y = (unsigned char)((_opcode & 0x00F0) >> 4);

    // What is going to be executed
    //printf("PC: %04X - OpCode: %04X - Code: %04X - Current Frequency: %.4f - T: %ld\n", _pc, _opcode, code, frequencyInMs, t_start);

    _pc += 2;

    // Initialize Clock
    //t = clock();
    switch (code) {
    case 0x0000:
        // Special Codes for SCHIP & MCHIP Implementation
        if (Y == 0xC && X == 0x0) {
			_VBlank = true;
            scrollDown(K);
            break;
        } else if (Y == 0xB && X == 0x0) {
            // Misplaced 00BN+    Scroll display N lines up	(SCRU n)
            _VBlank = true;
            scrollUp(K);
            break;
        }

        // HiRes Chip8 Clearscreen
        if (NNN == 0x230) {
            clearGfx();
			_VBlank = true;
            break;
        }

        if (!opcodesMega(X, KK, K)) {
            opcodesSuper(KK, K);
        }

        break;


    case 0x1000:
        // Jump to adr - /!\ Activate Chip8 hires mode
        if (_pc == 0x202 && NNN == 0x260) {
            // Enable 64x64 screen
            ChangeMachineType(CHIP8_HiRes, true);
            // Continue program
            _pc = 0x2C0;
        } else
            // Normal jump
            _pc = NNN;
        break;

    case 0x2000:
        // Call subroutine
        _callStack.push(_pc);
        _pc = NNN;
        break;

    case 0x3000:
        // Si V[X] == valeur, on passe l'instruction suivante
        if (_V[X] == KK) {
            _pc += 2;
        }
        break;

    case 0x4000:
        // Si V[X] != valeur, on passe l'instruction suivante
        if (_V[X] != KK) {
            _pc += 2;
        }
        break;

    case 0x5000:
        // Si V[X] == V[Y], on passe l'instruction suivante
        if (_V[X] == _V[Y]) {
            _pc += 2;
        }
        break;

    case 0x6000:
        // Set Register X to KK
        _V[X] = KK;
       break;

    case 0x7000:
        // Set Register X to Register X + KK;
        // ATT: No carry in doc, we set one
        _V[X] += KK;
        break;

    case 0x8000:
        // Do the calcs
        opcodesCalc(K, X, Y);
        break;

    case 0x9000:
        // Si V[X] != V[Y], on passe l'instruction suivante
        if (_V[X] != _V[Y]) {
            _pc += 2;
        }
        break;

    case 0xA000:
        // I = NNN
        _I = NNN;
        break;

    case 0xB000:
        // Jump to V[0] + NNN
        _pc = (unsigned short)(_V[0] + NNN);
        break;

    case 0xC000:
        // _V[X] = Random and KK
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        _V[X] = (unsigned char)(KK & (unsigned char)(rand() %  0XFF));
        break;

    case 0xD000:
		// Force VBlank everytime we draw in chip8 modes
		_VBlank = getType() != MCHIP8;
        drawScreen(_V[X], _V[Y], K);
        break;

    case 0xE000:
        opcodesKeyboard(KK, X);
        break;

    case 0xF000:

        // Jump to 24bit Address ???
        /*if (X == 0 && getType() == MCHIP8) {
            _I = (KK << 16) + (_memory [_pc] << 8 | _memory [_pc + 1]);
            _pc += 2;
        }*/

        opcodesOther(KK, X);
        break;

    default:
        printBacktrace();
        printf("Unkown General opcode: %04X at address %04X.\n", _opcode, _pc-2);

    }

    t_end = chrono::high_resolution_clock::now();
    t = chrono::duration_cast<chrono::milliseconds>(t_end - t_start).count();

    // Force redraw but not for MCHIP with ExtendedModeOn=TimedToCLS
    if (getType() != MCHIP8) {
        flip();
    }

    if ((t >= BASE_CLOCK_MS) || !_syncClockToOriginal) {

        if (_timerDelay>0)
            _timerDelay--;

        if (_timerSound>0)
            _timerSound--;

        t_start = chrono::high_resolution_clock::now();
    }
}

void Chip8::flip() {
    if (_gfx != NULL && _gfxBuffer != NULL) {
        memcpy(_gfx, _gfxBuffer, getScreenSizeOf());
    }
}

void Chip8::loadFont() {
    unsigned int i;

    for (i = 0; i < _memorySize; i++) {
        if (i < sizeof(Chip8Font) + 1)
            _memory[i] = Chip8Font[i];
        else
            _memory[i] = 0;
    }
}

bool Chip8::loadGame(const char *filename) {
    bool res = false;

    if (filename != "") {
        std::ifstream file(filename, std::ios::binary);
        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        char *rom = new char[size];
        if ( file.read(rom, size) ) {
            load(rom, size);
            file.close();
            res = true;
        }
        delete[] rom;
    }
    return res;
}

// Charge le faux BIOS
void Chip8::loadBios() {
    load((char*)bios_ch8, bios_ch8_len);
}

// Charge keypad
void Chip8::loadKeypad() {
    load((char*)keypad_ch8, keypad_ch8_len);
}

// Charge une ROM en mémoire
void Chip8::load(char *rom, unsigned int size) {
    // Reset Emu
    initialize();

    if (_memory != NULL)
        free(_memory);

    // I'm generous, I always give 4Ko up ;-) (Blinky, MegaBlinky)
    _memorySize = size + 0x200 + 4096;
    _memory = (unsigned char *)malloc(_memorySize);

    // TODO:
    loadFont();

    for (unsigned int i = 0; i < size ; i++)
        _memory[0x200 + i] = rom[i];

    _loaded = true;
}
=======
#include "Chip8.h"
#ifndef WIN32
	#include <unistd.h>
#endif

#if defined(_MSC_VER) || defined(__MINGW32__)  || defined(WIN32)
int gettimeofday(struct timeval* tp, void* tzp) {
	clock_t t;

    t = clock();
	float secs = (float)t/CLOCKS_PER_SEC;
    tp->tv_sec = (long)floor(secs);
    tp->tv_usec = (long)floor(secs * 1000);
    // Success ;-)
    return 0;
}
#endif

Chip8::Chip8(): BaseCHIP8()
{
    //ctor
    _gfx = NULL;
    _gfxBuffer = NULL;
    _soundBuffer = NULL;
    _soundRepeat = 0;
    _memory = NULL;
    _memorySize = 0;
    _syncClockToOriginal = true;
    this->initialize(CHIP8);
}

Chip8::~Chip8() {
    //dtor
}

void Chip8::destroy() {
	_loaded = false;
    _isRunning = false;
    if (_soundBuffer != NULL)
        free(_soundBuffer);
    if (_gfx != NULL)
        free(_gfx);
    if (_gfxBuffer != NULL)
        free(_gfxBuffer);
    if (_memory != NULL)
        free(_memory);
    _soundBuffer = NULL;
    _gfx = NULL;
    _gfxBuffer = NULL;
    _memory = NULL;
}

void Chip8::initialize(Chip8Types Type = CHIP8) {
    // Init
    unsigned int i;

    ChangeMachineType(Type, getExtendedMode());

    if (_memory != NULL)
        free(_memory);
    // Default for Chip8
    _memorySize = 4096;
    _memory = (unsigned char *) malloc(_memorySize);

    for (i = 0x200; i < _memorySize; i++)
        _memory[i] = 0;

    reset();

    _loaded = false;
    _isRunning = false;
}

void Chip8::reset() {
    // reset rom
    unsigned int i;

    // Mega-Chip Sound
	if (_soundBuffer != NULL)
		free(_soundBuffer);
    _soundBuffer = NULL;
    _soundRepeat = 0;

	ChangeMachineType(CHIP8, false);

	_loaded = false;
	_pc = 0x200;
	_timerDelay = 0;
	_timerSound = 0;
	_I = 0;

	for (i = 0; i < 16; i++) {
		_key[i] = false;
		_V[i] = 0;
	}

	_isRunning = false;
}

/*
 * Change the machine type
 * Type: CHIP8, SCHIP8, CHIP8_HiRes, CHIP8_HiRes2, MCHIP8
 * extMode: Determine machine's extended mode (SCHIP, MCHIP)
 */

void Chip8::ChangeMachineType(Chip8Types Type, bool extMode) {
    setExtendedMode(extMode);
    setType(Type);

    // Allocate the screen
    if (_gfx != NULL)
        free(_gfx);
    _gfx = (unsigned char*)malloc(getScreenSizeOf());
	
	if (_gfx != NULL) {
		if (_gfxBuffer != NULL)
			free(_gfxBuffer);
		_gfxBuffer = (unsigned char *)malloc(getScreenSizeOf());

		if (_gfxBuffer != NULL) {
			// Clear screen & gfx
			clearScreen();
			clearGfx();
		} else {
			printf("Can't allocate screenBuffer (%dx%dx%d) !\n", getWidth(), getHeight(), getBytesPerPixel());
			if (_gfx != NULL)
				free(_gfx);

			_gfx = NULL;
			_gfxBuffer = NULL;
		}
	} else {
		if (_gfxBuffer != NULL)
			free(_gfxBuffer);
		printf("Can't allocate screen (%dx%dx%d) !\n", getWidth(), getHeight(), getBytesPerPixel());

		_gfx = NULL;
		_gfxBuffer = NULL;
	}
}

/********************************** Graphics **********************************/

/*
 * Get/Set a pixel on screen buffer
 * adr: Direct Access to buffer (Usually: ((Y * ScreenWidth) + X) * BytesPerPixels)
 * X, Y: Coordinates in screen resolution
 * R, G, B, Alpha: The color's components
 * color: Color to write (RGBA Format)
 */

unsigned int Chip8::getPixelAdr(byte X, byte Y) {
    // Screen overflow
    if (getType() != MCHIP8) {
        X %= getWidth();
        Y %= getHeight();
    } else {
        if (X > getWidth() || Y > getHeight())
            return 0;
    }
    // Calc adr
    unsigned int adr = ((Y * getWidth()) + X) * getBytesPerPixel();
    if (adr < getScreenSizeOf()) {
        return adr;
    } else {
        printf("Chip8::getPixelAdr(X,Y): Out of screen (%d x %d)", X, Y);
        return 0;
    }
}

void Chip8::setPixel(byte X, byte Y, byte R, byte G, byte B, byte Alpha) {
    if (_gfxBuffer != NULL) {
        unsigned int adr = getPixelAdr(X, Y);
        setPixel(adr, R, G, B, Alpha);
    } else {
        printf("CHIP8::setPixel(X,Y,RGBA): gfxBuffer is NULL !\n");
    }
}

void Chip8::setPixel(byte X, byte Y, unsigned int color) {
    if (_gfxBuffer != NULL) {
        unsigned int adr = getPixelAdr(X, Y);
        setPixel(adr, color);
    } else {
        printf("CHIP8::setPixel(X,Y,color): gfxBuffer is NULL !\n");
    }
}

unsigned int Chip8::getPixel(byte X, byte Y) {
    if (_gfxBuffer != NULL) {
        unsigned int adr = getPixelAdr(X, Y);
        return getPixel(adr);
    } else {
        printf("CHIP8::getPixel(X,Y): gfxBuffer is NULL !\n");
        return 0;
    }
}

void Chip8::setPixel(unsigned int adr, byte R, byte G, byte B, byte Alpha) {
    if (_gfxBuffer != NULL) {
        if (adr < getScreenSizeOf()) {
            // Colors are in RGBA
            _gfxBuffer[adr    ] = R;
            _gfxBuffer[adr + 1] = G;
            _gfxBuffer[adr + 2] = B;
            _gfxBuffer[adr + 3] = Alpha;
        } else {
            printf("CHIP8::setPixel(adr,RGBA): Trying to write from outside the screen ! Adress: %04X.\n", adr);
        }
    } else {
        printf("CHIP8::setPixel(adr,RGBA): gfxBuffer is NULL !\n");
    }
}

void Chip8::setPixel(unsigned int adr, unsigned int color) {
    if (_gfxBuffer != NULL) {
        // Colors are in RGBA
        byte R = getR(color);
        byte G = getG(color);
        byte B = getB(color);
        byte Alpha = getAlpha(color);

        setPixel(adr, R, G, B, Alpha);
    } else {
        printf("CHIP8::setPixel(adr, color): gfxBuffer is NULL !\n");
    }
}

unsigned int Chip8::getPixel(unsigned int adr) {
    if (_gfxBuffer != NULL) {
        // Ensure we're on screen
        if (adr < getScreenSizeOf()) {
            // Array Address
            return getRGBA(_gfxBuffer[adr    ],
                           _gfxBuffer[adr + 1],
                           _gfxBuffer[adr + 2],
                           _gfxBuffer[adr + 3]);
        } else {
            printf("CHIP8::getPixel(adr): Trying to read from outside of the screen ! Adress: %04X.\n", adr);
            return 0;
        }
    } else {
        printf("CHIP8::getPixel(adr): gfxBuffer is NULL !\n");
        return 0;
    }
}

/*
 * Get Mega Color
 * spriteX: Current pixel position X of the sprite to draw
 * spriteY: Current pixel position Y of the sprite to draw
 * destX  : Current pixel destination in screen coordinates of the sprite to draw
 * destY  : Current pixel destination in screen coordinates of the sprite to draw
 * Todo   : Pixel blending mode(0=Normal,1=25%,2=50%,3=75%,4=Additive,5=Multiply)
 * Return the pixel color to be drawn
 */

unsigned int getColorAlpha(unsigned int color, float alpha) {
    int R = (int)(alpha * getR(color));
    int G = (int)(alpha * getG(color));
    int B = (int)(alpha * getB(color));

    return getRGBA(R, G, B, 255);
}

unsigned int BlendAlpha(unsigned int color, unsigned int newColor, float alpha) {
    byte R = (byte)(alpha * getR(color)) + ((1 - alpha) * getR(newColor));
    byte G = (byte)(alpha * getG(color)) + ((1 - alpha) * getG(newColor));
    byte B = (byte)(alpha * getB(color)) + ((1 - alpha) * getB(newColor));

    return getRGBA(R, G, B, 255);
}

// Not used but can be cool
/*
unsigned int BlendAverage(unsigned int color, unsigned int newColor, float alpha) {
    byte R = (byte)((getR(color) + getR(newColor)) / 2);
    byte G = (byte)((getG(color) + getG(newColor)) / 2);
    byte B = (byte)((getB(color) + getB(newColor)) / 2);

    return getRGBA(R, G, B, 255);
}*/

unsigned int BlendAdd(unsigned int color, unsigned int newColor, float alpha) {
    byte R = (byte)min((int)(getR(color) + getR(newColor)), 255);
    byte G = (byte)min((int)(getG(color) + getG(newColor)), 255);
    byte B = (byte)min((int)(getB(color) + getB(newColor)), 255);

    return getRGBA(R, G, B, 255);
}

unsigned int BlendMul(unsigned int color, unsigned int newColor) {
    int RMul = (getR(color) * getR(newColor));
    int GMul = (getG(color) * getG(newColor));
    int BMul = (getB(color) * getB(newColor));
    byte R = (byte)(RMul / 0xFF);
    byte G = (byte)(GMul / 0xFF);
    byte B = (byte)(BMul / 0xFF);

    return getRGBA(R, G, B, 255);
}

void Chip8::setScreenAlpha(float alpha) {
    if (alpha < 1 && alpha > 0) {
        for (unsigned int i = 0; i < getScreenSizeOf(); i += getBytesPerPixel()) {
            // Calculate all pixels new alpha value from Screen
            unsigned int color = getRGBA(_gfx[ i % 4     ],
                                         _gfx[(i % 4) + 1],
                                         _gfx[(i % 4) + 2],
                                         _gfx[(i % 4) + 3]);
            // Calculate already defined color alpha
            if (_gfx[(i % 4) + 3] != 255)
                color = getColorAlpha(color, (float)_gfx[(i % 4) + 3] / 255.0);

            // Calculate the new alpha set by the instruction & blend it with the buffer
            color = BlendAlpha(getPixel(i), color, alpha);

            // Set the new pixel directly on screen
            _gfx[ i % 4     ] = (byte) getR(color) % 0xFF;
            _gfx[(i % 4) + 1] = (byte) getB(color) % 0xFF;
            _gfx[(i % 4) + 2] = (byte) getG(color) % 0xFF;
            _gfx[(i % 4) + 3] = (byte) getAlpha(color) % 0xFF;
        }
    }
}

unsigned int Chip8::getMegaColor(int spriteX, int spriteY, int destX, int destY) {
    // In the palette, colors are already in RGBA
    unsigned int color = _palette[readMemB(_I + spriteX + spriteY)];
    unsigned int oldColor = getPixel(destX, destY);

    // if color == 0, draw transparent
    if ((color >> 8) == 0) {
        //color = oldColor;
        return oldColor;
    } else if (getAlpha(color) != 255) {
        // Draw Alpha
        int alpha = getAlpha(color) / 255.0;

        // Calculate Alpha Component
        color = getColorAlpha(color, alpha);
    }

    // Mega Chip-8 Blend Modes - http://fr.wikibooks.org/wiki/Programmation_avec_la_SDL/La_transparence
    switch (getSpriteBlendMode()) {
        case BLEND_ADD:
            color = BlendAdd(color, oldColor, getAlpha(color) / 255.0);
            break;

        case BLEND_MUL:
            color = BlendMul(color, oldColor);
            break;

        case BLEND_25:
            color = BlendAlpha(color, oldColor, 0.25);
            break;

        case BLEND_50:
            color = BlendAlpha(color, oldColor, 0.50);
            break;

        case BLEND_75:
            color = BlendAlpha(color, oldColor, 0.75);
            break;

        case BLEND_NORMAL:
        default:
            // Nothing todo, color is the same
            break;
    }

    return color;
}

/*
 * Clear graphic buffer
 */

void Chip8::clearGfx() {
    int color;

    if (_gfxBuffer != NULL) {
        for (unsigned int i = 0; i < getScreenSizeOf(); i += getBytesPerPixel()) {
            color = getColor(getColorTheme()).backColor;
            setPixel(i, color);
        }
    }
}

// Private set all pixel to black non transparent
void Chip8::clearScreen() {
    int color;

    if (_gfx != NULL) {
        for (unsigned int i = 0; i < getScreenSizeOf(); i += getBytesPerPixel()) {
            color = getColor(getColorTheme()).backColor;
            _gfx[i    ] = 0;
            _gfx[i + 1] = 0;
            _gfx[i + 2] = 0;
            _gfx[i + 3] = 255;
        }
    }
}

/*
 * CHIP & SCHIP Color Themes
 */

void Chip8::setInverseColor(bool value) {
    TCOLOR col = getColor(getColorTheme());
    BaseCHIP8::setInverseColor(value);
    refreshScreen(col, getColor(getColorTheme()));
}

void Chip8::setColorTheme(Chip8ColorTheme value) {
    refreshScreen(getColor(getColorTheme()), getColor(value));
    BaseCHIP8::setColorTheme(value);
}

// If I used a bit-map instead of color-map, I wouldn't have to use that ...
void Chip8::refreshScreen(TCOLOR colorFrom, TCOLOR colorTo) {
    unsigned int actualColor, replaceColor;

    if (_gfxBuffer != NULL && getType() != MCHIP8) {
        for (unsigned int i = 0; i < getScreenSizeOf(); i += getBytesPerPixel()) {
            actualColor = getPixel(i);
            if (actualColor == colorFrom.foreColor) {
                replaceColor = colorTo.foreColor;
            } else {
                replaceColor = colorTo.backColor;
            }

            setPixel(i, replaceColor);
        }

        // Show changes
        flip();
    }
}

/* 0xDXYK
 * Draw K lines from Memory to GFX
 * Draws an 8xK (width x height) sprite at the coordinates given in register X and Y.
 * Chip8: If height specified is 0, draws a 16x16 sprite.
 * Mega : If height and width are 0, draws a 256x256 sprite.
 */

void Chip8::drawScreen(unsigned char coordX, unsigned char coordY, unsigned char K) {
    // Place l'index au début d'un caractère
    // Lit K lignes de pixels à partir de I et les affiche aux coordonnées (V[X], V[Y])
    // Reset flag
    _V[0xF] = 0;
    // Color Theme
    unsigned int color;

    // Mega Chip-8
    if (getType() == MCHIP8) {
        // The size of the sprite is set by an opcode (If height or width = 0 => 256)
        setSpriteHeight( (getSpriteHeight() == 0) ? 256 : getSpriteHeight() );
        setSpriteWidth ( (getSpriteWidth() == 0 ) ? 256 : getSpriteWidth() );
    } else {
        setSpriteWidth(8);
        setSpriteHeight(K);

        if (K == 0) {
            setSpriteHeight(16);
            // In extendedMode sprites are 16x16
            if (getExtendedMode())
                setSpriteWidth(16);
        }
    }

    // Count the bytes to load - Sprite may be less than 8 pixels ;-)
    int offset = max(1, (int)ceil( getSpriteWidth() ) / 8);

    // Pour chaque ligne
    for (unsigned int Y = 0; Y < getSpriteHeight(); Y++) {
        // Load bytes 1 by 1
        for (int iByte = 0; iByte < offset; iByte++) {
            // Get the first byte of the line to draw + bytes already readed
            byte byteToDraw = readMemB(_I + (Y * offset) + iByte);
            if (byteToDraw > 0) {
                byteToDraw = byteToDraw;
            }
            // Pour chaque pixels de la ligne - Une ligne = 8 pixels, sauf si K == 0
            int spriteWidth = min((int) getSpriteWidth(), 8);
            for (byte bit = 0; bit < spriteWidth; bit++) {
                // Calculate proper X coord for the sprite
                int X = bit + (iByte * spriteWidth);

                // Extrait le pixel à écrire (0 ou 1)
                unsigned char pixelToWrite = ((byteToDraw << bit) >> (spriteWidth - 1)) & 1;

                // On calcule l'adresse (On récupère tout byte par byte, ne pas oublier d'inclure le décalage) !
                unsigned int pixelAdr = getPixelAdr(coordX + X, coordY + Y);

                // get old pixel color (for collision detection) => RGB
                unsigned int oldPixelColor = getPixel(pixelAdr);

                // Si le pixel != noir, l'ancien pixel est 1
                unsigned char oldPixel;

                // Mega Chip8 le pixel est toujours le même suivant la palette
                if (getType() == MCHIP8) {
                    color = getMegaColor(X, Y * getSpriteWidth(), coordX + X, coordY + Y);

                    // Undocumented
                    oldPixel = ((_palette[getCollisionColorIndex()] == oldPixelColor) ? 1 : 0);

                    // Collision Detection - New pixel not black and pixel in buffer == ColorCollision
                    if (((color >> 8) != 0) && oldPixel != 0)
                        _V[0xF] = 1;
                } else {
                    // CHIP8, SCHIP8, HIRES-CHIP8: Si le pixel à écrire est différent du pixel présent
                    oldPixel = (oldPixelColor == getColor(getColorTheme()).foreColor) ? 1 : 0;
                    if ((pixelToWrite ^ oldPixel) == 1) {
                        // Draw theme foreColor
                        color = getColor(getColorTheme()).foreColor;
                    } else {
                        // Draw theme backColor
                        color = getColor(getColorTheme()).backColor;
                    }

                    // Collision Detection
                    //_V[0xF] = (byte)(oldPixel & pixelToWrite);
                    if (pixelToWrite != 0 && oldPixel != 0)
                        _V[0xF] = 1;
                }

                // Finally draw the pixel
                setPixel(pixelAdr, color);
            }
        }
    }
}

/*
 * 0x00CK
 * Scroll down K lines: Note that this opcode will scroll down the entire screen N lines.
 * /!\ Anything that gets outside the screen is erased (***)
 */

 // Mega: Now acts directly on the screen and takes missing pixels on buffer (Cool effect)

void Chip8::scrollDown(byte lines) {
    int nbBytesToMove;
    int start;
    unsigned int color;

    if (getType() == SCHIP8 || getType() == MCHIP8)
        nbBytesToMove = (getWidth() * getBytesPerPixel()) * lines;
    else
        nbBytesToMove = (int)ceil(lines / 2.0f) * ((getWidth() * getBytesPerPixel())/2);

    start = getScreenSizeOf() - nbBytesToMove;

    for (int i = getScreenSizeOf(); i >= 0; i -= 4) {
        // On bouge nos bytes
        if (start > 0) {
            // If it's transparent Color
            color = getRGB(_gfx[start    ], _gfx[start + 1], _gfx[start + 2]);

            if ((_gfx[start + 3] == 254 && getType() == MCHIP8) || color == 0) {
                _gfx[i    ] = _gfxBuffer[i    ];
                _gfx[i + 1] = _gfxBuffer[i + 1];
                _gfx[i + 2] = _gfxBuffer[i + 2];
                _gfx[i + 3] = 254; //_gfxBuffer[i + 3];
            } else {
                // else we take pixel on the screen
                _gfx[i    ] = _gfx[start    ];
                _gfx[i + 1] = _gfx[start + 1];
                _gfx[i + 2] = _gfx[start + 2];
                _gfx[i + 3] = _gfx[start + 3];
            }

            // Do the same in buffer if we're not in Mega Mode
            if (getType() != MCHIP8) {
                setPixel(i, getPixel(start));
            }
        } else {
            // We replace the missing pixels by
            if (getType() == MCHIP8) {
                // Transparent color in MChip mode
                color = getPixel(i) & 0xFFFFFF00;
            } else {
                // Selected background color in theme
                color = getColor(getColorTheme()).backColor;
            }

            // not buffer anymore
            _gfx[i    ] = getR(color);
            _gfx[i + 1] = getG(color);
            _gfx[i + 2] = getB(color);
            _gfx[i + 3] = 254; //getAlpha(color);

            // Do the same in buffer if we're not in Mega Mode
            if (getType() != MCHIP8) {
                setPixel(i, color);
            }
        }

        start -= 4;
    }
}

void Chip8::scrollUp(byte lines) {
    int nbBytesToMove, start;
    unsigned int color;
    int colorPos;

    if (getType() == SCHIP8 || getType() == MCHIP8)
        nbBytesToMove = (getWidth() * getBytesPerPixel()) * lines;
    else
        nbBytesToMove = (int)ceil(lines / 2.0f) * (getWidth() * getBytesPerPixel());

    start = nbBytesToMove;

    for (unsigned int i = 0; i < getScreenSizeOf(); i += 4) {
        // We move the color on the screen
        if (start < (int)(getScreenSizeOf() - nbBytesToMove)) {
            // If it's transparent Color
            color = getRGB(_gfx[start    ], _gfx[start + 1], _gfx[start + 2]);
            if ((_gfx[start + 3] == 254 && getType() == MCHIP8) || color == 0) {
                _gfx[i    ] = _gfxBuffer[i    ];
                _gfx[i + 1] = _gfxBuffer[i + 1];
                _gfx[i + 2] = _gfxBuffer[i + 2];
                _gfx[i + 3] = 254; //_gfxBuffer[i + 3];
            } else {
                // else we take pixel on the screen
                _gfx[i    ] = _gfx[start    ];
                _gfx[i + 1] = _gfx[start + 1];
                _gfx[i + 2] = _gfx[start + 2];
                _gfx[i + 3] = _gfx[start + 3];
            }

            // Do the same in buffer if we're not in Mega Mode
            if (getType() != MCHIP8) {
                setPixel(i, getPixel(start));
            }
        } else {
            // We replace the missing pixels by
            if (getType() == MCHIP8) {
                // Transparent color in MChip mode
                color = getPixel(i) & 0xFFFFFF00;
            } else {
                // Selected background color in theme
                color = getColor(getColorTheme()).backColor;
            }

            // not buffer anymore
            _gfx[i    ] = getR(color);
            _gfx[i + 1] = getG(color);
            _gfx[i + 2] = getB(color);
            _gfx[i + 3] = 254; //getAlpha(color);

            // Do the same in buffer if we're not in Mega Mode
            if (getType() != MCHIP8) {
                setPixel(i, color);
            }
        }

        start += 4;
    }
}

/*
 * Scroll 4 pixels right: Note that this opcode will scroll down the entire screen N lines.
 * /!\ Anything that gets outside the screen is erased (***)
 * /!\ 2 pixels in CHIP8 mode
 */

void Chip8::scrollRight() {
    byte shift;
    unsigned int realWidth;
    unsigned int color;

    shift = ((getType() != CHIP8) ? 4 * getBytesPerPixel() : 2 * getBytesPerPixel());

    realWidth = getWidth() * getBytesPerPixel();
    // For each lines
    for (int i = 0; i < getHeight(); i++) {
        // For each col
        for (int j = (realWidth - shift); j >= 0  ; j -= 4 ) {
            // Ce byte
            int current =  (i * realWidth) + j ;
            int newPosition = current + shift;

            if (getType() == MCHIP8) {
                // If it's transparent Color
                color = getRGB(_gfx[current    ], _gfx[current + 1], _gfx[current + 2]);
                if (_gfx[current + 3] == 254 && getType() == MCHIP8) {
                    _gfx[newPosition    ] = _gfxBuffer[newPosition    ];
                    _gfx[newPosition + 1] = _gfxBuffer[newPosition + 1];
                    _gfx[newPosition + 2] = _gfxBuffer[newPosition + 2];
                    _gfx[newPosition + 3] = 254; //_gfxBuffer[i + 3];
                } else {
                    // else we take pixel on the screen
                    _gfx[newPosition    ] = _gfx[current    ];
                    _gfx[newPosition + 1] = _gfx[current + 1];
                    _gfx[newPosition + 2] = _gfx[current + 2];
                    _gfx[newPosition + 3] = _gfx[current + 3];
                }
            } else {
                // And if we're not in MegaMode...
                // Do the same in buffer if we're not in Mega Mode
                setPixel(newPosition, getPixel(current));
            }

            if (j <= shift) {
                // We replace the missing pixels by
                if (getType() == MCHIP8) {
                    // Transparent color in MChip mode
                    color = getPixel(current) & 0xFFFFFF00;
                } else {
                    // Selected background color in theme
                    color = getColor(getColorTheme()).backColor;
                }

                // not buffer anymore
                _gfx[current    ] = getR(color);
                _gfx[current + 1] = getG(color);
                _gfx[current + 2] = getB(color);
                _gfx[current + 3] = 254; //getAlpha(color);

                // Do the same in buffer if we're not in Mega Mode
                if (getType() != MCHIP8) {
                    setPixel(current, color);
                }
            }
        }
    }
}

/*
 * Scroll 4 pixels left: Note that this opcode will scroll down the entire screen N lines.
 * /!\ Anything that gets outside the screen is erased (***)
 * /!\ 2 pixels in CHIP8 mode
 */

void Chip8::scrollLeft() {
    byte shift;
    int realWidth;
    unsigned int color;

    shift = ((getType() != CHIP8) ? 4 * getBytesPerPixel() : 2 * getBytesPerPixel());

    realWidth = getWidth() * getBytesPerPixel();
    // For each lines
    for (int i = 0; i < getHeight(); i++) {
        // For each col
        for (int j = shift; j < realWidth; j += 4 ) {
            // This byte
            int current =  (i * realWidth) + j ;
            int newPosition = current - shift;

            if (getType() == MCHIP8) {
                // If it's transparent Color
                color = getRGB(_gfx[current    ], _gfx[current + 1], _gfx[current + 2]);
                if ((_gfx[current + 3    ] == 254 && getType() == MCHIP8) || color == 0) {
                    _gfx[newPosition    ] = _gfxBuffer[newPosition    ];
                    _gfx[newPosition + 1] = _gfxBuffer[newPosition + 1];
                    _gfx[newPosition + 2] = _gfxBuffer[newPosition + 2];
                    _gfx[newPosition + 3] = 254; //_gfxBuffer[i + 3];
                } else {
                    // else we take pixel on the screen
                    _gfx[newPosition    ] = _gfx[current    ];
                    _gfx[newPosition + 1] = _gfx[current + 1];
                    _gfx[newPosition + 2] = _gfx[current + 2];
                    _gfx[newPosition + 3] = _gfx[current + 3];
                }
            } else {
                // And if we're not in MegaMode...
                // Do the same in buffer if we're not in Mega Mode
                setPixel(newPosition, getPixel(current));
            }

            // If we are over the old screen
            if (j >= (realWidth - shift)) {
                // We replace the missing pixels by
                if (getType() == MCHIP8) {
                    // Transparent color in MChip mode
                    color = getPixel(current) & 0xFFFFFF00;
                } else {
                    // Selected background color in theme
                    color = getColor(getColorTheme()).backColor;
                }

                // not buffer anymore
                _gfx[current    ] = getR(color);
                _gfx[current + 1] = getG(color);
                _gfx[current + 2] = getB(color);
                _gfx[current + 3] = 254; //getAlpha(color);

                // Do the same in buffer if we're not in Mega Mode
                if (getType() != MCHIP8) {
                    setPixel(current, color);
                }
            }
        }
    }
}

/********************************** Sound **********************************/

/*
 * Put wav sound mono 8bits into buffer
 */

void Chip8::playSound(byte repeat) {
    // Copy to soundBuffer
    if (_soundBuffer != NULL)
        free(_soundBuffer);
    // Apparently, the first two bytes are for the frequency
    // The next 3 bytes is the size
    //unsigned long bufSize = (_memorySize - _I - 4096);
    int BkpI = _I;
    _soundBufferFrequency = readMemS(BkpI);
    BkpI += 2;
    _soundBufferSize = (readMemS(BkpI) << 8) | readMemB(BkpI + 2); //bufSize;
    BkpI += 8;
    _soundBuffer = (unsigned char*)malloc(_soundBufferSize);
    // K = 0 => infinite; -1 for SDL_Mixer
    _soundRepeat = repeat - 1;

    for (unsigned int i = 0; i < _soundBufferSize; i++) {
        _soundBuffer[i] = readMemB(BkpI + i);
    }
    _soundRefresh = true;
    _soundState = OPEN;
}

void Chip8::clearSound() {
    if (_soundBuffer != NULL)
        free(_soundBuffer);
    _soundBuffer = NULL;
    _soundBufferSize = 0;
    _soundRepeat = 0;
    _soundRefresh = true;
    _soundState = CLOSE;
}

/********************************** Memory **********************************/

/*
 * Write byte in _memory
 */

void Chip8::writeMemB(unsigned int adr, byte toWrite) {
    if (_memory != NULL) {
        if (adr < _memorySize) {
            _memory[adr] = toWrite;
        }  else {
            printBacktrace();
            printf("CHIP8::writeMemB(adr, byte): adr out of bounds ! (Adr: %04X > MemorySize: %08X)\n", adr, _memorySize);
        }
    } else {
        printBacktrace();
        printf("CHIP8::writeMemB(adr, byte): _memory is NULL !\n");
    }
}

/*
 * Read byte in _memory
 */

byte Chip8::readMemB(unsigned int adr) {
    if (_memory != NULL) {
        if (adr < _memorySize) {
            return _memory[adr];
        }  else {
            printBacktrace();
            printf("CHIP8::ReadMemB(adr): adr out of bounds ! (Adr: %04X > MemorySize: %08X)\n", adr, _memorySize);
        }
    } else {
        printBacktrace();
        printf("CHIP8::ReadMemB(adr): _memory is NULL !\n");
    }
    return 0;
}

/*
 * Write short int in _memory
 */

unsigned short int Chip8::readMemS(unsigned int adr) {
    if (_memory != NULL) {
        if ((adr < _memorySize) && ((adr + 1) < _memorySize)) {
            return (unsigned short int) (_memory[adr] << 8) | _memory[adr + 1];
        }  else {
            printf("CHIP8::ReadMemS(adr): adr out of bounds ! (Adr/Adr+1: %04X(%04X) > MemorySize: %08X)\n", adr, adr + 1, _memorySize);
        }
    } else {
        printf("CHIP8::ReadMemS(adr): _memory is NULL !\n");
    }
    return 0;
}

/********************************** Opcodes **********************************/

bool Chip8::opcodesMega(byte Code, byte KK,  byte K) {
    string str;
    int to;
    // Mega-Chip8 0x0XNN (NNNN) Opcodes
    switch (Code) {
        case 0x0:
            // Not Mega-Chip8 (ClearScreen (NN: 0xE0) and return from routine (NN: 0xEE)
            break;
        case 0x1:
            // Mega-Chip8 0x01NN NNNN
            to = readMemS(_pc);
            _I = (KK << 16) + readMemS(_pc);
            char buf[50];
            sprintf(buf, "%04X: LDI I, %04X %04X", _pc-2, KK, readMemS(_pc));
            str = buf;
            backtrace(str);
            _pc += 2;
            return true;
            break;

        case 0x2:
            // 0x02KK -  Load nn-colors palette at I - Colors are in ARGB Format, ours are in RGBA
            int currentPALAddress;

            // Black / Transparent
            _palette[0] = 0;

            for (int i = 0; i <= KK; i++) {
                // La couleur 0 se trouve à _I - bytesPerPixels, la couleur 1 est à _I
                currentPALAddress = _I + ( i * getBytesPerPixel());

                _palette[i + 1] = getRGBA(readMemB(currentPALAddress + 1),
                                          readMemB(currentPALAddress + 2),
                                          readMemB(currentPALAddress + 3),
                                          readMemB(currentPALAddress));
            }
            return true;
            break;

        case 0x3:
            // 03KK 	 Set Sprite-width to nn
            setSpriteWidth(KK);
            return true;
            break;

        case 0x4:
            // 04nn+ 	 Set Sprite-height to nn	(SPRH  nn)
            setSpriteHeight(KK);
            return true;
            break;

        case 0x05:
            //05nn+ 	 Set Screenalpha to nn
            printf("TODO: ScreenAlpha - %04X\n", _opcode);
            setScreenAlpha((float)KK / 255.0);
            return true;
            break;

        case 0x06:
            // 060n+ 	 Play digitised sound at I	(DIGISND), will add n for loop/noloop
            printBacktrace();
            printf("%04X: DGISND, play sound at: %08X , Loop: %d\n", _pc-2, _I, K);
            playSound(K);
            return true;
            break;

        case 0x07:
            // 0700+ 	 Stop digitised sound 		(STOPSND)
            printf("%04X: STOPSND, stop sound\n", _pc-2);
            clearSound();
            return true;
            break;

        case 0x08:
            // 080n+ 	 Set sprite blendmode 		(BMODE n) (0=normal,1=25%,2=50%,3=75%,4=addative,5=multiply)
            switch (K) {
                case 1:
                    setSpriteBlendMode(BLEND_25);
                    break;
                case 2:
                    setSpriteBlendMode(BLEND_50);
                    break;
                case 3:
                    setSpriteBlendMode(BLEND_75);
                    break;
                case 4:
                    setSpriteBlendMode(BLEND_ADD);
                    break;
                case 5:
                    setSpriteBlendMode(BLEND_MUL);
                    break;
                case 0:
                default:
                    setSpriteBlendMode(BLEND_NORMAL);
            }
            return true;
            break;

        case 0x09:
            // Undocumented - MCHIP8 Set collision color index
            setCollisionColorIndex(K);
            return true;
            break;

        default:
            printBacktrace();
            printf("Unknown MEGA opcode: %04X (%02X) at address %04X.\n", _opcode, Code, _pc-2);
    }
    return false;
}

void Chip8::opcodesSuper(byte Code, byte K) {
    switch (Code) {
        case 0x10:
            // Mega-Chip8 Resolution 256x192
            ChangeMachineType(MCHIP8, false);
            break;
        case 0x11:
            // Standard SCHIP8 Resolution (Mega-Chip8)
            ChangeMachineType(MCHIP8, true);
            break;
        case 0xE0:
            // Clear screen - ATT in MCHIP8 mode this is used to update the screen
            if (getType() == MCHIP8) {
                //TODO: screenbuffer
                flip();
            }
            clearGfx();
            break;
        case 0xEE:
            // Return from subroutine
            if (_callStack.size() > 0) {
                _pc = _callStack.top();
                _callStack.pop();
            } else {
                printf("Invalid return from subroutine (Code: %04X), no returning address from stack ! (@%04X)\n", _opcode, _pc-2);
                _pc -= 2;
            }
            break;
        // SCHIP8
        case 0xFB:
            scrollRight();

            break;
        case 0xFC:
            scrollLeft();
            break;
        case 0xFD:
            // Quit the emulator
            printf("Rom ask to exit emulation.\n");
            break;
        case 0xFE:
            // Set CHIP8 Graphic Mode
            ChangeMachineType(SCHIP8, false);
            break;
        case 0xFF:
            // Set SCHIP8, MCHIP8 Extended Graphic Mode
            if (getType() != MCHIP8)
                ChangeMachineType(SCHIP8, true);
            else
                ChangeMachineType(MCHIP8, false);
            break;
        default:
            printBacktrace();
            printf("Unknown Super opcode: %04X (%02X) at address %04X.\n", _opcode, Code, _pc-2);
    }
}

/*
 * Opcodes for calculation (bitwise ops, add,...)
 */

void Chip8::opcodesCalc(byte Code, byte X, byte Y) {
    unsigned int tmp;

    switch (Code) {
    case 0x0:
        // V[X] = _V[Y]
        _V[X] = _V[Y];
        break;
    case 0x1:
        // V[X] = V[X] or V[Y]
        _V[X] |= _V[Y];
        break;
    case 0x2:
        // V[X] = V[X] and V[Y]
        _V[X] &= _V[Y];
        break;
    case 0x3:
        // V[X] = V[X] xor V[Y]
        _V[X] ^= _V[Y];
        break;
    case 0x4:
        // V[X] = V[X] + V[Y]
        // Si V[X] > 255, flag de report (v[0xF]) mis à 1
        // Result = 8 derniers bits
        tmp  = _V [X] + _V [Y];
        _V [0xF] = (unsigned char)((tmp > 255) ? 1 : 0);
        _V [X] = (unsigned char)((tmp % 256) & 0XFF);
        break;
    case 0x5:
        // V[X] = V[X] - V[Y]
        // Si V[X] >= V[Y], flag de report mis à 1  /!\ Si le résultat est à 0, VF = 12 !
        _V[0xF] = (unsigned char)((_V[X] >= _V[Y]) ? 1 : 0);
        _V[X] -= _V[Y];
        break;
    case 0x6:
        // Shift Right V[X]
        // Si le dernier bit de V[X] = 1, le flag de report est mis à 1
        _V[0xF] = (unsigned char)(_V[X] & 1);
        _V[X] = (unsigned char)(_V[X] >> 1);
        break;
    case 0x7:
        // V[X] = V[Y] - V[X]
        // Si V[Y] > V[X], flag de report mis à 1
        _V[0xF] = (unsigned char)((_V[Y] >= _V[X]) ? 1 : 0);
        _V[X] = (unsigned char)(_V[Y] - _V[X]);
        break;
    case 0xE:
        // Shift left V[X]
        // Si le dernier bit de V[X] = 1, le flag de report est mis à 1
        _V[0xF] = (unsigned char)((_V[X] & 0x80) >> 7);
        _V[X] = (unsigned char)(_V[X] << 1);
    break;
    default:
        printBacktrace();
        printf("Unkown Calc opcode: %04X (%02X) at address %04X.\n", _opcode, Code, _pc-2);
    }
}

/*
 * Keyboard opcodes
 */

void Chip8::opcodesKeyboard(byte Code, byte X) {
    switch (Code) {
        case 0x9E:
            // Passe l'instruction suivante si la touche V[X] est pressée
            _pc += (unsigned short)((_key[_V[X]]) ? 2 : 0);
            break;
        case 0xA1:
            // Passe l'instruction suivante si la touche V[X] est relâchée
            _pc += (unsigned short)((!_key[_V[X]]) ? 2 : 0);
            break;
        default:
            printBacktrace();
            printf("Unkown Keyboard opcode: %04X (%02X) at address %4X.\n", _opcode, Code, _pc-2);
    }
}

/*
 * Other opcodes
 */

void Chip8::opcodesOther(byte Code, byte X) {
    unsigned int tmp, maxOverflow, i;

    switch (Code) {
    case 0x07:
        // La valeur du timer delay est mise dans _V[X]
        _V[X] = _timerDelay;
        break;
    case 0x0A:
        // Attends qu'une touche soit pressée
        // Place le code de la touche dans V[X]
        for (i = 0; i < sizeof(_key); i++) {
            if (_key[i] == true) {
                _V[X] = i;
                return;
            }
        }
        // Not found - Redo the same opcode
        _pc -= 2;
        break;
    case 0x15:
        // Set timer delay
        _timerDelay = _V[X];
        break;
    case 0x18:
        // Set timer sound
        _timerSound = _V[X];
        break;
    case 0x1E:
        // Add V[X] to Index register
        // /!\ VF is set on overflow, undocumented feature, used on Spacefight 2019!
        tmp = _V[X] + _I;
        // Max overflow on MChip8 is 0xFFFFFF
        maxOverflow = ((getType() == MCHIP8) ? 0xFFFFFF : 0xFFF);
        _V[0xF] = ((tmp > maxOverflow) ? 1 : 0);
        _I = tmp & maxOverflow;
        break;
    case 0x29:
        // I reçoit la position du sprite V[X];
        // Les sprites ont une taille de 5 bytes et se trouvent à
        // _memory[0x0 à 0x1FF]
        // In CHIP8, _I is maximum 0xFFF !
        _I = (unsigned short)((_V[X] * 5) & 0xFFF);

        // ATT: MegaChip Sprite Size must be accorded - It overwrites sprite size
        setSpriteWidth(8);
        setSpriteHeight(5);
        break;
    // SCHIP8
    case 0x30:
        // I reçoit la position du sprite V[X];
        // Les sprites ont une taille de 10 bytes et se trouvent à
        // _memory[0x0 à 0x1FF]
        // Je les ai placés à partir de la position 80... Doivent-ils être lu depuis 0 ?
        _I = (unsigned short)(((_V[X] * 10) + 80) & 0xFFF);

        setSpriteWidth(8);
        setSpriteHeight(10);
        break;
    case 0x33:
        // Place les décimales de V[X] en mémoire
        // en commençant par l'adresse I
        writeMemB(_I    , (unsigned char)((_V[X] / 100)));
        writeMemB(_I + 1, (unsigned char)(((_V[X] % 100) / 10)));
        writeMemB(_I + 2, (unsigned char)(((_V[X] % 100) % 10)));
        break;
    case 0x55:
        // Place les valeurs des registres V[0] à V[X] en mémoire à partir de I
        // /!\ Selon certaines docs, le registre I doit être incrémenté... En pratique NE PAS LE FAIRE !
        for (i = 0; i <= X; i++) {
            writeMemB(_I + i, _V[i]);
        }
        break;
    case 0x65:
        // Récupère les valeurs des registres V[0] à V[X] en mémoire à partir de I
        // /!\ Selon certaines docs, le registre I doit être incrémenté... En pratique NE PAS LE FAIRE !
        for (i = 0; i <= X; i++) {
            _V[i] = readMemB(_I + i);
        }
        break;
    // HP48 registers
    case 0x75:
        // Place les valeurs des registres HP48 V[0] à V[7] en mémoire à partir de I
        // /!\ Selon certaines docs, le registre I doit être incrémenté... En pratique NE PAS LE FAIRE !
        for (i = 0; i <= X; i++) {
            _V48[i] = _V[i];
        }
        break;
    case 0x85:
        // Place les valeurs des registres HP48 V[0] à V[7] en mémoire à partir de I
        // /!\ Selon certaines docs, le registre I doit être incrémenté... En pratique NE PAS LE FAIRE !
        for (i = 0; i <= X; i++) {
            _V[i] = _V48[i];
        }
        break;
    default:
        printBacktrace();
        printf("Unkown Other opcode: %04X (%02X) at address %04X.\n", _opcode, Code, _pc-2);
    }
}

void Chip8::backtrace(string trace) {
    _opcodeTrace.push_back(trace);
    if (_opcodeTrace.size() > 30) {
        _opcodeTrace.pop_front();
    }
}

void Chip8::backtrace(unsigned short int Code, unsigned short int pc) {
    char buffer[20];
    sprintf(buffer, "%04X: %04X", pc, Code);
    string trace = buffer;
    _opcodeTrace.push_back(trace);
    if (_opcodeTrace.size() > 30) {
        _opcodeTrace.pop_front();
    }
}

void Chip8::printBacktrace() {
    std::deque<string>::iterator it = _opcodeTrace.begin();

    while (it != _opcodeTrace.end()) {
        string line = *(it++);
        printf("Trace %02d: %s\n", (int)((it - _opcodeTrace.begin())-30),  line.c_str());
    }
}

void Chip8::execute(double frequencyInMs = BASE_CLOCK_MS) {
    // Paramètres possibles - Décompose les possibilités de l'opcode
    unsigned short code;	    // 1er byte
    unsigned short NNN;		    // Une adresse - 3 derniers bytes
    unsigned char KK;		    // 2 derniers bytes
    unsigned char K;			// dernier byte
    unsigned char X;			// 2ème
    unsigned char Y;			// 3ème

    // This need C++11 compilator setting - ATT: In windows, the precision is in milliseconds ! For our timers @60hthat's no big deal...
    static auto t_start = chrono::high_resolution_clock::now();
    auto t_end = chrono::high_resolution_clock::now();
    double t;

    if (!_loaded)
        return;

    _isRunning = true;

    // Comme chaque emplacement est 1 byte et que le code opération est de 2
    // On shift de 8 bits le byte actuel
    _opcode = readMemS(_pc); // (unsigned short) (_memory [_pc] << 8 | _memory [_pc + 1]);
    // Log
    backtrace(_opcode, _pc);

    code = (unsigned short)(_opcode & 0xF000);
    NNN = (unsigned short)(_opcode & 0x0FFF);
    KK = (unsigned char)(_opcode & 0x00FF);
    K = (unsigned char)(_opcode & 0x000F);
    X = (unsigned char)((_opcode & 0x0F00) >> 8);
    Y = (unsigned char)((_opcode & 0x00F0) >> 4);

    // What is going to be executed
    //printf("PC: %04X - OpCode: %04X - Code: %04X - Current Frequency: %.4f - T: %ld\n", _pc, _opcode, code, frequencyInMs, t_start);

    _pc += 2;

    // Initialize Clock
    //t = clock();
    switch (code) {
    case 0x0000:
        // Special Codes for SCHIP & MCHIP Implementation
        if (Y == 0xC && X == 0x0) {
            scrollDown(K);
            break;
        } else if (Y == 0xB && X == 0x0) {
            // Misplaced 00BN+    Scroll display N lines up	(SCRU n)
            printf("TO TEST Scroll Up - %04X\n", _opcode);
            scrollUp(K);
            break;
        }

        // HiRes Chip8 Clearscreen
        if (NNN == 0x230) {
            clearGfx();
            break;
        }

        if (!opcodesMega(X, KK, K)) {
            opcodesSuper(KK, K);
        }

        break;


    case 0x1000:
        // Jump to adr - /!\ Activate Chip8 hires mode
        if (_pc == 0x202 && NNN == 0x260) {
            // Enable 64x64 screen
            ChangeMachineType(CHIP8_HiRes, true);
            // Continue program
            _pc = 0x2C0;
        } else
            // Normal jump
            _pc = NNN;
        break;

    case 0x2000:
        // Call subroutine
        _callStack.push(_pc);
        _pc = NNN;
        break;

    case 0x3000:
        // Si V[X] == valeur, on passe l'instruction suivante
        if (_V[X] == KK) {
            _pc += 2;
        }
        break;

    case 0x4000:
        // Si V[X] != valeur, on passe l'instruction suivante
        if (_V[X] != KK) {
            _pc += 2;
        }
        break;

    case 0x5000:
        // Si V[X] == V[Y], on passe l'instruction suivante
        if (_V[X] == _V[Y]) {
            _pc += 2;
        }
        break;

    case 0x6000:
        // Set Register X to KK
        _V[X] = KK;
       break;

    case 0x7000:
        // Set Register X to Register X + KK;
        // ATT: No carry in doc, we set one
        _V[X] += KK;
        break;

    case 0x8000:
        // Do the calcs
        opcodesCalc(K, X, Y);
        break;

    case 0x9000:
        // Si V[X] != V[Y], on passe l'instruction suivante
        if (_V[X] != _V[Y]) {
            _pc += 2;
        }
        break;

    case 0xA000:
        // I = NNN
        _I = NNN;
        break;

    case 0xB000:
        // Jump to V[0] + NNN
        _pc = (unsigned short)(_V[0] + NNN);
        break;

    case 0xC000:
        // _V[X] = Random and KK
        struct timeval time;
        gettimeofday(&time,NULL);
        srand((time.tv_sec * 1000) + (time.tv_usec / 1000));

        _V[X] = (unsigned char)(KK & (unsigned char)(rand() %  0XFF));
        break;

    case 0xD000:
        drawScreen(_V[X], _V[Y], K);
        break;

    case 0xE000:
        opcodesKeyboard(KK, X);
        break;

    case 0xF000:

        // Jump to 24bit Address ???
        /*if (X == 0 && getType() == MCHIP8) {
            _I = (KK << 16) + (_memory [_pc] << 8 | _memory [_pc + 1]);
            _pc += 2;
        }*/

        opcodesOther(KK, X);
        break;

    default:
        printBacktrace();
        printf("Unkown General opcode: %04X at address %04X.\n", _opcode, _pc-2);

    }

    t_end = chrono::high_resolution_clock::now();
    t = chrono::duration_cast<chrono::milliseconds>(t_end - t_start).count();

    // Force redraw but not for MCHIP with ExtendedModeOn=TimedToCLS
    if (getType() != MCHIP8) {
        flip();
    }

    if ((t >= BASE_CLOCK_MS) || !_syncClockToOriginal) {

        if (_timerDelay>0)
            _timerDelay--;

        if (_timerSound>0)
            _timerSound--;

        t_start = chrono::high_resolution_clock::now();
    }
}

void Chip8::flip() {
    if (_gfx != NULL && _gfxBuffer != NULL) {
        memcpy(_gfx, _gfxBuffer, getScreenSizeOf());
    }
}

void Chip8::loadFont() {
    unsigned int i;

    for (i = 0; i < _memorySize; i++) {
        if (i < sizeof(Chip8Font) + 1)
            _memory[i] = Chip8Font[i];
        else
            _memory[i] = 0;
    }
}

bool Chip8::loadGame(const char *filename) {
    bool res = false;

    if (filename != "") {
        std::ifstream file(filename, std::ios::binary);
        file.seekg(0, std::ios::end);
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        char *rom = new char[size];
        if ( file.read(rom, size) ) {
            load(rom, size);
            file.close();
            res = true;
        }
        delete[] rom;
    }
    return res;
}

// Charge le faux BIOS
void Chip8::loadBios() {
    load((char*)bios_ch8, bios_ch8_len);
}

// Charge keypad
void Chip8::loadKeypad() {
    load((char*)keypad_ch8, keypad_ch8_len);
}

// Charge une ROM en mémoire
void Chip8::load(char *rom, unsigned int size) {
    // Reset Emu
    initialize();

    if (_memory != NULL)
        free(_memory);

    // I'm generous, I always give 4Ko up ;-) (Blinky, MegaBlinky)
    _memorySize = size + 0x200 + 4096;
    _memory = (unsigned char *)malloc(_memorySize);

    // TODO:
    loadFont();

    for (unsigned int i = 0; i < size ; i++)
        _memory[0x200 + i] = rom[i];

    _loaded = true;
}
>>>>>>> 4dfbb63e5631206625c60576f26a61ed8632ee15
