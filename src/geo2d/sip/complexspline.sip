%ModuleHeaderCode
    #include "complexspline.h"
%End

std::vector<P2d> complexSplineXC(const std::vector<P2d>& pts,
                      std::vector<int> spigoli,
                      int levels=6,
                      bool closed=false,
                      int iters=20,
                      double kk=1.62,
                      int tipo_spline=0,
                      std::vector<int> *indices /Out/ = NULL);
