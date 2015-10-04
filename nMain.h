// Copyright 2004/2006 Miha Software

#pragma once

#include "nGraphics.h"
#include "nAudio.h"
#include "nInput.h"
#include "nGame.h"
#include "nTimer.h"
#include "nCommandLine.h"

class nMainFrame
{
public:
	nMainFrame(void);
	~nMainFrame(void);

	int Run(HWND hWnd,LPCSTR cmdLine);

	void OnIdle();
	
	static LRESULT CALLBACK WindowProcedure(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);

	nGraphics* GetGraphics() { return m_Graphics; }
	nAudio* GetAudio() { return m_Audio; }
	nInput* GetInput() { return m_Input; }
	nGame* GetGame() { return m_Game; }

	const nString& GetModulePath() const { return m_ModulePath; }
	const nString& GetCommandLineVar(const char* name,const nString& def) { return m_CommandLine.GetVar(name,def); }

	double GetElapsedTime() const { return m_ElapsedTime; }
	double GetAbsoluteTime() const { return m_AbsoluteTime; }

	HWND GetWindowHandle() { return m_hWnd; }

	static void MessageError(const char* message,const char* title);
	static void LastError(char* message,bool get = false);

	static nTimer* GetTimer() { static nTimer timer; return &timer; }

protected:
	nGraphics*			m_Graphics;
	nAudio*				m_Audio;
	nInput*				m_Input;
	nGame*				m_Game;

	nString				m_ModulePath;		// Path to the exe
	nCommandLine		m_CommandLine;		// Our command line vars

	HWND				m_hWnd;			// Our window handle

	double				m_ElapsedTime;	// Our global speed factor for moving objects
	double				m_AbsoluteTime;	// Abstolute app time
};

inline nMainFrame* nGetInstance(bool destroy = false)
{
	static nMainFrame* instance = NULL;	// Pointer to global nMainFrame instance

	if(!instance)
		instance = nMalloc(nMainFrame);

	if(destroy)
		nSafeFree(instance);

	return instance;
}