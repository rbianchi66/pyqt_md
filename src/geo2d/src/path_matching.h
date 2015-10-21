#if !defined(PATH_MATCHING_H)
#define PATH_MATCHING_H

#include <vector>
#include "geo2d.h"
#include <float.h>

struct match_result
{
    match_result() : A(X2d()), sse(DBL_MAX), max_se(DBL_MAX), n(-1), D(0.0) {}; //Invalid result
    match_result(X2d a, double sse, double max_se, int n, double D) : A(a), sse(sse), max_se(max_se), n(n), D(D) {};
    
    X2d A;
    X2d S;
    X2d S1;
    std::vector<P2d> pts;
    std::vector<P2d> vts;
    double sse;
    double max_se;
    int n;
    double D;
    double id;
};

/*
 * Dati pts e vts vettori di P2d, points_matching restituisce la miglior matrice di rototraslazione
 * per pts che minimizza la distanza euclidea tra le coppie (pts[i], vts[i])
 */
match_result points_matching(const std::vector<P2d>& pts, const std::vector<P2d>& vts, bool fast = false);

/*
 * Dati pts, vts vettori di P2d e dts vettore di double, points_matching restituisce la miglior matrice
 * di rototraslazione per pts che minimizza la distanza euclidea tra le coppie (pts[i], vts[i])
 * moltiplicata per il rispettivo peso dts[i]
 */
match_result points_matching(const std::vector<P2d>& pts, const std::vector<P2d>& vts, const std::vector<double>& dts, bool fast = false);

/*
 * Dati due path P, V tali che P.len() >= V.len().
 * La funzione pathOverPath restituisce la miglior matrice di rototraslazione per path1,
 * tale che la distanza euclidea dei punti corrispondeti tra P e V sia la minore possibile.
 * 
 * -------------------------------PUNTI CORRISPONDENTI-------------------------------------
 * Ogni punto del path piu' corto V avra' un punto corrispondente nel path piu' lungo P, ma
 * non vale necessariamente il viceversa!
 * 
 */
match_result pathOverPath(const Path2d& path1, const Path2d& path2, double res = 100.0);
match_result pathOverPath(const Path2d& path1, std::vector<double>& radius1, const Path2d& path2, std::vector<double>& radius2, double res = 100.0);

/*
 * come pathOverClosedPath, ma assume che a path1 corrisponda il path P e a path2 il path V.
 */
match_result __pathOverPath(const Path2d& path1, const std::vector<double>& w1, const Path2d& path2, const std::vector<double>& w2, double res, std::vector<match_result>* mrs = NULL);

/*
 * Miglior sovrapposizione tra un vector di path e un path chiuso
 */
match_result vecpathOverPath(const std::vector<Path2d>& vecpath, const Path2d& path2, double step = 100.0);
match_result vecpathOverPath(const std::vector<Path2d>& vecpath, const std::vector< std::vector<double> >& vecradius, const Path2d& path2, const std::vector<double>& radius2, double step);

std::vector<match_result> vecpathOverVecpath(const std::vector<Path2d>& vecpath1, const std::vector<Path2d>& vecpath2, double step = 15.0, double overlap = 100.0, double filter = 1.0);
std::vector<match_result> vecpathOverVecpath(const std::vector<Path2d>& vecpath1, const std::vector< std::vector<double> >& vecradius1, const std::vector<Path2d>& vecpath2, const std::vector< std::vector<double> >& vecradius2, double step = 15.0, double overlap = 100.0, double filter = 1.0);
std::vector<match_result> __vecpathOverVecpath(const std::vector<Path2d>& vecpath1, const std::vector< std::vector<double> >& vecradius1, const std::vector<Path2d>& vecpath2, const std::vector< std::vector<double> >& vecradius2, double step = 15.0, double overlap = 100.0, double filter = 1.0);

/*
 * Campiona un path con intervallo step_len
 */
std::vector<P2d> fixed_sampling(const Path2d& path, double step_len, double start_t = 0.0);

/*
 * Campiona un path prendendo npoints punti su questo a intervallo regolare.
 */
std::vector<P2d> uniform_sampling(const Path2d& path, int npoints, double* step_len = NULL, double start_t = 0.0, double end_t = -1.0);

/*
 * Recupera i pesi associati a dei punti su un path, partendo dal path e dai 
 * pesi associati ai punti che lo definiscono.
 */
std::vector<double> get_weights(const Path2d& path, const std::vector<double>& weights, const std::vector<P2d>& points);
#endif
