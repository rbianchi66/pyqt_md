#include "geo2d.h"
#include <set>
#include <map>
#include <math.h>

void circle_pivoting(std::vector<P2d>& pts, double r)
{
    const double pi2 = 2*3.141592654;
    typedef std::map<std::pair<int, int>, std::vector<int> > ProxMap;
    ProxMap pm;
    int iv1 = -1;
    double dia = r * 2;
    // Trova punto iniziale e crea mappa di prossimita' x ricerca candidati
    for (int i=0,n=pts.size(); i<n; i++)
    {
        if (iv1 == -1 || pts[i].x > pts[iv1].x) iv1 = i;
        std::pair<int, int> key = std::make_pair(int(floor(pts[i].x/dia)),
                                                 int(floor(pts[i].y/dia)));
        pm[key].push_back(i);
    }

    // Alloca risultato
    std::vector<P2d> res;
    res.push_back(pts[iv1]);
    std::set<std::pair<int, int> > seen;
    double a0 = 0.0;
    for(;;)
    {
        // Cerca prossimo punto di appoggio; le celle sono di larghezza pari
        // al diametro e quindi basta controllare 9 celle
        int best_i = -1;
        double best_angle = 0.0;
        P2d best_c(0, 0);
        int kx = int(floor(pts[iv1].x / dia));
        int ky = int(floor(pts[iv1].y / dia));
        for (int ix=kx-1; ix<=kx+1; ix++)
            for (int iy=ky-1; iy<=ky+1; iy++)
            {
                std::vector<int>& v = pm[std::make_pair(ix, iy)];
                for (int i=0,n=v.size(); i<n; i++)
                {
                    int j = v[i];
                    P2d d = ortho(pts[iv1] - pts[j]);
                    double d2 = d*d;
                    if (d2 > 1E-10)
                    {
                        // Calcolo centro cerchio e angolo relativo
                        double c = r*r - d2/4;
                        if (c > 0)
                        {
                            P2d m = (pts[iv1] + pts[j]) / 2;
                            double t = sqrt(c / d2);
                            P2d cc = m + t*d;
                            double aa = arg(cc - pts[iv1]) - a0;
                            aa -= floor(aa / pi2) * pi2;
                            if (best_i == -1 || aa < best_angle)
                            {
                                best_i = j;
                                best_angle = aa;
                                best_c = cc;
                            }
                        }
                    }
                }
            }

        std::pair<int, int> newc(iv1, best_i);
        if (best_i == -1 || seen.find(newc) != seen.end())
        {
            // nessun candidato oppure gia' incontrato prima... elaborazione terminata
            break;
        }

        // Registra il nuovo punto e il nuovo angolo di riferimento
        res.push_back(pts[best_i]);
        seen.insert(newc);
        iv1 = best_i;
        a0 = arg(best_c - pts[iv1]);
    }
    pts.swap(res);
}
