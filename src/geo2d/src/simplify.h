#ifndef GEO2D_SIMPLIFY_H_INCLUDED
#define GEO2D_SIMPLIFY_H_INCLUDED

/**
 * Simpilifies the given path
 *
 * @param[in,out]   pts     Path to simplify, processed in-place
 * @param[in]       eps     Epsilon
 *
 * @return %pts, edited in-place
 */
Path2d& simplify(Path2d& pts, double eps=0.01);

/*
 * Semplifica un path in modo tale che se simplify_type e'
 * 0 - Il poligono generato contiene il path dato
 * 1 - Il poligono generato e' contenuto nel path dato
 * 2 - Il poligono generato dipende dall'ordinamento dei vertici,
 *     risparmiando il calcolo dell'area.
 *     (senso orario: caso 0, senso antiorario: caso 1)
 */
Path2d& simplify_contained(Path2d& path, const int simplify_type = 0, const int iterations = 1, const double eps = 0.0);

#endif
