#if !defined(POLY2D_H_INCLUDED)
#define POLY2D_H_INCLUDED

#include <vector>
#include <math.h>
#include "image2d.h"
#include "geo2d.h"

/**
 Genera tute le span relative ad una lista di contorni (regola odd-even) e le
 memorizza nel vector<vector<int>> passato (per ogni scanline sono memorizzati
 i valori x corrispondenti ai limiti delle span, in ordine crescente).
 I pixel coperti dalle span sono tutti e soli quelli in cui il *centro* del pixel
 cade all'interno dell'area del poligono (allargata o ristretta di *expand*) e
 che sono compresi fra (0,0) e (w,h).
*/
void polyScan(const std::vector<Path2d>& boundaries,  //< Vettore di path di definizione dell'area (regola odd-even)
              int w, int h,                           //< Area di clipping delle span generate (0,0)-(w,h)
              std::vector<std::vector<int> >& ix,     //< Vector di output (risultera' di h elementi)
              double expand = 0);                     //< Raggio di "espansione" (o "restringimento" se negativo)

/**
 Genera tute le span relative ad una lista di contorni (regola odd-even) e chiama
 la callback specificata passando per ogni span x0, x1 e y.
 I pixel coperti dalle span sono tutti e soli quelli in cui il *centro* del pixel
 cade all'interno dell'area del poligono (allargata o ristretta di *expand*) e
 che sono compresi fra (0,0) e (w,h).
*/
template<typename CBack>
CBack polyScan(const std::vector<Path2d>& boundaries,  //< Vettore di path di definizione dell'area (regola odd-even)
               int w, int h,                           //< Area di clipping delle span generate (0,0)-(w,h)
               CBack cback,                            //< Callback da chiamare passando x0, x1, y
               double expand = 0)                      //< Raggio di "espansione" (o "restringimento" se negativo)
{
    std::vector<std::vector<int> > ix;
    polyScan(boundaries, w, h, ix, expand);
    for (int i=0; i<h; i++) {
        std::vector<int>& v = ix[i];
        for (int j=0,n=v.size(); j<n-1; j+=2) {
            cback(v[j], v[j+1], i);
        }
    }
    return cback;
}

template<typename CBack>
struct translate_span_callback
{
    CBack& cback;
    int dx, dy;

    translate_span_callback(CBack& cback,
                            int dx, int dy)
      : cback(cback), dx(dx), dy(dy)
    { }

    void operator()(int x0, int x1, int y) {
        cback(x0+dx, x1+dx, y+dy);
    }
};

/**
 Genera tute le span relative ad una lista di contorni (regola odd-even).
 I pixel coperti dalle span sono tutti e soli quelli in cui il *centro* del pixel
 cade all'interno dell'area e che sono compresi fra pmin e pmax (se passati)
*/
template<typename CBack>
CBack polyScan(const std::vector<Path2d>& boundaries,  //< Vettore di path di definizione dell'area (regola odd-even)
               CBack cback,                            //< Callback chiamata con (x0, x1, y) per ogni span, in ordine
               P2d pmin=P2d(-1E300,-1E300),            //< xmin/ymin di clipping
               P2d pmax=P2d(1E300, 1E300))             //< xmax/ymax di clipping
{
    P2d tl, br;
    boundingBox(boundaries, true, &tl, &br);
    tl -= P2d(1, 1); br += P2d(1, 1);
    if (pmin.x > tl.x) tl.x = pmin.x;
    if (pmin.y > tl.y) tl.y = pmin.y;
    if (pmax.x < br.x) br.x = pmax.x;
    if (pmax.y < br.y) br.y = pmax.y;
    int fx0 = int(floor(tl.x + 0.5)), fy0 = int(floor(tl.y + 0.5));
    polyScan(boundaries * X2d::xlate(P2d(-fx0, -fy0)),
             int(floor(br.x + 0.5)) - fx0, int(floor(br.y + 0.5)) - fy0,
             translate_span_callback<CBack>(cback, int(fx0), int(fy0)));
    return cback;
}

//
// Riempe un'area definita da una lista di contorni (regola odd-even).
// I pixel riempiti sono tutti e soli quelli in cui il *centro* del pixel
// cade all'interno dell'area.
//
template<typename Pixel>
struct ImageSpanFiller
{
    Pixel color;
    Image<Pixel>& image;
    ImageSpanFiller(Image<Pixel>& image, Pixel color)
    : color(color), image(image)
    {
    }

    void operator()(int xa, int xb, int y)
    {
        image.setSpan(xa, xb, y, color);
    }
};

template<typename Pixel>
void polyFill(Image<Pixel>& image, Pixel color,
              const std::vector<Path2d>& boundaries)
{
    polyScan(boundaries, image.w, image.h,
             ImageSpanFiller<Pixel>(image, color));
}

//
// Disegna tutti pixel di un'immagine che sono attraversati da linee
// appartenenti ai contorni specificati.
// Fill+Draw equivale ad un fill "conservativo" (risulteranno colorati
// tutti i pixel che intersecano l'area).
//
template<typename Pixel>
void polyDraw(Image<Pixel>& image, Pixel color,
              const std::vector<Path2d>& boundaries)
{
    for (int i=0,n=boundaries.size(); i<n; i++)
    {
        const Path2d& p = boundaries[i];
        for (int j1=0,m=p.size(),j0=m-1; j1<m; j0=j1++)
        {
            double xa = p[j0].x;
            double ya = p[j0].y;
            double xb = p[j1].x;
            double yb = p[j1].y;
            image.setPixel(int(floor(xa)), int(floor(ya)), color);
            if (ya > yb)
            {
                double tx=xa; xa=xb; xb=tx;
                double ty=ya; ya=yb; yb=ty;
            }
            int iy = int(floor(ya));
            if (iy < 1) iy = 1;
            while (iy < image.h && iy < yb)
            {
                if (iy >= ya)
                {
                    int x = int(floor(xa + (iy - ya)*(xb - xa)/(yb - ya)));
                    image.setPixel(x, iy-1, color);
                    image.setPixel(x, iy, color);
                }
                iy++;
            }
            if (xa > xb)
            {
                double tx=xa; xa=xb; xb=tx;
                double ty=ya; ya=yb; yb=ty;
            }
            int ix = int(floor(xa));
            if (ix < 1) ix = 1;
            while (ix < image.w && ix < xb)
            {
                if (ix >= xa)
                {
                    int y = int(floor(ya + (ix - xa)*(yb - ya)/(xb - xa)));
                    image.setPixel(ix-1, y, color);
                    image.setPixel(ix, y, color);
                }
                ix++;
            }
        }
    }
}

struct BlocksInfo
{
    int num;
    std::vector<Path2d> paths;
    std::vector<Path2d> grid;
    BlocksInfo(int num, std::vector<Path2d> paths, std::vector<Path2d> grid) : num(num), paths(paths), grid(grid) {}
};

BlocksInfo blocks(const std::vector<Path2d>& boundaries, double side, bool oddeven = true);

#endif
