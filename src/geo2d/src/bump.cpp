#include <math.h>
#include <set>
#include "bump.h"
#include "dpara.h"
#include "inside.h"
#include "xsarea.h"

using namespace bump;

// *********************************************************************
// PosRot
// *********************************************************************

X2d PosRot::xform() const {
    double cr = cos(rot), sr = sin(rot);
    return X2d::xlate(-centro) * X2d(P2d( cr, sr),
               P2d(-sr, cr),
               P2d(pos.x, pos.y)) * X2d::xlate(centro);
}
PosRot PosRot::operator*(const PosRot& pr) const {
    PosRot res;
    double c2 = cos(pr.rot);
    double s2 = sin(pr.rot);

    res.pos = P2d(c2 * this->pos.x - s2 * this->pos.y + pr.pos.x,
                  s2 * this->pos.x + c2 * this->pos.y + pr.pos.y);
    res.rot = this->rot + pr.rot;
    res.NormRot();
    return res;
}

PosRot& PosRot::operator*=(const PosRot& pr) {
    *this = *this * pr;
    return *this;
}

void PosRot::NormRot() {
    if (rot + PI < 0 || rot + PI >= 2 * PI) {
        rot -= 2 * PI * floor((rot + PI) / (2 * PI));
    }
}

// *********************************************************************
// Shape
// *********************************************************************

Shape::Shape(std::vector<Path2d> net_border,
             std::vector<Path2d> bump_border,
             double extra) : net_border(net_border), bump_border(bump_border), extra(extra) {
    updateBump();
    calcCentro();
}

void Shape::merge(const Shape& other, PosRot pr) {
    X2d x = pr.xform();
    for (std::vector<Path2d>::const_iterator it = other.net_border.begin(); it != other.net_border.end(); it++) {
        net_border.push_back(*it);
        net_border.back() *= x;
    }
    for (std::vector<Path2d>::const_iterator it = other.bump_border.begin(); it != other.bump_border.end(); it++) {
        bump_border.push_back(*it);
        bump_border.back() *= x;
    }
    updateBump();
    calcCentro();
}

void Shape::updateBump() {
    if (extra > 1e-5) {
        calcPara(bump_border, extra, x_bump_border);
        int n = (int)x_bump_border.size();

        InsideTester it = InsideTester(bump_border);

        for (int i = n - 1; i >= 0; i--) {
            P2d pt = x_bump_border[i][0];
            if (it(pt)) {
                x_bump_border.erase(x_bump_border.begin() + i);
            }
        }
    } else {
        x_bump_border = bump_border;
    }
}

void Shape::calcCentro() {
    double min_x = 1e30, min_y = 1e30;
    double max_x = -1e30, max_y = -1e30;
    int npaths = (int)bump_border.size();
    for (int i = 0; i < npaths; i++) {
        int npoints = (int)bump_border[i].size();
        for (int j = 0; j < npoints; j++) {
            P2d &p = bump_border[i][j];
            if (p.x < min_x) min_x = p.x;
            if (p.x > max_x) max_x = p.x;
            if (p.y < min_y) min_y = p.y;
            if (p.y > max_y) max_y = p.y;
        }
    }
    c = P2d((min_x + max_x) / 2, (min_y + max_y) / 2);
}

// *********************************************************************
// Bumper
// *********************************************************************

double Bumper::objFunc(const Shape& as, PosRot apos, const Shape& bs, PosRot bpos) {
    // Calcola l'area di sovrapposizione A1 tra (a.bump_border, pos) e (b.net_border, bpos);
    double a1 = xsarea(as.bump(), apos.xform(), bs.net(), bpos.xform());
    // Calcola l'area di sovrapposizione A2 tra (b.bump_border, bpos) e (a.net_border, pos);
    double a2 = xsarea(as.net(), apos.xform(), bs.bump(), bpos.xform());

    return (a1 > a2 ? a1 : a2);
}

PosRot Bumper::bump(const Shape& sa, PosRot apos, const Shape& sb, PosRot bpos, double& overlap) {
    double p = 2;
    double bsf = objFunc(sa, apos, sb, bpos);
    PosRot pr = apos;
    PosRot res = apos;
    while (p >= 1) {
        double d = 2;

        double dfx = -(objFunc(sa, PosRot(pr.pos + P2d(d, 0), pr.rot, sa.centro()), sb, bpos) -
                       objFunc(sa, PosRot(pr.pos - P2d(d, 0), pr.rot, sa.centro()), sb, bpos)) / (2 * d);
        double dfy = -(objFunc(sa, PosRot(pr.pos + P2d(0, d), pr.rot, sa.centro()), sb, bpos) -
                       objFunc(sa, PosRot(pr.pos - P2d(0, d), pr.rot, sa.centro()), sb, bpos)) / (2 * d);
        double dfa = -(objFunc(sa, PosRot(pr.pos, pr.rot + d * 2 * PI / 360, sa.centro()), sb, bpos) -
                       objFunc(sa, PosRot(pr.pos, pr.rot - d * 2 * PI / 360, sa.centro()), sb, bpos)) / (2 * d);

        double n = sqrt(dfx * dfx + dfy * dfy + dfa * dfa);

        PosRot dir;
        if (n < 1e-10) {
            overlap = bsf;
            return res;
        }
        dir = PosRot(dfx / n, dfy / n, dfa / n);

        pr = PosRot(pr.pos + p * dir.pos, pr.rot + p * dir.rot * 2 * PI / 360, sa.centro());

        double m = objFunc(sa, pr, sb, bpos);
        if (m > bsf - 1E-10) {
            p /= 2;
        } else {
            bsf = m;
            res = pr;
        }
    }
    overlap = bsf;
    return res;
}
