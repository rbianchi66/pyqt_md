#if !defined(DGRID_H_INCLUDED)
#define DGRID_H_INCLUDED

#include <vector>
#include <math.h>

struct CMatrix
{
    enum { BSZ=16 };
    int dx, dy;                       // Dimensioni in punti
    int bdx, bdy;                     // Dimensioni in blocchi
    std::vector<float *> data;        // Puntatori a blocchi BSZ*BSZ
    std::vector<int> left, right;
    int iy0, iy1;

    CMatrix(int dx, int dy)
        : dx(dx), dy(dy),
          bdx((dx+BSZ-1)/BSZ), bdy((dy+BSZ-1)/BSZ),
          data(bdx*bdy),
          left(dy), right(dy),
          iy0(dy), iy1(0)
    {
    }

    ~CMatrix()
    {
        for (int i=0; i<bdx*bdy; i++)
            delete[] data[i];
    }

    void beginPoly()
    {
        iy0=dy; iy1=0;
    }

    void addLine(double xa, double ya, double xb, double yb)
    {
        std::vector<int>& side = ya > yb ? left : right;
        if (ya > yb)
        {
            double ty=ya; ya=yb; yb=ty;
            double tx=xa; xa=xb; xb=tx;
        }
        int iya = int(ya);
        if (iya < 0) iya=0;
        if (iya < ya) iya++;
        if (iya < yb)
        {
            if (iya < iy0) iy0 = iya;
            double m = (xb - xa)/(yb - ya);
            double x = xa + (iya - ya)*m;
            while (iya < yb && iya < dy)
            {
                side[iya++] = int(x);
                x += m;
            }
            if (iya > iy1) iy1 = iya;
            if (iy1 > dy) iy1 = dy;
        }
    }

    void pointFill(double x, double y)
    {
        for (int i=iy0; i<iy1; i++)
        {
            int x0 = left[i], x1 = right[i];
            if (x0 < 0) x0 = 0;
            if (x1 > dx) x1 = dx;
            if (x0 < x1)
            {
                double dy2 = (i - y)*(i - y);
                for (int bx=x0/BSZ,ex=(x1-1)/BSZ+1; bx<ex; bx++)
                {
                    int xa = bx*BSZ;
                    int xb = xa+BSZ;
                    if (xa < x0) xa = x0;
                    if (xb > x1) xb = x1;
                    int bi = i/BSZ*bdx+bx;
                    float *wp = data[bi];
                    if (wp == 0)
                    {
                        wp = data[bi] = new float[BSZ*BSZ];
                        for (int i=0; i<BSZ*BSZ; i++)
                            wp[i] = 10000000.0;
                    }
                    wp += ((i&(BSZ-1))*BSZ) + (xa&(BSZ-1));
                    int c = xb - xa;
                    int ix = xa;
                    do
                    {
                        double v = (ix - x)*(ix - x) + dy2;
                        if (*wp > v) *wp = float(v);
                        wp++; ix++;
                    } while (--c);
                }
            }
        }
    }

    void edgeFill(double xa, double ya, double xb, double yb)
    {
        double nx = yb - ya;
        double ny = xa - xb;
        double nn = sqrt(nx*nx + ny*ny);
        nx/=nn; ny/=nn;
        double nk = - xa*nx - ya*ny;

        for (int i=iy0; i<iy1; i++)
        {
            int x0 = left[i], x1 = right[i];
            if (x0 < 0) x0 = 0;
            if (x1 > dx) x1 = dx;
            if (x0 < x1)
            {
                double cy = i*ny+nk;
                for (int bx=x0/BSZ,ex=(x1-1)/BSZ+1; bx<ex; bx++)
                {
                    int xa = bx*BSZ;
                    int xb = xa+BSZ;
                    if (xa < x0) xa = x0;
                    if (xb > x1) xb = x1;
                    int bi = i/BSZ*bdx+bx;
                    float *wp = data[bi];
                    if (wp == 0)
                    {
                        wp = data[bi] = new float[BSZ*BSZ];
                        for (int i=0; i<BSZ*BSZ; i++)
                            wp[i] = 10000000.0;
                    }
                    wp += ((i&(BSZ-1))*BSZ) + (xa&(BSZ-1));
                    int c = xb - xa;
                    int ix = xa;
                    do
                    {
                        double c = ix*nx + cy;
                        double v = c*c;
                        if (*wp > v) *wp = float(v);
                        wp++; ix++;
                    } while (--c);
                }
            }
        }
    }

    void addEdge(double xa, double ya,
                 double xb, double yb, double d)
    {
        double dx = xb - xa;
        double dy = yb - ya;
        double dd = sqrt(dx*dx + dy*dy);
        dx /= dd; dy /= dd;
        double nx = -dy*(d+1);
        double ny = dx*(d+1);
        double x00 = xa - dx + nx;
        double y00 = ya - dy + ny;
        double x10 = xb + dx + nx;
        double y10 = yb + dy + ny;
        double x11 = xb + dx - nx;
        double y11 = yb + dy - ny;
        double x01 = xa - dx - nx;
        double y01 = ya - dy - ny;
        beginPoly();
        addLine(x00, y00, x01, y01);
        addLine(x01, y01, x11, y11);
        addLine(x11, y11, x10, y10);
        addLine(x10, y10, x00, y00);
        edgeFill(xa, ya, xb, yb);
    }

    void wedge(double xb, double yb,
               double a0, double a1,
               double d)
    {
        const int NP = 20;
        const double kt = tan(2*3.141592654/NP);
        const double k = sqrt(kt*kt + 1.0);
        int n = int((a1-a0)*NP/2/3.141592654) + 1;
        double d1 = d*k + 1;
        double abx = d1*cos(a0);
        double aby = d1*sin(a0);
        double ox = xb + abx;
        double oy = yb + aby;
        beginPoly();
        addLine(xb, yb, ox, oy);
        for (int i=1; i<=n; i++)
        {
            double a = a0 + i*(a1 - a0)/n;
            double x = xb + d1*cos(a);
            double y = yb + d1*sin(a);
            addLine(ox, oy, x, y);
            ox = x; oy = y;
        }
        addLine(ox, oy, xb, yb);
        pointFill(xb, yb);
    }

    void addVertex(double xc, double yc,
                   double d)
    {
        const int NP = 20;
        const double kt = tan(2*3.141592654/NP);
        const double k = sqrt(kt*kt + 1.0);
        double d1 = d*k;
        beginPoly();
        double ox = xc + d1;
        double oy = yc;
        for (int i=1; i<=20; i++)
        {
            double t = 2*3.141592654*i/20;
            double x = xc + d1*cos(t);
            double y = yc + d1*sin(t);
            addLine(ox, oy, x, y);
            ox = x; oy = y;
        }
        pointFill(xc, yc);
    }

    void addVertex(double xa, double ya,
                   double xb, double yb,
                   double xc, double yc,
                   double d)
    {
        double abx = yb - ya;
        double aby = xa - xb;
        double ab = sqrt(abx*abx + aby*aby);
        double bcx = yc - yb;
        double bcy = xb - xc;
        double bc = sqrt(bcx*bcx + bcy*bcy);
        if (ab < 1E-20 || bc < 1E-20)
        {
            addVertex(xb, yb, d);
            return;
        }
        abx /= ab; aby /= ab;
        bcx /= bc; bcy /= bc;
        double cp = abx*bcy - aby*bcx;
        if (cp < 0)
        {
            double tx = abx; abx = -bcx; bcx = -tx;
            double ty = aby; aby = -bcy; bcy = -ty;
        }
        double a0 = atan2(aby, abx);
        double a1 = atan2(bcy, bcx);
        if (a1 < a0) a1 += 2*3.141592654;
        wedge(xb, yb, a0, a1, d);
    }

    private:
        // Taboo
        CMatrix(const CMatrix&);
        CMatrix operator=(const CMatrix&);
};

#endif
