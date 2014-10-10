#------------------------------------------------------------------------------#
# This makefile was generated by 'cbp2make' tool rev.147                       #
#------------------------------------------------------------------------------#


WORKDIR = `pwd`

CC0 = gcc
CXX0 = g++
AR0 = ar
LD0 = g++
WINDRES0 = windres

INC0 = -Iinclude -IChip8 -IConfig
CFLAGS0 = -std=c++11
RESINC0 = 
LIBDIR0 = 
LIB0 = 
LDFLAGS0 = 

CC1 = i686-w64-mingw32-gcc
CXX1 = i686-w64-mingw32-g++
AR1 = i686-w64-mingw32-ar
LD1 = i686-w64-mingw32-ld

INC1 = -Iinclude -IChip8 -I/usr/local/mingw-w64-i686/include
CFLAGS1 = -std=c++11
RESINC1 = 
LIBDIR1 = 
LIB1 = 
LDFLAGS1 = 

CC2 = x86_64-w64-mingw32-gcc
CXX2 = x86_64-w64-mingw32-g++
AR2 = x86_64-w64-mingw32-ar
LD2 = x86_64-w64-mingw32-ld

INC2 = -Iinclude -IChip8 -I/usr/local/mingw-w64-x86_64/include
CFLAGS2 = -std=c++11
RESINC2 = 
LIBDIR2 = 
LIB2 = 
LDFLAGS2 = 

INC_DEBUG = $(INC0)
CFLAGS_DEBUG = $(CFLAGS0) -g `wx-config --prefix=/usr/local --cflags`
RESINC_DEBUG = $(RESINC0)
RCFLAGS_DEBUG = $(RCFLAGS0)
LIBDIR_DEBUG = $(LIBDIR0)
LIB_DEBUG = $(LIB0)
LDFLAGS_DEBUG =  `wx-config --prefix=/usr/local --libs --gl-libs` -lGL -lSDL2 -lSDL2_mixer -lrt -lftgl $(LDFLAGS0)
OBJDIR_DEBUG = obj/Debug
DEP_DEBUG = 
OUT_DEBUG = bin/Debug/Mega8

INC_RELEASE = $(INC0)
CFLAGS_RELEASE = $(CFLAGS0) -O2 `wx-config --prefix=/usr/local --cflags --static`
RESINC_RELEASE = $(RESINC0)
RCFLAGS_RELEASE = $(RCFLAGS0)
LIBDIR_RELEASE = $(LIBDIR0)
LIB_RELEASE = $(LIB0)
LDFLAGS_RELEASE =  -s `wx-config --prefix=/usr/local --libs --gl-libs --static` -lSDL2 -lSDL2_mixer -lrt -lftgl -lGL $(LDFLAGS0)
OBJDIR_RELEASE = obj/Release
DEP_RELEASE = 
OUT_RELEASE = bin/Release/Mega8

INC_WIN32 = $(INC1) -I/usr/local/mingw-w64-i686/include/freetype2
CFLAGS_WIN32 = $(CFLAGS1) `wx-config --prefix=/usr/local/mingw-w64-i686 --host=i686-w64-mingw32 --cflags --static` `sdl2-config --prefix=/usr/local/mingw-w64-i686 --cflags` `freetype-config --prefix=/usr/local/mingw-w64-i686 --cflags`
RESINC_WIN32 = $(RESINC1)
RCFLAGS_WIN32 = $(RCFLAGS1)
LIBDIR_WIN32 = $(LIBDIR1) -L/usr/local/mingw-w64-i686/lib
LIB_WIN32 = $(LIB1)
LDFLAGS_WIN32 = $(LDFLAGS1) -lrt -lftgl -lfreetype -lpng `wx-config --prefix=/usr/local/mingw-w64-i686 --host=i686-w64-mingw32 --libs --gl-libs` `sdl2-config --libs` -lSDL2_mixer
OBJDIR_WIN32 = .objs
DEP_WIN32 = 
OUT_WIN32 = bin/Win32/Mega8_i686exe

INC_WIN64 = $(INC2)
CFLAGS_WIN64 = $(CFLAGS2) -std=c++11 `wx-config --prefix=/usr/local/mingw-w64-x86_64 --host=x86_64-w64-mingw32 --cflags --static` `sdl2-config --prefix=/usr/local/mingw-w64-x86_64 --cflags` `freetype-config --prefix=/usr/local/mingw-w64-x86_64 --cflags`
RESINC_WIN64 = $(RESINC2)
RCFLAGS_WIN64 = $(RCFLAGS2)
LIBDIR_WIN64 = $(LIBDIR2)
LIB_WIN64 = $(LIB2)
LDFLAGS_WIN64 = $(LDFLAGS2) -lrt -lftgl -lfreetype -lpng `wx-config --prefix=/usr/local/mingw-w64-x86_64 --host=x86_64-w64-mingw32 --libs --gl-libs` `sdl2-config --libs` -lSDL2_mixer
OBJDIR_WIN64 = .objs
DEP_WIN64 = 
OUT_WIN64 = bin/Win64/Mega8_x86-64exe

OBJ_DEBUG = $(OBJDIR_DEBUG)/Chip8/Chip8.o $(OBJDIR_DEBUG)/Mega8App.o $(OBJDIR_DEBUG)/Mega8Config.o $(OBJDIR_DEBUG)/Mega8Main.o $(OBJDIR_DEBUG)/src/Chip8GL.o

OBJ_RELEASE = $(OBJDIR_RELEASE)/Chip8/Chip8.o $(OBJDIR_RELEASE)/Mega8App.o $(OBJDIR_RELEASE)/Mega8Config.o $(OBJDIR_RELEASE)/Mega8Main.o $(OBJDIR_RELEASE)/src/Chip8GL.o

OBJ_WIN32 = $(OBJDIR_WIN32)/Chip8/Chip8.o $(OBJDIR_WIN32)/Mega8App.o $(OBJDIR_WIN32)/Mega8Config.o $(OBJDIR_WIN32)/Mega8Main.o $(OBJDIR_WIN32)/src/Chip8GL.o

OBJ_WIN64 = $(OBJDIR_WIN64)/Chip8/Chip8.o $(OBJDIR_WIN64)/Mega8App.o $(OBJDIR_WIN64)/Mega8Config.o $(OBJDIR_WIN64)/Mega8Main.o $(OBJDIR_WIN64)/src/Chip8GL.o

all: debug release win32 win64

clean: clean_debug clean_release clean_win32 clean_win64

before_debug: 
	test -d bin/Debug || mkdir -p bin/Debug
	test -d $(OBJDIR_DEBUG)/Chip8 || mkdir -p $(OBJDIR_DEBUG)/Chip8
	test -d $(OBJDIR_DEBUG) || mkdir -p $(OBJDIR_DEBUG)
	test -d $(OBJDIR_DEBUG)/src || mkdir -p $(OBJDIR_DEBUG)/src

after_debug: 

debug: before_debug out_debug after_debug

out_debug: before_debug $(OBJ_DEBUG) $(DEP_DEBUG)
	$(LD0) $(LDFLAGS_DEBUG) $(LIBDIR_DEBUG) $(OBJ_DEBUG) $(LIB_DEBUG) -o $(OUT_DEBUG)

$(OBJDIR_DEBUG)/Chip8/Chip8.o: Chip8/Chip8.cpp
	$(CXX0) $(CFLAGS_DEBUG) $(INC_DEBUG) -c Chip8/Chip8.cpp -o $(OBJDIR_DEBUG)/Chip8/Chip8.o

$(OBJDIR_DEBUG)/Mega8App.o: Mega8App.cpp
	$(CXX0) $(CFLAGS_DEBUG) $(INC_DEBUG) -c Mega8App.cpp -o $(OBJDIR_DEBUG)/Mega8App.o

$(OBJDIR_DEBUG)/Mega8Config.o: Mega8Config.cpp
	$(CXX0) $(CFLAGS_DEBUG) $(INC_DEBUG) -c Mega8Config.cpp -o $(OBJDIR_DEBUG)/Mega8Config.o

$(OBJDIR_DEBUG)/Mega8Main.o: Mega8Main.cpp
	$(CXX0) $(CFLAGS_DEBUG) $(INC_DEBUG) -c Mega8Main.cpp -o $(OBJDIR_DEBUG)/Mega8Main.o

$(OBJDIR_DEBUG)/src/Chip8GL.o: src/Chip8GL.cpp
	$(CXX0) $(CFLAGS_DEBUG) $(INC_DEBUG) -c src/Chip8GL.cpp -o $(OBJDIR_DEBUG)/src/Chip8GL.o

clean_debug: 
	rm -f $(OBJ_DEBUG) $(OUT_DEBUG)
	rm -rf bin/Debug
	rm -rf $(OBJDIR_DEBUG)/Chip8
	rm -rf $(OBJDIR_DEBUG)
	rm -rf $(OBJDIR_DEBUG)/src

before_release: 
	test -d bin/Release || mkdir -p bin/Release
	test -d $(OBJDIR_RELEASE)/Chip8 || mkdir -p $(OBJDIR_RELEASE)/Chip8
	test -d $(OBJDIR_RELEASE) || mkdir -p $(OBJDIR_RELEASE)
	test -d $(OBJDIR_RELEASE)/src || mkdir -p $(OBJDIR_RELEASE)/src

after_release: 

release: before_release out_release after_release

out_release: before_release $(OBJ_RELEASE) $(DEP_RELEASE)
	$(LD0) $(LDFLAGS_RELEASE) $(LIBDIR_RELEASE) $(OBJ_RELEASE) $(LIB_RELEASE) -o $(OUT_RELEASE)

$(OBJDIR_RELEASE)/Chip8/Chip8.o: Chip8/Chip8.cpp
	$(CXX0) $(CFLAGS_RELEASE) $(INC_RELEASE) -c Chip8/Chip8.cpp -o $(OBJDIR_RELEASE)/Chip8/Chip8.o

$(OBJDIR_RELEASE)/Mega8App.o: Mega8App.cpp
	$(CXX0) $(CFLAGS_RELEASE) $(INC_RELEASE) -c Mega8App.cpp -o $(OBJDIR_RELEASE)/Mega8App.o

$(OBJDIR_RELEASE)/Mega8Config.o: Mega8Config.cpp
	$(CXX0) $(CFLAGS_RELEASE) $(INC_RELEASE) -c Mega8Config.cpp -o $(OBJDIR_RELEASE)/Mega8Config.o

$(OBJDIR_RELEASE)/Mega8Main.o: Mega8Main.cpp
	$(CXX0) $(CFLAGS_RELEASE) $(INC_RELEASE) -c Mega8Main.cpp -o $(OBJDIR_RELEASE)/Mega8Main.o

$(OBJDIR_RELEASE)/src/Chip8GL.o: src/Chip8GL.cpp
	$(CXX0) $(CFLAGS_RELEASE) $(INC_RELEASE) -c src/Chip8GL.cpp -o $(OBJDIR_RELEASE)/src/Chip8GL.o

clean_release: 
	rm -f $(OBJ_RELEASE) $(OUT_RELEASE)
	rm -rf bin/Release
	rm -rf $(OBJDIR_RELEASE)/Chip8
	rm -rf $(OBJDIR_RELEASE)
	rm -rf $(OBJDIR_RELEASE)/src

before_win32: 
	test -d bin/Win32 || mkdir -p bin/Win32
	test -d $(OBJDIR_WIN32)/Chip8 || mkdir -p $(OBJDIR_WIN32)/Chip8
	test -d $(OBJDIR_WIN32) || mkdir -p $(OBJDIR_WIN32)
	test -d $(OBJDIR_WIN32)/src || mkdir -p $(OBJDIR_WIN32)/src

after_win32: 

win32: before_win32 out_win32 after_win32

out_win32: before_win32 $(OBJ_WIN32) $(DEP_WIN32)
	$(LD1) /nologo /subsystem:windows $(LIBDIR_WIN32) /out:$(OUT_WIN32) $(LIB_WIN32) $(OBJ_WIN32)  $(LDFLAGS_WIN32)

$(OBJDIR_WIN32)/Chip8/Chip8.o: Chip8/Chip8.cpp
	$(CXX1) /nologo $(CFLAGS_WIN32) $(INC_WIN32) /c Chip8/Chip8.cpp /Fo$(OBJDIR_WIN32)/Chip8/Chip8.o

$(OBJDIR_WIN32)/Mega8App.o: Mega8App.cpp
	$(CXX1) /nologo $(CFLAGS_WIN32) $(INC_WIN32) /c Mega8App.cpp /Fo$(OBJDIR_WIN32)/Mega8App.o

$(OBJDIR_WIN32)/Mega8Config.o: Mega8Config.cpp
	$(CXX1) /nologo $(CFLAGS_WIN32) $(INC_WIN32) /c Mega8Config.cpp /Fo$(OBJDIR_WIN32)/Mega8Config.o

$(OBJDIR_WIN32)/Mega8Main.o: Mega8Main.cpp
	$(CXX1) /nologo $(CFLAGS_WIN32) $(INC_WIN32) /c Mega8Main.cpp /Fo$(OBJDIR_WIN32)/Mega8Main.o

$(OBJDIR_WIN32)/src/Chip8GL.o: src/Chip8GL.cpp
	$(CXX1) /nologo $(CFLAGS_WIN32) $(INC_WIN32) /c src/Chip8GL.cpp /Fo$(OBJDIR_WIN32)/src/Chip8GL.o

clean_win32: 
	rm -f $(OBJ_WIN32) $(OUT_WIN32)
	rm -rf bin/Win32
	rm -rf $(OBJDIR_WIN32)/Chip8
	rm -rf $(OBJDIR_WIN32)
	rm -rf $(OBJDIR_WIN32)/src

before_win64: 
	test -d bin/Win64 || mkdir -p bin/Win64
	test -d $(OBJDIR_WIN64)/Chip8 || mkdir -p $(OBJDIR_WIN64)/Chip8
	test -d $(OBJDIR_WIN64) || mkdir -p $(OBJDIR_WIN64)
	test -d $(OBJDIR_WIN64)/src || mkdir -p $(OBJDIR_WIN64)/src

after_win64: 

win64: before_win64 out_win64 after_win64

out_win64: before_win64 $(OBJ_WIN64) $(DEP_WIN64)
	$(LD2) /nologo /subsystem:windows $(LIBDIR_WIN64) /out:$(OUT_WIN64) $(LIB_WIN64) $(OBJ_WIN64)  $(LDFLAGS_WIN64)

$(OBJDIR_WIN64)/Chip8/Chip8.o: Chip8/Chip8.cpp
	$(CXX2) /nologo $(CFLAGS_WIN64) $(INC_WIN64) /c Chip8/Chip8.cpp /Fo$(OBJDIR_WIN64)/Chip8/Chip8.o

$(OBJDIR_WIN64)/Mega8App.o: Mega8App.cpp
	$(CXX2) /nologo $(CFLAGS_WIN64) $(INC_WIN64) /c Mega8App.cpp /Fo$(OBJDIR_WIN64)/Mega8App.o

$(OBJDIR_WIN64)/Mega8Config.o: Mega8Config.cpp
	$(CXX2) /nologo $(CFLAGS_WIN64) $(INC_WIN64) /c Mega8Config.cpp /Fo$(OBJDIR_WIN64)/Mega8Config.o

$(OBJDIR_WIN64)/Mega8Main.o: Mega8Main.cpp
	$(CXX2) /nologo $(CFLAGS_WIN64) $(INC_WIN64) /c Mega8Main.cpp /Fo$(OBJDIR_WIN64)/Mega8Main.o

$(OBJDIR_WIN64)/src/Chip8GL.o: src/Chip8GL.cpp
	$(CXX2) /nologo $(CFLAGS_WIN64) $(INC_WIN64) /c src/Chip8GL.cpp /Fo$(OBJDIR_WIN64)/src/Chip8GL.o

clean_win64: 
	rm -f $(OBJ_WIN64) $(OUT_WIN64)
	rm -rf bin/Win64
	rm -rf $(OBJDIR_WIN64)/Chip8
	rm -rf $(OBJDIR_WIN64)
	rm -rf $(OBJDIR_WIN64)/src

.PHONY: before_debug after_debug clean_debug before_release after_release clean_release before_win32 after_win32 clean_win32 before_win64 after_win64 clean_win64

