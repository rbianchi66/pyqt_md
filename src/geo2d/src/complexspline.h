#if !defined(COMPLEXSPLINE_H_INCLUDED)
#define COMPLEXSPLINE_H_INCLUDED

#include "geo2d.h"
#include <vector>

/*
 * Calcola una linea complessa:
 * pts           lista dei punti da interpolare (almeno 2)
 * levels=6      numero livelli di suddivisione
 * closed=false  true se la spline e' chiusa
 * iters=20      numero iterazioni per calcolo interpolazione
 * kk=1.62       coefficiente di rilassamento
 *
 * Restituisce una lista di punti e una lista di tanti interi quanti sono
 * gli elementi di pts che contiene l'indice del punto risultante associato
 * per ciascuno dei punti in ingresso.
 *
 * In input, una linea chiusa deve essere passata *senza* duplicazione del
 * primo/ultimo punto. Nel valore di ritorno, invece, il punto risulterà
 * duplicato. FIXME: potremmo correggere questa asimmetria.
 *
 * La lista di mapping ritornata è sicuramente ordinata, ma può contenere
 * duplicati: infatti, è possibile che due punti geometricamente sovrapposti
 * nella linea di input corrispondano ad un solo punto nella spline di output.
 */
std::vector<P2d> complexSplineXC(const std::vector<P2d>& pts,
                       std::vector<int> spigoli,
                       int levels=6,
                       bool closed=false,
                       int iters=20,
                       double kk=1.62,
                       int tipo_spline=0,
                       std::vector<int> *indices=NULL);

#endif /* COMPLEXSPLINE_H_INCLUDED */
