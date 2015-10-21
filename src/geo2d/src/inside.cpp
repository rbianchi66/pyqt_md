#include "sysdep.h"
#include <algorithm>
#include "inside.h"

bool inside(const P2d &pt, const Path2d& pts)
{
    bool i = false;
    int n = (int)pts.size();
    if (n > 0) {
        P2d p0 = pts.back();
        for (int j = 0; j < n; j++) {
            P2d p1 = pts[j];
            if ((p0.y <= pt.y && pt.y < p1.y) || (p1.y <= pt.y && pt.y < p0.y)) {
                if (p0.x + (pt.y - p0.y)*(p1.x - p0.x)/(p1.y - p0.y) < pt.x) {
                    i = !i;
                }
            }
            p0 = p1;
        }
    }
    return i;
}

bool inside_equal(const P2d &pt, const Path2d& pts, const bool overlap, const double eps)
{
    bool i = false;
    int n = (int)pts.size();
    if (n > 0) {
        P2d p0 = pts.back();
        for (int j = 0; j < n; j++) {
            P2d p1 = pts[j];
            //Se pt e' sovrapposto a p1 o se pt giage sul segmento p0-p1
            if ( (fabs(pt.x -p1.x) < eps && fabs(pt.y -p1.y) < eps) || (fabs(p0.y - p1.y) < eps &&
                      fabs(p1.y - pt.y) < eps &&
                      (
                       ( ( pt.x > p1.x || fabs(pt.x - p1.x) < eps) && ( pt.x < p0.x || fabs(pt.x - p0.x) < eps) ) ||
                       ( ( pt.x > p0.x || fabs(pt.x - p0.x) < eps) && ( pt.x < p1.x || fabs(pt.x - p1.x) < eps) )
                      ))
                      ){
                return overlap;
            }
            else if ((p0.y <= pt.y && pt.y < p1.y) || (p1.y <= pt.y && pt.y < p0.y)) {
                const double val = p0.x + (pt.y - p0.y)*(p1.x - p0.x)/(p1.y - p0.y);
                if ( fabs(val - pt.x) < eps) { // Se il punto appartiene ad un lato del Path a meno di una certa eps
                    return overlap;
                }
                else if (val < pt.x) {
                    i = !i;
                }

            }
            p0 = p1;
        }
    }
    return i;
}

bool inside(const P2d &pt, const std::vector<Path2d>& paths)
{
    bool i = false;
    for (int j = 0; j < (int)paths.size(); j++) i = i ^ inside(pt, paths[j]);
    return i;
}

void InsideTester::init(const std::vector<Path2d>& paths, double eps) {
    kp = 1.0 / eps;

    double y0 = 1E20;
    double y1 = -1E20;
    int npaths = (int)paths.size();
    assert(npaths > 0);
    for(int i = 0; i < npaths; i++) {
        const Path2d &path = paths[i];
        int npath = (int)path.size();
        assert(npath > 0);
        for(int j = 0; j < npath; j++) {
            const P2d &p = path[j];
            if (p.y < y0) y0 = p.y;
            if (p.y > y1) y1 = p.y;
        }
    }

    Y0 = (int)floor(y0 * kp) - 1;
    Y1 = (int)floor(y1 * kp) + 1;

    xi = std::vector<std::vector<double> >(Y1 - Y0);
    for(int j = 0; j < npaths; j++) {
        const Path2d &path = paths[j];
        P2d p0 = path.back();
        int npath = (int)path.size();
        for(int k = 0; k < npath; k++) {
            const P2d& p1 = path[k];

            double xa = p0.x * kp;
            double ya = p0.y * kp;
            double xb = p1.x * kp;
            double yb = p1.y * kp;
            if (ya > yb) {
                double tmp;
                tmp = xa;
                xa = xb;
                xb = tmp;

                tmp = ya;
                ya = yb;
                yb = tmp;
            }
            int i = (int)floor(ya);
            double yi = i + 0.5;
            if (yi < ya) {
                yi++;
                i++;
            }
            if (yi < yb) {
                double de = (xb - xa) / (yb - ya);
                double x = xa + (yi - ya) * de;
                while (yi < yb) {
                    xi[i - Y0].push_back(x);
                    x += de;
                    i++;
                    yi++;
                }
            }
            p0 = p1;
        }
    }
    int nxi = (int)xi.size();
    for (int i = 0; i < nxi; i++) {
        std::vector<double> &L = xi[i];
        std::sort(L.begin(), L.end());
    }
}

InsideTester::InsideTester(const std::vector<Path2d>& paths, double eps) {
    init(paths, eps);
}

InsideTester::InsideTester(const Path2d& path, double eps) {
    init(std::vector<Path2d>(1, path), eps);
}

bool InsideTester::contains(const Path2d &path, bool partial) const {
    for (int i=0,n=path.size(); i<n; i++)
        if (partial ^ !(*this)(path[i]))
            return partial;
    return !partial;
}

bool InsideTester::operator()(const P2d &pt) const {
    double x = pt.x * kp;
    int y = (int)(pt.y * kp);
    if (y < Y0 || y >= Y1) {
        return false;
    }
    bool i = false;
    int nxi = (int)xi[y - Y0].size();
    for(int j = 0; j < nxi; j++) {
        if (x < xi[y - Y0][j]) return i;
        i = !i;
    }
    return i;
}
