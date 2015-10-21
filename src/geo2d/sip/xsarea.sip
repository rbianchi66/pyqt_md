%ModuleHeaderCode
    #include "xsarea.h"
%End

double xsarea(const Path2d& path1, const X2d& X1,
              const Path2d& path2, const X2d& X2);

double xsarea(const std::vector<Path2d>& path1, const X2d& X1,
              const std::vector<Path2d>& path2, const X2d& X2);
