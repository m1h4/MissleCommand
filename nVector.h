// Copyright 2003/2005 Miha Software
// 
// 31.10.2004 Marko
// - Changed compare operators from "if(m[0] != other.m[0])" to "return m[0] == other.m[0] && m[1] == other ..."

#pragma once

class nVector3
{
public:
	nVector3() { this->SetEmpty(); }
	nVector3(float x,float y,float z) { this->Set(x,y,z); }
	nVector3(float xyz) { this->Set(xyz); }
	nVector3(const float* m) { this->Set(m); }
	nVector3(const nVector3& other) { this->Set(other); }

	void Set(float x,float y,float z) { this->x = x; this->y = y; this->z = z; }
	void Set(float xyz) { this->x = this->y = this->z = xyz; }
	void Set(const float* m) { this->m[0] = m[0]; this->m[1] = m[1]; this->m[2] = m[2]; }
	void Set(const nVector3& other) { this->x = other.x; this->y = other.y; this->z = other.z; }
	void SetEmpty() { this->x = this->y = this->z = 0.0f; }
	bool IsEmpty() const { return x == 0.0f && y == 0.0f && z == 0.0f; }

	float Length() { return sqrtf(x*x+y*y+z*z); }
	void Normalize() { float l = this->Length(); x /= l; y /= l; z /= l; }
	void Lerp(const nVector3& dest,float s) { this->x = lerpf(this->x,dest.x,s); this->y = lerpf(this->y,dest.y,s); this->z = lerpf(this->z,dest.z,s); }

    // Operators
	nVector3 operator + (const nVector3& other) const { return nVector3(x+other.x,y+other.y,z+other.z); }
	nVector3 operator - (const nVector3& other) const { return nVector3(x-other.x,y-other.y,z-other.z); }
	nVector3 operator * (const nVector3& other) const { return nVector3(x*other.x,y*other.y,z*other.z); }
	nVector3 operator / (const nVector3& other) const { return nVector3(x/other.x,y/other.y,z/other.z); }

	nVector3 operator + (float scalar) const { return nVector3(x+scalar,y+scalar,z+scalar); }
	nVector3 operator - (float scalar) const { return nVector3(x-scalar,y-scalar,z-scalar); }
	nVector3 operator * (float scalar) const { return nVector3(x*scalar,y*scalar,z*scalar); }
	nVector3 operator / (float scalar) const { return nVector3(x/scalar,y/scalar,z/scalar); }
    
	nVector3& operator += (const nVector3& other) { x += other.x; y += other.y; z += other.z; return *this; }
	nVector3& operator -= (const nVector3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
	nVector3& operator *= (const nVector3& other) { x *= other.x; y *= other.y; z *= other.z; return *this; }
	nVector3& operator /= (const nVector3& other) { x /= other.x; y /= other.y; z /= other.z; return *this; }

	nVector3& operator += (float scalar) { x += scalar; y += scalar; z += scalar; return *this; }
	nVector3& operator -= (float scalar) { x -= scalar; y -= scalar; z -= scalar; return *this; }
	nVector3& operator *= (float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
	nVector3& operator /= (float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

	friend nVector3 operator * (float,const nVector3&);

	nVector3 operator-() const { return nVector3(-x,-y,-z); }

	bool operator == (const nVector3& other) const { return x == other.x && y == other.y && z == other.z; }
	bool operator != (const nVector3& other) const { return x != other.x && y != other.y && z != other.z; }

	// Float array conversion
	operator float*() { return m; }
	operator const float*() const { return m; }

public:
	union 
	{
        struct 
		{
        	float x, y, z;		// Vector coordinates
        };
        float m[3];				// Array of vector coordinates
    };
};

class nVector2
{
public:
	nVector2() { this->SetEmpty(); }
	nVector2(float x,float y) { this->Set(x,y); }
	nVector2(float xy) { this->Set(xy); }
	nVector2(const float* m) { this->Set(m); }
	nVector2(const nVector2& other) { this->Set(other); }

	void Set(float x,float y) { this->x = x; this->y = y; }
	void Set(float xy) { this->x = this->y = xy; }
	void Set(const float* m) { this->m[0] = m[0]; this->m[1] = m[1]; }
	void Set(const nVector2& other) { this->x = other.x; this->y = other.y; }
	void SetEmpty() { this->x = this->y = 0.0f; }
	bool IsEmpty() const { return x == 0.0f && y == 0.0f; }

	float Length() { return sqrtf(x*x+y*y); }
	void Normalize() { float l = this->Length(); x /= l; y /= l; }
	void Lerp(const nVector2& dest,float s) { this->x = lerpf(this->x,dest.x,s); this->y = lerpf(this->y,dest.y,s); }

    // Operators
	nVector2 operator + (const nVector2& other) const { return nVector2(x+other.x,y+other.y); }
	nVector2 operator - (const nVector2& other) const { return nVector2(x-other.x,y-other.y); }
	nVector2 operator * (const nVector2& other) const { return nVector2(x*other.x,y*other.y); }
	nVector2 operator / (const nVector2& other) const { return nVector2(x/other.x,y/other.y); }

	nVector2 operator + (float scalar) const { return nVector2(x+scalar,y+scalar); }
	nVector2 operator - (float scalar) const { return nVector2(x-scalar,y-scalar); }
	nVector2 operator * (float scalar) const { return nVector2(x*scalar,y*scalar); }
	nVector2 operator / (float scalar) const { return nVector2(x/scalar,y/scalar); }
    
	nVector2& operator += (const nVector2& other) { x += other.x; y += other.y; return *this; }
	nVector2& operator -= (const nVector2& other) { x -= other.x; y -= other.y; return *this; }
	nVector2& operator *= (const nVector2& other) { x *= other.x; y *= other.y; return *this; }
	nVector2& operator /= (const nVector2& other) { x /= other.x; y /= other.y; return *this; }

	nVector2& operator += (float scalar) { x += scalar; y += scalar; return *this; }
	nVector2& operator -= (float scalar) { x -= scalar; y -= scalar; return *this; }
	nVector2& operator *= (float scalar) { x *= scalar; y *= scalar; return *this; }
	nVector2& operator /= (float scalar) { x /= scalar; y /= scalar; return *this; }

	friend nVector2 operator * (float,const nVector2&);

	nVector2 operator-() const { return nVector2(-x,-y); }

	bool operator == (const nVector2& other) const { if(x != other.x) return false; if(y != other.y) return false; return true; }
	bool operator != (const nVector2& other) const { if(x != other.x) return true; if(y != other.y) return true; return false; }

	// Float array conversion
	operator float*() { return m; }
	operator const float*() const { return m; }

public:
	union 
	{
        struct 
		{
        	float x, y;		// Vector coordinates
        };
        float m[2];				// Array of vector coordinates
    };
};

// Some global functions that operate on vectors
// TODO Maybe move them to nVector3
inline float nVector3Distance(const nVector3& v1,const nVector3& v2)
{
	return (v2-v1).Length();
}

inline float nVector2Distance(const nVector2& v1,const nVector2& v2)
{
	return (v2-v1).Length();
}

inline float nVector3Dot(const nVector3& v1,const nVector3& v2)
{
	return v1.x*v2.x+v1.y*v2.y+v1.z*v2.z;
}

inline float nVector2Dot(const nVector2& v1,const nVector2& v2)
{
	return v1.x*v2.x+v1.y*v2.y;
}

inline nVector3 nVector3LinearInterpolation(const nVector3& v1,const nVector3& v2,float s)
{
	return nVector3(v1 + s * (v2 - v1));
}

inline nVector2 nVecotr2LinearInterpolation(const nVector2& v1,const nVector2& v2,float s)
{
	return nVector2(v1 + s * (v2 - v1));
}

inline nVector3 nVector3CubicInterpolation(const nVector3& v1,const nVector3& v2,float s)
{
	float s1 = 2.0f * powf(s,3.0f);
	float s2 = 3.0f * powf(s,2.0f);

	return nVector3(v1 * (s1-s2+1.0f) + v2 * (s2-s1));
}

inline nVector2 nVector2CubicInterpolation(const nVector2& v1,const nVector2& v2,float s)
{
	float s1 = 2.0f * powf(s,3.0f);
	float s2 = 3.0f * powf(s,2.0f);

	return nVector2(v1 * (s1-s2+1.0f) + v2 * (s2-s1));
}

inline nVector3 nVector3HermiteInterpolation(const nVector3& v1,const nVector3& v2,const nVector3& v3,const nVector3& v4, float s)
{
	float t2 = s * s;
	float t3 = t2 * s;
	float kp0 = (2.0f * t2 - t3 - s) / 2.0f;
	float kp1 = (3.0f * t3 - 5.0f * t2 + 2.0f) / 2.0f;
	float kp2 = (4.0f * t2 - 3.0f * t3 + s) / 2.0f;
	float kp3 = (t3 - t2) / 2.0f;

	return v1 * kp0 + v2 * kp1 + v3 * kp2 + v4 * kp3;
}

inline nVector3 nVector3CatmullRomInterpolation(const nVector3& v1,const nVector3& v2,const nVector3& v3,const nVector3& v4, float s)
{
	return ((v1 * 2.0f) + (-v2 + v3) * s + (v1 * 2.0f - v2 * 5.0f + v3 * 4.0f - v4) * s * s + ( -v1 + v2 * 3.0f - v3 * 3.0f + v4) * s * s * s) * 0.5f;
}

inline nVector3 nVector3CatmullRomTangentInterpolation(const nVector3& v1,const nVector3& v2,const nVector3& t1,const nVector3& t2, float s)
{
	float s3 = powf(s,3.0f);
	float s2 = powf(s,2.0f);

	return nVector3(v1 * (2.0f * s3 - 3.0f * s2 + 1.0f) + v2 * (3.0f * s2 - 2.0f * s3) + t1 * (s3 - 2.0f * s2 + s) + t2 * (s3 - s2));
}

inline nVector3 nVector3Cross(const nVector3& v1,const nVector3& v2)
{
	return nVector3(v1.y * v2.z - v1.z * v2.y,-v1.x * v2.z + v1.z * v2.x,v1.x * v2.y - v1.y * v2.x);
}

inline nVector3 nVector3Normal(const nVector3& v1, const nVector3& v2, const nVector3& v3)
{
	nVector3 vNormal = nVector3Cross(v3-v1,v2-v1);

	vNormal.Normalize();

	return vNormal;
}

inline nVector2 nVector2Perpendicular(const nVector2& vec)
{
	return nVector2(-vec.y,vec.x);
}

inline nVector3 nVector3FromString(const char* string)
{
	// Assumes that the passed string is in the following format: "%f %f %f"
	nVector3 out(1.0f);

	nString temp(string);
	temp.trimAll();
	if(!temp.size())
		return out;

	out.x = (float)strtod(temp.c_str(),NULL);

	if(temp.find(' ') == nString::npos)
		return out;	// Invalid string

	temp = temp.c_str() + temp.find(' ')+1;
	out.y = (float)strtod(temp.c_str(),NULL);
	
	if(temp.find(' ') == nString::npos)
		return out;	// Invalid string

	temp = temp.c_str() + temp.find(' ')+1;
	out.z = (float)strtod(temp.c_str(),NULL);
	
	return out;
}

inline nVector2 nVector2FromString(const char* string)
{
	// Assumes that the passed string is in the following format: "%f %f"
	nVector2 out(1.0f);

	nString temp(string);
	temp.trimAll();
	if(!temp.size())
		return out;

	out.x = (float)strtod(temp.c_str(),NULL);

	if(temp.find(' ') == nString::npos)
		return out;	// Invalid string

	temp = temp.c_str() + temp.find(' ')+1;
	out.y = (float)strtod(temp.c_str(),NULL);
	
	return out;
}