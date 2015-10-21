#ifndef _marshaller_geo2d_h_
#define _marshaller_geo2d_h_

#include <Stream.h>
#include <geo2d.h>

inline C2Stream& operator>>(C2Stream& s, P2d& p)
{
    s >> C2TypeCheck("P2d") >> p.x >> p.y;
    return s;
}

inline C2Stream& operator<<(C2Stream& s, const P2d& p)
{
    s << C2TypeCheck("P2d") << p.x << p.y;
    return s;
}

inline C2Stream& operator>>(C2Stream& s, X2d& p)
{
    s >> C2TypeCheck("X2d");
    s >> p.a.x >> p.a.y;
    s >> p.b.x >> p.b.y;
    s >> p.c.x >> p.c.y;
    return s;
}

inline C2Stream& operator<<(C2Stream& s, const X2d& p)
{
    s << C2TypeCheck("X2d");
    s << p.a.x << p.a.y;
    s << p.b.x << p.b.y;
    s << p.c.x << p.c.y;
    return s;
}

inline C2Stream& operator>>(C2Stream& s, Path2d& v)
{
    int size;
    P2d t;

    s >> C2TypeCheck("Path2d") >> size;
    v.resize(size);

    for (int i=0; i<size; i++)
    {
        s >> t.x >> t.y;
        v[i] = t;
    }
    return s;
}

inline C2Stream& operator<<(C2Stream& s, const Path2d& v)
{
    s << C2TypeCheck("Path2d") << int(v.size());

    for (int i=0,n=v.size(); i<n; i++)
    {
        s << v[i].x << v[i].y;
    }
    return s;
}

#endif
