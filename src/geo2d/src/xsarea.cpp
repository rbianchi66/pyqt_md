#include <math.h>
#include "xsarea.h"

namespace {

struct XI {
    double x;
    int id;
    XI *next;
};

struct XIPage
{
    XI xis[256];
    XIPage *next;
};

struct XIAllocator
{
    XIPage *pg;
    int i;

    XIAllocator() : pg(0), i(256)
    {
    }

    XI *newXI()
    {
        if (i==256)
        {
            XIPage *npg = new XIPage;
            npg->next = pg;
            pg = npg;
            i = 0;
        }
        return &pg->xis[i++];
    }

    ~XIAllocator()
    {
        while (pg)
        {
            XIPage *npg = pg->next;
            delete pg;
            pg = npg;
        }
    }
};

void render(XIAllocator& xia, XI **ixs, int y0, int y1,
            double *A, int na, int id,
            double m00, double m01,
            double m10, double m11,
            double m20, double m21)
{
    int na2 = na * 2;
    double xb = A[na2-2]*m00 + A[na2-1]*m10 + m20;
    double yb = A[na2-2]*m01 + A[na2-1]*m11 + m21;
    for (int i=0; i<na2; i+=2)
    {
        double xa = A[i]*m00 + A[i+1]*m10 + m20;
        double ya = A[i]*m01 + A[i+1]*m11 + m21;
        double yy0 = ya < yb ? ya : yb;
        if (yy0 < y0) yy0 = y0;
        double yy1 = ya > yb ? ya : yb;
        if (yy1 > y1) yy1 = y1;
        int iy0 = int(floor(yy0));
        if (iy0 < yy0) iy0++;
        if (iy0 < yy1)
        {
            double dx = (xb - xa)/(yb - ya);
            double x = xa + (iy0 - ya)*dx;
            while (iy0 < yy1)
            {
                XI *nxi = xia.newXI();
                nxi->x = x;
                nxi->id = id;
                XI **prev = &ixs[iy0-y0], *xi = *prev;
                while (xi && xi->x < x)
                {
                    prev = &xi->next; xi = *prev;
                }
                nxi->next = xi;
                *prev = nxi;
                iy0++; x += dx;
            }
        }
        xb = xa; yb = ya;
    }
}

}

double xsarea(double *A, int na, double *B, int nb,
              double *mA, double *mB)
{
    double ay0 = 1E20, ay1=-1E20;
    for (int i=0; i<na; i++)
    {
        double y = A[i*2]*mA[1] + A[i*2+1]*mA[3] + mA[5];
        if (ay0 > y) ay0 = y;
        if (ay1 < y) ay1 = y;
    }

    double by0 = 1E20, by1=-1E20;
    for (int i=0; i<nb; i++)
    {
        double y = B[i*2]*mB[1] + B[i*2+1]*mB[3] + mB[5];
        if (by0 > y) by0 = y;
        if (by1 < y) by1 = y;
    }

    double y0 = ay0>by0 ? ay0 : by0;
    double y1 = ay1<by1 ? ay1 : by1;
    if (y0 >= y1)
        return 0;
    int iy0 = int(floor(y0));
    int iy1 = int(ceil(y1));
    int ny = iy1 - iy0;

    XIAllocator xia;
    XI **ixs = new XI*[ny];
    for (int i=0; i<ny; i++)
        ixs[i] = 0;
    render(xia, ixs, iy0, iy1, A, na, 1, mA[0], mA[1], mA[2], mA[3], mA[4], mA[5]);
    render(xia, ixs, iy0, iy1, B, nb, 2, mB[0], mB[1], mB[2], mB[3], mB[4], mB[5]);
    double AA = 0;
    for (int y = iy0; y<iy1; y++)
    {
        int cc = 0;
        XI *xi = ixs[y-iy0];
        double ox = 0;
        while (xi)
        {
            if (cc == 3)
                AA += xi->x - ox;
            ox = xi->x;
            cc ^= xi->id;
            xi = xi->next;
        }
    }
    delete[] ixs;
    return AA;
}

double xsarea(const std::vector<Path2d>& paths1, const X2d& X1,
              const std::vector<Path2d>& paths2, const X2d& X2) {
    std::vector<P2d> cpath1;
    std::vector<P2d> cpath2;

    for (int k = 1; k <=2; k++) {
        const std::vector<Path2d>& paths = k == 1 ? paths1 : paths2;
        //Path2d& cpath = k == 1 ? cpath1 : cpath2;
        std::vector<P2d>& cpath = k == 1 ? cpath1 : cpath2;
        int npaths = (int)paths.size();
        for (int i = 0; i < npaths; i++) {
            const Path2d& path = paths[i];
            int npts = (int)path.size();
            for(int j = 0; j < npts; j++) {
                cpath.push_back(path[j]);
            }
            cpath.push_back(paths[i][0]);
        }
        for (int i = npaths - 1; i >= 0; --i) {
            cpath.push_back(paths[i][0]);
        }
    }
    Path2d p1(cpath1);
    Path2d p2(cpath2);
    return xsarea(p1, X1, p2, X2);
}

double xsarea(const Path2d& path1, const X2d& X1, const Path2d& path2, const X2d& X2)
{
    int path1_size = path1.size();
    int path2_size = path2.size();

    std::vector<double> s(2*path1_size + 2*path2_size+6 + 6);
    double* A   = &s[0];
    double* B   = A + 2*path1_size;
    double* mA  = B + 2*path2_size;
    double* mB  = mA + 6;

    mA[0] = X1.a.x; mA[1] = X1.a.y;
    mA[2] = X1.b.x; mA[3] = X1.b.y;
    mA[4] = X1.c.x; mA[5] = X1.c.y;

    mB[0] = X2.a.x; mB[1] = X2.a.y;
    mB[2] = X2.b.x; mB[3] = X2.b.y;
    mB[4] = X2.c.x; mB[5] = X2.c.y;

    for (int i = 0; i < path1_size; i++)
    {
        A[2*i]      = path1[i].x;
        A[2*i + 1]  = path1[i].y;
    }

    for (int i = 0; i < path2_size; i++)
    {
        B[2*i]      = path2[i].x;
        B[2*i + 1]  = path2[i].y;
    }

    return xsarea(A, path1_size, B, path2_size, mA, mB);
}
