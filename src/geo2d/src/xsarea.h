#if !defined(XSAREA_H_INCLUDED)
#define XSAREA_H_INCLUDED

#include "geo2d.h"

// Questa funzione calcola e restituisce la misura dell'area di sovrapposizione
// di due poligoni espressi come Path2d, ai quali viene applicata una matrice
// di trasformazione

double xsarea(const Path2d& path1, const X2d& X1,
              const Path2d& path2, const X2d& X2);

// Stessa cosa della funzione precedente, ma accetta forme definite da
// liste di percorsi (in pratica gestisce pezzi bucati).
double xsarea(const std::vector<Path2d>& path1, const X2d& X1,
              const std::vector<Path2d>& path2, const X2d& X2);

#endif
