#include "memorytest.h"
#include <cstdlib>

std::map<int, std::vector<Path2d> > mapvecpath_test(int sz, int path_sz)
{
    std::map<int, std::vector<Path2d> > ret;
    for (int i = 0; i < sz; ++i) // per ogni entry
    {
        std::vector<Path2d> nvp(1); // 1 solo path
        for (int j = 0; j < path_sz; ++j) // mille punti
            nvp[0].push_back(P2d(rand(), rand()));
        ret[i] = nvp;
    }
    return ret;
}

std::vector< std::vector<Path2d> > vecvecpath_test(int sz, int path_sz)
{
    std::vector< std::vector<Path2d> > ret;
    for (int i = 0; i < sz; ++i)
    {
        std::vector<Path2d> nvp(1);
        for (int j = 0; j < path_sz; ++j)
            nvp[0].push_back(P2d(rand(), rand()));
        ret.push_back(nvp);
    }
    return ret;
}

std::vector<Path2d> vecpath_test(int sz, int path_sz)
{
    std::vector<Path2d> ret;
    for (int i = 0; i < sz; ++i)
    {
        Path2d pa;
        for (int j = 0; j < path_sz; ++j)
            pa.push_back(P2d(rand(), rand()));
        ret.push_back(pa);
    }
    return ret;
}

