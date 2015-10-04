// Copyright 2004/2006 Miha Software

#include "nGlobals.h"
#include "nMain.h"
#include "nGraphics.h"

struct n2DVertex
{
	float x, y, z, h;
	unsigned long color;

	static const unsigned long FVF = D3DFVF_XYZRHW|D3DFVF_DIFFUSE;
};

struct n2DVertexBump
{
	float x, y, z, h;
	float tu1, tv1;
	float tu2, tv2;

	static const unsigned long FVF = D3DFVF_XYZRHW|D3DFVF_TEX2;
};

nGraphics::nGraphics() :
m_BoldFontHeight(NULL),
m_NormalFontHeight(NULL),
m_pDirect3D(NULL),
m_pDevice(NULL),
m_pSprite(NULL),
m_pLine(NULL),
m_pFontBold(NULL),
m_pFontNormal(NULL),
m_DrawFps(true)	// Default
{
	m_pStateBlocks[0] = NULL;
	m_pStateBlocks[1] = NULL;
}

nGraphics::~nGraphics()
{
	CleanGraphics();
}

bool nGraphics::ValidateDevice()
{
	// Get the device's caps
	D3DCAPS9 d3dCaps;
	m_pDirect3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dCaps);

	// Check if device is HAL
	if(d3dCaps.DeviceType != D3DDEVTYPE_HAL)
	{
		nMainFrame::LastError("Device isn't HAL.");
		return TraceRet(__FUNCTION__" Device isn't HAL.\n",false);
	}

	// Check if device supports environment bumpmaping
	if(!(d3dCaps.TextureOpCaps & D3DTEXOPCAPS_BUMPENVMAP))
	{
		nMainFrame::LastError("Device doesn't support Environment Bumpmaping.");
		return TraceRet(__FUNCTION__" Device doesn't support Environment Bumpmaping.\n",false);
	}

	// Accept devices that can create D3DFMT_V8U8 textures
    if(FAILED(m_pDirect3D->CheckDeviceFormat(d3dCaps.AdapterOrdinal,
                                              d3dCaps.DeviceType, 
											  D3DFMT_X8R8G8B8,
                                              0, D3DRTYPE_TEXTURE,
                                              D3DFMT_V8U8)))
	{
		nMainFrame::LastError("Device doesn't support D3DFMT_V8U8.");
		return TraceRet(__FUNCTION__" Device doesn't support D3DFMT_V8U8.\n",false);
	}

	// Accept devices that can create D3DFMT_A8R8G8B8 render target
    if(FAILED(m_pDirect3D->CheckDeviceFormat(d3dCaps.AdapterOrdinal,
                                              d3dCaps.DeviceType, 
											  D3DFMT_X8R8G8B8,
                                              D3DUSAGE_RENDERTARGET, D3DRTYPE_TEXTURE,
                                              D3DFMT_A8R8G8B8)))
	{
		nMainFrame::LastError("Device doesn't support D3DFMT_A8R8G8B8 Render Targets.");
		return TraceRet(__FUNCTION__" Device doesn't support D3DFMT_A8R8G8B8 Render Targets.\n",false);
	}

	return true;
}

bool nGraphics::InitGraphics()
{
	// Create our base direct3d object
	m_pDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
	if(!m_pDirect3D)
	{
		nMainFrame::LastError("Failed to create direct3d 9, directx 9 not installed?");
		return TraceRet(__FUNCTION__" Failed to create direct3d 9, directx 9 not installed?\n",false);
	}

	// Check if the device support's the caps we will use
	if(!ValidateDevice())
		return false;

	// Get the desktop's display mode
	D3DDISPLAYMODE displayMode;
	if(FAILED(m_pDirect3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode)))
	{
		nMainFrame::LastError("Failed to get current display adapter mode.");
		return TraceRet(__FUNCTION__" Failed to get current display adapter mode.\n",false);
	}

	// The struct that describes our direct3d device
    ZeroMemory(&m_PresentParameters,sizeof(m_PresentParameters));
	
	m_Fullscreen = !nGetInstance()->GetCommandLineVar("-windowed",SETTINGS_WINDOWED).asBool();

    m_PresentParameters.Windowed               = !m_Fullscreen;
	m_PresentParameters.SwapEffect             = D3DSWAPEFFECT_DISCARD;
	m_PresentParameters.BackBufferFormat       = D3DFMT_X8R8G8B8;
    m_PresentParameters.EnableAutoDepthStencil = TRUE;
    m_PresentParameters.AutoDepthStencilFormat = D3DFMT_D24S8;

	// Check if we should use fsaa
	if(nGetInstance()->GetCommandLineVar("-fsaa",SETTINGS_FSAA).asBool())
	{
		unsigned long samples = 0;

		// Check if multisampling is supported and enable it if it is
		if(SUCCEEDED(m_pDirect3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,m_PresentParameters.BackBufferFormat,!m_Fullscreen,D3DMULTISAMPLE_NONMASKABLE,&samples)))
		{
			m_PresentParameters.MultiSampleQuality	 = samples-1;
			m_PresentParameters.MultiSampleType		 = D3DMULTISAMPLE_NONMASKABLE;

			Trace(__FUNCTION__" Using %dx multisampling.\n",samples-1);
		}
	}

	// Check if we should display in fullscreen
	if(m_Fullscreen)
	{
		m_PresentParameters.BackBufferWidth	 = displayMode.Width;
		m_PresentParameters.BackBufferHeight = displayMode.Height;
	}
	else
	{
		nRect rect;
		GetClientRect(nGetInstance()->GetWindowHandle(),(LPRECT)&rect);
		
		m_PresentParameters.BackBufferWidth = rect.GetWidth();
		m_PresentParameters.BackBufferHeight = rect.GetHeight();
	}
	
	// Check if we shouldn't use vsync
	if(!nGetInstance()->GetCommandLineVar("-vsync",SETTINGS_VSYNC).asBool())
		m_PresentParameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	Trace(__FUNCTION__" Display mode %dx%d.\n",m_PresentParameters.BackBufferWidth,m_PresentParameters.BackBufferHeight);

	// Now create our direct3d device from the base direct3d object
	HRESULT hr = m_pDirect3D->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,nGetInstance()->GetWindowHandle(),D3DCREATE_SOFTWARE_VERTEXPROCESSING,&m_PresentParameters,&m_pDevice);
	if(FAILED(hr))
		return TraceRet(__FUNCTION__" Failed to create the device.\n",false);

	// Turn on color dithering
	hr = m_pDevice->SetRenderState(D3DRS_DITHERENABLE,TRUE);
	if(FAILED(hr))
		Trace(__FUNCTION__" Display adapter doesn't support color dithering, using default.\n");

	// Turn off culling
	m_pDevice->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	// Enable anisotropic filtering if fsaa is enabled
	//if(flags & RECFLAG_FSAA)
	//	EnableAnisotropic(m_p3DDevice);

	// Create our line drawing object
	hr = D3DXCreateLine(m_pDevice, &m_pLine);
	if(FAILED(hr))
	{
		nMainFrame::LastError("Failed to create line drawing object.");
		return TraceRet(__FUNCTION__" Failed to create line drawing object.\n",false);
	}
	
	// Don't use gl lines
	m_pLine->SetGLLines(false);

	// Init our font
	hr = D3DXCreateFont(m_pDevice,SETTINGS_FONTHEIGHT,0,FW_BOLD,0,FALSE,DEFAULT_CHARSET,OUT_TT_PRECIS,6/*CLEARTYPE_QUALITY*/,DEFAULT_PITCH,SETTINGS_FONTNAME,&m_pFontBold);
	if(FAILED(hr))
	{
		nMainFrame::LastError("Failed to create font. Font not found?");
		return TraceRet(__FUNCTION__" Failed to create font. Font not found?",false);
	}

	// Init our thin font
	hr = D3DXCreateFont(m_pDevice,SETTINGS_FONTHEIGHT,0,FW_NORMAL,0,FALSE,DEFAULT_CHARSET,OUT_TT_PRECIS,6/*CLEARTYPE_QUALITY*/,DEFAULT_PITCH,SETTINGS_FONTNAME,&m_pFontNormal);
	if(FAILED(hr))
	{
		nMainFrame::LastError("Failed to create bold font. Font not found?");
		return TraceRet(__FUNCTION__" Failed to create bold font. Font not found?",false);
	}
	
	// Get the font sizes
	nRect fontSize;
	GetTextRect(GetBoldFont(),"str",&fontSize,NULL);
	m_BoldFontHeight = fontSize.GetHeight();
	GetTextRect(GetNormalFont(),"str",&fontSize,NULL);
	m_NormalFontHeight = fontSize.GetHeight();

	// Create our sprite drawing object
	hr = D3DXCreateSprite(m_pDevice, &m_pSprite);
	if(FAILED(hr))
	{
		nMainFrame::LastError("Failed to create sprite.");
		return TraceRet(__FUNCTION__" Failed to create sprite.\n",false);
	}

	// Turn on texture filtering
	m_pDevice->SetSamplerState(0,D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(0,D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(1,D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(1,D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(2,D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pDevice->SetSamplerState(2,D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	return true;
}

bool nGraphics::InitTextures()
{
	//
	// Here we initialize any D3DPOOL_DEFAULT textures that get recreated on device loss
	//

	// Create our state blocks
	HRESULT hr = m_pDevice->CreateStateBlock(D3DSBT_ALL,&m_pStateBlocks[0]);
	if(FAILED(hr))
	{
		nMainFrame::LastError("Failed to create state block 1.");
		return TraceRet(__FUNCTION__" Failed to create state block 1.\n",false);
	}

	hr = m_pDevice->CreateStateBlock(D3DSBT_ALL,&m_pStateBlocks[1]);
	if(FAILED(hr))
	{
		nMainFrame::LastError("Failed to create state block 2.");
		return TraceRet(__FUNCTION__" Failed to create state block 2.\n",false);
	}

	return true;
}

void nGraphics::CleanTextures()
{
	// Delete the textures
	nSafeRelease(m_pStateBlocks[0]);
	nSafeRelease(m_pStateBlocks[1]);
}

void nGraphics::CleanGraphics()
{
	CleanTextures();

	// Release our interfaces
	nSafeRelease(m_pSprite);
	nSafeRelease(m_pLine);
	nSafeRelease(m_pFontBold);
	nSafeRelease(m_pFontNormal);
	nSafeRelease(m_pStateBlocks[0]);
	nSafeRelease(m_pStateBlocks[1]);
	nSafeRelease(m_pDevice);
	nSafeRelease(m_pDirect3D);
}

bool nGraphics::EnableAnisotropic()
{
	// Get the device's caps
	D3DCAPS9 caps;
	if(FAILED(m_pDevice->GetDeviceCaps(&caps)))
	{
		nMainFrame::LastError("Failed to get device caps.");
		return TraceRet(__FUNCTION__" Failed to get device caps.\n",false);
	}

	// Check if anisotropic min filtering is supported and enabel it if it is
	if(caps.TextureFilterCaps & D3DPTFILTERCAPS_MINFANISOTROPIC)
	{
		m_pDevice->SetSamplerState(0,D3DSAMP_MINFILTER,D3DTEXF_ANISOTROPIC);
		m_pDevice->SetSamplerState(0,D3DSAMP_MAXANISOTROPY,caps.MaxAnisotropy);
		Trace(__FUNCTION__" %dx Min Anisotropic filtering enabled.\n",caps.MaxAnisotropy);
	}
	else
		Trace(__FUNCTION__" Min Anisotropic filtering not supported by device.\n");

	// Check if anisotropic mag filtering is supported and enabel it if it is
	if(caps.TextureFilterCaps & D3DPTFILTERCAPS_MAGFANISOTROPIC)
	{
		m_pDevice->SetSamplerState(0,D3DSAMP_MAGFILTER,D3DTEXF_ANISOTROPIC);
		m_pDevice->SetSamplerState(0,D3DSAMP_MAXANISOTROPY,caps.MaxAnisotropy);
		Trace(__FUNCTION__" %dx Mag Anisotropic filtering enabled.\n",caps.MaxAnisotropy);
	}
	else
		Trace(__FUNCTION__" Mag Anisotropic filtering not supported by device.\n");

	return true;
}

void nGraphics::DrawLoadScreen(HWND hWnd,const char* initmodule,unsigned long pos)
{
	//Sleep(333);

	HFONT hFont = CreateFont(SETTINGS_FONTHEIGHT,NULL,NULL,NULL,FW_BLACK,NULL,NULL,NULL,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,6,DEFAULT_PITCH,SETTINGS_FONTNAME);
	if(!hFont)
		Trace(__FUNCTION__" Failed to create font.\n");

	// Get the needed window information
	nRect rect,rect2;
	GetClientRect(hWnd,(LPRECT)&rect);
	HDC hDc = GetDC(hWnd);
	HBRUSH hBrush = CreateSolidBrush(RGB(190,190,190));
	HBRUSH hBrush2 = CreateSolidBrush(RGB(220,220,220));
	HBRUSH hBrush3 = CreateSolidBrush(RGB(240,240,240));

	HGDIOBJ oldFont = SelectObject(hDc,hFont);

	// First fill the background with color
	FillRect(hDc,(LPRECT)&rect,CreateSolidBrush(RGB(255,255,255)));

	// Our text buffer
	char buffer[100];

	_snprintf(buffer,sizeof(buffer),"|initializing: %s|",initmodule);

	// Draw text shadow
	rect.Offset(1,1);
	SetTextColor(hDc,RGB(222,222,222));
	::DrawText(hDc,buffer,-1,(LPRECT)&rect,DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_NOCLIP);

	static const nRect loadRect(200,10);

	// Write a loading message while the app is loading
	rect.Offset(-1,-1);
	SetTextColor(hDc,RGB(188,188,188));
	::DrawText(hDc,buffer,-1,(LPRECT)&rect,DT_SINGLELINE|DT_VCENTER|DT_CENTER|DT_NOCLIP);

	rect2.Set(rect.GetWidth()/2-loadRect.GetWidth()/2,rect.GetHeight()/2+20,rect.GetWidth()/2+loadRect.GetWidth()/2,rect.GetHeight()/2+20+loadRect.GetHeight());
	rect2.Inflate(1,1);
	FrameRect(hDc,(LPRECT)&rect2,hBrush);
	rect2.Inflate(1,1);
	FrameRect(hDc,(LPRECT)&rect2,hBrush3);

	rect2.Set(rect.GetWidth()/2-loadRect.GetWidth()/2,rect.GetHeight()/2+20,rect.GetWidth()/2-loadRect.GetWidth()/2+pos*(loadRect.GetWidth()/100),rect.GetHeight()/2+20+loadRect.GetHeight());
	FillRect(hDc,(LPRECT)&rect2,hBrush2);

	DeleteObject(SelectObject(hDc,oldFont));

	ReleaseDC(hWnd,hDc);
	DeleteObject(hBrush);
	DeleteObject(hBrush2);
	DeleteObject(hBrush3);
}

void nGraphics::DrawStatusText()
{
	// Out text buffers
	char buffer[256];

    // Keep track of the frame count
    static double lasttime = 0.0;
	static bool blink = false;

	// Update the fps stats every second
    if(nGetInstance()->GetAbsoluteTime() - lasttime > 1.0f)
    {
		m_Fps = SETTINGS_DESIREDFRAMERATE/nGetInstance()->GetElapsedTime();
        lasttime = nGetInstance()->GetAbsoluteTime();

		blink = !blink;
	}
	
	// Draw the fps stats
	if(m_DrawFps)
	{
		_snprintf(buffer,sizeof(buffer),"Fps %06.2f",m_Fps);
		DrawShadowText(GetBoldFont(),nRect(5,5,0,0),buffer,DT_NOCLIP|DT_SINGLELINE,COLOR_TEXT);
	}
}

void nGraphics::Render()
{
	// Check if we have a valid render device, if not we don't have anything to render with
	if(!m_pDevice)
		return;

	HRESULT hr = NULL;

	// Check if device ok
	if(FAILED(hr = m_pDevice->TestCooperativeLevel()))
	{
		// Yield some CPU time to other processes
		Trace(__FUNCTION__" Sleeping on test coop level (100 ms).\n");
		Sleep(100);

		// The device has been lost but cannot be reset at this time. 
		// Therefore, rendering is not possible and we'll have to return 
		// and try again at a later time.
		if(hr == D3DERR_DEVICELOST)
		{
			nMainFrame::LastError("Display Device lost.");
			Trace(__FUNCTION__" Display Device lost.\n");
			return;
		}

		// The device has been lost but it can be reset at this time. 
		if(hr == D3DERR_DEVICENOTRESET)
		{
			// If the device can be restored, the application prepares the 
			// device by destroying all video-memory resources and any 
			// swap chains. 
			this->InvalidateDeviceObjects();

			if(FAILED(m_pDevice->Reset(&m_PresentParameters)))
			{
				nMainFrame::LastError("Failed to reset Display Device.");
				Trace(__FUNCTION__" Failed to reset Display Device.\n");
				return;
			}

			Trace(__FUNCTION__" Display Device reset.\n");

			// Finally, a lost device must re-create resources (including  
			// video memory resources) after it has been reset.

			this->RestoreDeviceObjects();
		}
	}

	// Clear the screen with the backgroun color
	m_pDevice->Clear(0,0,D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER,COLOR_BACKGROUND,1.0f,0);

	// Begin the scene
	if(SUCCEEDED(m_pDevice->BeginScene()))
	{
		// Begin sprite drawing
		SpriteBegin();

		// Draw the game
		nGetInstance()->GetGame()->Render();

		// Draw the status text
		DrawStatusText();

		// End 2D drawing
		SpriteEnd();

		// End the scene
		m_pDevice->EndScene();
	}

	// Present the scene to the user
	m_pDevice->Present(0,0,0,0);
}

void nGraphics::DrawShadowFillRect(const nRect& rect,const nColor& color)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	// Draw the rect
	static nRect rect2;
	static nColor color2;

	rect2 = rect;
	rect2.Offset(1,1);

	color2.r = color2.g = color2.b = 0.0f;
	color2.a = color.a * 0.1f;

	DrawFillRect(rect2,color2);
	DrawFillRect(rect,color);
}

void nGraphics::DrawShadowRect(const nRect& rect,float width,const nColor& color,unsigned char flags)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	// Draw the rect
	static nRect rect2;
	static nColor color2;

	rect2 = rect;
	rect2.Offset(1,1);

	color2.r = color2.g = color2.b = 0.0f;
	color2.a = color.a * 0.1f;

	DrawRect(rect2,width,color2,flags);
	DrawRect(rect,width,color,flags);
}

void nGraphics::DrawRect(const nRect& rect,float width,const nColor& color,unsigned char flags)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	// Horizontal lines
	if(flags & RectFlagTop)
		this->DrawFillRect(nRect(rect.left-width,rect.top-width,rect.right+width,rect.top),color);
	if(flags & RectFlagBottom)
		this->DrawFillRect(nRect(rect.left-width,rect.bottom,rect.right+width,rect.bottom+width),color);

	// Vertical lines
	if(flags & RectFlagLeft)
		this->DrawFillRect(nRect(rect.left-width,rect.top,rect.left,rect.bottom),color);
	if(flags & RectFlagRight)
		this->DrawFillRect(nRect(rect.right,rect.top,rect.right+width,rect.bottom),color);
}

void nGraphics::DrawShadowCornerRect(const nRect& rect,float size,float width,const nColor& color,unsigned char flags)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	// Draw the rect
	static nRect rect2;
	static nColor color2;

	rect2 = rect;
	rect2.Offset(1,1);

	color2.r = color2.g = color2.b = 0.0f;
	color2.a = color.a * 0.1f;

	DrawCornerRect(rect2,size,width,color2,flags);
	DrawCornerRect(rect,size,width,color,flags);
}

void nGraphics::DrawCornerRect(const nRect& rect,float size,float width,const nColor& color,unsigned char flags)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	// Horizontal lines
	if(flags & RectFlagTop)
	{
		this->DrawFillRect(nRect(rect.left-width,rect.top-width,rect.left+size+width,rect.top),color);
		this->DrawFillRect(nRect(rect.right-width-size,rect.top-width,rect.right+width,rect.top),color);
	}
	if(flags & RectFlagBottom)
	{
		this->DrawFillRect(nRect(rect.left-width,rect.bottom,rect.left+size+width,rect.bottom+width),color);
		this->DrawFillRect(nRect(rect.right-width-size,rect.bottom,rect.right+width,rect.bottom+width),color);
	}

	// Vertical lines
	if(flags & RectFlagLeft)
	{
		this->DrawFillRect(nRect(rect.left-width,rect.top,rect.left,rect.top+size),color);
		this->DrawFillRect(nRect(rect.left-width,rect.bottom-size,rect.left,rect.bottom),color);
	}
	if(flags & RectFlagRight)
	{
		this->DrawFillRect(nRect(rect.right,rect.top,rect.right+width,rect.top+size),color);
		this->DrawFillRect(nRect(rect.right,rect.bottom-size,rect.right+width,rect.bottom),color);
	}
}

void nGraphics::DrawShadowCenterRect(const nRect& rect,float size,float width,const nColor& color,unsigned char flags)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	// Draw the rect
	static nRect rect2;
	static nColor color2;

	rect2 = rect;
	rect2.Offset(1,1);

	color2.r = color2.g = color2.b = 0.0f;
	color2.a = color.a * 0.1f;

	DrawCenterRect(rect2,size,width,color2,flags);
	DrawCenterRect(rect,size,width,color,flags);
}

void nGraphics::DrawCenterRect(const nRect& rect,float size,float width,const nColor& color,unsigned char flags)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	// Horizontal lines
	if(flags & RectFlagTop)
		this->DrawFillRect(nRect(rect.left-width+(rect.GetWidth()-size)/2,rect.top-width,rect.right+width-(rect.GetWidth()-size)/2,rect.top),color);
	if(flags & RectFlagBottom)
		this->DrawFillRect(nRect(rect.left-width+(rect.GetWidth()-size)/2,rect.bottom,rect.right+width-(rect.GetWidth()-size)/2,rect.bottom+width),color);

	// Vertical lines
	if(flags & RectFlagLeft)
		this->DrawFillRect(nRect(rect.left-width,rect.top+(rect.GetHeight()-size)/2,rect.left,rect.bottom-(rect.GetHeight()-size)/2),color);
	if(flags & RectFlagRight)
		this->DrawFillRect(nRect(rect.right,rect.top+(rect.GetHeight()-size)/2,rect.right+width,rect.bottom-(rect.GetHeight()-size)/2),color);
}

void nGraphics::DrawTexture(IDirect3DTexture9* texture,const nPoint& point,const nColor& color)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	D3DSURFACE_DESC desc;
	texture->GetLevelDesc(0,&desc);
	DrawTexture(texture,nRect(point.x,point.y,point.x+desc.Width,point.y+desc.Height),color);
}

void nGraphics::DrawTexture(IDirect3DTexture9* texture,const nRect& rect,const nColor& color)
{
	if(!m_pSprite)
		return;

	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	D3DSURFACE_DESC desc;
	texture->GetLevelDesc(0,&desc);

	static nMatrix4 matScal,matTran,matIdent;
	D3DXMatrixScaling(&matScal,(float)rect.GetWidth()/(float)desc.Width,(float)rect.GetHeight()/(float)desc.Height,1.0f);
	D3DXMatrixTranslation(&matTran,(float)rect.left,(float)rect.top,0.0f);

	matScal *= matTran;

	m_pSprite->SetTransform(&matScal);

	m_pSprite->Draw(texture,NULL,NULL,NULL,color);

	D3DXMatrixIdentity(&matIdent);
	m_pSprite->SetTransform(&matIdent);
}

void nGraphics::DrawShadowText(ID3DXFont* font,const nRect& rect,const char* text,unsigned long flags,const nColor& color)
{
	nAssert(font);

	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	// Draw the text
	static nRect rect2;
	static nColor color2;

	rect2 = rect;
	rect2.Offset(1,1);

	color2.r = color2.g = color2.b = 0.0f;
	color2.a = color.a * 0.1f;

	DrawText(font,rect2,text,flags,color2);
	DrawText(font,rect,text,flags,color);
}

void nGraphics::DrawText(ID3DXFont* font,const nRect& rect,const char* text,unsigned long flags,const nColor& color)
{
	nAssert(font);

	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	// Draw the text
	font->DrawText(m_pSprite,text,-1,(LPRECT)&rect,flags,color);
}

void nGraphics::DrawPrintLine(ID3DXFont* font,const nRect& rect,const char* text,float delta,unsigned long flags,const nColor& color)
{
	nAssert(font);

	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	nString textString(text);
	nColor color2(color);
	
	if(delta < 0.0f)
		color2.a = 0.0f;
	else if(delta < 1.0f)
		color2.a *= cubicf(0.0f,1.0f,delta);
	else if(delta > textString.size())
		color2.a = 0.0f;
	else if(delta > textString.size() - 1.0f)
		color2.a *= cubicf(0.0f,1.0f,textString.size() - min(delta,textString.size()));

	unsigned long numChars = min(max(0.0f,delta),textString.size());
	numChars = textString.size() * cubicf(0.0f,1.0f,(float)numChars/(float)textString.size());	// Comment-out for no cubic interpolation
	textString.erase(textString.end() - textString.size() + numChars,textString.end());

	nSize size;
	GetTextSize(font,textString.c_str(),&size,NULL);

	nSize space;
	GetTextSize(font," ",&space,NULL);
	space.cx = 8;

	// Draw the text
	DrawText(font,rect,textString.c_str(),flags,color);

	// Draw block
	DrawFillRect(nRect(rect.left + size.cx,rect.top,rect.left + size.cx + space.cx,rect.top + space.cy),color2);
}

void nGraphics::DrawPrintShadowLine(ID3DXFont* font,const nRect& rect,const char* text,float delta,unsigned long flags,const nColor& color)
{
	nAssert(font);

	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	nString textString(text);
	nColor color2(color);
	
	if(delta < 0.0f)
		color2.a = 0.0f;
	else if(delta < 1.0f)
		color2.a *= cubicf(0.0f,1.0f,delta);
	else if(delta > textString.size())
		color2.a = 0.0f;
	else if(delta > textString.size() - 1.0f)
		color2.a *= cubicf(0.0f,1.0f,textString.size() - min(delta,textString.size()));

	unsigned long numChars = min(max(0.0f,delta),textString.size());
	numChars = textString.size() * cubicf(0.0f,1.0f,(float)numChars/(float)textString.size());	// Comment-out for no cubic interpolation
	textString.erase(textString.end() - textString.size() + numChars,textString.end());

	nSize size;
	GetTextSize(font,textString.c_str(),&size,NULL);

	nSize space;
	GetTextSize(font," ",&space,NULL);
	space.cx = 8;

	// Draw the text
	DrawShadowText(font,rect,textString.c_str(),flags,color);
	
	// Draw block
	DrawShadowFillRect(nRect(rect.left + size.cx,rect.top,rect.left + size.cx + space.cx,rect.top + space.cy),color2);
}

void nGraphics::GetTextRect(ID3DXFont* font,const char* text,nRect* rect,unsigned long flags)
{
	nAssert(font);

	font->DrawText(m_pSprite,text,-1,(LPRECT)rect,flags|DT_CALCRECT,0xFFFFFFFF);
}

void nGraphics::GetTextSize(ID3DXFont* font,const char* text,nSize* size,unsigned long flags)
{
	UNREFERENCED_PARAMETER(flags);

	nAssert(font);

	GetTextExtentPoint(font->GetDC(),text,strlen(text),(LPSIZE)size);
}

bool nGraphics::SpriteBegin()
{
	m_pStateBlocks[0]->Capture();

	// Set-up for 2D drawing
    m_pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    m_pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    m_pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    m_pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

    m_pDevice->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_SELECTARG2);
    m_pDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_DIFFUSE);

    m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
    m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);

    m_pDevice->SetVertexShader(NULL);
    m_pDevice->SetPixelShader(NULL);

    m_pDevice->SetTextureStageState(0,D3DTSS_COLOROP, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(0,D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0,D3DTSS_COLORARG2, D3DTA_DIFFUSE);
    
    m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP, D3DTOP_MODULATE);
    m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
    m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

    m_pDevice->SetSamplerState(0,D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE);

	if(FAILED(m_pSprite->Begin(D3DXSPRITE_DONOTSAVESTATE)))
	{
		nMainFrame::LastError("Failed to begin sprite drawing.");
		Trace(__FUNCTION__" Failed to begin sprite drawing.\n");
		return false;
	}

	return true;
}

void nGraphics::SpriteEnd()
{
	nAssert(m_pSprite);

	m_pSprite->End();

	m_pStateBlocks[0]->Apply();
}

void nGraphics::InvalidateDeviceObjects()
{
	//
	// TODO Add any device object invalidation here
	//

	CleanTextures();

	m_pSprite->OnLostDevice();
	m_pLine->OnLostDevice();
	m_pFontBold->OnLostDevice();
	m_pFontNormal->OnLostDevice();
}

void nGraphics::RestoreDeviceObjects()
{
	//
	// TODO Add any device object restoration here
	//

	InitTextures();

	m_pSprite->OnResetDevice();
	m_pLine->OnResetDevice();
	m_pFontBold->OnResetDevice();
	m_pFontNormal->OnResetDevice();
}

void nGraphics::SetClipper(const nRect* rect)
{
	nAssert(m_pDevice);

	if(rect)
	{
		m_pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE,TRUE);
		m_pDevice->SetScissorRect((LPRECT)rect);
	}
	else
		m_pDevice->SetRenderState(D3DRS_SCISSORTESTENABLE,FALSE);
}

bool nGraphics::GetClipper(nRect* rect)
{
	nAssert(m_pDevice);

	unsigned long val = 0;
	m_pDevice->GetRenderState(D3DRS_SCISSORTESTENABLE,&val);
	
	if(rect)
		m_pDevice->GetScissorRect((LPRECT)rect);

	return val ? true : false;
}

void nGraphics::SpriteFlush()
{
	nAssert(m_pSprite);

	m_pSprite->Flush();
}

void nGraphics::DrawFillRect(const nRect& rect,const nColor& color)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	// Since we are not using per-vertex color modulation (all vertices have the same
	// color) we can safely clip the rect to the screen rect
	nRect localRect = rect;
	localRect.Intersect(nRect(m_PresentParameters.BackBufferWidth,m_PresentParameters.BackBufferHeight));

	if(localRect.IsEmpty())
		return;

	// Have to flush any currently buffered sprite data or we won't have proper
	// Z positioning
	this->SpriteFlush();

	n2DVertex vertices[4] =
	{
		(float) rect.left -0.5f,  (float) rect.top -0.5f,    0.5f, 1.0f, color,
		(float) rect.right -0.5f, (float) rect.top -0.5f,    0.5f, 1.0f, color,
		(float) rect.right -0.5f, (float) rect.bottom -0.5f, 0.5f, 1.0f, color,
		(float) rect.left -0.5f,  (float) rect.bottom -0.5f, 0.5f, 1.0f, color,
    };

	m_pSprite->Flush();
    IDirect3DVertexDeclaration9 *pDecl = NULL;
	m_pDevice->GetVertexDeclaration(&pDecl);  // Preserve the sprite's current vertex decl
	m_pDevice->SetFVF(n2DVertex::FVF);

    m_pDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG2);
    m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG2);

    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,vertices,sizeof(n2DVertex));

	m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

    // Restore the vertex decl
    m_pDevice->SetVertexDeclaration(pDecl);
	nSafeRelease(pDecl);
}

void nGraphics::DrawFillRect(const nRect& rect,const nColor& colorTl,const nColor& colorTr,const nColor& colorBr,const nColor& colorBl)
{
	// Don't draw if fully transparent
	if(colorTl.a <= 0.0f && colorTr.a <= 0.0f && colorBr.a <= 0.0f && colorBl.a <= 0.0f)
		return;

	this->SpriteFlush();

	n2DVertex vertices[4] =
	{
		(float) rect.left -0.5f,  (float) rect.top -0.5f,    0.5f, 1.0f, colorTl,
		(float) rect.right -0.5f, (float) rect.top -0.5f,    0.5f, 1.0f, colorTr,
		(float) rect.right -0.5f, (float) rect.bottom -0.5f, 0.5f, 1.0f, colorBr,
		(float) rect.left -0.5f,  (float) rect.bottom -0.5f, 0.5f, 1.0f, colorBl,
    };

    IDirect3DVertexDeclaration9 *pDecl = NULL;
	m_pDevice->GetVertexDeclaration(&pDecl);  // Preserve the sprite's current vertex decl
	m_pDevice->SetFVF(n2DVertex::FVF);

    m_pDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_SELECTARG2);
    m_pDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG2);

    m_pDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN,2,vertices,sizeof(n2DVertex));

	m_pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	m_pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

    // Restore the vertex decl
    m_pDevice->SetVertexDeclaration(pDecl);
	nSafeRelease(pDecl);
}

void nGraphics::DrawLines(const nVector2* lines,unsigned long count,const nColor& color)
{
	nAssert(m_pLine);

	m_pLine->Draw((LPD3DXVECTOR2)lines,count,color);
}

void nGraphics::DrawCircle(const nPoint& point,float radius,unsigned long elements,const nColor& color)
{
	nAssert(elements);

	nVector2* lines = new nVector2[elements];

	for(unsigned long i = 0; i < elements; i++)
		lines[i] = nVector2(point.x + radius * sinf(D3DX_PI*2/(elements-1) * i),point.y + radius * cosf(D3DX_PI*2/(elements-1) * i));

	DrawLines(lines,elements,color);

	nSafeFree(lines);
}

void nGraphics::DrawTarget(const nPoint& point,float time,const nColor& color)
{
	static const float sizeA = 80.0f * 4;
	static const float sizeB = 40.0f * 4;
	static const float sizeC = 2.5f;

	nVector2 line[2];

	if(time > 0.0f)
	{
		nMatrix4 matRot,matTrans,matFinal;

		D3DXMatrixRotationZ(&matRot,D3DX_PI/4 + D3DX_PI * (1.0f - time));
		D3DXMatrixTranslation(&matTrans,point.x,point.y,0.0f);

		matFinal = matRot * matTrans;

		SetLineWidth(3);

		line[0] = nVector2(-sizeA * time,-sizeB * time);
		line[1] = nVector2(-sizeA * time,sizeB * time);

		D3DXVec2TransformCoordArray((LPD3DXVECTOR2)line,sizeof(nVector2),(LPD3DXVECTOR2)line,sizeof(nVector2),&matFinal,2);

		DrawLines(line,2,color);

		line[0] = nVector2(sizeA * time,-sizeB * time);
		line[1] = nVector2(sizeA * time,sizeB * time);

		D3DXVec2TransformCoordArray((LPD3DXVECTOR2)line,sizeof(nVector2),(LPD3DXVECTOR2)line,sizeof(nVector2),&matFinal,2);

		DrawLines(line,2,color);

		line[0] = nVector2(-sizeB * time,-sizeA * time);
		line[1] = nVector2(sizeB * time,-sizeA * time);

		D3DXVec2TransformCoordArray((LPD3DXVECTOR2)line,sizeof(nVector2),(LPD3DXVECTOR2)line,sizeof(nVector2),&matFinal,2);

		DrawLines(line,2,color);

		line[0] = nVector2(-sizeB * time,sizeA * time);
		line[1] = nVector2(sizeB * time,sizeA * time);

		D3DXVec2TransformCoordArray((LPD3DXVECTOR2)line,sizeof(nVector2),(LPD3DXVECTOR2)line,sizeof(nVector2),&matFinal,2);

		DrawLines(line,2,color);
	}
	else
	{
		SetLineWidth(1);

		line[0] = nVector2(point.x - sizeC,point.y - sizeC);
		line[1] = nVector2(point.x + sizeC,point.y + sizeC);

		DrawLines(line,2,color);

		line[0] = nVector2(point.x + sizeC,point.y - sizeC);
		line[1] = nVector2(point.x - sizeC,point.y + sizeC);

		DrawLines(line,2,color);
	}
}

void nGraphics::DrawCursor(const nVector2& position,float internal,float external,float angle,const nColor& color)
{
	SetLineWidth(2);

	nMatrix4 matRot,matTrans,matFinal;

	D3DXMatrixRotationZ(&matRot,angle);
	D3DXMatrixTranslation(&matTrans,position.x,position.y,0.0f);

	matFinal = matRot * matTrans;

	nVector2 lines[2];

	lines[0] = nVector2(internal,internal);
	lines[1] = nVector2(external,external);

	D3DXVec2TransformCoordArray((LPD3DXVECTOR2)lines,sizeof(nVector2),(LPD3DXVECTOR2)lines,sizeof(nVector2),&matFinal,2);

	DrawLines(lines,2,color);

	lines[0] = nVector2(-internal,internal);
	lines[1] = nVector2(-external,external);

	D3DXVec2TransformCoordArray((LPD3DXVECTOR2)lines,sizeof(nVector2),(LPD3DXVECTOR2)lines,sizeof(nVector2),&matFinal,2);

	DrawLines(lines,2,color);

	lines[0] = nVector2(internal,-internal);
	lines[1] = nVector2(external,-external);

	D3DXVec2TransformCoordArray((LPD3DXVECTOR2)lines,sizeof(nVector2),(LPD3DXVECTOR2)lines,sizeof(nVector2),&matFinal,2);

	DrawLines(lines,2,color);

	lines[0] = nVector2(-internal,-internal);
	lines[1] = nVector2(-external,-external);

	D3DXVec2TransformCoordArray((LPD3DXVECTOR2)lines,sizeof(nVector2),(LPD3DXVECTOR2)lines,sizeof(nVector2),&matFinal,2);

	DrawLines(lines,2,color);
}

void nGraphics::DrawTextureTransition(IDirect3DTexture9* texture,const nPoint& point,const nColor& color,TransitionEffect effect,double time)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	D3DSURFACE_DESC desc;
	texture->GetLevelDesc(0,&desc);
	DrawTextureTransition(texture,nRect(point.x,point.y,point.x+desc.Width,point.y+desc.Height),color,effect,time);
}

void nGraphics::DrawTextureTransition(IDirect3DTexture9* texture,const nRect& rect,const nColor& color,TransitionEffect effect,double time)
{
	// Don't draw if fully transparent
	if(color.a <= 0.0f)
		return;

	switch(effect)
	{
	case TransitionEffectTVHorizontal:
		{
			float width = time*4.0f;
			float height = time*4.0f;

			width -= 1.0f;

			width = min(1.0f,max(0.0f,width));
			height = min(1.0f,max(0.0f,height));

			width += 0.005f;
			height += 0.005f;

			width = min(1.0f,max(0.0f,width));
			height = min(1.0f,max(0.0f,height));

			nRect rectOut;
			rectOut.left = rect.left + rect.GetWidth()/2-width*rect.GetWidth()/2;
			rectOut.top = rect.top + rect.GetHeight()/2-height*rect.GetHeight()/2;
			rectOut.right = rectOut.left + width*rect.GetWidth();
			rectOut.bottom = rectOut.top + height*rect.GetHeight();

			DrawTexture(texture,rectOut,color);
		}
		break;

	case TransitionEffectTVVertical:
		{
			float width = time*4.0f;
			float height = time*4.0f;

			height -= 1.0f;

			width = min(1.0f,max(0.0f,width));
			height = min(1.0f,max(0.0f,height));

			width += 0.005f;
			height += 0.005f;

			width = min(1.0f,max(0.0f,width));
			height = min(1.0f,max(0.0f,height));

			nRect rectOut;
			rectOut.left = rect.left + rect.GetWidth()/2-width*rect.GetWidth()/2;
			rectOut.top = rect.top + rect.GetHeight()/2-height*rect.GetHeight()/2;
			rectOut.right = rectOut.left + width*rect.GetWidth();
			rectOut.bottom = rectOut.top + height*rect.GetHeight();

			DrawTexture(texture,rectOut,color);
		}
		break;

	case TransitionEffectTVCenter:
		{
			float width = time*4.0f;
			float height = time*4.0f;

			width = min(1.0f,max(0.0f,width));
			height = min(1.0f,max(0.0f,height));

			width += 0.005f;
			height += 0.005f;

			width = min(1.0f,max(0.0f,width));
			height = min(1.0f,max(0.0f,height));

			nRect rectOut;
			rectOut.left = rect.left + rect.GetWidth()/2-width*rect.GetWidth()/2;
			rectOut.top = rect.top + rect.GetHeight()/2-height*rect.GetHeight()/2;
			rectOut.right = rectOut.left + width*rect.GetWidth();
			rectOut.bottom = rectOut.top + height*rect.GetHeight();

			DrawTexture(texture,rectOut,color);
		}
		break;

	case TransitionEffectStretchLeft:
		{
			float delta = time*4.0f;

			delta = min(1.0f,max(0.0f,delta));

			nRect rectOut;
			rectOut.left = rect.left;
			rectOut.top = rect.top;
			rectOut.right = rect.left + delta*rect.GetWidth();
			rectOut.bottom = rect.bottom;

			DrawTexture(texture,rectOut,color);
		}
		break;

	case TransitionEffectStretchTop:
		{
			float delta = time*4.0f;

			delta = min(1.0f,max(0.0f,delta));

			nRect rectOut;
			rectOut.left = rect.left;
			rectOut.top = rect.top;
			rectOut.right = rect.right;
			rectOut.bottom = rect.top + delta*rect.GetHeight();

			DrawTexture(texture,rectOut,color);
		}
		break;

	case TransitionEffectStretchRight:
		{
			float delta = time*4.0f;

			delta = min(1.0f,max(0.0f,delta));

			nRect rectOut;
			rectOut.left = rect.right - delta*rect.GetWidth();
			rectOut.top = rect.top;
			rectOut.right = rect.right;
			rectOut.bottom = rect.bottom;

			DrawTexture(texture,rectOut,color);
		}
		break;

	case TransitionEffectStretchBottom:
		{
			float delta = time*4.0f;

			delta = min(1.0f,max(0.0f,delta));

			nRect rectOut;
			rectOut.left = rect.left;
			rectOut.top = rect.bottom - delta*rect.GetHeight();
			rectOut.right = rect.right;
			rectOut.bottom = rect.bottom;

			DrawTexture(texture,rectOut,color);
		}
		break;
	}
}