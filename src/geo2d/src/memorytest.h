#if !defined(MEMORYTEST_H)
#define MEMORYTEST_H

#include "geo2d.h"
#include <map>
std::map<int, std::vector<Path2d> > mapvecpath_test(int sz, int path_sz);
std::vector< std::vector<Path2d> > vecvecpath_test(int sz, int path_sz);
std::vector<Path2d> vecpath_test(int sz, int path_sz);
#endif

