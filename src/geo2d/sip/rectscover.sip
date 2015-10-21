%ModuleHeaderCode
    #include "rectscover.h"
%End

struct coveringRect{
    coveringRect();
    coveringRect(double x, double y, double w, double h);
    double x;
    double y;
    double w;
    double h;
};

std::vector< coveringRect > rects_cover(const std::vector<Path2d>& v, int nrects, int start_res=10, bool safety=true);
std::vector< coveringRect > rects_cover(const Path2d& p, int nrects, int start_res=10, bool safety=true);
