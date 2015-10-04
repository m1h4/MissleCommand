// Copyright 2004/2006 Miha Software

#pragma once

class nSize
{
public:
	nSize() { this->SetEmpty(); }
	nSize(long cx,long cy) { this->Set(cx,cy); }
	nSize(long c) { this->Set(c); }
	nSize(const nSize& size) { this->Set(size); }

	void Set(long cx,long cy) { this->cx = cx; this->cy = cy; }
	void Set(long c) { this->Set(c,c); }
	void Set(const nSize& size) { *this = size; }
	void SetEmpty() { cx = cy = 0; }
	bool IsEmpty() const { if(cx <= 0) return true; if(cy <= 0) return true; return false; }

	void Expand(long dx,long dy) { cx += dx; cy += dy; }
	void Expand(long d) { this->Expand(d,d); }

	bool operator==(const nSize& size) const { if(cx != size.cx) return false; if(cy != size.cy) return false; return true; }
	bool operator!=(const nSize& size) const { if(cx == size.cx) return false; if(cy == size.cy) return false; return true; }

	nSize operator+(const nSize& size) const { return nSize(cx+size.cx,cy+size.cy); }
	nSize operator-(const nSize& size) const { return nSize(cx-size.cx,cy-size.cy); }

	nSize& operator+=(const nSize& size) { cx += size.cx; cy += size.cy; return *this; }
	nSize& operator-=(const nSize& size) { cx -= size.cx; cy -= size.cy; return *this; }

public:
	long cx,cy;
};

inline nSize SizeFromString(const char* string)
{
	// Assumes that the passed string is in the following format: "%i %i"
	nSize out(0,0);

	nString temp(string);
	temp.trimAll();
	if(!temp.size())
		return out;

	out.cx = (float)strtod(temp.c_str(),NULL);
	
	if(temp.find(L' ') == nString::npos)
		return out;	// Invalid string

	temp = temp.c_str() + temp.find(L' ')+1;
	out.cy = (float)strtod(temp.c_str(),NULL);

	return out;
}