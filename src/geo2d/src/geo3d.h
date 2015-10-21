#ifndef GEO3D_H_INCLUDED
#define GEO3D_H_INCLUDED

#include "geo2d.h"

//
// Punto tridimensionale
//
struct P3d
{
	double x,y,z;

	P3d(double x,double y,double z) : x(x),y(y),z(z) {}
	P3d() : x(0),y(0),z(0) {}

	inline P2d xy() const { return P2d(x, y); }

	inline P3d& operator+=(const P3d& o) { x+=o.x; y+=o.y; z+=o.z; return *this; }
	inline P3d& operator-=(const P3d& o) { x-=o.x; y-=o.y; z-=o.z;  return *this; }
	inline P3d& operator*=(double k)     { x*=k; y*=k; z*=k; return *this; }
	inline P3d& operator/=(double k)     { x/=k; y/=k; z/=k; return *this; }
};

inline P3d operator+(const P3d& a,const P3d& b) { return P3d(a.x+b.x, a.y+b.y, a.z+b.z); }
inline P3d operator-(const P3d& a,const P3d& b) { return P3d(a.x-b.x, a.y-b.y, a.z-b.z); }
inline P3d operator*(double k,const P3d& a)     { return P3d(a.x*k, a.y*k, a.z*k); }
inline P3d operator*(const P3d& a,double k)     { return P3d(a.x*k, a.y*k, a.z*k); }
inline P3d operator/(const P3d& a,double k)     { return (1.0/k)*a; }
inline P3d operator-(const P3d& a)       { return P3d(-a.x,-a.y,-a.z); }
inline bool operator==(const P3d& a, const P3d& b) { return a.x==b.x && a.y==b.y && a.z==b.z; }
inline bool operator!=(const P3d& a, const P3d& b) { return !(a==b); }

inline double operator*(const P3d& a,const P3d& b) { return a.x*b.x+a.y*b.y+a.z*b.z; }
inline double len2(const P3d& a) { return a*a; }
inline double len(const P3d& a)  { return sqrt(a*a); }
inline double dist2(const P3d& a,const P3d& b) { return len2(a-b); }
inline double dist(const P3d& a,const P3d& b)  { return len(a-b); }

// Project point p onto segment a,b
inline P3d project(const P3d& p, const P3d& a, const P3d& b)
{
	P3d d = b - a;
	double L2 = len2(d);

	if (L2 < 1E-20)
		return (a+b)/2;

	double t = (p-a) * d / L2;
	if (t <= 0)
		return a;
	else if (t >= 1)
		return b;
	else
		return a + t*d;
}

#endif /* GEO3D_H_INCLUDED */
