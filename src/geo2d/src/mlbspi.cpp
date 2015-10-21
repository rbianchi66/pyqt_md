#include <math.h>
#include "mlbspi.h"

MLBSplineInterpolator::MLBSplineInterpolator(int levels)
    : levels(levels)
{
}

void MLBSplineInterpolator::clear()
{
    xy.clear();
    z.clear();
    invalidate();
}

void MLBSplineInterpolator::addPoint(const NDVector& p0, const NDVector& p1)
{
    if (p0.size() != 2)
        throw RuntimeError("Domain dimension must be 2");
    if (z.size() != 0 && z[0].size() != p1.size())
        throw RuntimeError("Codomain dimension must be uniform");
    xy.push_back(p0);
    z.push_back(p1);
    invalidate();
}

void MLBSplineInterpolator::invalidate()
{
    if (delta.size() != 0)
    {
        delta.clear();
        omega.clear();
    }
}

#define det3(a, b, c,\
             d, e, f,\
             g, h, i)\
    ((a)*(e)*(i) + (b)*(f)*(g) + (c)*(d)*(h) - (c)*(e)*(g) - (b)*(d)*(i) - (a)*(f)*(h))

static double BSpline(int b, double t)
{
    double t2 = t*t;
    double t3 = t2*t;
    switch(b)
    {
        case 0: return (-t3 + 3*t2 - 3*t + 1)/6;
        case 1: return (3*t3 - 6*t2 + 4)/6;
        case 2: return (-3*t3 + 3*t2 + 3*t + 1)/6;
        case 3: return t3/6;
    }
    throw RuntimeError("Invalid BSpline call");
}

NDVector MLBSplineInterpolator::calc(const NDVector& p)
{
    if (delta.size() == 0)
        calc();

    double x = p[0];
    double y = p[1];

    // Start from approximation plane
    NDVector res(x*ra + y*rb + rc);

    // Add multilevel b-spline contribution
    int n=1, m=1;
    for (int L=0; L<levels; L++)
    {
        // Get to next level
        n = n*2;
        m = m*2;
        std::vector< std::vector< NDVector > >& Ldelta(delta[L]);
        std::vector< std::vector< double > >& Lomega(omega[L]);
        double dx = (x1 - x0) / n;
        double dy = (y1 - y0) / m;
        double xx = (x - x0) / dx;
        double yy = (y - y0) / dy;
        int i = int(xx);
        int j = int(yy);
        double s = xx - i;
        double t = yy - j;
        for (int k=0; k<4; k++)
        {
            for (int l=0; l<4; l++)
            {
                int jj = j+l;
                int kk = i+k;
                if (jj>=0 && jj<m+3 && kk>=0 && kk<n+3)
                {
                    double den = Lomega[jj][kk];
                    if (den > 0)
                    {
                        res += Ldelta[jj][kk] *
                               ( BSpline(k, s) * BSpline(l, t) / den );
                    }
                }
            }
        }
    }

    return res;
}


void MLBSplineInterpolator::calc()
{
    // Starts by computing a least squares approximation plane, this will provide
    // reasonable behaviour for extrapolation (the multilevel b-spline interpolator
    // goes to zero when far from the data).
    double X=0, Y=0, X2=0, Y2=0, XY=0;
    int zd = z[0].size();
    int N = xy.size();
    NDVector Z(zd), XZ(zd), YZ(zd);
    for (int i=0; i<N; i++)
    {
        double xv = xy[i][0];
        double yv = xy[i][1];
        const NDVector& zv = z[i];
        X += xv;
        Y += yv;
        XY += xv*yv;
        X2 += xv*xv;
        Y2 += yv*yv;
        Z += zv;
        XZ += xv*zv;
        YZ += yv*zv;
    }

    // aX2 + bXY + cX = XZ
    // aXY + bY2 + cY = YZ
    // aX  + bY  + cN = Z
    double pdelta = det3(X2, XY, X,
                         XY, Y2, Y,
                         X,  Y,  N);

    ra = det3(XZ, XY, X,
              YZ, Y2, Y,
              Z,  Y,  N) / pdelta;

    rb = det3(X2, XZ, X,
              XY, YZ, Y,
              X,  Z,  N) / pdelta;

    rc = det3(X2, XY, XZ,
              XY, Y2, YZ,
              X,  Y,  Z) / pdelta;

    // Compute the bounding box for the input data
    x0 = y0 = 1E20;
    x1 = y1 = -1E20;
    for (int i=0; i<N; i++)
    {
        double x = xy[i][0];
        double y = xy[i][1];
        if (x < x0) x0 = x;
        if (x > x1) x1 = x;
        if (y < y0) y0 = y;
        if (y > y1) y1 = y;
    }

    // Subtract approximation plane from input data
    zr.clear();
    for (int i=0; i<N; i++)
    {
        double x = xy[i][0];
        double y = xy[i][1];
        zr.push_back(z[i] - (x*ra + y*rb + rc));
    }

    int n=1, m=1;
    for (int L=0; L<levels; L++)
    {
        // Build next level
        n = n*2;
        m = m*2;
        double dx = (x1 - x0) / n;
        double dy = (y1 - y0) / m;
        delta.resize(1 + delta.size());
        omega.resize(1 + omega.size());
        std::vector< std::vector< NDVector > >& Ldelta(delta.back());
        std::vector< std::vector< double > >& Lomega(omega.back());
        for (int i=0; i<n+3; i++)
        {
            Ldelta.resize(1 + Ldelta.size());
            Lomega.resize(1 + Lomega.size());
            for (int j=0; j<m+3; j++)
            {
                Ldelta.back().push_back(NDVector(zd));
                Lomega.back().push_back(0.0);
            }
        }

        // Compute delta/omega matrix elements
        for (int pi=0; pi<N; pi++)
        {
            double x = xy[pi][0];
            double y = xy[pi][1];
            NDVector z = zr[pi];
            double xx = (x - x0) / dx;
            double yy = (y - y0) / dy;
            int i = int(xx); if (i > m-1) i = m-1;
            int j = int(yy); if (j > n-1) j = n-1;
            double s = xx - i;
            double t = yy - j;
            double wkl[4][4] = {{0.0, 0.0, 0.0, 0.0},
                                {0.0, 0.0, 0.0, 0.0},
                                {0.0, 0.0, 0.0, 0.0},
                                {0.0, 0.0, 0.0, 0.0}};
            double sw2 = 0.0;
            for (int l=0; l<4; l++)
            {
                for (int k=0; k<4; k++)
                {
                    double w = BSpline(l, s) * BSpline(k, t);
                    wkl[k][l] = w;
                    sw2 += w*w;
                }
            }
            if (sw2 != 0)
            {
                for (int l=0; l<4; l++)
                {
                    for (int k=0; k<4; k++)
                    {
                        double w = wkl[k][l];
                        NDVector h = z*w/sw2;
                        double wkl2 = w*w;
                        Ldelta[j+l][i+k] += wkl2 * h;
                        Lomega[j+l][i+k] += wkl2;
                    }
                }
            }
        }

        // Subtract level approximation
        for (int pi=0; pi<N; pi++)
        {
            double x = xy[pi][0];
            double y = xy[pi][1];
            double xx = (x - x0) / dx;
            double yy = (y - y0) / dy;
            int i = int(xx);
            int j = int(yy);
            double s = xx - i;
            double t = yy - j;
            NDVector res(zd);
            for (int k=0; k<4; k++)
            {
                for (int l=0; l<4; l++)
                {
                    int jj = j+l;
                    int kk = i+k;
                    if (jj>=0 && jj<m+3 && kk>=0 && kk<n+3)
                    {
                        double den = Lomega[jj][kk];
                        if (den > 0)
                        {
                            res += Ldelta[jj][kk] *
                                   ( BSpline(k, s) * BSpline(l, t) / den );
                        }
                    }
                }
            }
            zr[pi] -= res;
        }
    }
}
