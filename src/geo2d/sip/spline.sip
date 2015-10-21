%ModuleHeaderCode
    #include "spline.h"
%End

Path2d splineX(const std::vector<P2d>& pts,
               std::vector<int> *indices /Out/ = NULL,
               int levels=6,
               bool closed=false,
               int iters=20,
               double kk=1.62);

Path2d hermite(const P2d& p0, const P2d& p1,
               const P2d& t0, const P2d& t1,
               int np);

std::vector<P2d> ispline(const Path2d& path, double eps=0.05, unsigned maxpt=0);
