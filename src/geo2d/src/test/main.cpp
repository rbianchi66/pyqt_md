#include <stdio.h>
#include <geo2d.h>
#include <geo3d.h>
#include <bump.h>
#include <dpara.h>
#include <dxgrid.h>
#include <earcut.h>
#include <inside.h>
#include <offset.h>
#include <oldspline.h>
#include <placement.h>
#include <spline.h>
#include <unione_path.h>
#include <unisci.h>
#include <xsarea.h>
#include <vector>
#include <string>

int main()
{
    // P2d
    P2d p(10, 10);
    P2d n = P2d::none();

    p = p + p;
    n -= n;

    printf("\n%f %f", p.x, p.y);
    printf("\n%f %f", n.x, n.y);

    printf("\n%f", len2(p));

    // X2d
    X2d m;

    printf("\n%f %f", m.a.x, m.a.y);
    printf("\n%f %f", m.b.x, m.b.y);

    p *= m;
    printf("\n%f %f", p.x, p.y);

    // Path2d
    std::vector<P2d> vt;
    vt.push_back(p);
    vt.push_back(p + P2d(10, 0));
    vt.push_back(p + P2d(10, 10));
    vt.push_back(p + P2d(0, 10));

    Path2d path, pathv(vt);
    Path2d paths(Path2d::fromstring(pathv.tostring()));

    printf("\n%f", area(paths));

    // calcPara
    std::vector<Path2d> vp, result;
    vp.push_back(pathv);

    calcPara(vp, 1.0, result);
    printf("\n%f", area(result[0]));

    // bump
    bump::Shape shape(vp, vp, 10.0);
    printf("\n%f %f", shape.centro().x, shape.centro().y);

    // offset
    printf("\n%f", area(para(pathv, 10.0, true)));

    // oldspline
    Path2d os(oldSplineX(vt));

    // placement

    return -1;
}
