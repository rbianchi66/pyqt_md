#ifndef _GEO2D_MINIMUMBBOX_H_
#define _GEO2D_MINIMUMBBOX_H_

#include <vector>
#include "geo2d.h"

class MinimumBB {
    public:
        MinimumBB(const std::vector<P2d>& pts, int refine_step_size=1, double angle=0.0,
                  bool refine_box=true);
        void getRect(P2d *a, P2d *b, P2d *c, P2d *d);
        double bestAngle();
        bool test(P2d p);
        P2d center();

    private:
        void fix (const std::vector<P2d>& pts);
        void refine(const std::vector<P2d>& pts, double angle, int step);
        void bestArea(int max_val, int offset);
        void createRect();
        void createRectWithAngle(const std::vector<P2d>& pts, double angle);
        void createRectFindingAngle(const std::vector<P2d>& pts);

        void findDimension(double a, int offset, const std::vector<P2d>& pts);

    private:

        enum {NA4=90, NA=360};
        std::vector<double> r;
        std::vector<double> perpendicular_r;
        std::vector<double> angles;
        P2d pts[4];
        int best_angle_index;
        double best_angle;
        double area;
        int refine_step_size;
        bool refine_box;
};

#endif
