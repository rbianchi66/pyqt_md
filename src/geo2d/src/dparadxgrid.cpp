#include "sysdep.h"
#include "dpara.h"
#include "dxgrid.h"
#include "offset.h"  // simplify (FIXME: muovere da un'altra parte)
#include <vector>
#include <math.h>
#include <stdio.h>

#define SM2 2

namespace {

struct XSect
{
    int x, y;           // Coordinate top oppure left del lato
    double v;           // Valore numerico intersezione
    XSect *prev, *next; // Link di lista intersezioni
    XSect *hnext;       // Lista in collisioni hash table
};

inline unsigned int hkey(int x, int y)
{
    unsigned int k = x;
    k = k*69069 + y + 1;
    k = k*69069 + x + 1;
    k = k*69069 + y + 1;
    return k;
}

struct HMap
{
    int index_size;
    std::vector<XSect *> index;
    HMap(int index_size = 256)
        : index_size(index_size), index(index_size)
    {
    }

    XSect *get(int x, int y)
    {
        XSect **parent = &index[hkey(x, y) & (index_size - 1)];
        XSect *s = *parent;
        while (s && (s->x!=x  || s->y!=y))
        {
            parent = &s->hnext;
            s = *parent;
        }
        if (s) *parent = s->hnext;
        return s;
    }

    void put(XSect *s)
    {
        XSect **parent = &index[hkey(s->x, s->y) & (index_size - 1)];
        s->hnext = *parent;
        *parent = s;
    }
};

static
XSect *free_xsect = 0;

XSect *newXSect(int x, int y, double v, XSect *& first, XSect *& last)
{
    if (!free_xsect)
    {
        free_xsect = new XSect[1024];
        for (int i=0; i<1024; i++)
            free_xsect[i].next = (i==1023 ? 0 : &free_xsect[i+1]);
    }
    XSect *xs = free_xsect;
    free_xsect = xs->next;
    xs->x=x; xs->y=y; xs->v=v;
    xs->next = 0; xs->prev = last;
    if (xs->prev) xs->prev->next = xs; else first = xs;
    last = xs;
    return xs;
}

void freeXSect(XSect *xs, XSect *& first, XSect *& last)
{
    if (xs->prev) xs->prev->next = xs->next; else first = xs->next;
    if (xs->next) xs->next->prev = xs->prev; else last = xs->prev;
    xs->next = free_xsect;
    free_xsect = xs;
}

bool tainted(P2d p, P2d a, P2d b, double td2)
{
    return dist2(p, a) < td2 || dist2(p, b) < td2;
}

void getCycles(HMap& vm, XSect *vfirst, XSect *vlast,
               HMap& hm, XSect *hfirst, XSect *hlast,
               std::vector< Path2d >& result,
               P2d a, P2d b, double td2)
{
	result.reserve(16);

    // Ricostruzione cicli
    while (hfirst || vfirst)
    {
        //
        // x, y = Topleft cella corrente
        // code = Codice di posizione nella cella corrente:
        //      0 = lato sinistro
        //      1 = lato destro
        //      2 = lato superiore
        //      3 = lato inferiore
        //
        Path2d res;
        std::vector<unsigned char> taint;
        int code;
        int x, y;

        if (hfirst)
        {
            XSect *i = hm.get(hfirst->x, hfirst->y);
            code = 2;
            x = i->x; y = i->y;
            res.push_back(P2d(i->v, y));
            taint.push_back(td2>0 && tainted(P2d(i->v, y), a, b, td2));
            freeXSect(i, hfirst, hlast);
        }
        else
        {
            XSect *i = vm.get(vfirst->x, vfirst->y);
            code = 0;
            x = i->x; y=i->y;
            res.push_back(P2d(x, i->v));
            taint.push_back(td2>0 && tainted(P2d(x, i->v), a, b, td2));
            freeXSect(i, vfirst, vlast);
        }
        int stop_code = code;
        int stop_x = x;
        int stop_y = y;
        int stop = 0;
        while(!stop)
        {
            //
            // Loop sui lati della cella corrente
            // per ogni lato:
            //
            //    codice lato, x, y cella corrente
            //    codice lato, x, y cella comunicante
            //    0=hm, 1=vm, e (x, y) di ricerca nella mappa
            //
            int sides[4][9] = { {0, x, y, 1, x-1, y, 1, x, y},
                                {1, x, y, 0, x+1, y, 1, x+1, y},
                                {2, x, y, 3, x, y-1, 0, x, y},
                                {3, x, y, 2, x, y+1, 0, x, y+1} };
            int s = 0;
            for (; s<4; s++)
            {
                int ccode = sides[s][0];
                int cx = sides[s][1];
                int cy = sides[s][2];
                int ncode = sides[s][3];
                int nx = sides[s][4];
                int ny = sides[s][5];
                HMap& m = sides[s][6] ? vm : hm;
                int mx = sides[s][7];
                int my = sides[s][8];
                if (ccode == code && cx==x && cy==y)
                    continue; // Salta il lato attuale
                if (ccode == stop_code && cx == stop_x && cy == stop_y)
                {
                    // Trovato il punto di partenza, ciclo completato
                    stop = 1;
                    break;
                }
                XSect *i = m.get(mx, my);
                if (i == 0)
                    continue; // salta le pareti senza informazioni
                // Output nuovo punto del ciclo
                if (sides[s][6])
                {
                    res.push_back(P2d(mx, i->v));
                    taint.push_back(td2>0 && tainted(P2d(mx, i->v), a, b, td2));
                    freeXSect(i, vfirst, vlast);
                }
                else
                {
					res.push_back(P2d(i->v, my));
                    taint.push_back(td2>0 && tainted(P2d(i->v, my), a, b, td2));
                    freeXSect(i, hfirst, hlast);
                }
                // Passa nella nuova cella
                code = ncode;
                x = nx;
                y = ny;
                break;
            }
            if (s == 4)
                stop = 1;
        }

        // Chiusura e smoothing 1+2+1
        res.push_back(res[0]);
        taint.push_back(taint[0]);
        for (int hp=0; hp<SM2; hp++)
        {
            P2d o = res[0];
            for (int i=1,n=res.size(); i<n-1; i++)
            {
				P2d pn = (o + 2*res[i] + res[i+1]) / 4;
				o = res[i];
                res[i] = pn;
            }
        }

        // Eventuale rimozione parti di testa e coda
        int i=0;
        int tsz = taint.size();
        if (td2>0)
        {
            for (int j=0; j<tsz; j++)
                if (!taint[i]) { if (++i == tsz) i = 0; }
                          else break;
            for (int j=0; j<tsz; j++)
                if (taint[i]) { if (++i == tsz) i = 0; }
                         else break;
        }
        if (i < tsz)
        {
            int i0 = i;
            for(;;)
            {
                Path2d nres;
                do
                {
                    nres.push_back(res[i]);
                    if (++i == tsz) i = 0;
                } while (i!=i0 && !taint[i]);

                // Check di lunghezza minima
                double L = nres.len();
                if (L > 10)
                {
                    // Aggiunge il percorso al risultato finale

                    // Semplificazione risultato
					simplify(nres, 0.05);
                    result.resize(1+result.size());
                    result.back().swap(nres);
                }
                while (i!=i0 && taint[i])
                    if (++i == tsz) i = 0;
                if (i == i0) break;
            }
        }
    }
}

} // Unnamed namespace

void calcPara(const std::vector<Path2d>& xys,
              double dd,
              std::vector<Path2d>& result)
{
    // Calcolo bounding box
    double x0=1E20, y0=1E20, x1=-1E20, y1=-1E20;
    for (int ii=0,nn=xys.size(); ii<nn; ii++)
    {
        const Path2d& path = xys[ii];
        for (int i=0,n=path.size(); i<n; i++)
        {
			P2d p = path[i];
            if (p.x<x0) x0=p.x; if (p.x>x1) x1=p.x;
            if (p.y<y0) y0=p.y; if (p.y>y1) y1=p.y;
        }
    }

    // Allocazione griglia virtuale
    int ix0 = int( x0 - dd - 4 );
    int iy0 = int( y0 - dd - 4 );
    int ix1 = int( x1 + dd + 5 );
    int iy1 = int( y1 + dd + 5 );
    CMatrix cm(ix1-ix0, iy1-iy0);

    // Calcolo griglia distanze
    for (int ii=0,nn=xys.size(); ii<nn; ii++)
    {
        const Path2d& path=xys[ii];
        for (int i=0,n=path.size(); i<n-1; i++)
        {
            cm.addEdge(path[i].x-ix0, path[i].y-iy0,
					   path[i+1].x-ix0, path[i+1].y-iy0, dd+2);
            if (i > 0)
                cm.addVertex(path[i-1].x-ix0, path[i-1].y-iy0,
                             path[i].x-ix0, path[i].y-iy0,
                             path[i+1].x-ix0, path[i+1].y-iy0, dd+2);
        }
        cm.addVertex(path[0].x-ix0, path[0].y-iy0, dd+2);
        cm.addVertex(path[path.size()-1].x-ix0, path[path.size()-1].y-iy0, dd+2);
    }

    // Ricerca intersezioni fra la griglia e le curve dist==dd
    double dd2 = dd*dd;
    HMap hm, vm;
    XSect *hfirst=0, *hlast=0, *vfirst=0, *vlast=0;
    for (int by=0,bi=0; by<cm.bdy; by++)
        for (int bx=0; bx<cm.bdx; bx++,bi++)
            if (cm.data[bi])
            {
                float *b = cm.data[bi];
                for (int iy=0; iy<CMatrix::BSZ; iy++)
                {
                    float *f = b + iy*CMatrix::BSZ;
                    int x = bx*CMatrix::BSZ + ix0;
                    int y = by*CMatrix::BSZ + iy0 + iy;
                    for (int ix=0; ix<CMatrix::BSZ-1; ix++)
                        if ((f[ix]>=dd2 && f[ix+1]<dd2) ||
                            (f[ix+1]>=dd2 && f[ix]<dd2))
                        {
                            double v0 = sqrt(f[ix]);
                            double v1 = sqrt(f[ix+1]);
                            hm.put(newXSect(x+ix, y, x + ix + (dd-v0)/(v1-v0),
                                            hfirst, hlast));
                        }
                    if (bx < cm.bdx-1 && cm.data[bi+1])
                    {
                        float *f2 = cm.data[bi+1] + iy*CMatrix::BSZ;
                        int ix = CMatrix::BSZ-1;
                        double v02 = f[ix];
                        double v12 = f2[0];
                        if ((v02>=dd2 && v12<dd2)||(v12>=dd2 && v02<dd2))
                        {
                            double v0 = sqrt(v02);
                            double v1 = sqrt(v12);
                            hm.put(newXSect(x + ix, y, x + ix + (dd-v0)/(v1-v0),
                                            hfirst, hlast));
                        }
                    }
                }
                for (int ix=0; ix<CMatrix::BSZ; ix++)
                {
                    float *f = b + ix;
                    int x = bx*CMatrix::BSZ + ix0 + ix;
                    int y = by*CMatrix::BSZ + iy0;
                    for (int iy=0; iy<CMatrix::BSZ-1; iy++,f+=CMatrix::BSZ)
                        if ((f[0]>=dd2 && f[CMatrix::BSZ]<dd2) ||
                            (f[CMatrix::BSZ]>=dd2 && f[0]<dd2))
                        {
                            double v0 = sqrt(f[0]);
                            double v1 = sqrt(f[CMatrix::BSZ]);
                            vm.put(newXSect(x, y + iy, y + iy + (dd-v0)/(v1-v0),
                                            vfirst, vlast));
                        }
                    if (by < cm.bdy-1 && cm.data[bi+cm.bdx])
                    {
                        float *f2 = cm.data[bi+cm.bdx] + ix;
                        int iy = CMatrix::BSZ-1;
                        double v02 = f[0];
                        double v12 = f2[0];
                        if ((v02>=dd2 && v12<dd2)||(v12>=dd2 && v02<dd2))
                        {
                            double v0 = sqrt(v02);
                            double v1 = sqrt(v12);
                            vm.put(newXSect(x, y + iy, y + iy + (dd-v0)/(v1-v0),
                                            vfirst, vlast));
                        }
                    }
                }
            }

    // Ricostruzione contorni chiusi e generazione output
    // Se l'input e' un singolo percorso aperto allora genera
    // un risultato composto da curve aperte eliminando le
    // parti vicine agli estremi
    P2d a = xys[0][0];
    P2d b = xys[0][xys.size()-1];
    double td2 = (xys.size()==1 && a!=b) ? dd*dd*1.01 : 0.0;
    getCycles(vm, vfirst, vlast,
              hm, hfirst, hlast,
              result,
              a, b, td2);
}

