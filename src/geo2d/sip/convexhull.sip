%ModuleHeaderCode
#include "convexhull.h"
%End
Path2d convex_hull( const Path2d& V);
Path2d convex_hull( const std::vector<Path2d>& V);
Path2d convex_hull( std::vector<P2d>& V);
Path2d convex_hull_presorted( const std::vector<P2d>& V);

Path2d regularize(const Path2d& path);
Path2d quasi_convex_hull( const std::vector<Path2d>& paths, const double alpha);
