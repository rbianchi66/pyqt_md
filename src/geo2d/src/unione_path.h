#ifndef UNIONEPATH_H
#define UNIONEPATH_H

#include "geo2d.h"
#include "offset.h"


/*
 * Funzione per unire un insieme di path, unendo di volta in volta
 * gli estremi più vicini e iterando finché non è rimasta una sola linea o
 * finché non è più possibile unire linee.
*/
path2d_array unione_path(path2d_array &apts, double eps=1.0);


#endif /* UNIONEPATH_H */
