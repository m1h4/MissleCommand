// Copyright 2004/2006 Miha Software

#pragma once

class nColor
{
public:
	nColor() { this->SetEmpty(); }
	nColor(float a,float r,float g,float b) { this->Set(a,r,g,b); }
	nColor(float r,float g,float b) { this->Set(r,g,b); }
	nColor(float a,float value) { this->Set(a,value); }
	nColor(float value) { this->Set(value); }
	nColor(const nColor& other) { this->Set(other); }
	nColor(float a,const nColor& other) { this->Set(a,other); }

	void Set(float a,float r,float g,float b) { this->a = min(max(a,0.0f),1.0f); this->r = min(max(r,0.0f),1.0f); this->g = min(max(g,0.0f),1.0f); this->b = min(max(b,0.0f),1.0f); }
	void Set(float r,float g,float b) { this->a = 1.0f; this->r = min(max(r,0.0f),1.0f); this->g = min(max(g,0.0f),1.0f); this->b = min(max(b,0.0f),1.0f); }
	void Set(float a,float value) { this->a = min(max(a,0.0f),1.0f); this->r = this->g = this->b = min(max(value,0.0f),1.0f); }
	void Set(float value)  { this->a = this->r = this->g = this->b = min(max(value,0.0f),1.0f); }
	void Set(const nColor& other) { this->a = other.a; this->r = other.r; this->g = other.g; this->b = other.b; }
	void Set(float a,const nColor& other) { this->a = min(max(a,0.0f),1.0f); this->r = other.r; this->g = other.g; this->b = other.b; }
	void SetEmpty() { a = r = g = b = 0.0f; }
	bool IsEmpty() { if(a != 0.0f) return false; if(r != 0.0f) return false; if(g != 0.0f) return false; if(b != 0.0f) return false; return true; }

	bool operator==(const nColor& other) const { if(a != other.a) return false; if(r != other.r) return false; if(g != other.g) return false; if(b != other.b) return false; return true; }
	bool operator!=(const nColor& other) const { if(a == other.a) return false; if(r == other.r) return false; if(g == other.g) return false; if(b == other.b) return false; return true; }

	void Lerp(const nColor& other,float s) { a = lerpf(a,other.a,s); r = lerpf(r,other.r,s); g = lerpf(g,other.g,s); b = lerpf(b,other.b,s); }
	void Cubic(const nColor& other,float s) { a = cubicf(a,other.a,s); r = cubicf(r,other.r,s); g = cubicf(g,other.g,s); b = cubicf(b,other.b,s); }
	
	// Lerp between white, not changing the alpha
	void Bright(float s) { r = lerpf(r,1.0f,s); g = lerpf(g,1.0f,s); b = lerpf(b,1.0f,s); }

	// Lerp between black, not changing the alpha
	void Dark(float s) { r = lerpf(r,0.0f,s); g = lerpf(g,0.0f,s); b = lerpf(b,0.0f,s); }
	
	// Lerp between grayscale, not changing the alpha
	void Saturation(float s) { float v = (r+g+b)/3.0f; r = lerpf(r,v,s); g = lerpf(g,v,s); b = lerpf(b,v,s); }

	// Converts the color to negative, not changing the alpha
	void Negative() { r = 1.0f-r; g = 1.0f-g; b = 1.0f-b; }

	// Converts the color to grayscale, not changing the alpha
	void Grayscale() { float v = (r+g+b)/3.0f; r = v; g = v; b = v; }

	// Converts the color to a shadow color
	void Shadow() { a *= 0.1f; r = g = b = 0.0f; }

	// The next line is a pure beauty ;)
	operator unsigned long() const { return (unsigned long)(((((unsigned long)((a)*255.0f))&0xFF)<<24)|((((unsigned long)((r)*255.0f))&0xFF)<<16)|((((unsigned long)((g)*255.0f))&0xFF)<<8)|(((unsigned long)((b)*255.0f))&0xFF)); }

	nColor operator+(const nColor& other) const { return nColor(a+other.a,r+other.r,g+other.g,b+other.b); }
	nColor operator-(const nColor& other) const { return nColor(a-other.a,r-other.r,g-other.g,b-other.b); }

	nColor& operator+=(const nColor& other) { this->Set(a+other.a,r+other.r,g+other.g,b+other.b); return *this; }
	nColor& operator-=(const nColor& other) { this->Set(a-other.a,r-other.r,g-other.g,b-other.b); return *this; }

	nColor operator*(const float scalar) const { return nColor(a*scalar,r*scalar,g*scalar,b*scalar); }
	nColor operator/(const float scalar) const { return nColor(a/scalar,r/scalar,g/scalar,b/scalar); }

	nColor& operator*=(const float scalar) { this->Set(a*scalar,r*scalar,g*scalar,b*scalar); return *this; }
	nColor& operator/=(const float scalar) { this->Set(a/scalar,r/scalar,g/scalar,b/scalar); return *this; }

	// Modulation (Blending)
	nColor operator*(const nColor& other) const { return nColor(a*other.a,r*other.r,g*other.g,b*other.b); }
	nColor operator*=(const nColor& other) { this->Set(a*other.a,r*other.r,g*other.g,b*other.b); return *this; }
public:
	float r,g,b,a;
};

inline nColor ColorFromString(const char* string)
{
	// Assumes that the passed string is in the following format: "%f %f %f %f"
	nColor out(1.0f);

	nString temp(string);
	temp.trimAll();
	if(!temp.size())
		return out;

	// Check if 4 parameters defined (should be more than 2 spaces), if not don't load the alpha
	if(temp.findTimes(" ") > 2)
	{
		out.a = min(max((float)strtod(temp.c_str(),NULL),0.0f),1.0f);

		if(temp.find(L' ') == nString::npos)
			return out;	// Invalid string

		temp = temp.c_str() + temp.find(L' ')+1;
	}

	out.r = min(max((float)strtod(temp.c_str(),NULL),0.0f),1.0f);
	
	if(temp.find(L' ') == nString::npos)
		return out;	// Invalid string

	temp = temp.c_str() + temp.find(L' ')+1;
	out.g = min(max((float)strtod(temp.c_str(),NULL),0.0f),1.0f);
	
	if(temp.find(L' ') == nString::npos)
		return out;	// Invalid string

	temp = temp.c_str() + temp.find(L' ')+1;
	out.b = min(max((float)strtod(temp.c_str(),NULL),0.0f),1.0f);

	return out;
}