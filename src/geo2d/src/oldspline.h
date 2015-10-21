#if !defined(OLDSPLINE_H_INCLUDED)
#define OLDSPLINE_H_INCLUDED

#include "geo2d.h"
#include <vector>

/*
 *  Spline di Calcad.
 *  Calcolo spline ricorsiva. Considera l'errore massimo ammesso (quello
 *  reale e` inferiore, essendo calcolato sul poligono di ingombro) con
 *  err >= 1.
 *
 * Il valore di ritorno è il Path corrispondente alla spline calcolata,
 * e opzionalmente una lista di indici numerici lunga quanto la lista dei 
 * punti passata che indica le posizioni dei punti della spline associati 
 * ai punti passati.
 */
Path2d oldSplineX(const std::vector<P2d>& pts,
                  std::vector<int> *indices = NULL);

#endif /* OLDSPLINE_H_INCLUDED */
