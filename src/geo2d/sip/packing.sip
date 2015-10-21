%ModuleHeaderCode
    #include "packing.h"
%End

struct Packing
{
    Packing();
    Packing(int p1, int p2, X2d px1, X2d px2, double s);
    
    int id1;
    int id2;
    X2d x1;
    X2d x2;
    double score;
};

double pack(const std::vector<Path2d>& path1, X2d& x1, const std::vector<Path2d>& path2, X2d& x2, int rots);
int pack(const std::vector<Path2d>& path1, X2d& x1, std::vector< std::vector<Path2d> >& other_paths, X2d& x2, int rots);
std::vector< Packing > pack(std::vector< std::vector<Path2d> >& paths, int rots);
