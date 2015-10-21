%ModuleHeaderCode
    #include "intersect.h"
%End

bool intersect(const Path2d& path1, const Path2d& path2, const bool overlap = true, const double eps = 0.00001);
bool intersect(const Path2d& path1, const std::vector<Path2d>& paths2, const bool overlap = true, const double eps = 0.00001);
bool intersect(const std::vector<Path2d>& paths1, const std::vector<Path2d>& paths2, const bool overlap = true, const double eps = 0.00001);
bool intersect(const P2d &a, const P2d &b, const P2d &c, const P2d &d);
bool intersect(const Path2d& path);

std::vector<P2d> intersections(const Path2d& path1, const Path2d& path2);
std::vector<P2d> self_intersections(const Path2d& path);
