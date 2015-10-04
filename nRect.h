// Copyright 2004/2006 Miha Software

#pragma once

#include "nPoint.h"
#include "nSize.h"

class nRect
{
public:
	nRect() { this->SetEmpty(); }
	nRect(long left,long top,long right,long bottom) { this->Set(left,top,right,bottom); }
	nRect(long width,long height) { this->Set(width,height); }
	nRect(const nPoint& topLeft,const nSize& size) { this->Set(topLeft,size); }
	nRect(const nPoint& topLeft,const nPoint& bottomRight) { this->Set(topLeft,bottomRight); }
	nRect(const nRect& rect) { this->Set(rect); }

	long GetWidth() const { return right - left; }
	long GetHeight() const { return bottom - top; }

	void SetWidth(long width) { this->right = this->left + width; }
	void SetHeight(long height) { this->bottom = this->top + height; }
	void SetWidthRight(long width) { this->left = this->right - width; }
	void SetHeightRight(long height) { this->top = this->bottom - height; }

	void Set(long left,long top,long right,long bottom) { this->left = left; this->top = top; this->right = right; this->bottom = bottom; }
	void Set(long width,long height) { left = 0; top = 0; right = width; bottom = height; }
	void Set(const nPoint& topLeft,const nSize& size) { left = topLeft.x; top = topLeft.y; right = left + size.cx; bottom = top + size.cy; }
	void Set(const nPoint& topLeft,const nPoint& bottomRight) { left = topLeft.x; top = topLeft.y; right = bottomRight.x; bottom = bottomRight.y; }
	void Set(const nRect& rect) { *this = rect; }
	void SetEmpty() { left = top = right = bottom = 0; }
	bool IsEmpty() const { if(right <= left) return true; if(bottom <= top) return true; return false; }

	void Inflate(long dx,long dy) { left -= dx; right += dx; top -= dy; bottom += dy; }
	void Inflate(long d) { this->Inflate(d,d); }

	void Offset(long dx,long dy) { left += dx; right += dx; top += dy; bottom += dy; }
	void Offset(long d) { this->Offset(d,d); }

	bool PointIn(const nPoint& point) const { if(point.x < left || point.x > right) return false; if(point.y < top || point.y > bottom) return false; return true; }

	nPoint GetCenter() const { return nPoint((right+left)/2,(bottom+top)/2); }

	// TODO You should check if the result rect is valid if there is a chance that the two rects don't intersect!
	void Intersect(const nRect& rect) { left = max(left,rect.left); right = min(right,rect.right); top = max(top,rect.top); bottom = min(bottom,rect.bottom); /*return RectIsEmpty(*rect);*/ }

	bool operator==(const nRect& other) const { if(bottom != other.bottom) return false; if(left != other.left) return false; if(right != other.right) return false; if(top != other.top) return false; return true; }
	bool operator!=(const nRect& other) const { if(bottom == other.bottom) return false; if(left == other.left) return false; if(right == other.right) return false; if(top == other.top) return false; return true; }

public:
	long left,top,right,bottom;
};