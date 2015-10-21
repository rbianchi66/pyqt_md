#include "sysdep.h"
#include <assert.h>
#include <math.h>
#include <algorithm>
#include <utility>

#include "offset.h"
#include "unione_path.h"
#include "inside.h"
#include "simplify.h"

#ifdef _MSC_VER
#if _MSC_VER <= 1310
#define min std::_cpp_min
#define max std::_cpp_max
#else
using std::min;
using std::max;
#endif
#else
using std::min;
using std::max;
#endif

using std::transform;
using std::min_element;
using std::vector;
using std::copy;
using std::pair;
using std::make_pair;

typedef pair<P2d, P2d> tuple_p2d;

Path2d raccordo(P2d o, P2d p1, P2d p2, double delta)
{
    /*
    Dati i punti p1 e p2,  restituisce la lista dei punti intermedi, ad un
    intervallo delta uno dall'altro, e che giacciono sull'arco di circonferanza
    di centro o e raggio p1-o = p2-o. Se p1-o != p2-o, adatta il raggio di
    curvatura da p1-o a p2-o.
    */
    if (len(p1 - p2) < delta)
        return Path2d();

    double r1 = len(p1 - o);
    double r2 = len(p2 - o);

    Path2d res;
    double a1 = arg(p1 - o);
    double a2 = arg(p2 - o);

    double da = sfmod(a2 - a1, 2.0 * PI);

    if (da > PI)
        da = da - 2.0 * PI;

    int n = (int)(len(p1 - p2) / delta);

    for (int i=0; i < n; ++i)
    {
        double a = a1 + (i + 1) * da / (n + 1);
        double r = r1 + (r2 - r1) * (i + 1) / (n + 1);

        P2d t = o + P2d(r * cos(a), r * sin(a));

        res.push_back(t);
    }

    return res;
}

tuple_p2d nd2(const Path2d &pts, int i, bool chiusa)
{
    /*
    Data la spezzata pts, calcola i 2 versori normali ai segmenti incidenti
    al vertice di indice `i`.
    Se il vertice è un estremo, il secondo elemento della tupla è None.
    */
    P2d prev_pt, next_pt;

    if (i == 0)
    {
        // Estremo iniziale
        if (!chiusa)
        {
            // FIXME: se c'e' solo 1 elemento non si puo' calcolare la direzione
            P2d d = dir(pts[1] - pts[0]);
            return make_pair(ortho(d), P2d::none());
        }

        // Linea chiusa: il punto precedente e' l'estremo finale
        // -2 perché se è chiusa il punto -1 è uguale al punto 0
        prev_pt = pts[pts.size() - 2];
    }
    else
        prev_pt = pts[i - 1];

    if (i == (pts.size() - 1))
    {
        // Estremo finale
        if (!chiusa)
        {
            unsigned int index_meno1 = pts.size() - 1;
            unsigned int index_meno2 = (pts.size() - 2) % pts.size();

            P2d d = dir(pts[index_meno1] - pts[index_meno2]);
            return make_pair(ortho(d), P2d::none());
        }

        // Linea chiusa: il punto successivo e' l'estremo iniziale
        // 1 perché se è chiusa il punto -1 è uguale al punto 0
        next_pt = pts[1];
    }
    else
        next_pt = pts[i + 1];

    P2d d1 = dir(pts[i] - prev_pt);
    P2d d2 = dir(next_pt - pts[i]);
    return make_pair(ortho(d1), ortho(d2));
}

P2d nd(const Path2d &pts, int i, double d, bool chiusa)
{
    /*
    Data la spezzata `pts` calcola il punto a distanza `d` lungo la bisettrice
    dei segmenti incidenti sul vertice di indice `i`. La distanza viene inoltre
    corretta per ottenere che i segmenti che uniscono questi punti si trovino a
    distanza `d` dai segmenti della spezzata (usando `d` come distanza lungo la
    bisettrice si otterrebbero segmenti sempre troppo vicini ai segmenti in
    input) `chiusa` è un flag per indicare che la linea è chiusa.
    */

    tuple_p2d n = nd2(pts, i, chiusa);

    if (n.second == P2d::none() || len(n.first - n.second) < 0.001)
        return n.first * d;

    // Se i due vettori n1 ed n2 si annullano a vicenda significa che il
    // secondo ha stessa direzione e verso opposto al primo; posso
    // mantenere la semantica della funzione usando la direzione di
    // "andata".
    if (len2(n.first + n.second) < 1E-20)
        return ortho(ortho(ortho(n.first))) * d;

    P2d ddir = dir(n.first + n.second) * d;
    return (ddir * sqrt(2.0 / (1.0 + fabs(n.first * n.second) * 0.999)));
}

Path2d para_external(const Path2d &pts, double dist, bool chiusa)
{
    if (dist == 0.0)
        return pts;

    Path2d pts_clockwise(pts);

    if (sarea(pts_clockwise) > 0.0)
        pts_clockwise.reverse();
    return para(pts_clockwise, dist, chiusa);
}

Path2d para_internal(const Path2d &pts, double dist, bool chiusa)
{
    if (dist == 0.0)
        return pts;

    Path2d pts_anticlockwise(pts);

    if (sarea(pts_anticlockwise) < 0.0)
        pts_anticlockwise.reverse();
    return para(pts_anticlockwise, dist, chiusa);
}

/*
 * Calcola la parallela "sporca" (con auto-intersezioni)
 * alla spezzata `pts` in ingresso.
 */
Path2d para(const Path2d &pts_in, double dist, bool chiusa)
{
    // La lunghezza del path in ingresso non può essere zero, perché non
    // ha senso geometricamente.
    assert(pts_in.len() != 0);

    Path2d pts(pts_in);
    pts.clean();
    if (chiusa)
        pts.pop_back();

    Path2d res;
    for (int i=0; i < pts.size(); ++i)
        res.push_back(pts[i] + nd(pts, i, dist, chiusa));

    if (chiusa)
        res.push_back(res[0]);
    return res;
}


/*
 * Calcola la parallela "sporca" (con auto-intersezioni) alla spezzata `pts` in
 * ingresso, smussando gli spigoli.
 */
Path2d para_smussa(const Path2d &pts_in, double dist, bool chiusa)
{
    vector<tuple_p2d> nr;

    // La lunghezza del path in ingresso non può essere zero, perché non
    // ha senso geometricamente.
    assert(pts_in.len() != 0);

    // Elimina singolarità del path (punti sovrapposti) che causano
    // problemi di calcolo.
    Path2d pts(pts_in);
    pts.clean();

    if (chiusa)
        pts.pop_back();

    for (int i=0; i < pts.size(); ++i)
    {
        tuple_p2d n = nd2(pts, i, chiusa);
        const P2d &n0 = n.first;
        const P2d &n1 = n.second;
        const P2d &p = pts[i];

        nr.push_back(make_pair((n0 == P2d::none()) ? n0 : p + dist * n0,
                               (n1 == P2d::none()) ? n1 : p + dist * n1));
    }
    if (chiusa)
    {
        nr.push_back(make_pair(nr[0].first, P2d::none()));
    }

    Path2d res;

    for (int i=0; i < pts.size(); ++i)
    {
        const P2d &p = pts[i];

        res.push_back(nr[i].first);

        if (nr[i].second != P2d::none())
        {
            Path2d pta = raccordo(p, nr[i].first, nr[i].second, max(min(dist / 10.0, 1.0), 0.1));
            for (int pt=0; pt < pta.size(); ++pt)
                res.push_back(pta[pt]);
        }

        if ((nr[i].second != P2d::none()) && (len(nr[i].second - nr[i].first) > 0.1))
        {
            res.push_back(nr[i].second);
        }
    }

    return res;
}

path2d_array _offset(Path2d pts,
                    const dist_array& d,
                    int tipo,
                    double soglia_unibili,
                    bool *pChiusa,
                    double *pMd)
{
    path2d_array LL;

    dist_array d_abs(d.size());

    int min_el = 0;
    for (int i=0; i < (int)d.size(); ++i)
    {
        d_abs[i] = fabs(d[i]);

        if (d_abs[i] < d_abs[min_el])
            min_el = i;
    }

    double md;
    if (d.size())
        md = max(d_abs[min_el] / 10.0, 0.01);
    else
        md = 0.01;

    if (pMd) *pMd = md;

    // Data la spezzata pts aggiunge i punti necessari perché la risultante
    // inter_pts abbia tutti i punti a distanza al più md con il successivo.
    simplify(pts, 1.0/100.0);
    Path2d inter_pts;
    inter_pts.push_back(pts[0]);
    for (int i=0; i<(pts.size()-1); ++i)
    {
        P2d delta = pts[i+1] - pts[i];

        int steps = (int)floor((len(delta) / md) + 0.5);

        // Qua assume che delta non sia mai nullo; è abbastanza ragionevole
        // visto che stiamo lavorando sul risultato di una simplify.
        P2d step = dir(delta) * md;

        for(int j=1; j<steps; ++j)
            inter_pts.push_back(pts[i] + j * step);

        inter_pts.push_back(pts[i+1]);
    }

    // L = geo2d.Path(pts, True)
    Path2d &L = inter_pts;
    L.clean();

    // Una linea di 2 punti (anche se coincidenti) non formano una linea chiusa
    bool chiusa = inter_pts.size() > 2 && len(inter_pts[0] - inter_pts[inter_pts.size()-1]) < 1E-8;
    if (pChiusa) *pChiusa = chiusa;

    Path2d (*par)(const Path2d&, double, bool) = NULL;

    switch (tipo)
    {
        case SPIGOLO_VIVO:
            par = &para;
        break;

        case SMUSSATA:
            par = &para_smussa;
        break;

        default:
            return LL;
        break;
    }

    // Gli utilizzatori attuali della offset si aspettano che le curve
    // chiuse vengano sempre espanse verso l'esterno con offset positivo
    // e verso l'interno con offset negativo; ora, il lato scelto dalla
    // funzione offset dipende dal verso della curva, ma visto che non
    // vogliamo modificarlo solo per mantenere questa invariante, per
    // ottenere lo stesso risultato inverte il segno dell'offset richiesto.
    double s = 1.0;
    if (chiusa && sarea(L) > 0)
        s = -1.0;

    for (dist_array::const_iterator ite=d.begin(); ite != d.end(); ++ite)
    {
        double dd = (*ite);
        double d1 = fabs(dd) * 0.999;

        dd *= s;

        Path2d ps = par(L, dd, chiusa);
        Path2d xs;

        P2d last = P2d::none();
        double lastx = 0.0;

        for (int pi=0; pi < ps.size(); ++pi)
        {
            P2d p = ps[pi];
            double dist, x;

            L.project(p, NULL, &dist);
            x = dist - d1;

            if (x < 0.0)
            {
                if (xs.size())
                {
                    for (int i=99; i>0; --i)
                    {
                        P2d p1 = last + i*(p - last)/100.0;
                        double dist;
                        L.project(p1, NULL, &dist);

                        if ((dist - d1) > 0.0)
                        {
                            xs.push_back(p1);
                            break;
                        }
                    }
                    if (xs.size() > 1)
                    {
                        LL.push_back(xs);
                    }
                }

                xs.clear();
            }
            else
            {
                if ((last != P2d::none()) && (lastx < 0.0))
                {
                    for (int i=1; i < 100; ++i)
                    {
                        P2d p1 = last + i*(p - last)/100.0;
                        double dist;
                        L.project(p1, NULL, &dist);

                        if ((dist - d1) > 0.0)
                        {
                            xs.push_back(p1);
                            break;
                        }
                    }
                }
                xs.push_back(p);
            }

            last = p;
            lastx = x;
        }

        if (xs.size() > 1)
        {
            LL.push_back(xs);
        }
    }

    return LL;
}

static Path2d fixc(const Path2d &pts, double eps)
{
    /*
    Questa funzione rimuove piccole asole che si formano a volte applicando
    la offset.
    */
    if (pts.size() < 4)
        return Path2d(pts);

    Path2d res;
    //res->reserve(pts.size());

    res.push_back(pts[0]);

    int i = 1;
    while(i < pts.size()-2)
    {
        double d = len(pts[i] - pts[i+1]);
        if (d < eps)
        {
            const P2d &p1 = pts[i-1];
            const P2d &p2 = pts[i];
            const P2d &p3 = pts[i+1];
            const P2d &p4 = pts[i+2];
            double delta = (p2 - p1)^(p3 - p4);
            if (fabs(delta) > 1E-10)
            {
                double s = (p3 - p1)^(p3 - p4) / delta;
                double t = (p2 - p1)^(p3 - p2) / delta;
                if (s>0.0 && s<1.0 && t>0.0 && t<1.0)
                {
                    res.push_back(p1 + s*(p2 - p1));
                    i += 2;
                    continue;
                }
            }
        }
        res.push_back(pts[i]);
        i++;
    }

    for (;i<pts.size();++i)
        res.push_back(pts[i]);


    return res;
}

path2d_array offset(Path2d &pts, const dist_array& d, int tipo, double soglia_unibili)
{
    if (pts.size() <= 2 && len(pts[0] - pts.back()) < 1E-8)
    {
        path2d_array LL;
        LL.push_back(pts);
        return LL;
    }

    bool chiusa;
    double md;

    path2d_array LL = _offset(pts, d, tipo, soglia_unibili, &chiusa, &md);
    path2d_array ret = unione_path(LL, 1.0);

    for (int i=0; i<(int)ret.size(); ++i)
    {
        Path2d p = simplify(ret[i]);

        if (chiusa)
        {
            if (len(p[0] - p[1]) < md)
                p.back() = p[0];
            else
                p.push_back(p[0]);
        }

        ret[i] = fixc(p, 1.0);
    }

    return ret;
}

std::vector<Path2d> enlarge_area(std::vector<Path2d> paths, double dist, double simplifyEps)
{
    std::vector<Path2d> extraLarge;
    if(dist>0.)
    {
        if(simplifyEps>0.)
        {
            for (size_t i = 0; i < paths.size(); ++i)
                simplify(paths[i], simplifyEps);
        }

        for (size_t i = 0; i < paths.size(); ++i)
        {
            Path2d &path = paths[i];
            if (path.size() > 3)
            {
                // check if the path is a hole or not counting the number of
                // paths that contain the current path (check only one point
                // of the current path); if the counter is odd then call the
                // para_internal function to shrink the path, otherwise call
                // the para_external function to enlarge it
                int containingPaths = 0;

                for (size_t j = 0; j < paths.size(); ++j)
                    if (i != j && inside(path[0], paths[j]))
                        ++containingPaths;

                bool closed = (path[0] == path.back());

                if (containingPaths & 1)
                    extraLarge.push_back(para_internal(path, dist, closed));
                else
                    extraLarge.push_back(para_external(path, dist, closed));
            }
        }
    }
    return extraLarge;
}
