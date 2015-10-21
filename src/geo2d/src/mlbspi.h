#if !defined(MLBSPI_H_INCLUDED)
#define MLBSPI_H_INCLUDED

#include "ndvect.h"

class MLBSplineInterpolator
{
    public:
        MLBSplineInterpolator(int levels);
        void clear();
        void addPoint(const NDVector& p0, const NDVector& p1);
        NDVector calc(const NDVector& p);

    private:
        NDVector ra, rb, rc;
        int levels;
        std::vector<NDVector> xy;
        std::vector<NDVector> z, zr;

        void calc();
        void invalidate();

        double x0, y0, x1, y1;
        std::vector< std::vector< std::vector<NDVector> > > delta;
        std::vector< std::vector< std::vector<double> > > omega;
};

#endif
