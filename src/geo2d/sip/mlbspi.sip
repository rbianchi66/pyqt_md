%ModuleHeaderCode
    #include "mlbspi.h"
%End

struct RuntimeError
{
    const char *msg;
    RuntimeError(const char *msg);
};

struct NDVector
{
    double operator[](int k);
    int size();
    NDVector& operator += (const NDVector& m);
    NDVector& operator -= (const NDVector& m);
    NDVector& operator *= (double k);
    NDVector& operator /= (double k);
};

NDVector operator+(NDVector a, const NDVector& b);
NDVector operator-(NDVector a, const NDVector& b);
NDVector operator*(NDVector a, double k);
NDVector operator*(double k, NDVector a);
NDVector operator/(NDVector a, double k);
double operator*(const NDVector& a, const NDVector& b);
NDVector NDVect(double x0, double x1);
NDVector NDVect(double x0, double x1, double x2);
NDVector NDVect(double x0, double x1, double x2, double x3);
NDVector NDVect(double x0, double x1, double x2, double x3, double x4);

class MLBSplineInterpolator
{
    public:
        MLBSplineInterpolator(int levels);
        void clear();
        void addPoint(const NDVector& p0, const NDVector& p1);
        NDVector calc(const NDVector& p);
};
