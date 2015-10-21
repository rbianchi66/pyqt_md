%ModuleHeaderCode
    #include "isurf.h"
%End

class ISurf
{
public:
    ISurf(const std::vector< std::vector< P2d > >& pts,
          int ext_mode=1,
          int levels=4,
          int passes=8);
    P2d operator()(double ii, double jj) const;
};

