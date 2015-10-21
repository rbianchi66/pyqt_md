%ModuleHeaderCode
    #include "offset.h"

    inline std::vector<Path2d> offset(std::vector<P2d> &pts, const std::vector<double>& d, int tipo, double soglia_unibili)
    {
        Path2d p2d(pts);
        return offset(p2d, d, tipo, soglia_unibili);
    }

%End

Path2d raccordo(P2d o, P2d p1, P2d p2, double delta);

Path2d para_smussa(const Path2d &pts_in, double dist, bool chiusa) /PyName=para_smussaf/;
Path2d para_smussa(const std::vector<P2d> &pts_in, double dist, bool chiusa) /PyName=para_smussaf/;

Path2d para(const Path2d &pts_in, double dist, bool chiusa) /PyName=paraf/;
Path2d para(const std::vector<P2d> &pts_in, double dist, bool chiusa) /PyName=paraf/;

std::vector<Path2d> offset(Path2d &pts, const std::vector<double>& d, int tipo, double soglia_unibili) /PyName=offsetf/;
std::vector<Path2d> offset(std::vector<P2d> &pts, const std::vector<double>& d, int tipo, double soglia_unibili) /PyName=offsetf/;

P2d nd(const Path2d &pts, int i, double d, bool chiusa);

std::vector<Path2d> enlarge_area(std::vector<Path2d> paths, double dist, double simplifyEps=0.01);
