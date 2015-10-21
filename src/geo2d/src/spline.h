 #if !defined(SPLINE_H_INCLUDED)
#define SPLINE_H_INCLUDED

#include "geo2d.h"
#include <vector>

/*
 * Calcolo spline interpolante:
 * pts           lista dei punti da interpolare (almeno 2)
 * levels=6      numero livelli di suddivisione
 * closed=false  true se la spline e' chiusa
 * iters=20      numero iterazioni per calcolo interpolazione
 * kk=1.62       coefficiente di rilassamento
 *
 * Il valore di ritorno è il Path corrispondente alla spline calcolata,
 * e opzionalmente una lista di indici numerici lunga quanto la lista dei
 * punti passata che indica le posizioni dei punti della spline associati
 * ai punti passati.
 */
Path2d splineX(const std::vector<P2d>& pts,
               std::vector<int> *indices=NULL,
               int levels=6,
               bool closed=false,
               int iters=20,
               double kk=1.62);

/*
 * Genera un tratto di hermite di np punti tra i punti p0, p1
 * con tangenti rispettive t0, t1.
 */
Path2d hermite(const P2d& p0, const P2d& p1,
               const P2d& t0, const P2d& t1, int np);


/*
 * Calcola i punti base di una linea spline senza spigoli che approssima i
 * punti passati in pts con precisione eps e non superando il numero di punti
 * base specificato da maxpt (se maxpt==0 non ci sono limiti)
 */
std::vector<P2d> ispline(const Path2d& path, double eps=0.05, unsigned maxpt=0);

#endif /* SPLINE_H_INCLUDED */
