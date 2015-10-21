#include "intersect.h"

bool intersect(const Path2d& path1, const Path2d& path2, const bool overlap, const double eps)
{
    const int len1 = path1.size();
    const int len2 = path2.size();
    P2d p11 = path1[0];
    P2d p12;
    P2d p21 = path2[0];
    P2d p22;
    for (int i = 1; i < len1; ++i)
    {
        p12 = path1[i];
        P2d B = p12-p11;
        for (int j = 1; j < len2; ++j)
        {
            p22 = path2[j];
            P2d D = p22-p21;
            D = P2d(D.y, -D.x);
            double denom = D*B;
            if (fabs(denom) > eps)
            {
                P2d C = p21-p11;
                double numer = D*C;
                double t = numer/denom;
                C = P2d(C.y, -C.x);
                double u = (B*C)/(-denom);
                if (overlap)
                {
                    if ( t >= 0.0 && t <= 1.0 && u <= 1.0 && u >= 0.0 )
                    {
                        return true;
                    }
                }
                else
                {
                    if ( t > 0.0 && t < 1.0 && u < 1.0 && u > 0.0 )
                    {
                        return true;
                    }
                }
            }
            p21 = p22;
        }
        p11 = p12;
    }
    return false;
}

bool intersect(const Path2d& path1, const std::vector<Path2d>& paths2, bool overlap, const double eps)
{
    for (size_t i = 0; i < paths2.size(); ++i)
        if (intersect(path1, paths2[i], overlap, eps))
            return true;
    return false;
}

bool intersect(const std::vector<Path2d>& paths1, const std::vector<Path2d>& paths2, bool overlap, const double eps)
{
    for (size_t i = 0; i < paths1.size(); ++i)
        if (intersect(paths1[i], paths2, overlap, eps))
            return true;
    return false;
}

bool intersect(const P2d &a, const P2d &b, const P2d &c, const P2d &d)
{
    double ka = (a - c) ^ (d - c);
    double kb = (b - c) ^ (d - c);
    double kc = (c - a) ^ (b - a);
    double kd = (d - a) ^ (b - a);
    return (ka * kb < 0) && (kc * kd < 0);
}

bool intersect(const Path2d& path)
{
    bool closed = false;
    for (int i=0; i<path.size()-2 && !closed; i++)
    {
        for (int j=i+2; j<path.size()-1 && !closed; j++)
        {
            closed = _intersSegmenti(path[i], path[i+1], path[j], path[j+1]) != P2d::none();
         }
    }
    return closed;
}

//FIXME: queste sono le versioni C di geo2d.intersRette e geo2d.intersSegmenti
P2d _intersRette(const P2d& p1, const P2d& p2, const P2d& p3, const P2d& p4,
                double *s, double *t, double eps=1e-20)
{
    double delta = (p2 - p1)^(p3 - p4);
    if(fabs(delta) < eps)
    {
        return P2d::none();
    }
    *s = (p3 - p1)^(p3 - p4) / delta;
    *t = (p2 - p1)^(p3 - p2) / delta;
    return p1 + (*s)*(p2 - p1);
}

P2d _intersSegmenti(const P2d& a1, const P2d& a2, const P2d& b1, const P2d& b2)
{
    double s, t;
    P2d p = _intersRette(a1, a2, b1, b2, &s, &t);
    if (p == P2d::none())
        return P2d::none();

    if (0.0 <= s && s <= 1.0 && 0.0 <= t && t <= 1.0)
        return p;
    return P2d::none();
}

std::vector<P2d> _intersections(const Path2d& path1, const Path2d& path2, const bool self_intersect)
{
    std::vector<P2d> rv;

    if (path1.size() < 2 || path2.size() < 2)
        return rv;

    int i1, i2, i2_start;

    P2d p1a, p1b;
    P2d p2a, p2b;

    for(i1 = 0, p1a = path1[i1]; i1 < (path1.size() - 1); p1a = p1b)
    {
        ++i1;
        p1b = path1[i1];

        if (!self_intersect)
        {
            i2_start = 0;
        } else {
            // in casi non degeneri due segmenti consecutivi di una linea non si intersecano
            i2_start = i1 + 1;
            if (i2_start >= (path2.size() - 1))
                break;
        }
        for(i2 = i2_start, p2a = path2[i2]; i2 < (path2.size() - 1); p2a = p2b)
        {
            ++i2;
            p2b = path2[i2];

            P2d intr = _intersSegmenti(p1a, p1b, p2a, p2b);
            if (intr != P2d::none())
            {
                rv.push_back(intr);
            }
        }
    }

    return rv;
}

std::vector<P2d> intersections(const Path2d& path1, const Path2d& path2)
{
  return _intersections(path1, path2, false);
}

std::vector<P2d> self_intersections(const Path2d& path)
{
  return _intersections(path, path, true);
}
