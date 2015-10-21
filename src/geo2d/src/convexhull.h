#ifndef _convexhull_h_
#define _convexhull_h_
#include <algorithm>
#include <vector>
#include <math.h>
#include "geo2d.h"

// Calcola la Convex Hull di un poligono semplice (ovvero senza intersezioni) in O(n)
// http://softsurfer.com/Archive/algorithm_0203/algorithm_0203.htm#simpleHull_2D()
Path2d convex_hull( const Path2d& V );

// Calcola la Convex Hull di un insieme di punti in O(nlogn)
// http://softsurfer.com/Archive/algorithm_0109/algorithm_0109.htm#MC%20Pseudo-Code
// Se i punti sono già ordinati per x crescenti (e in caso di x uguali, per y crescenti)
// allora si puo' chiamare la convex_hull_presorted
Path2d convex_hull( const std::vector<Path2d>& V);
Path2d convex_hull( std::vector<P2d>& V );

Path2d convex_hull_presorted( const std::vector<P2d>& V );


Path2d regularize(const Path2d& path);
// alpha is the required angle for convexity check, alpha = PI is the standard convex hull
Path2d quasi_convex_hull(const std::vector<Path2d>& paths, const double alpha = PI);

#endif
