// Copyright 2004/2006 Miha Software

#pragma once

class nPoint
{
public:
	nPoint() { this->SetEmpty(); }
	nPoint(long x,long y) { this->Set(x,y); }
	nPoint(long at) { this->Set(at); }
	nPoint(const nPoint& point) { this->Set(point); }

	void Set(long x,long y) { this->x = x; this->y = y; }
	void Set(long at) { x = at; y = at; }
	void Set(const nPoint& point) { *this = point; }
	void SetEmpty() { x = y = 0; }
	bool IsEmpty() const { if(x != 0) return false; if(y != 0) return false; return true; }

	void Offset(long dx,long dy) { x += dx; y += dy; }
	void Offset(long d) { this->Offset(d,d); }

	bool operator==(const nPoint& point) const { if(x != point.x) return false; if(y != point.y) return false; return true; }
	bool operator!=(const nPoint& point) const { if(x == point.x) return false; if(y == point.y) return false; return true; }

	nPoint operator+(const nPoint& point) const { return nPoint(x+point.x,y+point.y); }
	nPoint operator-(const nPoint& point) const { return nPoint(x-point.x,y-point.y); }

	nPoint& operator+=(const nPoint& point) { x += point.x; y += point.y; return *this; }
	nPoint& operator-=(const nPoint& point) { x -= point.x; y -= point.y; return *this; }

public:
	long x,y;
};

inline nPoint PointFromString(const char* string)
{
	// Assumes that the passed string is in the following format: "%i %i"
	nPoint out(0,0);

	nString temp(string);
	temp.trimAll();
	if(!temp.size())
		return out;

	out.x = (float)strtod(temp.c_str(),NULL);
	
	if(temp.find(L' ') == nString::npos)
		return out;	// Invalid string

	temp = temp.c_str() + temp.find(L' ')+1;
	out.y = (float)strtod(temp.c_str(),NULL);

	return out;
}