#include "bbox.h"

OrientedBB::OrientedBB(const std::vector<P2d>& pts, double extra)
{
    for (int i = 0; i < NA; i++) {
        double a = 2*3.141592654*i/NA;
        double cs = cos(a);
        double sn = sin(a);
        double mx = 0;

        for (int j = 0, n = pts.size(); j < n; j++) {
            double d = pts[j].x * cs + pts[j].y * sn;
            if (j == 0 || d > mx) mx = d;
        }

        r[i] = mx + extra;
    }

    fix();
}

OrientedBB::OrientedBB(const std::vector<OrientedBB *>& bb)
{
    // Costruzione per unione di n bbox
    for (int i = 0; i < NA; i++)
        r[i] = bb[0]->r[i];
    for (int j = 1, n = bb.size(); j < n; j++)
        for (int i = 0; i < NA; i++)
            if (bb[j]->r[i] > r[i]) r[i] = bb[j]->r[i];
    fix();
}

bool OrientedBB::test (const P2d& p) const
{
    // FIXME
    std::vector<P2d> vp;
    vp.push_back(p);
    return(xsect(OrientedBB(vp, 0)));
}

void OrientedBB::merge (const std::vector<OrientedBB *>& bb) {
    for (int j = 0, n = bb.size(); j < n; j++)
        for (int i = 0; i < NA; i++)
            if (bb[j]->r[i] > r[i]) r[i] = bb[j]->r[i];
    fix();
}

void OrientedBB::rotate (int angle) {
    fix(angle);
}

void OrientedBB::fix (int angle) {
    // Calcolo angolo ottimale e coordinate esplicite
    area = -1;
    if (angle < 0) {
        for (int i = 0; i < NA4; i++) {
            double dx = fabs(r[i] + r[(i+2*NA4)%NA]);
            double dy = fabs(r[(i+NA4)%NA] + r[(i+3*NA4)%NA]);
            double aa = dx*dy;
            if (area == -1 || aa < area) {
                area = aa; best_angle = i;
            }
        }
    } else {
        double dx = fabs(r[angle] + r[(angle+2*NA4)%NA]);
        double dy = fabs(r[(angle+NA4)%NA] + r[(angle+3*NA4)%NA]);
        double aa = dx*dy;
        area = aa;
        best_angle = angle;
    }

    for (int i = 0; i < 4; i++) {
        double a0 = (best_angle+i*NA4)%NA*2*3.141592654/NA;
        double cs0 = cos(a0);
        double sn0 = sin(a0);
        double k0 = r[(best_angle+i*NA4)%NA];
        double a1 = (best_angle+(i+1)*NA4)%NA*2*3.141592654/NA;
        double cs1 = cos(a1);
        double sn1 = sin(a1);
        double k1 = r[(best_angle+(i+1)*NA4)%NA];
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
}

bool OrientedBB::xsect(const OrientedBB& other) const
{
    // True se questo bbox interseca quello passato in other.
    // Basta controllare se le facce dell'uno o dell'altro
    // "separano" i due box (separating axis theorem per
    // poliedri convessi nel caso 2D).
    for (int i=0; i<4; i++) {
        int j = (best_angle+i*NA4)%NA;
        double a = j*2*3.141592654/NA;
        double cs = cos(a);
        double sn = sin(a);
        if (other.xa*cs + other.ya*sn >= r[j] &&
            other.xb*cs + other.yb*sn >= r[j] &&
            other.xc*cs + other.yc*sn >= r[j] &&
            other.xd*cs + other.yd*sn >= r[j])
        {
            return false;
        }
    }

    for (int i = 0; i < 4; i++) {
        int j = (other.best_angle+i*NA4)%NA;
        double a = j*2*3.141592654/NA;
        double cs = cos(a);
        double sn = sin(a);
        if (xa*cs + ya*sn >= other.r[j] &&
            xb*cs + yb*sn >= other.r[j] &&
            xc*cs + yc*sn >= other.r[j] &&
            xd*cs + yd*sn >= other.r[j])
        {
            return false;
        }
    }

    return true;
}

void OrientedBB::getRect (P2d *a, P2d *b, P2d *c, P2d *d) const {
    P2d pts[4] = {P2d(xa, ya), P2d(xb, yb), P2d(xc, yc), P2d(xd, yd)};

    for (int i = 3; i > 0; --i) {
        for (int j = 1; j <= i; ++j) {
            if (pts[j-1].x > pts[j].x) {
                P2d k = pts[j - 1];
                pts[j-1] = pts[j];
                pts[j] = k;
            }
        }
    }

    // a +-----+ b
    //   |     |
    // d +-----+ c
    if (pts[0].y > pts[1].y) {
        *a = pts[0];
        *d = pts[1];
    } else {
        *a = pts[1];
        *d = pts[0];
    }

    if (pts[2].y > pts[3].y) {
        *b = pts[2];
        *c = pts[3];
    } else {
        *b = pts[3];
        *c = pts[2];
    }
}
