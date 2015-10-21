#if !defined(AREAS_H_INCLUDED)
#define AREAS_H_INCLUDED

#include "geo2d.h"
#include "boundary_line.h"

//
// La funzione implementa l'algoritmo descritto in
// http://support.comelz.com/default.php?W46
//
// Input:
//     lines      lista di linee di confine (incluso bordo pelle)
//     markers    indicatori di qualita' dell'area (coppie qualita'/punto)
//     defects    indicatori linee difettose (coppie qualita'/path)
//     qa, qb     valori di default per aree chiuse senza marker
//     k          coefficiente pesatura perimetro
//
// Output:
//     lista di aree chiuse (ogni area definita da una lista di path),
//     L'elemento i-esimo e' l'area di qualita' (i+1).
//

struct QualityMarker
{
    int q;
    P2d p;
    QualityMarker() {}
    QualityMarker(int q, const P2d& p) : q(q), p(p) {}
};

struct Defect
{
    int q;
    Path2d p;
    int size;
    Defect(int q, const Path2d& p, int size) : q(q), p(p), size(size) {}
};

// Ricava i contorni dell'area che racchiude il colore <color> (ed eventualmente
// tutti i colori minori) nell'immagine <img> di larghezza <w> con origine in
// <xx0, yy0> e li mette in <p>. Il comportamento cambia in base alla classe
// scelta come color comparator.
template <class ColorComparator>
void marching_squares(int color, int xx0, int yy0,
    std::vector<int>& img, int w, std::vector<Path2d>& p)
{
    int h = int(img.size() / w);
    std::vector<BoundaryLine> b;
    ColorComparator compare;

    for (int y = 1; y < h-1; y++) {
        for (int x = 1; x < w-1; x++) {
            int va = img[y*w+x];
            int vb = img[y*w+x+1];
            int vc = img[y*w+x+w];
            int vd = img[y*w+x+w+1];

            int code=(compare(va, color) ? 8 : 0)+
                     (compare(vb, color) ? 4 : 0)+
                     (compare(vc, color) ? 2 : 0)+
                     (compare(vd, color) ? 1 : 0);

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
                out.push_back(P2d(L->x0 + xx0, L->y0 + yy0));
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
        }
    }
}
std::vector<Path2d> unionAreaPaths(const std::vector<std::vector<Path2d> >& regions);
double unionAreaValue(const std::vector<std::vector<Path2d> >& regions);

std::vector< std::vector<Path2d> > areas(
                          const std::vector<Path2d>& boundary,
                          const std::vector<Path2d>& lines,
                          const std::vector<QualityMarker>& markers,
                          const std::vector<Defect>& defects,
                          int qa, int qb,
                          double k,
                          bool dilate = true);

#endif
