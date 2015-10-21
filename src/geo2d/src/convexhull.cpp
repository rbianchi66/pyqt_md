#include "convexhull.h"
#include "poly2d.h"
#include "image2d.h"
#include "marching.h"
#include "offset.h"
#include <math.h>

#ifdef _MSC_VER
static inline double round( double value )
{
  return floor( value + 0.5 );
}
#endif

// cross(): tests if a point is Left|On|Right of an infinite line.
//    Input:  three points P0, P1, and P2
//    Return: >0 for P2 left of the line through P0 and P1
//            =0 for P2 on the line
//            <0 for P2 right of the line
//    See: the January 2001 Algorithm on Area of Triangles
inline double cross( P2d P0, P2d P1, P2d P2 )
{
    return (P1.x - P0.x)*(P2.y - P0.y) - (P2.x - P0.x)*(P1.y - P0.y);
}
inline bool isLeft( P2d P0, P2d P1, P2d P2 )
{
    return cross(P0, P1, P2) <= 0;
}

// Functor ordinatore di P2d: Ordina per x crescenti e a parità
// di x, per y crescenti
class CompareP2d
{
    public:
    inline bool operator()(const P2d& l, const P2d& r)
    {
        if (l.x != r.x)
            return l.x < r.x;
        else
            return l.y < r.y;
    }
};
class CompareP2dy
{
    public:
    inline bool operator()(const P2d& l, const P2d& r)
    {
        if (l.y != r.y)
            return l.y < r.y;
        else
            return l.x < r.x;
    }
};

Path2d convex_hull( std::vector<P2d>& P)
{
    std::sort(P.begin(), P.end(), CompareP2d());
    return convex_hull_presorted(P);
}

Path2d convex_hull( const std::vector<Path2d>& V)
{
    std::vector<P2d> p2d_vec;
    for (int i = 0; i < int(V.size()); ++i)
    {
        const std::vector<P2d>& pts = V[i].points();
        p2d_vec.insert(p2d_vec.end(), pts.begin(), pts.end());
    }
    return convex_hull(p2d_vec);
}

//     Input:  P = an array of 2D points
//                 presorted by increasing x- and y-coordinates
//     Return: H = Path of the convex hull vertices
// Note: the last point in the returned list is the same as the first one.
// http://en.wikibooks.org/wiki/Algorithm_Implementation/Geometry/Convex_hull/Monotone_chain
Path2d convex_hull_presorted(const std::vector<P2d>& P)
{
    int n = P.size(), k = 0;

    if (n <= 3)
        return Path2d(P);

    Path2d H(2*n);

    // Build lower hull
    for (int i = 0; i < n; i++) {
        while (k >= 2 && isLeft(H[k-2], H[k-1], P[i]))
            --k;
        H[k++] = P[i];
    }

    // Build upper hull
    for (int i = n-2, t = k+1; i >= 0; i--) {
        while (k >= t && isLeft(H[k-2], H[k-1], P[i]))
            --k;
        H[k++] = P[i];
    }

    H.resize(k);
    return H;
}

inline double angle3(const P2d o, const P2d a, const P2d b)
{
    if (len2(a-o) < 1E-20) return 0.0;
    if (len2(b-o) < 1E-20) return 0.0;
    double ang = angle(a-o, b-o);
    if (ang < 0)
        return 2*PI + ang;
    else
        return ang;
}

// Input:  V     = array of 2D points
//         alpha = angle in radians we accept (usually alpha < PI)
// Return: H     = Path of quasi convex polygon cointaining all the given
//                 points
Path2d _quasi_convex_hull(const std::vector<P2d>& V, const double alpha)
{
    int n = V.size(), k = 0;

    if (n <= 3)
        return Path2d(V);

    Path2d H(2*n);

    // Build lower hull
    for (int i = 0; i < n; i++) {
        while (k >= 2 && angle3(H[k-1], V[i], H[k-2]) < alpha)
            --k;
        H[k++] = V[i];
    }

    // Build upper hull
    for (int i = n-2, t = k+1; i >= 0; i--) {
        while (k >= t && angle3(H[k-1], V[i], H[k-2]) < alpha)
            --k;
        H[k++] = V[i];
    }

    H.resize(k);
    return H;
}

Path2d regularize(const Path2d& path)
{
    P2d a, b;
    std::vector<P2d> pts;
    int extra_points;
    double mean_len = path.len() / path.size();
    for (int i=0; i<path.size()-1; ++i)
    {
        a = path[i];
        b = path[i+1];
        pts.push_back(a);
        extra_points = std::min(int(round(dist(a, b) / mean_len)), 1);
        for (int k=0; k<extra_points; ++k)
            pts.push_back(a + (b-a) * (k+1) / (extra_points+1));
    }
    Path2d rpath(pts);
    return rpath;
}


static bool compare_size_path2d(const Path2d& l, const Path2d& r)
{
    return l.size() < r.size();
}
Path2d quasi_convex_hull(const std::vector<Path2d>& paths, const double alpha)
{
    int npoints = 0, w, h;
    double dw, dh;
    double scale = 1;
    P2d p0, p1;
    std::vector<Path2d> qchulls;
    Path2d qchull;

    for (int i=0; i<int(paths.size()); ++i)
    {
        npoints += paths[i].size();
        Path2d path(paths[i].points());
        path.boundingBox(p0, p1, i!=0);
    }
    dw = p1.x - p0.x;
    dh = p1.y - p0.y;

    // scala in modo da avere sempre ~1000 suddivisioni per lato.
    scale = 1000 / (1 + (dw + dh) / 2.0);
    w = int(dw * (scale + 1));
    h = int(dh * (scale + 1));

    std::vector<P2d> points(npoints);
    std::vector<Path2d> qpaths(paths.size());
    for (int i=0, c=0; i<int(paths.size()); ++i)
    {
        qpaths[i] = (paths[i] - p0) * scale; /* sposta sull'origine del bbox */
        for (int k=0; k<int(qpaths[i].size()); ++k, ++c)
             points[c] = qpaths[i][k];
    }

    if (npoints <= 3)
        return Path2d(points);

    /* Usa due ordinamenti diversi per raggiungere tutte le concavità */
    std::sort(points.begin(), points.end(), CompareP2d());
    Path2d ch1 = _quasi_convex_hull(points, alpha);
    std::sort(points.begin(), points.end(), CompareP2dy());
    Path2d ch2 = _quasi_convex_hull(points, alpha);

    /* (ch1 && ch2) prende il quasi_convex_hull più piccolo */
    Image<int> image(w, h);
    Image<int> image_tmp(w, h);

    std::vector<Path2d> chulls(1);

    // FIXME: fattorizzare creazione immagine
    chulls[0] = ch1;
    image.fastBox(0, 0, w, h, 0); /* zero fill */
    polyFill(image, 1, chulls);
    polyDraw(image, 1, chulls);

    chulls[0] = ch2;
    image_tmp.fastBox(0, 0, w, h, 0); /* zero fill */
    polyFill(image_tmp, 1, chulls);
    polyDraw(image_tmp, 1, chulls);

    image.andImage(image_tmp);

    /* (ch || paths) assicura di non avere poligoni degeneri */
    image_tmp.fastBox(0, 0, w, h, 0); /* zero fill */
    polyFill(image_tmp, 1, qpaths);
    polyDraw(image_tmp, 1, qpaths);

    image.orImage(image_tmp);

    qchulls = marching_squares_gt(image, 0, 0);
    // marching_squares può restituire più poligoni, scelgo quello con più punti
    // TODO: meglio quello con area maggiore?
    qchull = *std::max_element(qchulls.begin(), qchulls.end(), compare_size_path2d);
    qchull = (qchull / scale) + p0; /* ripristina la posizione e la dimensione */
    smooth121(qchull);
    return simplify(qchull);
}

//    Input:  V = polyline array of 2D vertex points
//    Return: H = output convex hull Path of vertices
// Melkman's convex hull algorithm for simple polygons O(n)
Path2d convex_hull(const Path2d& V)
{
    const int n = V.size();
    
    //Un poligono semplice, chiuso e avente almeno 4 lati, ha almeno 5 punti
    if (n < 5)
        return V;
    
    Path2d H(n+1);
    
    Path2d D(2*n+2);
    int bot = n-2, top = bot+3;
    D[bot] = D[top] = V[2];
    if (cross(V[0], V[1], V[2]) > 0) {
        D[bot+1] = V[0];
        D[bot+2] = V[1];
    }
    else {
        D[bot+1] = V[1];
        D[bot+2] = V[0];
    }
    
    for (int i=3; i < n-1; i++) {
        
        if ((cross(D[bot], D[bot+1], V[i]) > 0) &&
            (cross(D[top-1], D[top], V[i]) > 0) )
                continue;
        
        while (cross(D[bot], D[bot+1], V[i]) <= 0)
            ++bot;
        D[--bot] = V[i];

        while (cross(D[top-1], D[top], V[i]) <= 0)
            --top;
        D[++top] = V[i];
    }

    int h;
    for (h=0; h <= (top-bot); h++)
        H[h] = D[bot + h];
    
    H[h] = H[0];
    H.resize(h);
    return H;
}
