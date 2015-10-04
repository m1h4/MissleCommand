// Copyright 2004/2006 Miha Software

#pragma once

#pragma warning(disable : 4244)	// Conversions (double to float etc.)

#define STRICT
#define DIRECTINPUT_VERSION 0x0800

#include <windows.h>
#include <commctrl.h>
#include <commdlg.h>
#include <basetsd.h>
#include <math.h>
#include <stdio.h>
#include <tchar.h>

#include <d3d9.h>
#include <dsound.h>
#include <d3dx9.h>
#include <dxerr9.h>
#include <dinput.h>

#include <shlwapi.h>

#include "resource.h"
#include "time.h"		// For clock()

#include "TinyXml/TinyXml.h"

#include <string>
#include <vector>

#include <crtdbg.h>

#define COLOR_TEXT nColor(1.0f,1.0f,1.0f,1.0f)
#define COLOR_BACKGROUND nColor(1.0f,0.0f,0.0f,0.0f)
#define COLOR_FOREGROUND nColor(1.0f,1.0f,1.0f,1.0f)

#ifdef _DEBUG
	#define new new(_CLIENT_BLOCK, __FILE__, __LINE__)
#endif

#define SETTINGS_CLASSNAME "MissleCommand"

#define SETTINGS_MUTEXSTRING "MissleCommand 856379857515467257473294"__TIMESTAMP__

#ifdef _DEBUG
#define SETTINGS_WINDOWTITLE "Missle Command Debug"
#else
#define SETTINGS_WINDOWTITLE "Missle Command"
#endif

#define nMalloc(ptr) new ptr
#define nFree(ptr) delete ptr
#define nSafeFree(ptr) { if(ptr) { delete ptr; ptr = NULL; } }
#define nSafeRelease(ptr) { if(ptr) { ptr->Release(); ptr = NULL; } }

#define nAssert assert
#define nArray std::vector
#define nPI D3DX_PI
#define nMatrix4 D3DXMATRIXA16

#define nPresentParameters D3DPRESENT_PARAMETERS

__forceinline DWORD F2DW(FLOAT f) { return *((DWORD*)&f); }

inline bool GetUseLog(bool* use = NULL)
{
#ifdef _DEBUG
	static bool log = true;
#else
	static bool log = false;
#endif

	if(use)
		log = *use;

	return log;
}

inline void LogWrite(const char* str,bool close)
{
	static HANDLE file = NULL;
	if(!file && GetUseLog())
	{
		file = CreateFile("Log.log",GENERIC_WRITE,FILE_SHARE_READ,NULL,CREATE_ALWAYS,NULL,NULL);
		if(file == INVALID_HANDLE_VALUE)
			return;
	}

	unsigned long written = NULL;

	if(str && file && file != INVALID_HANDLE_VALUE)
	{
		WriteFile(file,str,strlen(str),&written,NULL);
		FlushFileBuffers(file);
	}

	if(close)
	{
		if(file && file != INVALID_HANDLE_VALUE)
			CloseHandle(file);

		file = NULL;
	}
}

inline void Trace(LPCTSTR fmt,...)
{
#if defined(DEBUG) | defined(_DEBUG)
    TCHAR strBuffer[8096];
    
    va_list args;
    va_start(args,fmt);
    _vsntprintf(strBuffer,8096,fmt,args);
    va_end(args);

    OutputDebugString(strBuffer);

	if(GetUseLog())
		LogWrite(strBuffer,false);
#else
	if(!GetUseLog())
		return;

	TCHAR strBuffer[8096];
    
    va_list args;
    va_start(args,fmt);
    _vsntprintf(strBuffer,8096,fmt,args);
    va_end(args);

	LogWrite(strBuffer,false);
#endif
}

inline bool TraceRet(LPTSTR msg,bool ret)
{
	Trace(msg);
	return ret;
}

// Linear interpolation function
__forceinline float lerpf(float p1,float p2,float s)
{ 
	return p1 + s * (p2 - p1);
}

__forceinline double lerp(double p1,double p2,double s)
{
	return p1 + s * (p2 - p1);
}

// Cubic interpolation function
__forceinline float cubicf(float p1,float p2,float s)
{
	float s1 = 2.0f * powf(s,3.0f);
	float s2 = 3.0f * powf(s,2.0f);

	return p1 * (s1 - s2 + 1.0f) + p2 * (s2 - s1);
}

__forceinline double cubic(double p1,double p2,double s)
{
	double s1 = 2.0 * pow(s,3.0);
	double s2 = 3.0 * pow(s,2.0);

	return p1 * (s1 - s2 + 1.0) + p2 * (s2 - s1);
}

__forceinline float randf(float min, float max)
{
    float randnum = (float)rand() / RAND_MAX;
    return min + (max - min) * randnum;
}

__forceinline double rand(double min, double max)
{
    double randnum = (double)rand() / RAND_MAX;
    return min + (max - min) * randnum;
}

#include "nString.h"
#include "nRect.h"
#include "nSize.h"
#include "nPoint.h"
#include "nVector.h"

#define SETTINGS_SCORE_MAGIC 0x4EABC21F
#define SETTINGS_SCORE_VERSION 1.0

#define SETTINGS_DESIREDFRAMERATE 30.0f		// 30.0f default

#define SETTINGS_FONTNAME "Verdana"			// Verdana default
#define SETTINGS_FONTHEIGHT 13				// 13 default

// Flags
#define SETTINGS_WINDOWED		"false"		// Should we display in a window
#define SETTINGS_VSYNC			"true"		// Should we display with no vertical sync
#define SETTINGS_FSAA			"false"		// Should we display with fullscreen antialiasing
#define SETTINGS_HIGHPRIORITY	"false"		// Should we set the app's priority to a higher level
#define SETTINGS_LOG			"false"		// Should we output debug messages to the log file
#define SETTINGS_MOUSESMOOTH	"false"		// Should we smooth the mouse input
#define SETTINGS_DRAWFPS		"false"		// Should we show the fps count
#define SETTINGS_LINEANTIALIAS	"true"		// Should we antialias lines

// DirectX Libraries
#pragma comment (lib, "shlwapi.lib")	/* link with Windows Shell API lib */
#pragma comment (lib, "winmm.lib")		/* link with Windows MultiMedia lib */
#pragma comment (lib, "dxguid.lib")		/* link with DirectX GUID lib */
//#pragma comment (lib, "version.lib")	/* link with Version lib */
#pragma comment (lib, "dinput8.lib")	/* link with DirectInput 8 lib */
#pragma comment (lib, "dsound.lib")		/* link with DirectSound 8 lib */
//#pragma comment (lib, "ddraw.lib")		/* link with DirectDraw lib */
#pragma comment (lib, "dxerr9.lib")		/* link with DirectX Helper lib */
#pragma comment (lib, "d3d9.lib")		/* link with Direct3D 9 lib */

// Use debug libraries where possible
#ifdef _DEBUG
	#pragma comment (lib, "d3dx9d.lib")		/* link with DirectX Extension lib */
#else
	#pragma comment (lib, "d3dx9.lib")		/* link with DirectX Extension lib */
#endif