%ModuleHeaderCode
    #include "geo2d.h"
    #include "poly2d.h"
%End

struct BlocksInfo
{
    int num;
    std::vector<Path2d> paths;
    std::vector<Path2d> grid;
    BlocksInfo(int num, std::vector<Path2d> paths, std::vector<Path2d> grid);
};

BlocksInfo blocks(const std::vector<Path2d>& boundaries, double side, bool oddeven = true);
