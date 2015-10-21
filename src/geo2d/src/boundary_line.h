#ifndef BOUNDARY_LINE_H_INCLUDED
#define BOUNDARY_LINE_H_INCLUDED

struct BoundaryLine
{
    double x0, y0, x1, y1;
    BoundaryLine *next; // Hash collision
    BoundaryLine(double x0, double y0, double x1, double y1)
        : x0(x0), y0(y0), x1(x1), y1(y1), next(0)
    {
    }
};

#endif

