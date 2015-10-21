#ifndef _GEO2D_BBOX_H_
#define _GEO2D_BBOX_H_

#include <vector>
#include "geo2d.h"

class OrientedBB {
    public:
        OrientedBB(const std::vector<P2d>& pts, double extra);
        OrientedBB(const std::vector<OrientedBB *>& bb);

        bool test (const P2d& p) const;

        void merge (const std::vector<OrientedBB *>& bb);

        bool xsect(const OrientedBB& other) const;

        double getArea (void) const { return area; }
        int getAngle (void) const { return best_angle; }

        void rotate (int angle);

        void getRect (P2d *a, P2d *b, P2d *c, P2d *d) const;

        bool contains(const OrientedBB& other) const {
            P2d a, b, c, d;
            other.getRect(&a, &b, &c, &d);
            return(test(a) && test(b) && test(c) && test(d));
        }

    private:
        void fix (int angle=-1);

    private:
        enum { NA4=90, NA=360 };
        double r[NA];     // Valori di distanza <p, d_i> al variare dei versori d_i
        int best_angle;   // Angolo bbox di area minima
        double xa, ya, xb, yb, xc, yc, xd, yd;   // BBox esplicito
        double area;      // Area bbox
};

#endif /* !_GEO2D_BBOX_H_ */

