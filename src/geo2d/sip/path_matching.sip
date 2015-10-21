%ModuleHeaderCode
    #include "path_matching.h"
%End

struct match_result
{
    match_result();
    match_result(X2d a, double sse, double max_se, int n, double D);
    
    X2d A;
    X2d S;
    X2d S1;
    std::vector<P2d> pts;
    std::vector<P2d> vts;
    double sse;
    double max_se;
    int n;
    double D;
    double id;
};

match_result points_matching(const std::vector<P2d>& pts, const std::vector<P2d>& vts, bool fast = false);
match_result points_matching(const std::vector<P2d>& pts, const std::vector<P2d>& vts, const std::vector<double>& dts, bool fast = false);

match_result pathOverPath(const Path2d& path1, const Path2d& path2, double res = 100.0);
match_result pathOverPath(const Path2d& path1, std::vector<double>& radius1, const Path2d& path2, std::vector<double>& radius2, double res = 100.0);

match_result vecpathOverPath(const std::vector<Path2d>& vecpath, const Path2d& path2, double step);
match_result vecpathOverPath(const std::vector<Path2d>& vecpath, const std::vector< std::vector<double> >& vecradius, const Path2d& path2, const std::vector<double>& radius2, double step = 100.0);

std::vector<match_result> vecpathOverVecpath(const std::vector<Path2d>& vecpath1, const std::vector<Path2d>& vecpath2, double step = 15.0, double overlap = 100.0, double filter = 1.0);
std::vector<match_result> vecpathOverVecpath(const std::vector<Path2d>& vecpath1, const std::vector< std::vector<double> >& vecradius1, const std::vector<Path2d>& vecpath2, const std::vector< std::vector<double> >& vecradius2, double step = 15.0, double overlap = 100.0, double filter = 1.0);

std::vector<P2d> fixed_sampling(const Path2d& path, double step_len, double start_t = 0.0);
SIP_PYTUPLE uniform_sampling(const Path2d& path, int npoints, double* step_len = NULL, double start_t = 0.0, double end_t = -1.0);
%MethodCode
    double r1 = 0.0;
    std::vector<P2d>* r0 = new std::vector<P2d>( uniform_sampling(*a0, a1, &r1) );
    // a2 is a double, that SIP probably tries to "backport" to the passed argument
    // even if it isn't like that, it's better than leaving it uninitialized as it was
    // previously (see the generated code)
    a2 = r1;
    const sipMappedType *mt = sipFindMappedType("std::vector<P2d>");
    PyObject *pobj = sipConvertFromMappedType(r0, mt, NULL);
    
    sipRes = PyTuple_New(2);
    PyTuple_SetItem(sipRes, 0, pobj);
    PyTuple_SetItem(sipRes, 1, PyFloat_FromDouble(r1));
%End

std::vector<double> get_weights(const Path2d& path, const std::vector<double>& weights, const std::vector<P2d>& points);
