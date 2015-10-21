%ModuleHeaderCode
    #include "geo2d.h"
    #include "bbox.h"
%End

class OrientedBB /PyName=OrientedBB, Factory/
{
    public:
        OrientedBB(const std::vector<P2d>& pts, double extra);
        OrientedBB(const std::vector<OrientedBB *>& bb);

        bool test (const P2d& p);

        bool xsect(const OrientedBB& other);
        bool contains(const OrientedBB& other);

        void merge (const std::vector<OrientedBB *>& bb);

        double getArea();
        int getAngle();
        void rotate (int angle);

        void getRect(P2d *a /Out/, P2d *b /Out/, P2d *c /Out/, P2d *d /Out/);
};

