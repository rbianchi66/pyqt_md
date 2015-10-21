#if !defined(DPARA_H_INCLUDED)
#define DPARA_H_INCLUDED

#include "geo2d.h"
#include <vector>

// passo della griglia = 1.0, opportuno che il passo della griglia
// sia minore di dd (es. dd/4)
// I vector sono sequenze di x ed y.
//    x1 y1 x2 y2 ... xn yn
//
// Sempre bilaterale!!!
//
void calcPara(const std::vector<Path2d>& xys,
              double dd,
              std::vector<Path2d>& result);

#endif
