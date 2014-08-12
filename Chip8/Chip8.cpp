#include "Chip8.h"

#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */
#include <cstdio>
#include <math.h>

Chip8::Chip8(): BaseCHIP8()
{
    //ctor
    _gfx = NULL;
    _syncClockToOriginal = true;
    Chip8Types type = CHIP8;
    setType(type);
    this->initialize(getType());
}

Chip8::~Chip8() {
    //dtor
    free(_gfx);
}

void Chip8::initialize(Chip8Types Type = CHIP8) {
    // Init
    unsigned int i;

    setType(Type);
    for (i = 0; i < sizeof(_memory); i++)
        _memory[i] = 0;

    // Load font
    loadFont();

    reset();

    _loaded = false;
    _isRunning = false;
}

void Chip8::ChangeMachineType(Chip8Types Type) {
    setType(Type);
    free(_gfx);
    _gfx = (unsigned char *)malloc(getScreenSizeOf());
    clearGfx();
}

void Chip8::reset() {
    // reset rom
    unsigned int i;

    // Allocate the screen
    free(_gfx);
    _gfx = (unsigned char*)malloc(getScreenSizeOf());

    _loaded = false;
    _pc = 0x200;
    _timerDelay = 0;
    _timerSound = 0;

    for (i = 0; i < 16; i++) {
        _key[i] = false;
        _V[i] = 0;
    }

    clearGfx();
    _isRunning = false;
}

void Chip8::clearGfx() {
    int color;

    if (_gfx != NULL) {
        for (unsigned int i = 0; i < getScreenSizeOf(); i += getBytesPerPixel()) {
            color = getColor(getColorTheme()).backColor;
            _gfx[i]     = getR(color);
            _gfx[i + 1] = getG(color);
            _gfx[i + 2] = getB(color);
            _gfx[i + 3] = 255;
        }
    }
}

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

    for (int i = getScreenSizeOf(); i >= 0; i -= getBytesPerPixel()) {
        actualColor = getRGB(_gfx[i], _gfx[i + 1], _gfx[i + 2]);
        if (actualColor == colorFrom.foreColor) {
            replaceColor = colorTo.foreColor;
        } else {
            replaceColor = colorTo.backColor;
        }

        _gfx[i    ] = getR(replaceColor);
        _gfx[i + 1] = getG(replaceColor);
        _gfx[i + 2] = getB(replaceColor);
        _gfx[i + 3] = 255;
    }
}

/* Draw K lines from Memory to GFX
   Draws an 8xK (width x height) sprite at the coordinates given in register X and Y.
   If height specified is 0, draws a 16x16 sprite. See notes below.*/
void Chip8::UpdateScreen(unsigned char X, unsigned char Y, unsigned char K) {
    // Place l'index au début d'un caractère
    // Lit K lignes de pixels à partir de I et les affiche aux coordonnées (V[X], V[Y])
    // Reset flag
    _V[0xF] = 0;
    // Si K est à 0, on doit dessiner un sprite de 16x16...
    int spriteX = 8;
    // Color Theme
    unsigned int color;

    if (K == 0) {
        K = 16;
        // Si on se trouve en mode Super les sprites sont de 16x16
        if (getType() == SCHIP8)
            spriteX = 16;
    }

    // Donc on charge 2 bytes ;-)
    int offset = (spriteX == 16 ? 2 : 1);
    // Pour chaque ligne
    for (unsigned int i = 0; i < K; i ++) {
        // Début de la ligne
        // /!\ Si le sprite est de 16 pixels de large, on récupère 2 bytes !
        unsigned short line = (unsigned short) (_memory[_I + (i * offset)]);
        if (offset == 2) {
            line = (unsigned short)((line << 8) + _memory[_I + (i * offset) + 1]);
        }

        // Pour chaque pixels de la ligne - Une ligne = 8 pixels, sauf si K == 0
        for (int j = 0; j < spriteX; j++) {
            // Extrait le pixel j >> 7 en 8x8, >> 15 en 16x16
            unsigned char pixelToWrite = (unsigned char)((line << j) >> (spriteX-1)) & 1;

            // On calcule l'adresse - Adr = (_V[Y] * 64) + V[X] + j
            // /!\ 2 bytes par lignes en 16x16 !
            int pixelAdr = _V[X] + j + (_V[Y] + i) * getWidth();
            // 3 bytes par couleur
            pixelAdr *= getBytesPerPixel();
            // Si dépassement de la taille de l'ecran, le sprite s'affiche
            // au début de l'écran
            pixelAdr %= getScreenSizeOf();
            // Récupère la couleur de l'ancien pixel
            //unsigned char oldPixel = (unsigned char)(_gfx[pixelAdr] + _gfx[pixelAdr + 1] + _gfx[pixelAdr + 2]);
            //oldPixel = (unsigned char)((oldPixel > 0) ? 1 : 0);
            color = getColor(getColorTheme()).backColor;
            unsigned int oldPixelColor = (unsigned int)getRGB(_gfx[pixelAdr], _gfx[pixelAdr + 1], _gfx[pixelAdr + 2]);
            unsigned char oldPixel = (oldPixelColor - color == 0) ? 0 : 1; //((oldPixelColor == color) ? 1 : 0);
            //oldPixel = (unsigned char)((oldPixel == color) ? 1 : 0);

            // Si le pixel à écrire est différent du pixel présent
            //if ((pixelToWrite != 0 && oldPixel == 0) || (pixelToWrite == 0 && oldPixel != 0)) {
            if ((pixelToWrite ^ oldPixel) == 1) {
                color = getColor(getColorTheme()).foreColor;
                _gfx[pixelAdr]     = getR(color);
                _gfx[pixelAdr + 1] = getG(color);
                _gfx[pixelAdr + 2] = getB(color);
                _gfx[pixelAdr + 3] = 255;
            } else {
                color = getColor(getColorTheme()).backColor;
                _gfx[pixelAdr]     = getR(color);
                _gfx[pixelAdr + 1] = getG(color);
                _gfx[pixelAdr + 2] = getB(color);
                _gfx[pixelAdr + 3] = 255;
            }

            // Détection de collision
            //_V[0xF] = (byte)(oldPixel & pixelToWrite);
            if (pixelToWrite != 0 && oldPixel != 0)
                _V[0xF] = 1;
        }
    }
}

// Frequency is the frequency set by the emultor, by default, it's 60 hz

void Chip8::execute(double frequencyInMs = BASE_CLOCK_MS) {
    // Paramètres possibles - Décompose les possibilités de l'opcode
    unsigned short code;	    // 1er byte
    unsigned short NNN;		    // Une adresse - 3 derniers bytes
    unsigned char KK;		    // 2 derniers bytes
    unsigned char K;			// dernier byte
    unsigned char X;			// 2ème
    unsigned char Y;			// 3ème

    unsigned char shift;        // Used to know how many pixels we must shift
    unsigned int color;

    // This need C++11 compilator setting
    static auto t_start = chrono::high_resolution_clock::now();
    auto t_end = chrono::high_resolution_clock::now();
    double t;

    // Previously declared in switch
    int realWidth;
    unsigned int i;
    int tmp;

    if (!_loaded)
        return;

    _isRunning = true;

    // Comme chaque emplacement est 1 byte et que le code opération est de 2
    // On shift de 8 bits le byte actuel
    _opcode = (unsigned short) (_memory [_pc] << 8 | _memory [_pc + 1]);

    code = (unsigned short)(_opcode & 0xF000);
    NNN = (unsigned short)(_opcode & 0x0FFF);
    KK = (unsigned char)(_opcode & 0x00FF);
    K = (unsigned char)(_opcode & 0x000F);
    X = (unsigned char)((_opcode & 0x0F00) >> 8);
    Y = (unsigned char)((_opcode & 0x00F0) >> 4);

    // What is going to be executed
    //printf("PC: %04X - OpCode: %04X - Code: %04X - Current Frequency: %.4f \n", _pc, _opcode, code, frequencyInMs);

    _pc += 2;

    // Initialize Clock
    //t = clock();
    switch (code) {
    case 0x0000:
        //SCHIP8
        if (Y == 0xC) {
            // 0x00CK
            // Scroll down K lines: Note that this opcode will scroll down the entire screen N lines.
            // /!\ Anything that gets outside the screen is erased (***)
            int nbBytesToMove, start;

            if (getType() == SCHIP8)
                nbBytesToMove = (getWidth() * getBytesPerPixel()) * K;
            else
                nbBytesToMove = (int)ceil(K / 2.0f) * (getWidth() * getBytesPerPixel());

            start = getScreenSizeOf() - nbBytesToMove;

            for (i = getScreenSizeOf(); i > 0; i--) {
                // On bouge nos bytes
                if (start > 0) {
                    _gfx[i] = _gfx[start];
                } else {
                    color = getColor(getColorTheme()).backColor;
                    if (i % 4 == 0)
                        _gfx[i] = getR(color);
                    if (i % 4 == 1)
                        _gfx[i] = getG(color);
                    if (i % 4 == 2)
                        _gfx[i] = getB(color);
                }

                start--;
            }

        }

        if (X == 1) {
            // Mega-Chip8 0x01NN NNNN
            //unsigned int lowAddress = (unsigned int) (_memory [_pc] << 8 | _memory [_pc + 1]);
            //_I = (int)(KK << 8) & lowAddress;
        }

        // HiRes Chip8 Clearscreen
        if (NNN == 0x230) {
            clearGfx()
;        }

        switch (KK) {
            case 0x10:
                // Mega-Chip8 Resolution 256x192
                ChangeMachineType(MCHIP8);
            case 0x11:
                // Standard SCHIP8 Resolution (Mega-Chip8)
                ChangeMachineType(SCHIP8);
            case 0xE0:
                // Clear screen
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
                // Scroll 4 pixels right: Note that this opcode will scroll down the entire screen N lines.
                // /!\ Anything that gets outside the screen is erased (***)
                // /!\ 2 pixels in CHIP8 mode
                shift = ((getType() == SCHIP8) ? 4 * getBytesPerPixel() : 2 * getBytesPerPixel());

                realWidth = getWidth() * getBytesPerPixel();
                // For each lines
                for (i = 0; i < getHeight(); i++) {
                    // For each col
                    for (int j = (realWidth - shift); j >= 0  ; j-- ) {
                        // Ce byte
                        int current =  (i * realWidth) + j ;
                        // Sera décalé vers la droite
                        _gfx[current + shift] = _gfx[current];

                        if (j <= shift) {
                            // Mais effacé car en dehors du scroll
                            color = getColor(getColorTheme()).backColor;
                            if (current % 4 == 0)
                                _gfx[current] = getR(color);
                            if (current % 4 == 1)
                                _gfx[current] = getG(color);
                            if (current % 4 == 2)
                                _gfx[current] = getB(color);
                        }
                    }
                }

                break;
            case 0xFC:
                // Scroll 4 pixels left: Note that this opcode will scroll down the entire screen N lines.
                // /!\ Anything that gets outside the screen is erased (***)
                // /!\ 2 pixels in CHIP8 mode
                shift = ((getType() == SCHIP8) ? 4 * getBytesPerPixel() : 2 * getBytesPerPixel());

                realWidth = getWidth() * getBytesPerPixel();
                // For each lines
                for (i = 0; i < getHeight(); i++) {
                    // For each col
                    for (int j = shift; j <= realWidth ; j ++ ) {
                        // Ce byte
                        int current =  (i * realWidth) + j ;
                        // Sera décalé vers la gauche
                        _gfx[current - shift] = _gfx[current];

                        if (j >= (realWidth - shift)) {
                            // Sera effacé car en dehors du scroll
                            color = getColor(getColorTheme()).backColor;
                            if (current % 4 == 0)
                                _gfx[current] = getR(color);
                            if (current % 4 == 1)
                                _gfx[current] = getG(color);
                            if (current % 4 == 2)
                                _gfx[current] = getB(color);
                        }
                    }
                }
                break;
            case 0xFD:
                // Quit the emulator
                break;
            case 0xFE:
                // Set CHIP8 Graphic Mode
                ChangeMachineType(CHIP8);
                break;
            case 0xFF:
                // Set SCHIP8 Graphic Mode
                ChangeMachineType(SCHIP8);
                break;
            default:
                // Added SCHIP Scroll down
                // Added Chip8 HiRes
                if (Y != 0xC && X != 1 && NNN != 0x230)
                    printf("Unkown opcode: %04X at address %04X.\n", _opcode, _pc-2);
        }
            break;
    case 0x1000:
        // Jump to adr - /!\ Activate Chip8 hires mode
        if (_pc == 0x202 && NNN == 0x260) {
            // Enable 64x64 screen
            ChangeMachineType(CHIP8_HiRes);
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
        _V[X] += KK;
        break;
    case 0x8000:
        switch (K) {
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
                printf("Unkown opcode: %04X at address %04X.\n", _opcode, _pc-2);
        }
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
        srand(time(NULL));
        _V[X] = (unsigned char)(KK & (unsigned char)(rand() %  0XFF));
        break;
    case 0xD000:
        UpdateScreen(X, Y, K);
        break;
    case 0xE000:
        switch (KK) {
            case 0x9E:
                // Passe l'instruction suivante si la touche V[X] est pressée
                _pc += (unsigned short)((_key[_V[X]]) ? 2 : 0);
                break;
            case 0xA1:
                // Passe l'instruction suivante si la touche V[X] est relâchée
                _pc += (unsigned short)((!_key[_V[X]]) ? 2 : 0);
                break;
            default:
                printf("Unkown opcode: %4X at address %4X.\n", _opcode, _pc-2);
        }
        break;
    case 0xF000:
        switch (KK) {
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
                _V[0xF] = ((tmp > 0xFFF) ? 1 : 0);
                _I = (tmp & 0xFFF);
                break;
            case 0x29:
                // I reçoit la position du sprite V[X];
                // Les sprites ont une taille de 5 bytes et se trouvent à
                // _memory[0x0 à 0x1FF]
                _I = (unsigned short)(_V[X] * 5);
                break;
            // SCHIP8
            case 0x30:
                // I reçoit la position du sprite V[X];
                // Les sprites ont une taille de 10 bytes et se trouvent à
                // _memory[0x0 à 0x1FF]
                // Je les ai placés à partir de la position 80... Doivent-ils être lu depuis 0 ?
                _I = (unsigned short)((_V[X] * 10) + 80);
                break;
            case 0x33:
                // Place les décimales de V[X] en mémoire
                // en commençant par l'adresse I
                _memory[_I    ] = (unsigned char)((_V[X] / 100));
                _memory[_I + 1] = (unsigned char)(((_V[X] % 100) / 10));
                _memory[_I + 2] = (unsigned char)(((_V[X] % 100) % 10));
                break;
            case 0x55:
                // Place les valeurs des registres V[0] à V[X] en mémoire à partir de I
                // /!\ Selon certaines docs, le registre I doit être incrémenté... En pratique NE PAS LE FAIRE !
                for (i = 0; i <= X; i++) {
                    _memory[_I + i] = _V[i];
                }
                break;
            case 0x65:
                // Récupère les valeurs des registres V[0] à V[X] en mémoire à partir de I
                // /!\ Selon certaines docs, le registre I doit être incrémenté... En pratique NE PAS LE FAIRE !
                for (i = 0; i <= X; i++) {
                    _V[i] = _memory[_I + i];
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
                printf("Unkown opcode: %04X at address %04X.\n", _opcode, _pc-2);
        }
        break;

        default:
            printf("Unkown opcode: %04X at address %04X.\n", _opcode, _pc-2);

    }

    //printf(" - Wait: %.4f - Count: %.4f\n", waitInterval, countInterval);

    t_end = chrono::high_resolution_clock::now();
    t = chrono::duration_cast<chrono::milliseconds>(t_end - t_start).count();

    if ((t >= BASE_CLOCK_MS) || !_syncClockToOriginal) {
        if (_timerDelay>0)
            _timerDelay--;

        if (_timerSound>0)
            _timerSound--;

        t_start = chrono::high_resolution_clock::now();
    }
}

void Chip8::loadFont() {
    unsigned int i;

    for (i = 0; i < sizeof(_memory); i++) {
        if (i < sizeof(Chip8Font))
            _memory[i] = Chip8Font[i];
        else
            _memory[i] = 0;
    }
}

bool Chip8::loadGame(const char *filename) {
    bool res = false;

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

    for (unsigned int i = 0; i < size; i++)
        _memory[0x200 + i] = rom[i];

    _loaded = true;
}
