#include "minimumbbox.h"
#include <math.h>
#include <limits>

#define PI 3.14159265358979323846264338327950288419716939937510
MinimumBB::MinimumBB(const std::vector<P2d>& pts,
                     int refine_step_size,
                     double angle,
                     bool refine_box):
    refine_step_size(refine_step_size),
    refine_box(refine_box)
{
    if (!refine_box)
        createRectWithAngle(pts, angle);
    else
        createRectFindingAngle(pts);
}

void MinimumBB::createRectFindingAngle(const std::vector<P2d>& pts)
{
    r.resize(NA);
    angles.resize(NA);
    perpendicular_r.resize(NA);

    best_angle = -1;
    best_angle_index = -1;
    area = std::numeric_limits<double>::infinity();
    for (int i = 0; i < NA; i++)
        findDimension((double)i, i, pts);

    bestArea(NA4, NA4);
    refine(pts, best_angle, 0);
    createRect();
}

void MinimumBB::createRectWithAngle(const std::vector<P2d>& pts, double angle)
{
    r.resize(4);
    angles.resize(4);
    perpendicular_r.resize(4);

    best_angle = angle;
    best_angle_index = 0;
    for (int i = 0; i < 4; ++i)
        findDimension(double(i*90 + angle), i, pts);

    createRect();
}

// Cerca l'angolo che da l'area più piccola. Prende in ingresso il numero di punti su cui fare la ricerca
void MinimumBB::bestArea(int max_val, int offset)
{
    
    double _area = std::numeric_limits<double>::infinity();
    int _best_angle_index = best_angle_index;
    int size = r.size();
    for (int i = 0; i <= max_val; i++) {
        double dx = fabs(r[i] + r[(i+2*offset)%size]);
        double dy = fabs(perpendicular_r[i] + perpendicular_r[(i+2*offset)%size]);
        double aa = dx*dy;
        if (aa < _area) {
            _area = aa;
            _best_angle_index = i;
        }
    }
    area = _area;
    best_angle_index = _best_angle_index;
    best_angle = angles[best_angle_index];
}

void MinimumBB::findDimension(double a, int i, const std::vector<P2d> &pts)
{
    angles[i] = a;
    a *= (double)2*PI/NA;
    double perp_a = a - (double)PI/2;
    double cs = cos(a);
    double perp_cs = cos(perp_a);
    double sn = sin(a);
    double perp_sn = sin(perp_a);
    double mx = 0;
    double perp_mx = 0;
    for (int j = 0, n = pts.size(); j < n; j++) {
        double d = pts[j].x * cs + pts[j].y * sn;
        if (j == 0 || d > mx) mx = d;

        d = pts[j].x * perp_cs + pts[j].y * perp_sn;
        if (j == 0 || d > perp_mx) perp_mx = d;
    }

    r[i] = mx;
    perpendicular_r[i] = perp_mx;

}

void MinimumBB::refine(const std::vector<P2d>& pts, double angle, int step)
{
    double old_area = area;
    double s = refine_step_size * pow(10.0, step);
    for (int t = 0; t < 2; ++t)
    {
        for (int i = 0; i < NA/2; i++) {
            double a = (angle - s * (1 - (double)4*i/NA) + NA/2*t);
            findDimension(a, i + NA/2*t, pts);
        }
    }

    // Continua a cercare un angolo migliore fino a quando l'area diminuisce
    // La ricerca viene fatta in un intorno sempre più piccolo dell'angolo che di volta in volta da l'aria minore
    bestArea(NA4*2, NA4);
    if (area < old_area && area - old_area > -1E-3 && step > -5)
        refine(pts, best_angle, --step);

    createRect();
}

void MinimumBB::createRect()
{
    double xa=0., ya=0., xb=0., yb=0., xc=0., yc=0., xd=0., yd=0.;
    int size = r.size();
    int size4 = size/4;
    for (int i = 0; i < 4; i++) {
        double a0 = (best_angle+(double)i*90.);
        while (a0 > 360.)
            a0 -= 360.;
        a0 *= (double)2*PI/360;
        double cs0 = cos(a0);
        double sn0 = sin(a0);
        double k0;
        if (i % 2 == 0)
            k0 = r[(best_angle_index+i*size4)%size];
        else
            k0 = -perpendicular_r[(best_angle_index+(i-1)*size4)%size];
        double a1 = (best_angle+(double)(i+1)*90.);
        while (a1 > 360.)
            a1 -= 360.;

        a1 *= (double)2*PI/360;
        double cs1 = cos(a1);
        double sn1 = sin(a1);
        double k1;
        if (i % 2 == 0)
            k1 = -perpendicular_r[(best_angle_index+i*size4)%size];
        else
            k1 = r[(best_angle_index+(i+1)*size4)%size];

        double delta = cs0*sn1 - cs1*sn0;
        double dx = k0*sn1 - k1*sn0;
        double dy = cs0*k1 - cs1*k0;
        switch(i) {
            case 0: xa=dx/delta; ya=dy/delta; break;
            case 1: xb=dx/delta; yb=dy/delta; break;
            case 2: xc=dx/delta; yc=dy/delta; break;
            case 3: xd=dx/delta; yd=dy/delta; break;
        }
    }

    P2d _pts[4] = {P2d(xa, ya), P2d(xb, yb), P2d(xc, yc), P2d(xd, yd)};

    for (int i = 3; i > 0; --i) {
        for (int j = 1; j <= i; ++j) {
            if (_pts[j-1].x > _pts[j].x) {
                P2d k = _pts[j - 1];
                _pts[j-1] = _pts[j];
                _pts[j] = k;
            }
        }
    }

    // 0 +-----+ 1
    //   |     |
    // 3 +-----+ 2
    if (_pts[0].y > _pts[1].y) {
        pts[0] = _pts[0];
        pts[3] = _pts[1];
    } else {
        pts[0] = _pts[1];
        pts[3] = _pts[0];
    }

    if (_pts[2].y > _pts[3].y) {
        pts[1] = _pts[2];
        pts[2] = _pts[3];
    } else {
        pts[1] = _pts[3];
        pts[2] = _pts[2];
    }
}

void MinimumBB::getRect (P2d *a, P2d *b, P2d *c, P2d *d) {
    *a = pts[0];
    *b = pts[1];
    *c = pts[2];
    *d = pts[3];
}

double MinimumBB::bestAngle()
{
    return angles[best_angle_index];
}

bool MinimumBB::test(P2d p)
{
    double ap = (p - pts[0]) * (pts[1] - pts[0]);
    double ab = (pts[1] - pts[0]) * (pts[1] - pts[0]);
    double apd = (p - pts[0]) * (pts[3] - pts[0]);
    double ad = (pts[3] - pts[0]) * (pts[3] - pts[0]);

    return (0 < ap && ap < ab) && (0 < apd && apd < ad);
}

P2d MinimumBB::center()
{
    double x;
    double y;

    x = (pts[0].x + pts[2].x) / 2;
    y = (pts[0].y + pts[2].y) / 2;
    return P2d(x, y);
}
