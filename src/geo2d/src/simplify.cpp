#include "geo2d.h"
#include "simplify.h"

int _simplify(P2d *array, int n, double eps)
{
    double wd;
    int wi;

    if (n > 2)
    {
        P2d delta_p = array[n-1] - array[0];
        double L2 = len2(delta_p);
        wd = 0.0;
        wi = -1;

        if (L2 < 1E-10)
        {
            for (int i = 1; i < (n-1); ++i)
            {
                double d = dist2(array[i], array[0]);
                if ((wi == -1) || (d > wd))
                {
                    wd = d;
                    wi = i;
                }
            }
        }
        else
        {
             P2d k = delta_p / L2;
             for (int i = 1; i < (n-1); ++i)
             {
                double t = (array[i] - array[0])*k;
                P2d diff;

                if (t <= 0.0)
                {
                    diff = array[i] - array[0];
                }
                else
                if (t >= 1.0)
                {
                    diff = array[i] - array[n-1];
                }
                else
                {
                    diff = array[i] - (array[0] + t*delta_p);
                }
                double d = len2(diff);
                if ((wi == -1) || (d > wd))
                {
                    wd = d;
                    wi = i;
                }
             }
        }
        if (wd > (eps*eps))
        {
            int n0 = _simplify(array, wi+1, eps);
            int n1 = _simplify(array+wi, n-wi, eps);

            for (int i=1; i < n1; ++i)
            {
                array[n0+i-1] = array[wi+i];
            }

            n = n0 + n1 - 1;
        }
        else
        {
            array[1] = array[n-1];
            n = 2;
        }
    }

    return n;
}

Path2d& simplify(Path2d& path, double eps)
{
    int n = path.size();
    if (n < 3)
        return path;

    std::vector<P2d> pts;

    path.swap(pts);
    n = _simplify(&pts[0], n, eps);
    pts.resize(n);

    path.swap(pts);
    return path;
}

Path2d& simplify_contained(Path2d& path, const int simplify_type, const int iterations, const double eps)
{
    if (simplify_type == 0)
    {
        // se il path è in senso antiorario si reverta
        if( sarea(path) > 0.0)
            path.reverse();
    }
    else if (simplify_type == 1)
    {
        // se il path è in senso orario si reverta
        if( sarea(path) < 0.0)
            path.reverse();
    }

    int last_pts_number = path.size()+1; // +1 per eseguire almeno 1 passo
    int it = 0;
    while (it++ < iterations && path.size() < last_pts_number)
    {
        // Blocco per la semplificazione delle convessita'
        {
            const int n = path.size();
            last_pts_number = n;
            std::vector<P2d> s_path;
            s_path.push_back(path[0]);
            s_path.push_back(path[1]);
            bool last = false;

            for (int i = 2; i < n-2; ++i)
            {
                last = false;
                P2d p0 = path[i-2];
                P2d p1 = path[i-1];
                P2d p2 = path[i];
                P2d p3 = path[i+1];
                P2d p4 = path[i+2];

                P2d v31 = p3-p1;
                P2d v31_perp(v31.y, -v31.x);
                double den = v31_perp*v31_perp;
                if( fabs(den) <= eps )
                        continue;
                double u = (v31_perp*(p1-p2))/den;

                // Il punto e' una convessita'
                // FIXME: forse c'e' un modo piu' semplice per sapere se
                // il punto p2 fa parte di una convessita'
                if ( u > 0.0 )
                {
                    P2d v10 = p1-p0;
                    P2d v10_perp = P2d(v10.y, -v10.x);
                    P2d v43 = p4-p3;
                    P2d v43_perp = P2d(v43.y, -v43.x);
                    double den1 = v10_perp*v43;
                    double den2 = v43_perp*v10;
                    double den3 = v31_perp*v31_perp;
                    if (fabs(den1) <= eps || fabs(den2) <= eps || fabs(den3) <= eps)
                            continue;
                    double u = (v10_perp*(p1-p3))/den1; // < 0
                    double t = (v43_perp*(p3-p1))/den2; // > 0

                    P2d it = p1+v10*t;
                    double s = (v31_perp*(p1-it))/den3;
                    // FIXME: il discorso fatto per p2 vale anche per it
                    if (t > 0.0 && u < 0.0 && s > u) // La convessita' e' semplificabile
                    {
                        double den1 = v10_perp*v31;
                        double den2 = v43_perp*v31;
                        if (fabs(den1) <= eps || fabs(den2) <= eps)
                            continue;
                        u = (v10_perp*(p0-p2))/den1;
                        P2d primopunto = p2+v31*u; // u <= 0

                        t = (v43_perp*(p4-p2))/den2;
                        P2d secondopunto = p2+v31*t; // t >= 0;
                        if (u <= 0.0 && t >= 0.0)
                        {
                            s_path.pop_back();
                            s_path.push_back(primopunto);
                            s_path.push_back(secondopunto);
                            s_path.push_back(p4);
                            i+=2;
                            last = true;
                        }
                        else
                        {
                            s_path.push_back(p2);
                        }
                    }
                    else
                    {
                        s_path.push_back(p2);
                    }
                }
                else
                {
                    s_path.push_back(p2);
                }
            }
            if (!last)
                s_path.push_back(path[n-2]);
            s_path.push_back(path[n-1]);

            path.swap(s_path);
        }

    }
    return path;
}

