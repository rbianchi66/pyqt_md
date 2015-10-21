#ifndef UNISCI_H
#define UNISCI_H

#include "geo2d.h"

/*
 * Cerca l'unione migliore tra i due `TrattoPath` (`A`, `a1`, `a2`) e
 * (`B`, `b1`, `b2`) nelle vicinanze dei rispettivi parametri `ta` e `tb`.
 * A meno di path degenere, la funzione non può fallire: torna sempre l'unione
 * considerata "migliore" (anche se i tratti sono lontanissimi).
 *
 * Ritorna una tupla ``(pi, ai, bi)``, contenente il punto di unione e i
 * parametri sui relativi path dove far cominciare l'unione. Si noti in
 * particolare che ``pi`` può essere distante a piacere, e spesso non
 * corrisponde né a ``A(ai)``, né a ``B(bi)``, anche nei casi di unione
 * per intersezione.
 *
 * La funzione assume che i tratti siano orientati in modo che la parte da
 * conservare di ogni tratto sia quella dalla parte di t0. In altre parole,
 * l'unione dovrà conservare i segmenti (A.t0, ai) e (B.t0, bi).
 *
 * I prolungamenti non vengono mai considerati. I parametri ritornati
 * (``ai`` e ``bi``) sono sempre contenuti all'interno del range originale
 * dei tratti.
 *
 * :todo: Documentare l'algoritmo nel dettaglio.
 * :todo: Esporre l'angolo discriminante come argomento di questa funzione.
 *
 */
P2d unisci(Path2d A, double a1, double a2,
           Path2d B, double b1, double b2,
           double ta, double tb,
           double *ai=NULL, double *bi=NULL);


#endif /* UNISCI_H */
