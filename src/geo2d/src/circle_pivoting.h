#if !defined(CIRCLE_PIVOTING_H_INCLUDED)
#define CIRCLE_PIVOTING_H_INCLUDED

#include "geo2d.h"
#include <vector>

//
// Dato un insieme di punti restituisce il sottoinsieme
// che si incontra "appoggiando" un disco di raggio r al punto
// piu' a destra e facendo "rotolare" questo disco in
// senso orario alla ricerca di un successivo punto di appoggio.
// La ricerca si interrompe quando il disco in questa procedura
// incontra di nuovo un punto gia' incontrato in precedenza.
//
// Se l'insieme di punti e' sufficientemente "denso" rispetto
// al raggio da impedire che il disco "entri" allora la lista
// restituita definisce un "contorno" dell'insieme passato.
//
void circle_pivoting(std::vector<P2d>& pts, double r);

#endif
