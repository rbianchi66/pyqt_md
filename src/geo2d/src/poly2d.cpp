#include "poly2d.h"
#include "p2i.h"

BlocksInfo blocks(const std::vector<Path2d>& boundaries, double side, bool oddeven)
{
    std::vector<Path2d> paths;
    std::vector<Path2d> grid;
    int h, w;
    int x0, y0, x1, y1;
    P2d p0, p1, po, pa, pb;

    for (int j=0,m=boundaries.size(); j<m; j++)
    {
        Path2d path = boundaries[j] / side;
        path.boundingBox(p0, p1, j!=0);
        paths.push_back(path);
    }

    x0 = int(floor(p0.x - side));
    y0 = int(floor(p0.y - side));
    x1 = int(ceil(p1.x + side));
    y1 = int(ceil(p1.y + side));

    po.x = x0;
    po.y = y0;
    for (int j=0,m=paths.size(); j<m; j++)
    {
        paths[j] -= po; /* align to image origin */
    }

    w = x1 - x0;
    h = y1 - y0;
    Image<int> image(w, h);
    image.fastBox(0, 0, w, h, 0); /* zero fill */

    if (oddeven) {
        polyFill(image, 1, paths);
        polyDraw(image, 1, paths);
    } else {
    /* vogliamo solo marcare le aree coperte dai perimetri esterni
     * senza considerare eventuali fori */
        std::vector<Path2d> spath(1);
        for (int j=0,m=paths.size(); j<m; j++)
        {
            spath[0] = paths[j];
            polyFill(image, 1, spath);
            polyDraw(image, 1, spath);
        }
    }

    paths.clear();

    int num=0;
    for(int x=-1; x<w; ++x) {
        for (int y=-1; y<h; ++y) {
            if (image.pixel(x, y, 0) != 0)
                {
                    std::vector<P2d> points;
                    pa.x = (x0 + x) * side;
                    pa.y = (y0 + y) * side;
                    pb.x = pa.x;
                    pb.y = pa.y + side;
                    points.push_back(pa);
                    points.push_back(pb);
                    pa.x = pb.x + side;
                    pa.y = pb.y;
                    points.push_back(pa);
                    pb.x = pa.x;
                    pb.y = pa.y - side;
                    points.push_back(pb);
                    pa.x = pb.x - side;
                    pa.y = pb.y;
                    points.push_back(pa);
                    Path2d block(points);
                    grid.push_back(block);
                    num++;
                }
            /* due pixel accanto diversi*/
            if (image.pixel(x, y, 0) != image.pixel(x+1, y, 0)) {
                std::vector<P2d> points;
                pa.x = (x0 + x + 1) * side;
                pa.y = (y0 + y) * side;
                pb.x = pa.x;
                pb.y = pa.y + side;
                points.push_back(pa);
                points.push_back(pb);
                Path2d block(points);
                paths.push_back(block);
            /* due pixel uno sopra l'altro diversi*/
            } if (image.pixel(x, y, 0) != image.pixel(x, y+1, 0)) {
                std::vector<P2d> points;
                pa.x = (x0 + x) * side;
                pa.y = (y0 + y + 1) * side;
                pb.x = pa.x + side;
                pb.y = pa.y;
                points.push_back(pa);
                points.push_back(pb);
                Path2d block(points);
                paths.push_back(block);
            }
        }
    }
    BlocksInfo iblocks(num, paths, grid);
    return iblocks;
}

namespace {

// Helper structure for polyscan

struct LF {
    int w, h;
    std::vector< std::vector<int> >& ix;

    LF(int w, int h, std::vector<std::vector<int> >& ix) : w(w), h(h), ix(ix) {
        ix.clear(); ix.resize(h);
    }

    void point_(int x, int iy) {
        std::vector<int>& iix(ix[iy]);
        iix.push_back(x);
        int *p0=&iix.front(), *p1=&iix.back();
        while (p1>p0 && p1[-1]>x) {
            p1[0]=p1[-1];
            --p1;
        }
        p1[0] = x;
    }

    void line_(int ix0, int iy0, int ix1, int iy1) {
        if (iy0 != iy1) {
            int ya = iy0 < iy1 ? iy0 : iy1;
            int yb = iy0 + iy1 - ya;
            int iy = ya >> 8; if (iy < 0) iy = 0;
            int y = (iy << 8) + 128;
            if (y < ya) { iy++; y += 256; }
            if (iy < h && y < yb) {
                double dx = double(ix1 - ix0) / (iy1 - iy0);
                double xx = ix0 + (y - iy0)*dx + 128;
                dx *= 256;
                int sf = iy0 > iy1;
                while (y < yb && iy < h) {
                    int x = int(xx) >> 8;
                    if (x < 0) x = 0; if (x > w) x = w;
                    point_(x*2+sf, iy);
                    xx += dx; iy++; y+=256;
                }
            }
        }
    }
};

}

void polyScan(const std::vector<Path2d>& boundaries,
              int w, int h,
              std::vector<std::vector<int> >& ix,
              double expand)
{
    LF lf(w, h, ix);
    std::vector<Path2i> ibounds;
    for (int i=0,n=boundaries.size(); i<n; i++) {
        const Path2d& rp(boundaries[i]);
        Path2i wp;
        for (int j=0,m=rp.size(); j<m; j++) {
            P2i p(int(floor(rp[j].x*256.+.5)),
                  int(floor(rp[j].y*256.+.5)));
            if (wp.size() == 0 || p!=wp.back()) {
                wp.push_back(p);
            }
        }
        if (wp.size() > 1 && wp.back() == wp[0]) wp.pop_back();
        if (wp.size() > 1) {
            ibounds.resize(1 + ibounds.size());
            ibounds.back().swap(wp);
        }
    }

    for (int i=0,n=ibounds.size(); i<n; i++) {
        const Path2i& p = ibounds[i];
        for (int j1=0,m=p.size(),j0=m-1; j1<m; j0=j1++) {
            lf.line_(p[j0].x, p[j0].y, p[j1].x, p[j1].y);
        }
    }

    if (expand < 0) {
        for (int i=0; i<h; i++) {
            lf.point_(0, i);
            lf.point_(w*2, i);
        }
    }
    for (int i=0; i<h; i++) {
        std::vector<int>& iix(lf.ix[i]);
        for (int j=0,n=iix.size(); j<n; j++) {
            iix[j] = (iix[j] & ~1) | (j & 1);
        }
    }

    if (expand != 0) {
        int fe = int(floor(fabs(expand)*256 + 0.5));
        std::vector<P2i> pts(32);
        for (int i=0; i<32; i++) {
            double t = i*2*3.141692654/32;
            pts[i]=P2i(int(floor(expand*256*cos(t)+0.5)),
                       int(floor(expand*256*sin(t)+0.5)));
        }
        double dmin = fe*fe*0.8;
        for (int i=0,n=ibounds.size(); i<n; i++) {
            const Path2i& p = ibounds[i];
            Path2i nn(p.size());
            for (int j1=0,n=p.size(),j0=n-1; j1<n; j0=j1++) {
                const P2i& p0=p[j0], p1=p[j1];
                double dx = p1.x - p0.x;
                double dy = p1.y - p0.y;
                double k = fe / sqrt(dx*dx + dy*dy);
                nn[j0] = P2i(int(floor(-dy*k+0.5)),int(floor(dx*k+0.5)));
            }
            for (int i=0,sz=p.size(); i<sz; i++) {
                int i1 = i+1; if (i1 == sz) i1 = 0;
                const P2i& n = nn[i];
                const P2i& n1 = nn[i1];
                if (double(n.x)*n1.x+double(n.y)*n1.y < dmin) {
                    int x = p[i1].x, y=p[i1].y;
                    for (int k=0,kk=31; k<32; kk=k++) {
                        lf.line_(pts[k].x+x, pts[k].y+y, pts[kk].x+x, pts[kk].y+y);
                    }
                }
                lf.line_(p[i].x+n.x, p[i].y+n.y, p[i1].x+n.x, p[i1].y+n.y);
                lf.line_(p[i1].x+n.x, p[i1].y+n.y, p[i1].x+n1.x, p[i1].y+n1.y);
                lf.line_(p[i1].x-n.x, p[i1].y-n.y, p[i].x-n.x, p[i].y-n.y);
                lf.line_(p[i1].x-n1.x, p[i1].y-n1.y, p[i1].x-n.x, p[i1].y-n.y);
            }
        }
    }

    int neg = expand < 0;
    for (int iy=0; iy<h; iy++) {
        std::vector<int>& v = lf.ix[iy];
        int wp = 0;
        for (int i=0,n=v.size(),lx=0,c=0,f=0; i<n; i++) {
            int delta = (v[i] & 1) ? -1 : 1;
            int x = v[i] >> 1;
            c += delta;
            int of = f;
            f = (neg ^ (c != 0));
            if (f && !of) {
                lx = x;
            } else if (!f && of && (lx != x)) {
                v[wp] = lx; v[wp+1] = x; wp += 2;
            }
        }
        v.resize(wp);
    }
}
