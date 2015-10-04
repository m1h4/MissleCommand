// Copyright 2004/2006 Miha Software

#include "nGlobals.h"
#include "nMain.h"

// Our main entry point
INT WINAPI WinMain(HINSTANCE /*hInstance*/, HINSTANCE /*hPrevInstance*/, LPSTR lpCmdLine, INT nCmdShow)
{
	// Create a system wide mutex
	HANDLE mutex = CreateMutex(NULL,FALSE,SETTINGS_MUTEXSTRING);
	if(GetLastError() == ERROR_ALREADY_EXISTS)
	{
		MessageBox(GetDesktopWindow(),"A instance of "SETTINGS_WINDOWTITLE" is already running.\n\nPlease close it before running again.","Already running",MB_OK|MB_ICONEXCLAMATION);
		return 0;
	}

	// Randomize the random seed
	srand((unsigned int)time(NULL));

	// Init COM so we can use CoCreateInstance
    CoInitialize(0);

	// Create our main app object
	nGetInstance();

    // Register the window class
	WNDCLASSEX wc = {sizeof(WNDCLASSEX), CS_CLASSDC, nGetInstance()->WindowProcedure, 0L, 0L, 
					 GetModuleHandle(0), LoadIcon(GetModuleHandle(0),MAKEINTRESOURCE(IDI_MAIN_ICON)), LoadCursor(0,IDC_ARROW), 0, 0,
                     SETTINGS_CLASSNAME, 0};
    RegisterClassEx(&wc);

    // Create the application's window
    HWND hWnd = CreateWindow(SETTINGS_CLASSNAME, SETTINGS_WINDOWTITLE, 
							WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX
							, 100, 100, 640, 480,
							GetDesktopWindow(), 0, wc.hInstance, 0);

	// Set the client size
	nRect rect(0,0,640,480);
	AdjustWindowRect((LPRECT)&rect,WS_OVERLAPPED|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX,FALSE);
	SetWindowPos(hWnd,NULL,0,0,rect.GetWidth(),rect.GetHeight(),SWP_NOMOVE);

	// Show the window the way windows wants it to show instad of creating it with WS_VISIBLE
	ShowWindow(hWnd,nCmdShow);

	// Call our main frame's main function that will block until the app exits
	int ret = nGetInstance()->Run(hWnd,lpCmdLine);

	// Destroy the app
	nGetInstance(true);

	// Unregister the window class registrated at the begining of this function
    UnregisterClass(SETTINGS_CLASSNAME, wc.hInstance);

	// Uninit COM
	CoUninitialize();

	// Close mutex
	CloseHandle(mutex);

	// Close log
	LogWrite(NULL,true);

	// Dump memory leaks
	_CrtMemDumpAllObjectsSince(NULL);

	// Return the value previously returned by our app's main function
    return ret;
}

nMainFrame::nMainFrame() :
m_hWnd(NULL),
m_Graphics(NULL),
m_Audio(NULL),
m_Input(NULL),
m_Game(NULL)
{
	// Reset times
	m_ElapsedTime = 0.0f;
	m_AbsoluteTime = 0.0f;
}

nMainFrame::~nMainFrame()
{
	nSafeFree(m_Graphics);
	nSafeFree(m_Audio);
	nSafeFree(m_Input);
	nSafeFree(m_Game);

	Trace(__FUNCTION__" Main Frame destroyed.\n");
}

void nMainFrame::MessageError(const char* message,const char* title)
{
	MessageBox(GetDesktopWindow(),message,title,MB_OK|MB_ICONEXCLAMATION);
}

void nMainFrame::LastError(char* message,bool get)
{
	static char buffer[512] = {""};

	if(get)
		strcpy(message,buffer);
	else
		strncpy(buffer,message,min(strlen(message),510));
}

int nMainFrame::Run(HWND hWnd,LPCSTR cmdLine)
{
	// Build the command line var list
	m_CommandLine.Set(cmdLine);

	// Enable loging
	bool use = GetCommandLineVar("-log",SETTINGS_LOG).asBool();

	// Set log
	GetUseLog(&use);

	// Increase our priority
	if(GetCommandLineVar("-highpriority",SETTINGS_HIGHPRIORITY).asBool())
	{
		Trace(__FUNCTION__" Setting thread priority level to above normal.\n");
		SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL);
	}

	// Start our app timer, start stop it just to get the current time
	GetTimer()->Start();
	GetTimer()->Stop();

	// Update the absolute time so any outputed strings douring initialization are visible
	m_AbsoluteTime = GetTimer()->GetAbsoluteTime();

	// Store our window handle
	m_hWnd = hWnd;

	// Get the current module directory
	char buffer[MAX_PATH];
	GetModuleFileName(NULL,buffer,MAX_PATH);
	PathRemoveFileSpec(buffer);
	PathAddBackslash(buffer);

	m_ModulePath = buffer;

	// Draw the load screen
	m_Graphics->DrawLoadScreen(hWnd,"Systems",5);

	m_Graphics = nMalloc(nGraphics);
	m_Audio = nMalloc(nAudio);
	m_Input = nMalloc(nInput);
	m_Game = nMalloc(nGame);

	// Draw the load screen
	m_Graphics->DrawLoadScreen(hWnd,"Graphics",15);

	// Init direct3d
	if(!m_Graphics->InitGraphics())
	{
		char buffer[512],buffer2[512];
		LastError(buffer,true);
		_snprintf(buffer2,sizeof(buffer2),"Failed to initialize graphics system.\n\n%s",buffer);
		MessageError(buffer2,"Error");
		return -1;
	}

	// Draw the load screen
	m_Graphics->DrawLoadScreen(hWnd,"Resources",20);

	// Init textures
	if(!m_Graphics->InitTextures())
	{
		char buffer[512],buffer2[512];
		LastError(buffer,true);
		_snprintf(buffer2,sizeof(buffer2),"Failed to initialize resources.\n\n%s",buffer);
		MessageError(buffer2,"Error");
		return -1;
	}

	// Set draw fps
	m_Graphics->SetDrawFps(GetCommandLineVar("-drawfps",SETTINGS_DRAWFPS).asBool());

	// Set line antialiasing
	m_Graphics->SetLineAntialias(GetCommandLineVar("-lineantialias",SETTINGS_LINEANTIALIAS).asBool());

	// Draw the load screen
	m_Graphics->DrawLoadScreen(hWnd,"Sound System",25);

	// Init the sound capture objects
	if(!m_Audio->InitAudio())
	{
		char buffer[512],buffer2[512];
		LastError(buffer,true);
		_snprintf(buffer2,sizeof(buffer2),"Failed to initialize audio system.\n\n%s",buffer);
		MessageError(buffer2,"Error");
		return -1;
	}

	// Draw the load screen
	m_Graphics->DrawLoadScreen(hWnd,"Input",30);

	if(!m_Input->InitInput())
	{
		char buffer[512],buffer2[512];
		LastError(buffer,true);
		_snprintf(buffer2,sizeof(buffer2),"Failed to initialize input system.\n\n%s",buffer);
		MessageError(buffer2,"Error");
		return -1;
	}

	// Set mouse smooth
	m_Input->GetMouse()->SetSmooth(GetCommandLineVar("-mousesmooth",SETTINGS_MOUSESMOOTH).asBool());

	// Draw the load screen
	m_Graphics->DrawLoadScreen(hWnd,"Configuration",90);

	// Init the game data
	m_Game->InitGame();

	// Load hiscores
	m_Game->ReadHiScores();

	// Draw the load screen
	m_Graphics->DrawLoadScreen(hWnd,"Done",100);

	// Actualy start the timer
	GetTimer()->Start();

	// This will hold our messages
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));  // Must do this otherwise the app would hang

	// Enter the message loop
	while(msg.message != WM_QUIT)
	{
		if(PeekMessage(&msg, 0, 0U, 0U, PM_REMOVE))
		{
			// We got a message so translate and dispatch it
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else // No message recived so call our idle function
			OnIdle();
	}

	Trace(__FUNCTION__" Exited message loop.\n");

	// Save the hiscores
	m_Game->WriteHiScores();

	if(m_Graphics)
		m_Graphics->CleanGraphics();

	if(m_Audio)
		m_Audio->CleanAudio();

	if(m_Input)
		m_Input->CleanInput();

	if(m_Game)
		m_Game->CleanGame();

	nSafeFree(m_Graphics);
	nSafeFree(m_Audio);
	nSafeFree(m_Input);
	nSafeFree(m_Game);

	// Return the wParam cause it contains our exit code
	return (int)msg.wParam;
}

void nMainFrame::OnIdle()
{
	// Don't do anything if we are paused
	if(GetTimer()->IsStopped())
	{
		Trace(__FUNCTION__" Sleeping while paused (100 ms).\n");
		Sleep(100);
		return;
	}

	// Update the speed factor
	m_ElapsedTime = GetTimer()->GetElapsedTime();
	m_AbsoluteTime += m_ElapsedTime;
	m_ElapsedTime *= SETTINGS_DESIREDFRAMERATE;
	if(m_ElapsedTime > 5.0f)
	{
		Trace(__FUNCTION__" Overhead by %g, reset to 5.0\n",m_ElapsedTime);
		m_ElapsedTime = 5.0f;
	}

	// Process input
	if(m_Input)
		m_Input->Process();

	// Process game data
	if(m_Game)
		m_Game->Process();

	// Process the capture and preparation of audio
	if(m_Audio)
		m_Audio->Process();

	// Render the captured data
	if(m_Graphics)
		m_Graphics->Render();
}

LRESULT CALLBACK nMainFrame::WindowProcedure(HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
{
	// Our message handler
	switch(msg)
	{
		case WM_CLEAR:
			return TRUE;

		case WM_SETCURSOR:
			if(nGetInstance()->GetGraphics() && nGetInstance()->GetGraphics()->GetFullscreen())
			{
				SetCursor(NULL);
				return TRUE;
			}
			break;

		case WM_EXITMENULOOP:
		case WM_EXITSIZEMOVE:
			// Unpause the app
			nGetInstance()->GetTimer()->Reset();
			nGetInstance()->GetTimer()->Start();
			break;

		case WM_ENTERMENULOOP:
		case WM_ENTERSIZEMOVE:
			// Pause the app
			nGetInstance()->GetGame()->SetPause(true);
			nGetInstance()->GetTimer()->Stop();
			break;

		case WM_ACTIVATE:
			// Check if the app lost focus
			if(LOWORD(wParam) == WA_INACTIVE)
			{
				nGetInstance()->GetGame()->SetPause(true);
			}

			if(HIWORD(wParam))
			{
				nGetInstance()->GetGame()->SetPause(true);
				nGetInstance()->GetTimer()->Stop();
			}
			else
			{
				nGetInstance()->GetTimer()->Reset();
				nGetInstance()->GetTimer()->Start();
			}
			break;

		case WM_CLOSE:
		case WM_DESTROY:
			PostQuitMessage(0); // Post the quit message so we exit our message pump
			return TRUE;

		case WM_KEYDOWN:
			switch(wParam)
			{
#ifdef _DEBUG
			case VK_ESCAPE:
				PostQuitMessage(0);
				break;
#endif
		}
	}

	// Let the default message handler handle all unhandled messages
    return DefWindowProc(hWnd,msg,wParam,lParam);
}