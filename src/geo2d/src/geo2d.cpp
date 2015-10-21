#include <assert.h>
#include <algorithm>
#include <math.h>
#include <string.h>
#include <cfloat>
#include "geo2d.h"
#include "inside.h"
#include "base64.h"

#ifdef _DEBUG
long Path2d::alloc_count = 0;

void* Path2d::operator new(size_t sz)
{
    Path2d::alloc_count++;
    return malloc(sz);
}

void Path2d::operator delete(void* p)
{
    Path2d::alloc_count--;
    free(p);
}
#endif

long Path2d::get_alloc_count()
{
#ifdef _DEBUG
    return Path2d::alloc_count;
#else
    return -1;
#endif
}

Path2d& Path2d::operator+=(const P2d& p)
{
    for (int i=0,n=(int)pts.size(); i<n; i++)
        pts[i] += p;
    l.clear();
    return *this;
}

Path2d& Path2d::operator-=(const P2d& p)
{
    for (int i=0,n=(int)pts.size(); i<n; i++)
        pts[i] -= p;
    l.clear();
    return *this;
}

Path2d& Path2d::operator*=(double k)
{
    for (int i=0,n=(int)pts.size(); i<n; i++)
        pts[i] *= k;
    l.clear();
    return *this;
}

Path2d& Path2d::operator*=(const X2d& t)
{
    for (int i=0,n=(int)pts.size(); i<n; i++)
        pts[i] *= t;
    l.clear();
    return *this;
}

Path2d& Path2d::operator/=(double k)
{
    return operator*=(1/k);
}

Path2d& Path2d::operator/=(const X2d& t)
{
    return operator*=(inv(t));
}

int Path2d::calc() const
{
    int n=(int)pts.size();
    if ((int)l.size()!=n)
    {
        l.resize(n);
        for (int i=1; i<n; i++)
            l[i] = l[i-1]+::dist(pts[i],pts[i-1]);
    }
    return n;
}

P2d Path2d::operator()(double s) const
{
    int n = calc();
    assert(n>0);

    if (n == 1) return pts[0];

    int a = 0, b = n-1;

    // Evita punti sovrapposti all'inizio del path
    // che non vorrebbero gestiti correttamente.
    while (fabs(l[a+1] - l[a]) < 1E-20 && a < b-1)
        ++a;
    while (fabs(l[b-1] - l[b]) < 1E-20 && a < b-1)
        --b;

    while (a < b-1)
    {
        int t = (a + b) / 2;
        if (s < l[t]) b = t;
        else a = t;
    }

    assert(b == a+1);
    if (fabs(l[b] - l[a]) < 1E-20)
        return pts[a];
    return linterp(a, s);
}

static void fixdist(const P2d& p,         // punto
                    const Path2d& l,      // spezzata
                    int a,int b,          // tratto da considerare
                    double da,double db,  // distanze estremi
                    double& bsf,          // distanza best-so-far
                    double& bsl,          // distanza lineare best-so-far
                    int& bss,             // segmento best-so-far
                    P2d& bsp)             // proiezione best-so-far
{
    double ll = l.len(b) - l.len(a);
    if (da-ll<bsf && db-ll<bsf)
    {
        // Questo tratto potenzialmente puo'
        // migliorare il valore bsf
        if (b==a+1)
        {
            // Singolo segmento
            P2d d = l[b]-l[a];
            double l2 = ll*ll;
            double t = (l2>1E-10 ? (p-l[a])*d/l2 : 0.5);
            if (t>-1E-5 && t<1+1E-5)
            {
                P2d j = l[a]+t*d;
                double x = dist(j,p);
                if (x<bsf) {
                    bsf=x;
                    bsp=j;
                    bsl=l.len(a)+t*ll;
                    bss=a;
                }
            }
        }
        else
        {
            int i=(a+b)/2;
            double di=dist(p,l[i]);
            if (di<bsf) {
                bsf=di;
                bsp=l[i];
                bsl=l.len(i);
                bss=i;
            }
            if (da<db)
            {
                fixdist(p,l,a,i,da,di,bsf,bsl,bss,bsp);
                fixdist(p,l,i,b,di,db,bsf,bsl,bss,bsp);
            }
            else
            {
                fixdist(p,l,i,b,di,db,bsf,bsl,bss,bsp);
                fixdist(p,l,a,i,da,di,bsf,bsl,bss,bsp);
            }
        }
    }
}

P2d Path2d::project(const P2d& p,double *lc,double *dist,int *seg,int begin,int end) const
{
    int n=calc();
    assert(n>0);

    if (n==1)
    {
        if (lc) *lc=0;
        if (dist) *dist=::dist(p,get(begin));
        if (seg) *seg=begin;
        return pts[0];
    }

    if (end < 0)
        end = n-1;

    double b0=::dist(p,get(begin));  // get() gives us validation for begin/end
    double b1=::dist(p,get(end));
    double bsf,bsl;
    int bss;
    P2d bsp;
    if (b0<b1) {
        bsf=b0;
        bsl=0;
        bsp=pts[begin];
        bss=begin;
    }
    else {
        bsf=b1;
        bsl=len();
        bsp=pts[end];
        bss=end-1;
    }
    fixdist(p,*this,0,n-1,b0,b1,bsf,bsl,bss,bsp);
    if (lc) *lc=bsl;
    if (dist) *dist=bsf;
    if (seg) *seg=bss;
    return bsp;
}

double Path2d::dist(const P2d& p) const
{
    return ::dist(project(p),p);
}

void Path2d::segment(double t0, double t1)
{
    std::vector<P2d> npts;
    int n = calc();

    npts.reserve(n);
    if (t0<t1)
    {
        npts.push_back((*this)(t0));
        for (int i=0;i<n;i++)
            if (t0 < l[i] && l[i] < t1)
                npts.push_back(pts[i]);
        npts.push_back((*this)(t1));
    }
    else if (t0 > t1)
    {
        npts.push_back((*this)(t0));
        for (int i=n-1;i>=0;i--)
            if (t1 < l[i] && l[i] < t0)
                npts.push_back(pts[i]);
        npts.push_back((*this)(t1));
    }
    else
        npts.push_back((*this)(t0));
    swap(npts);
}

void Path2d::clean(void)
{
    std::vector<P2d> npts;
    int n=size();

    npts.reserve(n);
    npts.push_back(pts[0]);
    for (int i=1;i<n;i++)
        if (::dist(pts[i-1], pts[i]) > 1E-7)
            npts.push_back(pts[i]);
    swap(npts);
}

void Path2d::reverse()
{
    std::reverse(pts.begin(), pts.end());
    l.clear();
}

P2d Path2d::tanl(double t) const
{
    return dir((*this)(t) - (*this)(t - 1E-7));
}

P2d Path2d::tanr(double t) const
{
    return dir((*this)(t + 1E-7) - (*this)(t));
}

P2d Path2d::tan(double t, bool closed, double eps) const
{
    P2d v;

    if (closed) {
        P2d p1 = (*this)(fmod((t + eps), this->len()));
        // Aggiunge la lunghezza del path al secondo parametro prima di
        // calcolare il modulo, per essere sicuro che il valore di
        // ritorno sia positivo (vedi differenze tra % di python e fmod)
        P2d p2 = (*this)(fmod((t + this->len() - eps), this->len()));
        v = p1 - p2;
    }
    else
        v = (*this)(t + eps) - (*this)(t - eps);

    if (len2(v) < 1E-20)
        return P2d::none();
    return dir(v);
}

double pathPathDist(const Path2d& path1, const Path2d& path2, int n)
{
    int n1 = path1.size();
    int n2 = path2.size();
    assert(n1 >= 2);
    assert(n2 >= 2);

    double L1 = path1.len();
    double L2 = path2.len();
    assert(L1 != 0.0);
    assert(L2 != 0.0);
    double s1 = 0.0;       // distanza percorsa sul path1
    double s2 = 0.0;       // distanza percorsa sul path2
    int i1 = 0;            // Indice segmento corrente sul path 1 (0..n1-2)
    int i2 = 0;            // Indice segmento corrente sul path 2 (0..n2-2)
    double d1 = L1 / n;    // passo sul path1
    double d2 = L2 / n;    // passo sul path2
    assert(d1 != 0.0);
    assert(d2 != 0.0);

    double maxdist = 0.0;

    // Ignora eventuali punti sovrapposti in fondo al path. Questo è
    // necessario perché il ciclo while() che identifica il segmento
    // di appartenza di un punto all'interno dell'algoritmo si deve
    // obbligatoriamente fermare alla fine del path: se l'ultimo segment
    // avesse lunghezza 0, ci sarebbe una divisione per zero.
    while (path1.len(n1-1) == path1.len(n1-2))
    {
        n1--;
        // Non succederà mai perché i path sono di lunghezza non nulla (pre-
        // condizione dell'algoritmo).
        assert(n1 > 0);
    }

    while (path2.len(n2-1) == path2.len(n2-2))
    {
        n2--;
        // Non succederà mai perché i path sono di lunghezza non nulla (pre-
        // condizione dell'algoritmo).
        assert(n2 >= 0);
    }

    for (int i=0;i<n;i++)
    {
        // Avanza sul path 1.
        s1 += d1;

        // Calcola pp1 = path1(s1), ma sfrutta il fatto che ci stiamo muovendo
        // linearmente, e quindi usa linterp() invece che "cascare dalle
        // nuvole" come sarebbe costretta a fare la funzione operator().

        // Identifica il segmento dove risiede s1 (evita automaticamente
        // punti sovrapposti).
        while (i1 < n1-2 && path1.len(i1+1) <= s1)
            i1++;
        P2d pp1 = path1.linterp(i1, s1);

        // Come sopra, ma per il secondo path.
        s2 += d2;
        while (i2 < n2-2 && path2.len(i2+1) <= s2)
            i2++;
        P2d pp2 = path2.linterp(i2, s2);

        // Aggiorna la distanza massima misurata
        double dd = ::dist2(pp1, pp2);
        if (dd > maxdist)
            maxdist = dd;
    }

    /*
     * Gestione ultimo punto
     * (non viene fatto nel loop per evitare problemi di arrotondamento
     * in concomitanza ad una replicazione dell'ultimo punto che
     * porterebbero ad una divisione per zero nel calcolo di w)
     */
    P2d pp1 = path1[n1-1];
    P2d pp2 = path2[n2-1];
    double dd = ::dist2(pp1, pp2);
    if (dd > maxdist)
        maxdist = dd;
    return sqrt(maxdist);
}

/*
 * Raffinamento di un path
 */
Path2d refine(const Path2d& p, double maxstep)
{
    Path2d res;
    if (p.size() != 0)
    {
        P2d p0 = p[0];
        res.push_back(p0);
        for (int i=1,n=p.size(); i<n; i++)
        {
            P2d p1 = p[i];
            P2d delta = p1 - p0;
            double d = len(delta);
            if (d > maxstep)
            {
                // Segmento troppo lungo, aggiunge punti di split
                int nsub = 1 + int(d / maxstep);
                delta /= nsub;
                for (int j=1; j<nsub; j++)
                    res.push_back(p0 + j*delta);
            }
            res.push_back(p1);
            p0 = p1;
        }
    }
    return res;
}

/*
 *  Calcola l'area di un poligono; l'area è positiva se il poligono è definito
 *  in senso antiorario, negativa altrimenti.
 */
double sarea(const Path2d& poly)
{
    int numero_punti = poly.size();
    if (numero_punti < 3) return 0.0;
    double area = 0.0;

    P2d p1 = poly[numero_punti - 1];
    for (int i = 0; i < numero_punti; i++)
    {
        P2d p2 = poly[i];
        area += p1.x * p2.y - p2.x * p1.y;
        p1 = p2;
    }
    return area/2;
}

double area(const Path2d& poly) {
    return fabs(sarea(poly));
}


/*
 *  Calcola l'area contenuta in una lista di Path2d, che rappresentano contorni
 *  di un poligono od un gruppo di poligoni non semplicemente connessi.
 *  In caso di bordi sovrapposti è possibile specificare num_checks>1 in modo
 *  da ridurre la probabilità di fare test su punti che risultano esterni perché
 *  sul bordo del poligono di test.
 *     _ _ _
 *  1.|_ _  |
 *  2.|_ _| |
 *    |_ _ _|
 *
 *  IMPORTANTE: l'ipotesi che viene assunta e che l'algoritmo non controlla che
 *  che non ci siano intersezioni.
 */

inline bool inside_test(const Path2d& points, const Path2d& poly, int num_checks)
{
    if (num_checks < 1)
        return false;
    if (num_checks == 1) // compatibilita' col passato
        return inside(points[0], poly);
    int npt = points.size();
    int step = npt/num_checks>0?npt/num_checks:1;
    for (int i = 0; i < npt; i += step)
        if (inside_equal(points[i], poly, false)) // se sul bordo, continua a cercare
            return true;
    return false;
}

double area(const std::vector<Path2d>& path_list, int num_checks)
{
    int numero_contorni = path_list.size();
    double area_calcolata = 0.0;

    for (int i = 0; i < numero_contorni; i++)
    {
        bool dentro = false;

        for (int j = 0; j < numero_contorni; j++)
        {
            if (j == i) continue;
            if (inside_test(path_list[i], path_list[j], num_checks))
                dentro = !dentro;
        }

        if (dentro == true)
            area_calcolata -= area(path_list[i]);
        else
            area_calcolata += area(path_list[i]);
    }

    return area_calcolata;
}

Path2d clipPoly(const Path2d& poly, const P2d& p, const P2d& n)
{
    int npt = poly.size();
    std::vector<double> d(npt);
    for (int i=0; i<npt; i++)
        d[i] = (poly[i] - p)*n;
    Path2d res;
    for (int i=0,j=npt-1; i<npt; j=i++)
    {
        if ((d[i]<0 && d[j]>=0) || (d[j] < 0 && d[i] >= 0))
        {
            double t = d[j] / (d[j] - d[i]);
            res.push_back(poly[j] + t*(poly[i] - poly[j]));
        }
        if (d[i] >= 0)
            res.push_back(poly[i]);
    }
    return res;
}

std::string Path2d::tostring(void) const
{
    return base64_encode(reinterpret_cast<const unsigned char*>(&pts[0]), sizeof(pts[0])*pts.size());
}

Path2d Path2d::fromstring(const std::string& s)
{
    std::string bin = base64_decode(s);
    std::vector<P2d> vp(bin.size() / sizeof(P2d));
    memcpy(&vp[0], &bin[0], bin.size());
    return Path2d(vp);
}

void bezto(const P2d& a, const P2d& b, const P2d& c, const P2d& d,
           std::vector<P2d>& path, double err)
{
    P2d ab = (a + b) / 2;
    P2d bc = (b + c) / 2;
    P2d cd = (c + d) / 2;
    if (dist(a, b) + dist(b, c) + dist(c, d) < err)
    {
        path.push_back(d);
    }
    else
    {
        P2d abc = (ab + bc) / 2;
        P2d bcd = (bc + cd) / 2;
        P2d abcd = (abc + bcd) / 2;
        bezto(a, ab, abc, abcd, path, err);
        bezto(abcd, bcd, cd, d, path, err);
    }
}

void smooth121(Path2d &path) {
    P2d o = path[0];
    int n = int(path.size());
    for (int i = 1; i < n - 1; i++) {
        P2d pn = (o + 2 * path[i] + path[i + 1]) / 4;
        o = path[i];
        path[i] = pn;
    }
}

std::vector<bool> oddEven(const std::vector<Path2d>& paths) {
    std::vector<bool> res;
    int numero_contorni = paths.size();
    std::vector<InsideTester> it;

    for (int i = 0; i < numero_contorni; i++) {
        it.push_back(InsideTester(paths[i]));
    }

    for (int i = 0; i < numero_contorni; i++) {
        P2d primo_punto = paths[i][0];
        bool dentro = false;

        for (int j = 0; j < numero_contorni; j++) {
            if (j != i) {
                if (it[j](primo_punto)) {
                    dentro = !dentro;
                }
            }
        }
        res.push_back(dentro);
    }
    return res;
}

void orient(std::vector<Path2d>& paths) {
    std::vector<bool> odd_even = oddEven(paths);
    for (int i = 0; i < int(paths.size()); ++i) {
        bool cclockwise = sarea(paths[i]) > 0;
        if (odd_even[i] != cclockwise) {
            paths[i].reverse();
        }
    }
}

P2d mostInternalP(const std::vector<Path2d> &boundary) {
    const int N_PASSI = 100;

    int np = int(boundary.size());
    std::vector<InsideTester> its;
    double x0 = 1e30, y0 = 1e30, x1 = -1e30, y1 = -1e30;
    P2d bsf_p;
    for (int i = 0; i < np; ++i) {
        const Path2d &path = boundary[i];
        its.push_back(InsideTester(path));
        int npts = int(path.size());
        for (int j = 0; j < npts; ++j) {
            const P2d &p = path[j];
            if (p.x < x0) x0 = p.x;
            if (p.x > x1) x1 = p.x;
            if (p.y < y0) y0 = p.y;
            if (p.y > y1) y1 = p.y;
            bsf_p = p;
        }
    }
    assert(x0 <= 1e30 - 1);
    double dx = (x1 - x0) / N_PASSI;
    double dy = (y1 - y0) / N_PASSI;

    double bsf = 0.0;
    double x = x0 + dx;
    while (x < x1) {
        double y = y0 + dy;
        while (y < y1) {
            P2d p = P2d(x, y);
            int inside = 0;
            for (int i = 0; i < np; ++i) {
                if (its[i](p)) {
                    inside++;
                }
            }
            if (inside % 2 == 1) {
                double d = 1e30;
                for (int i = 0; i < np; ++i) {
                    double d2 = boundary[i].dist(p);
                    if (d2 < d) d = d2;
                }
                if (d > bsf) {
                    bsf = d;
                    bsf_p = p;
                }
            }
            y += dy;
        }
        x += dx;
    }
    return bsf_p;
}

std::vector<Path2d> dash(const Path2d& path, double l_pen, double space, double epsilon) {
    int npts = path.size();

    // Esce se troppo pochi punti o il tratteggio e` insensato
    if (npts < 2 || l_pen < epsilon || space < epsilon) {
        return std::vector<Path2d>(1, path);
    }

    double passo = l_pen + space;
    double plen = path.len();
    bool chiuso = dist(path[0], path.back()) <= epsilon;

    int n_tratti;
    double next_t;
    double scala;
    if (chiuso) {
        n_tratti = int(floor(plen / passo + 0.5));
    } else {
        n_tratti = int(floor((plen + space) / passo + 0.5));
    }

    // Esce se il numero di tratti e` meno di due
    if (n_tratti <= 1) {
        return std::vector<Path2d>(1, path);
    }

    // Fasa e scala il tratteggio in modo da iniziare e finire con un tratto
    // pieno. In caso di path chiuso, inizia e finisce con mezzo tratto pieno.
    if (chiuso) {
        next_t = -l_pen / 2;
        scala = plen / (n_tratti * passo);
    } else {
        next_t = 0;
        scala = plen / (n_tratti * l_pen + (n_tratti - 1) * space);
    }
    l_pen *= scala;
    space *= scala;
    passo = l_pen + space;

    std::vector<Path2d> res;
    int i = 0;
    while (next_t < plen && i < npts) {
        res.push_back(Path2d());
        // Tratto pieno
        res.back().push_back(path(next_t < 0.0 ? 0.0 : next_t));
        next_t += l_pen;
        double t_i = path.len(i);
        while (i < npts && t_i < next_t) {
            if (len(path(t_i) - res.back().back()) > epsilon) {
                res.back().push_back(path(t_i));
            }
            ++i;
            if (i < npts) t_i = path.len(i);
        }
        P2d next_p = path(next_t);
        if (len(next_p - res.back().back()) > epsilon && next_t <= plen) {
            res.back().push_back(next_p);
        }
        // Tratto vuoto
        next_t += space;
        while (i < npts && path.len(i) <= next_t) {
            ++i;
        }
    }
    return res;
}

/* Calcolo del bounding box relativo a un vector di path. */
void boundingBox(const std::vector<Path2d>& paths, bool initPointsFirst,
                 P2d* bboxMin, P2d* bboxMax)
{
    if (!bboxMin || !bboxMax)
        return;

    if (initPointsFirst)
    {
        bboxMin->x = bboxMin->y = DBL_MAX;
        bboxMax->x = bboxMax->y = -DBL_MAX;
    }

    for (int i = 0; i < int(paths.size()); ++i)
    {
        const Path2d& path = paths[i];
        for (int j = 0; j < path.size(); ++j)
        {
            const P2d& pt = path[j];
            if (bboxMin->x > pt.x) bboxMin->x = pt.x;
            if (bboxMin->y > pt.y) bboxMin->y = pt.y;
            if (bboxMax->x < pt.x) bboxMax->x = pt.x;
            if (bboxMax->y < pt.y) bboxMax->y = pt.y;
        }

    }

}

/* Calcolo del bounding box relativo a un vector di vector di path. */
void boundingBox(const std::vector<std::vector<Path2d> >& regions, bool initPointsFirst,
                 P2d* bboxMin, P2d* bboxMax)
{
    if (!bboxMin || !bboxMax || regions.empty())
        return;

    boundingBox(regions[0], initPointsFirst, bboxMin, bboxMax);
    for (int i = 1; i < int(regions.size()); ++i)
        boundingBox(regions[i], false, bboxMin, bboxMax);

}

Path2d bridgedPath(const std::vector<Path2d> &paths) {
    bool first = true;
    P2d p0;
    Path2d res;
    for (int i = 0, n = paths.size(); i < n; ++i) {
        const Path2d &path = paths[i];
        if (!first) res.push_back(p0);
        for (int j = 0, m = path.size(); j < m; ++j) {
            if (first) {
                p0 = path[j];
                first = false;
            }
            res.push_back(path[j]);
        }
        res.push_back(p0);
    }
    return res;
}
