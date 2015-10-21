%ModuleHeaderCode
    #include "simplify.h"

    inline Path2d _simplify(Path2d path, double eps=0.01)
    {
        return simplify(path, eps);
    }

    inline std::vector<P2d> _simplify(std::vector<P2d> pts, double eps=0.01)
    {
        Path2d p2d(pts);
        simplify(p2d, eps);
        p2d.swap(pts);
        return pts;
    }

%End

Path2d _simplify(Path2d p, double eps=0.01) /PyName=simplifyf/;
Path2d simplify_contained(Path2d path, const int simplify_type = 0, const int iterations = 1, const double eps = 0.0);

std::vector<P2d> _simplify(std::vector<P2d> pts, double eps=0.01) /PyName=simplifyf/;

