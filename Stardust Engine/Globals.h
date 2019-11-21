#pragma once

// Warning disabled ---
#pragma warning( disable : 4577 ) // Warning that exceptions are disabled
#pragma warning( disable : 4530 ) // Warning that exceptions are disabled

#include <windows.h>
#include <stdio.h>

#define LOG(format, ...) log(__FILE__, __LINE__, format, __VA_ARGS__);

void log(const char file[], int line, const char* format, ...);

#define CAP(n) ((n <= 0.0f) ? n=0.0f : (n >= 1.0f) ? n=1.0f : n=n)

#define DEGTORAD 0.0174532925199432957f
#define RADTODEG 57.295779513082320876f
#define HAVE_M_PI

typedef unsigned int uint;
typedef unsigned int UID;

enum update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

// Configuration -----------
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 1024
#define SCREEN_SIZE 1
#define WIN_FULLSCREEN false
#define WIN_RESIZABLE true
#define WIN_BORDERLESS false
#define WIN_FULLSCREEN_DESKTOP false
#define TITLE "Stardust Engine"
#define ORGANIZATION "Speedwagon Foundation"
#define ENGINE_VER_SUP 0
#define ENGINE_VER_INF 7


#define RELEASE(x)\
{\
   if(x != nullptr)\
	{\
	  delete x;\
	  x = nullptr;\
    }\
}

#define RELEASE_ARRAY(x)\
{\
   if(x != nullptr)\
	{\
      delete[] x;\
	  x = nullptr;\
	}\
}


#define ASSETS_FOLDER "Assets/"
#define ASSETS_MESH_FOLDER "Assets/Meshes/"
#define ASSETS_TEX_FOLDER "Assets/Textures/"
#define ASSETS_SCENE_FOLDER "Assets/Scenes/"
//#define SETTINGS_FOLDER "/Settings/"
#define LIBRARY_FOLDER "Library/"
#define LIBRARY_MESH_FOLDER "Library/Meshes/"
#define LIBRARY_MAT_FOLDER "Library/Materials/"

#define MESH_EXTENSION "stdtmesh"