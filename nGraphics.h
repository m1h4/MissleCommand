// Copyright 2004/2006 Miha Software

#pragma once

#include "nColor.h"

class nGraphics
{
public:
	nGraphics();
	~nGraphics();

	// DrawRect flags
	enum RectFlag
	{
		RectFlagTop = 1,
		RectFlagLeft = 2,
		RectFlagRight = 4,
		RectFlagBottom = 8,
		RectFlagAll = 15,
	};

	// DrawTextureTransition flags
	enum TransitionEffect
	{
		TransitionEffectTVHorizontal,
		TransitionEffectTVVertical,
		TransitionEffectTVCenter,
		TransitionEffectStretchLeft,
		TransitionEffectStretchTop,
		TransitionEffectStretchRight,
		TransitionEffectStretchBottom,
	};

	bool InitGraphics();
	bool InitTextures();
	void CleanGraphics();
	void CleanTextures();

	bool ValidateDevice();

	void Render();

	bool SpriteBegin();
	void SpriteEnd();
	void SpriteFlush();

	void InvalidateDeviceObjects();
	void RestoreDeviceObjects();

	bool GetClipper(nRect* rect);
	void SetClipper(const nRect* rect);

	void DrawShadowText(ID3DXFont* font,const nRect& rect,const char* text,unsigned long flags,const nColor& color);
	void DrawText(ID3DXFont* font,const nRect& rect,const char* text,unsigned long flags,const nColor& color);
	
	void DrawPrintLine(ID3DXFont* font,const nRect& rect,const char* text,float delta,unsigned long flags,const nColor& color);
	void DrawPrintShadowLine(ID3DXFont* font,const nRect& rect,const char* text,float delta,unsigned long flags,const nColor& color);

	void DrawTexture(IDirect3DTexture9* texture,const nPoint& point,const nColor& color);
	void DrawTexture(IDirect3DTexture9* texture,const nRect& rect,const nColor& color);

	void DrawTextureTransition(IDirect3DTexture9* texture,const nPoint& point,const nColor& color,TransitionEffect effect,double time);
	void DrawTextureTransition(IDirect3DTexture9* texture,const nRect& rect,const nColor& color,TransitionEffect effect,double time);
		
	void DrawFillRect(const nRect& rect,const nColor& colorTl,const nColor& colorTr,const nColor& colorBr,const nColor& colorBl);
	void DrawFillRect(const nRect& rect,const nColor& color);
	void DrawShadowFillRect(const nRect& rect,const nColor& color);
	
	void DrawRect(const nRect& rect,const nColor& color,unsigned char flags = RectFlagAll) { this->DrawRect(rect,1.0f,color,flags); };
	void DrawRect(const nRect& rect,float width,const nColor& color,unsigned char flags = RectFlagAll);
	void DrawShadowRect(const nRect& rect,const nColor& color,unsigned char flags = RectFlagAll) { this->DrawShadowRect(rect,1.0f,color,flags); }
	void DrawShadowRect(const nRect& rect,float width,const nColor& color,unsigned char flags = RectFlagAll);

	void DrawCornerRect(const nRect& rect,float size,const nColor& color,unsigned char flags = RectFlagAll) { this->DrawCornerRect(rect,size,1.0f,color,flags); }
	void DrawCornerRect(const nRect& rect,float size,float width,const nColor& color,unsigned char flags = RectFlagAll);
	void DrawShadowCornerRect(const nRect& rect,float size,const nColor& color,unsigned char flags = RectFlagAll) { this->DrawShadowCornerRect(rect,size,1.0f,color,flags); }
	void DrawShadowCornerRect(const nRect& rect,float size,float width,const nColor& color,unsigned char flags = RectFlagAll);

	void DrawCenterRect(const nRect& rect,float size,const nColor& color,unsigned char flags = RectFlagAll) { this->DrawCenterRect(rect,size,1.0f,color,flags); }
	void DrawCenterRect(const nRect& rect,float size,float width,const nColor& color,unsigned char flags = RectFlagAll);
	void DrawShadowCenterRect(const nRect& rect,float size,const nColor& color,unsigned char flags = RectFlagAll) { this->DrawShadowCenterRect(rect,size,1.0f,color,flags); }
	void DrawShadowCenterRect(const nRect& rect,float size,float width,const nColor& color,unsigned char flags = RectFlagAll);

	void DrawLines(const nVector2* lines,unsigned long count,const nColor& color);

	void DrawCircle(const nPoint& point,float radius,unsigned long elements,const nColor& color);

	void DrawTarget(const nPoint& point,float time,const nColor& color);
	void DrawCursor(const nVector2& position,float internal,float external,float angle,const nColor& color);
	
	void GetTextRect(ID3DXFont* font,const char* text,nRect* rect,unsigned long flags);
	void GetTextSize(ID3DXFont* font,const char* text,nSize* size,unsigned long flags);

	static void DrawLoadScreen(HWND hWnd,const char* initmodule,unsigned long pos);

	void DrawStatusText();

	bool EnableAnisotropic();

	void SetLineWidth(float width) { m_pLine->SetWidth(width); }
	float GetLineWidth() const { m_pLine->GetWidth(); }

	void SetLineAntialias(bool antialias) { m_pLine->SetAntialias(antialias); }
	bool GetLineAntialias() const { return m_pLine->GetAntialias(); }

	bool GetFullscreen() const { return m_Fullscreen; }

	ID3DXFont* GetNormalFont() { return m_pFontNormal; }
	ID3DXFont* GetBoldFont() { return m_pFontBold; }

	long GetNormalFontHeight() { return m_NormalFontHeight; }
	long GetBoldFontHeight() { return m_BoldFontHeight; }

	void SetDrawFps(bool draw) { m_DrawFps = draw; }
	bool GetDrawFps() const { return m_DrawFps; }

	double GetFps() const { return m_Fps; }

	nSize GetDisplaySize() { return nSize(m_PresentParameters.BackBufferWidth,m_PresentParameters.BackBufferHeight); }

protected:
	// Our interfaces
	IDirect3D9*					m_pDirect3D;			// Our main direct3d9 object
	IDirect3DDevice9*			m_pDevice;				// Our primary direct3d9 device
	
	ID3DXSprite*				m_pSprite;				// Our object for drawing sprites
	ID3DXLine*					m_pLine;				// Our antialiased line drawing object
	ID3DXFont*					m_pFontNormal;			// Our text drawing object
	ID3DXFont*					m_pFontBold;			// Our thin text drawing object
	
	IDirect3DStateBlock9*		m_pStateBlocks[2];		// Our state capturing blocks

	long						m_NormalFontHeight;		// Height of the normal font
	long						m_BoldFontHeight;		// Height of the bold font

	double						m_Fps;					// Frames per second

	bool						m_Fullscreen;			// Are we in fullscreen mode
	nPresentParameters			m_PresentParameters;	// The current display mode

	bool						m_DrawFps;
};
