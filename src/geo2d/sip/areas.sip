%ModuleHeaderCode
#include "areas.h"
%End

struct QualityMarker
{
    int q;
    P2d p;
    QualityMarker(int q, const P2d& p);
};

struct Defect
{
    int q;
    Path2d p;
    Defect(int q, const Path2d& p, int size);
};

std::vector<Path2d> unionAreaPaths(const std::vector<std::vector<Path2d> >& regions);
double unionAreaValue(const std::vector<std::vector<Path2d> >& regions);

std::vector< std::vector<Path2d> > areas(
                          const std::vector<Path2d>& boundary,
                          const std::vector<Path2d>& lines,
                          const std::vector<QualityMarker>& markers,
                          const std::vector<Defect>& defects,
                          int qa, int qb,
                          double k,
                          bool dilate = true);

