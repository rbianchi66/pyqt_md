#if !defined(MARCHING_H_INCLUDED)
#define MARCHING_H_INCLUDED

#include <functional>
#include "image2d.h"
#include "geo2d.h"
#include "boundary_line.h"

template<typename CBack, typename Context, typename Pixel>
std::vector<Path2d> marching(const Image<Pixel> &img, CBack cback, Context context, Pixel frame) {
    std::vector<Path2d> p;
    int h = img.h;
    int w = img.w;
    std::vector<BoundaryLine> b;

    for (int y = -1; y < h; y++) {
        for (int x = -1; x < w; x++) {
            Pixel va = (x == -1 || y == -1) ? frame : img.data[y*w+x];
            Pixel vb = (x == w - 1 || y == -1) ? frame : img.data[y*w+x+1];
            Pixel vc = (x == -1 || y == h - 1) ? frame : img.data[y*w+x+w];
            Pixel vd = (x == w - 1 || y == h - 1) ? frame : img.data[y*w+x+w+1];

            int code = (cback(va, context) ? 8 : 0) +
                       (cback(vb, context) ? 4 : 0) +
                       (cback(vc, context) ? 2 : 0) +
                       (cback(vd, context) ? 1 : 0);

            switch(code) {
              case 0:
              case 15:
                break;
              case 1:
                b.push_back(BoundaryLine(x+0.5, y+1, x+1, y+0.5));
                break;
              case 14:
                b.push_back(BoundaryLine(x+1, y+0.5, x+0.5, y+1));
                break;
              case 2:
                b.push_back(BoundaryLine(x, y+0.5, x+0.5, y+1));
                break;
              case 13:
                b.push_back(BoundaryLine(x+0.5, y+1, x, y+0.5));
                break;
              case 3:
                b.push_back(BoundaryLine(x, y+0.5, x+1, y+0.5));
                break;
              case 12:
                b.push_back(BoundaryLine(x+1, y+0.5, x, y+0.5));
                break;
              case 4:
                b.push_back(BoundaryLine(x+1, y+0.5, x+0.5, y));
                break;
              case 11:
                b.push_back(BoundaryLine(x+0.5, y, x+1, y+0.5));
                break;
              case 5:
                b.push_back(BoundaryLine(x+0.5, y+1, x+0.5, y));
                break;
              case 10:
                b.push_back(BoundaryLine(x+0.5, y, x+0.5, y+1));
                break;
              case 6:
                b.push_back(BoundaryLine(x, y+0.5, x+0.5, y));
                b.push_back(BoundaryLine(x+1, y+0.5, x+0.5, y+1));
                break;
              case 9:
                b.push_back(BoundaryLine(x+0.5, y+1, x, y+0.5));
                b.push_back(BoundaryLine(x+0.5, y, x+1, y+0.5));
                break;
              case 7:
                b.push_back(BoundaryLine(x, y+0.5, x+0.5, y));
                break;
              case 8:
                b.push_back(BoundaryLine(x+0.5, y, x, y+0.5));
                break;
            }
        }
    }

    {
        // Indicizza x punto iniziale
        int count = b.size();
        std::vector<BoundaryLine *> ht(256);
        for (int i=0; i<count; i++)
        {
            int k = int(b[i].x0*2 + b[i].y0*16)&255;
            b[i].next = ht[k];
            ht[k] = &b[i];
        }

        // Loop sino a esaurimento segmenti generati
        while (count > 0)
        {
            // Prende un tratto qualunque
            BoundaryLine *L = 0;
            for (int i=0; i<256; i++)
                if (ht[i] != 0)
                {
                    L = ht[i];
                    ht[i] = L->next;
                    --count;
                    break;
                }
            p.resize(1 + p.size());
            Path2d& out = p.back();
            while (L) {
                out.push_back(P2d(L->x0, L->y0));
                int k = int(L->x1*2 + L->y1*16)&255;
                BoundaryLine **p = &ht[k];
                while (*p != 0 && ((*p)->x0 != L->x1 || (*p)->y0 != L->y1)) {
                    p = &(*p)->next;
                }
                L = *p;
                if (L) {
                    --count;
                    *p = L->next;
                }
            }
            out.push_back(P2d(out[0]));
        }
    }
    return p;
}

template <typename Pixel>
std::vector<Path2d> marching_squares_eq(const Image<Pixel> &img, Pixel color, Pixel frame) {
    return marching(img, std::equal_to<Pixel>(), color, frame);
}

template <typename Pixel>
std::vector<Path2d> marching_squares_gt(const Image<Pixel> &img, Pixel color, Pixel frame) {
    return marching(img, std::greater<Pixel>(), color, frame);
}

template <typename Pixel>
std::vector<Path2d> marching_squares_gteq(const Image<Pixel> &img, Pixel color, Pixel frame) {
    return marching(img, std::greater_equal<Pixel>(), color, frame);
}

#endif // MARCHING_H_INCLUDED
