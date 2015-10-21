#if !defined(EARCUT_H_INCLUDED)
#define EARCUT_H_INCLUDED

#include "geo2d.h"
#include <vector>

//
// Dato in input un poligono senza auto-intersezioni
// anche non convesso genera una sua triangolazione.
// Il poligono e' passato come una sequenza di 2n double
// x[0], y[0], x[1], y[1], ..., x[n-1], y[n-1]; il risultato
// viene scritto in result come sequenza di indici, con 3
// interi per ogni triangolo per ciascuno degli n-2
// triangoli generati.
//
void triangulate(const Path2d& p, std::vector<int>& result);

#endif
