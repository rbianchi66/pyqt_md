#if !defined(ISURF_H_INCLUDED)
#define ISURF_H_INCLUDED

#include "geo2d.h"
#include <vector>

class ISurf
{
public:
    ISurf(const std::vector< std::vector< P2d > >& pts,
          int ext_mode=1,
          int levels=4,
          int passes=8);
    P2d operator()(double ii, double jj) const;

private:
    std::vector< std::vector<P2d> > pts;
    std::vector< std::vector<P2d> > idata;
    int levels, passes;
};


#endif
