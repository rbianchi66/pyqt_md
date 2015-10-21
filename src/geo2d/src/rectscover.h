#ifndef _rectscover_h_
#define _rectscover_h_
#include "geo2d.h"
#include "poly2d.h"

// rettangolo coprente
struct coveringRect{
    coveringRect() {};
    coveringRect(double x, double y, double w, double h) : x(x), y(y), w(w), h(h) {};
    double x;
    double y;
    double w;
    double h;
};

// Data un'area (sottoforma di vettore di Path2d) restituisce [nrects] rettangoli coprenti.
// Se l'area di un rettangolo coprente ritornato e' 0, allora non e' detto che il rettangolo coprente
// sia contenuto all'interno dell'area
// Se il parametro safety e' impostato a true, il rettangolo coprente non degenere e' sempre
// strettamente contenuto nell'area passata in input. Altrimenti non e' garantito
std::vector< coveringRect > rects_cover(const std::vector<Path2d>& v, int nrects, int start_res=10, bool safety=true);
std::vector< coveringRect > rects_cover(const Path2d& p, int nrects, int start_res=10, bool safety=true);

#endif
