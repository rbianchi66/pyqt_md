#if !defined(P2I_H_INCLUDED)
#define P2I_H_INCLUDED
#include <vector>

struct P2i {
    int x, y;
    P2i() : x(0), y(0) {}
    P2i(int x, int y) : x(x), y(y) {}

    bool operator==(const P2i& other) const {
        return x==other.x && y==other.y;
    }

    bool operator!=(const P2i& other) const {
        return x!=other.x || y!=other.y;
    }

    bool operator<(const P2i& other) const {
        return x<other.x || (x==other.x && y<other.y);
    }

    bool operator<=(const P2i& other) const {
        return x<=other.x || (x==other.x && y<=other.y);
    }

    bool operator>(const P2i& other) const {
        return x>other.x || (x==other.x && y>other.y);
    }

    bool operator>=(const P2i& other) const {
        return x>=other.x || (x==other.x && y>=other.y);
    }

    P2i &operator+=(const P2i& r)
    {
        x+=r.x; y+=r.y;
        return *this;
    }

    P2i operator+(const P2i& r) const
    {
        P2i ret(*this);
        return ret+=r;
    }

    P2i &operator-=(const P2i &r)
    {
        x-=r.x; y-=r.y;
        return *this;
    }

    P2i operator-(const P2i& r)
    {
        P2i ret(*this);
        return ret-=r;
    }

    P2i &operator*=(int r)
    {
        x*=r; y*=r;
        return *this;
    }

    P2i operator*(int r)
    {
        P2i ret(*this);
        return ret*=r;
    }
};

typedef std::vector<P2i> Path2i;

inline int len2(P2i p)
{
    return p.x*p.x+p.y*p.y;
}

inline int dist2(P2i p1, P2i p2)
{
    return len2(p1-p2);
}

#endif
