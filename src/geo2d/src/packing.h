#ifndef _pack_h_
#define _pack_h_
#include "geo2d.h"
#include "poly2d.h"
#include "convexhull.h"

#ifdef _MSC_VER
#if _MSC_VER <= 1310
#define min std::_cpp_min
#define max std::_cpp_max
#else
using std::min;
using std::max;
#endif
#else
using std::min;
using std::max;
#endif

// Struttura utilizzata per ritornare i risultati d'impaccamento
// dalla funzione di packing-tutti-con-tutti
struct Packing
{
    Packing() {};
    Packing(int p1, int p2, X2d px1, X2d px2, double s) : id1(p1), id2(p2), x1(px1), x2(px2), score(s) {};

    int id1;
    int id2;
    X2d x1;
    X2d x2;
    double score;
};

// CLASSE DI COMPARAZIONE DI DEFAULT
//
// Se se ne usa una propria e' importante che sia veloce!
// Il functor viene chiamato per ogni accostamento possibile:
//        (rotazioni del primo pezzo) x
//        (rotazioni del secondo pezzo) x
//        (scanline del primo pezzo+scanline del secondo pezzo)
// UN VALORE DI RITORNO MINORE COSTITUISCE UN RISULTATO MIGLIORE PER IL PACKING
struct BBoxAreaCompare
{
    // PARAMETRI DISPONIBILI PER IL CALCOLO DELLO SCORE:
    // h1: Altezza (in numero di scanlines) del primo pezzo
    // w1: Larghezza del primo pezzo
    // h2: Altezza (in numero di scanlines) del secondo pezzo
    // w2: Larghezza del secondo pezzo
    // dx: Offset (sulle ascisse) del secondo pezzo rispetto al primo
    // dy: Offset (sulle ordinate) del secondo pezzo rispetto al primo
    // p1: Geometria del primo pezzo
    // x1: Matrice di rotazione del primo pezzo
    // p2: Geometria del secondo pezzo
    // x2: Matrice di rotazione del secondo pezzo
    inline double operator()(int h1, int w1, int h2, int w2, int dx, int dy, const std::vector<Path2d>& p1, const X2d x1, const std::vector<Path2d>& p2, const X2d x2)
    {
        // Area della bounding box risultante (calcolata in base alle scanlines)
        return double((max(h1, dy+h2) - min(dy, 0)) * max(w1, w2 + dx)) / double(w1*h1+w2*h2);
    }
};

// Altro esempio: COMPARAZIONE IN BASE ALLA CONVEX HULL DEI PEZZI IMPACCATI
struct ConvexHullCompare
{
    inline double operator()(int h1, int w1, int h2, int w2, int dx, int dy, const std::vector<Path2d>& p1, X2d x1, const std::vector<Path2d>& p2, X2d x2)
    {
        const int sz1 = p1.size(), sz2 = p2.size();
        x2 *= X2d::xlate(P2d(dx, dy));

        std::vector<Path2d> join(sz1+sz2);
        for (int i=0; i<sz1; ++i)
        {
            join[i] = p1[i]*x1;
        }
        for (int i=0; i<sz2; ++i)
        {
            join[i+sz1] = p2[i]*x2;
        }
        return area(convex_hull(join));
    };
};


// Packing con comparator personalizzato
template <class C> double pack(const std::vector<Path2d>& path1, X2d& x1, const std::vector<Path2d>& path2, X2d& x2, int rots, C comparator);

// Packing con comparator di default = BBoxAreaCompare
double pack(const std::vector<Path2d>& path1, X2d& x1, const std::vector<Path2d>& path2, X2d& x2, int rots);

// **DEPRECATED?** Packing di un'area con una serie di aree
int pack(const std::vector<Path2d>& path1, X2d& x1, std::vector< std::vector<Path2d> >& other_paths, X2d& x2, int rots);

// Esegue il packing di "tutti con tutti": O(n^2) sul numero di pezzi
std::vector< Packing > pack(std::vector< std::vector<Path2d> >& paths, int rots);

// Versione con comparator personalizzato
template< class C > std::vector< Packing > pack_t(std::vector< std::vector<Path2d> >& paths, int rots, C comparator);

#endif
