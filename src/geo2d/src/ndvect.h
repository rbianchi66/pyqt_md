#if !defined(NDVECT_H_INCLUDED)
#define NDVECT_H_INCLUDED

#include <vector>

struct RuntimeError
{
    const char *msg;
    RuntimeError(const char *msg) : msg(msg)
    {
    }
};

struct NDVector
{
    std::vector<double> x;
    NDVector(int n=0) : x(n) {}
    NDVector(const double *from, const double *to) : x(from, to) {}

    double operator[](int k) const
    {
        if (unsigned(k) >= x.size())
            throw RuntimeError("Index out of bounds");
        return x[k];
    }

    int size() const
    {
        return int(x.size());
    }

    NDVector& operator += (const NDVector& m)
    {
        if (int(x.size()) != m.size())
            throw RuntimeError("Dimension mismatch");
        for (int i=0,n=x.size(); i<n; i++)
            x[i] += m.x[i];
        return *this;
    }

    NDVector& operator -= (const NDVector& m)
    {
        if (int(x.size()) != m.size())
            throw RuntimeError("Dimension mismatch");
        for (int i=0,n=x.size(); i<n; i++)
            x[i] -= m.x[i];
        return *this;
    }

    NDVector& operator *= (double k)
    {
        for (int i=0,n=x.size(); i<n; i++)
            x[i] *= k;
        return *this;
    }

    NDVector& operator /= (double k)
    {
        for (int i=0,n=x.size(); i<n; i++)
            x[i] /= k;
        return *this;
    }
};

inline NDVector operator+(NDVector a, const NDVector& b) { return a += b; }
inline NDVector operator-(NDVector a, const NDVector& b) { return a -= b; }
inline NDVector operator*(NDVector a, double k) { return a *= k; }
inline NDVector operator*(double k, NDVector a) { return a *= k; }
inline NDVector operator/(NDVector a, double k) { return a /= k; }

inline double operator*(const NDVector& a, const NDVector& b)
{
    if (a.size() != b.size())
        throw RuntimeError("Dimension mismatch");
    double t = 0.0;
    for (int i=0,n=a.size(); i<n; i++)
        t += a.x[i]*b.x[i];
    return t;
}

inline NDVector NDVect(double x0, double x1)
{ double x[] = {x0, x1}; return NDVector(x, x+2); }

inline NDVector NDVect(double x0, double x1, double x2)
{ double x[] = {x0, x1, x2}; return NDVector(x, x+3); }

inline NDVector NDVect(double x0, double x1, double x2, double x3)
{ double x[] = {x0, x1, x2, x3}; return NDVector(x, x+4); }

inline NDVector NDVect(double x0, double x1, double x2, double x3, double x4)
{ double x[] = {x0, x1, x2, x3, x4}; return NDVector(x, x+5); }

#endif
