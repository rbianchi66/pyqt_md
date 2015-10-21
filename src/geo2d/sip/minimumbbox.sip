%ModuleHeaderCode
    #include "geo2d.h"
    #include "minimumbbox.h"
%End

class MinimumBB /PyName=MinimumBB, Factory/
{
    public:
        MinimumBB(const std::vector<P2d>& pts, int refine_step_size=1,
                  double angle=0.0, bool refine_box=true);

        void getRect(P2d *a /Out/, P2d *b /Out/, P2d *c /Out/, P2d *d /Out/);

        double bestAngle();

        bool test(P2d p);

        P2d center();

};
