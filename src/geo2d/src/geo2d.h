#if !defined(GEO2D_H)
#define GEO2D_H

#include "sysdep.h"

// Definisci a 1 a linea di comando se stai linkando al Python e vuoi usare
// il pool allocator del Python per le classi primarie.
#ifndef USE_PYMALLOC
#define USE_PYMALLOC 0
#endif

#include <vector>
#include <math.h>
#include <assert.h>
#include <climits>
#include <string>

#ifdef _MSC_VER
    #include <float.h>
#endif

#ifdef PI
#undef PI
#endif

#ifndef _PI
#define _PI
static const double PI = 3.14159265358979323846264338327950288419716939937510;
#endif

#if USE_PYMALLOC
#include <Python.h>

struct PyMallocObject
{
    static void* operator new(size_t n)
    {
        PyGILState_STATE state = PyGILState_Ensure();
        void *mem = PyObject_Malloc(n);
        PyGILState_Release(state);
        return mem;
    }

    static void* operator new(size_t n, void *p)
    {
        return ::operator new(n, p);
    }

    static void operator delete(void *p, size_t)
    {
        PyGILState_STATE state = PyGILState_Ensure();
        PyObject_Free(p);
        PyGILState_Release(state);
    }

    static void operator delete(void *p, void *a)
    {
        return ::operator delete(p, a);
    }
};

#endif

#ifdef GEO2D_QT_SUPPORT
#include <QPointF>
#include <QTransform>
#endif

const double P2DINF =  HUGE_VAL;


//
// Punto bidimensionale
//

//DOC:P2d

struct P2d
#if USE_PYMALLOC
            : public PyMallocObject
#endif
{
    double x,y;

    P2d(double x,double y) : x(x),y(y) {}
    P2d() : x(0),y(0) {}
#ifdef GEO2D_QT_SUPPORT
    P2d(const QPointF& p) : x(p.x()), y(p.y()) {}
    P2d(const QPoint& p) : x(p.x()), y(p.y()) {}
    operator QPointF() const {return QPointF(x, y);}
#endif

    inline P2d& operator+=(const P2d& o) {
        x+=o.x;
        y+=o.y;
        return *this;
    }
    inline P2d& operator-=(const P2d& o) {
        x-=o.x;
        y-=o.y;
        return *this;
    }
    inline P2d& operator*=(double k)     {
        x*=k;
        y*=k;
        return *this;
    }
    inline P2d& operator/=(double k)     {
        x/=k;
        y/=k;
        return *this;
    }
    inline P2d& operator*=(const struct X2d&);

    inline double operator[](int index) const {
        assert(index >= 0 && index <= 1);

        switch (index) {
        case 0:
            return(x);
        case 1:
            return(y);
        }

        /* Never Reached */
        return(0.0);
    }

    inline bool isValid() const {
#ifdef _MSC_VER
        int r;

        r = _fpclass(x);
        if ((r & _FPCLASS_SNAN) || (r & _FPCLASS_QNAN) ||
            (r & _FPCLASS_NINF) || (r & _FPCLASS_ND) ||
            (r & _FPCLASS_PD)   || (r & _FPCLASS_PINF))
            return(false);

        r = _fpclass(y);
        if ((r & _FPCLASS_SNAN) || (r & _FPCLASS_QNAN) ||
            (r & _FPCLASS_NINF) || (r & _FPCLASS_ND) ||
            (r & _FPCLASS_PD)   || (r & _FPCLASS_PINF))
            return(false);
#elif defined(isfinite) || __cplusplus > 199711L
        using namespace std;
        if(!isfinite(x) || !isfinite(y))
            return false;
#else
        if (!finite(x))
            return(false);

        if (!finite(y))
            return(false);
#endif

        return(true);
    }


    /*
     * Punto speciale non valido. Questo punto ? considerato "out-of-band" e
     * pu? essere usato per segnalare condizioni di errore.
     * Con le eccezioni floating point attive, genera un segfault non appena
     * si prova ad usarlo.
     */
    static P2d none()
    {
        return P2d(P2DINF, P2DINF);
    }
};


//ENDDOC

inline P2d operator+(const P2d& a,const P2d& b) {
    return P2d(a.x+b.x, a.y+b.y);
}
inline P2d operator-(const P2d& a,const P2d& b) {
    return P2d(a.x-b.x, a.y-b.y);
}
inline P2d operator*(double k,const P2d& a)     {
    return P2d(a.x*k, a.y*k);
}
inline P2d operator*(const P2d& a,double k)     {
    return P2d(a.x*k, a.y*k);
}
inline P2d operator/(const P2d& a,double k)     {
    return (1.0/k)*a;
}
inline P2d operator-(const P2d& a)       {
    return P2d(-a.x,-a.y);
}
inline bool operator==(const P2d& a, const P2d& b) {
    return a.x==b.x && a.y==b.y;
}
inline bool operator!=(const P2d& a, const P2d& b) {
    return !(a==b);
}

inline double operator*(const P2d& a,const P2d& b) {
    return a.x*b.x+a.y*b.y;
}
inline double operator^(const P2d& a,const P2d& b) {
    return a.x*b.y-a.y*b.x;
}
inline double len2(const P2d& a) {
    return a*a;
}
inline double len(const P2d& a)  {
    return sqrt(a*a);
}
inline double dist2(const P2d& a,const P2d& b) {
    return len2(a-b);
}
inline double dist(const P2d& a,const P2d& b)  {
    return len(a-b);
}

// Calcolo del modulo con segno
// (mutuato da Python, Objects/floatmodule.c, float_rem()
inline double sfmod(double vx, double wx)
{
    double mod = fmod(vx, wx);
    /* note: checking mod*wx < 0 is incorrect -- underflows to
       0 if wx < sqrt(smallest nonzero double) */
    if (mod && ((wx < 0) != (mod < 0))) {
        mod += wx;
    }
    return mod;
}

inline int imod(int x, int y)
{
    int xdivy = x / y;
    int xmody = x - xdivy * y;
    /* If the signs of x and y differ, and the remainder is non-0,
     * C89 doesn't define whether xdivy is now the floor or the
     * ceiling of the infinitely precise quotient.  We want the floor,
     * and we have it iff the remainder's sign matches y's.
     */
    if (xmody && ((y ^ xmody) < 0) /* i.e. and signs differ */) {
        xmody += y;
        assert(xmody && ((y ^ xmody) >= 0));
    }
    return xmody;
}

inline double arg(const P2d& a)
{
    assert(len2(a) > 1E-20);
    return atan2(a.y,a.x);
}

inline P2d dir(const P2d& a)
{
    assert(len2(a) > 1E-20);
    return a/len(a);
}

// Calcola l'angolo (con segno) tra i vettori p0 e p1
inline double angle(const P2d& p0, const P2d& p1)
{
    return sfmod((arg(p1) - arg(p0) + PI), (2*PI)) - PI;
}

inline P2d ortho(const P2d& a) {
    return P2d(-a.y,a.x);
}

// Project point p onto segment a,b
inline double _project(const P2d& p, const P2d& a, const P2d& b)
{
    P2d d = b - a;
    double L2 = len2(d);
    if (L2 < 1E-20)
        return 0.5;
    return (p-a) * d / L2;
}

inline P2d project(const P2d& p, const P2d& a, const P2d& b, bool cap=true)
{
    double t = _project(p, a, b);
    if ((t <= 0) && cap)
        return a;
    else if ((t >= 1) && cap)
        return b;
    else
        return a + t*(b-a);
}

// Calcola l'area del triangolo di vertici a, b, c
inline double triangleArea(const P2d& a, const P2d& b, const P2d& c)
{
    return 0.5 * ((b-a)^(c-a));
}

//
// Trasformazione bidimensionale
//

//DOC:X2d
struct X2d
{
    P2d a,b,c;

    X2d() : a(1,0),b(0,1),c(0,0) {}
    X2d(const P2d& a,const P2d& b,const P2d& c) : a(a),b(b),c(c) {}
#ifdef GEO2D_QT_SUPPORT
    X2d(const QTransform& x) : a(x.m11(), x.m12()), b(x.m21(), x.m22()),
                               c(x.m31(), x.m32()){}
    operator QTransform() const {
        return QTransform(a.x, a.y, 0, b.x, b.y, 0, c.x, c.y);
    }
#endif
    X2d& operator*=(const X2d& m);
    X2d& operator/=(const X2d& m);

    void toGL(void *mem) const;
    bool isMirror (void) const;
    bool isIdentity (void) const;

    static X2d rot(double alpha);
    static X2d xlate(const P2d& t);
    static X2d scale(double s);
    static X2d scale(double x,double y);
    static X2d rot(const P2d& c,double alpha);
    static X2d scale(const P2d& c,double s);
    static X2d scale(const P2d& c,double x,double y);
    static X2d xform(const P2d& a1,const P2d& b1,
                     const P2d& a2,const P2d& b2);
    static X2d mirror(const P2d& a,const P2d& b);
};
//ENDDOC

inline void X2d::toGL(void *vmem) const
{
    double *mem = static_cast<double*>(vmem);

    mem[0] = a.x;
    mem[1] = a.y;
    mem[2] = 0.0;
    mem[3] = 0.0;

    mem[4] = b.x;
    mem[5] = b.y;
    mem[6] = 0.0;
    mem[7] = 0.0;

    mem[8] = 0.0;
    mem[9] = 0.0;
    mem[10] = 1.0;
    mem[11] = 0.0;

    mem[12] = c.x;
    mem[13] = c.y;
    mem[14] = 0.0;
    mem[15] = 1.0;
}

inline bool X2d::isMirror (void) const {
    return((a ^ b) < 0);
}

inline bool X2d::isIdentity (void) const {
    return(((fabs(a.x - 1.0) < 1E-20) && (fabs(a.y - 0.0) < 1E-20)) &&
           ((fabs(b.x - 0.0) < 1E-20) && (fabs(b.y - 1.0) < 1E-20)) &&
           ((fabs(c.x - 0.0) < 1E-20) && (fabs(c.y - 0.0) < 1E-20)));
}

inline P2d& P2d::operator*=(const X2d& m)
{
    return *this = x*m.a+y*m.b+m.c;
}

inline P2d operator*(const P2d& p,const X2d& m)
{
    return p.x*m.a+p.y*m.b+m.c;
}

inline X2d& X2d::operator*=(const X2d& m)
{
    a = a.x*m.a+a.y*m.b;
    b = b.x*m.a+b.y*m.b;
    c = c*m;
    return *this;
}

// True se la trasformazione non e' invertibile
inline bool singular(const X2d& m)
{
    return fabs(m.a ^ m.b) < 1E-20;
}

inline X2d inv(const X2d& m)
{
    double k = 1/(m.a^m.b);
    return X2d(P2d(m.b.y,-m.a.y)*k,
               P2d(-m.b.x,m.a.x)*k,
               P2d(m.b^m.c,m.c^m.a)*k);
}

inline X2d xposinv(const X2d& m)
{
    X2d i = inv(m);
    return X2d(P2d(i.a.x, i.b.x),
               P2d(i.a.y, i.b.y),
               P2d(0, 0));
}

inline P2d operator/(P2d p,const X2d& m)
{
    p -= m.c;
    return P2d(p^m.b,m.a^p)/(m.a^m.b);
}

inline X2d operator*(X2d a,const X2d& b)  {
    return a*=b;
}
inline X2d& X2d::operator/=(const X2d& t) {
    return operator*=(inv(t));
}
inline X2d operator/(X2d a,const X2d& b) {
    return a*=inv(b);
}

inline X2d X2d::rot(double alpha)
{
    double ca=cos(alpha),sa=sin(alpha);
    return X2d(P2d(ca,sa),P2d(-sa,ca),P2d(0,0));
}

inline X2d X2d::xlate(const P2d& t)
{
    return X2d(P2d(1,0),P2d(0,1),t);
}

inline X2d X2d::scale(double s)
{
    return scale(s,s);
}

inline X2d X2d::scale(double x, double y)
{
    return X2d(P2d(x,0),P2d(0,y),P2d(0,0));
}

inline X2d X2d::rot(const P2d& c,double alpha)
{
    return xlate(-c)*rot(alpha)*xlate(c);
}

inline X2d X2d::scale(const P2d& c,double s)
{
    return scale(c,s,s);
}

inline X2d X2d::scale(const P2d& c,double x,double y)
{
    return xlate(-c)*scale(x,y)*xlate(c);
}

inline X2d X2d::mirror(const P2d& a,const P2d& b)
{
    double alpha=arg(b-a);
    return xlate(-a)
           *rot(-alpha)
           *X2d(P2d(1,0),P2d(0,-1),P2d(0,0))
           *rot(alpha)
           *xlate(a);
}

inline X2d X2d::xform(const P2d& a1,const P2d& b1,
                      const P2d& a2,const P2d& b2)
{
    P2d v1=b1-a1,v2=b2-a2;
    return xlate(-a1)
           *rot(arg(v2)-arg(v1))
           *scale(len(v2)/len(v1))
           *xlate(a2);
}

//
// Spezzate bidimensionali
//

//DOC:Path2d
class Path2d
#if USE_PYMALLOC
            : public PyMallocObject
#endif
{
public:
    Path2d();
    explicit Path2d(int n);
    Path2d(const std::vector<P2d>& x);
    Path2d(int n, double *v);
#ifdef GEO2D_QT_SUPPORT
    Path2d(const QPolygonF& p);
    operator QPolygonF() const;
#endif

    P2d  get(int index) const;
    void set(int index,const P2d& p);

    const std::vector<P2d>& points() const;

    int  size() const;
    P2d  operator[](int index) const;
    P2d& operator[](int index);
    P2d  back() const;
    P2d& back();
    P2d  front() const;
    P2d& front();
    void push_back(const P2d& p);
    void pop_back();
    void insert(int index,const P2d& p);
    void erase(int index);
    void clear();
    void resize(int size);
    void segment(double t0, double t1);
    void copy(const std::vector<P2d>& pv);
    void swap(std::vector<P2d>& pv);
    void swap(Path2d& x);
    double len(int index=-1) const;
    P2d operator()(double t) const;
    P2d linterp(int a, double s) const;
    double dist(const P2d& p) const;
    P2d project(const P2d& p,double *lc=0,double *dist=0,int *seg=0,int begin=0,int end=-1) const;
    void clean(void);
    void reverse();

    /*
     * Torna la tangente "morbida" al path, oppure P2d::none se la tangente
     * non è definita (es: punto in cui il path fa un angolo di 180°, cioè
     * torna indietro).
     */
    P2d tan(double t, bool closed=false, double eps=1e-7) const;

    /*
     * Tangente sinistra e destra al punto specificato. Torna in pratica il
     * vettore del segmento su cui poggia il punto. La differenza tra
     * sinistra e destra esiste solo se il punto è esattamente uno
     * dei punti di definizione del path.
     */
    P2d tanl(double t) const;
    P2d tanr(double t) const;

    Path2d& operator+=(const P2d& x);
    Path2d& operator-=(const P2d& x);
    Path2d& operator*=(double k);
    Path2d& operator/=(double k);
    Path2d& operator*=(const X2d& m);
    Path2d& operator/=(const X2d& m);

    static long get_alloc_count();
#ifdef _DEBUG
    static long alloc_count;
    void* operator new(size_t sz);
    void operator delete(void* p);
#endif

    std::string tostring(void) const;
    static Path2d fromstring(const std::string& s);

    void boundingBox(P2d& p0, P2d& p1, bool expand=false);
    P2d *data();
    const P2d *data() const;
private:
    // ...
    int calc() const;
    std::vector<P2d> pts;
    mutable std::vector<double> l;
    //END...
};
//ENDDOC

inline Path2d::Path2d() {}
inline Path2d::Path2d(const std::vector<P2d>& x) : pts(x) {}
inline Path2d::Path2d(int n) : pts(n) {}
inline Path2d::Path2d(int n, double *v)
{
    pts.resize(n);
    for (int i=0; i<n; i+=2)
        pts.push_back(P2d(v[i], v[i+1]));
}

#ifdef GEO2D_QT_SUPPORT
inline Path2d::Path2d(const QPolygonF& p) {
    this->resize(p.size());
    for (int i = 0, n = p.size(); i < n; ++i) (*this)[i] = p[i];
}

inline Path2d::operator QPolygonF() const {
    QPolygonF res(size());
    for (int i = 0, n = size(); i < n; ++i) res[i] = (*this)[i];
    return res;
}
#endif

inline P2d Path2d::linterp(int a, double s) const {
    int b = a+1;
    double w = (s-l[a]) / (l[b]-l[a]);
    return pts[a] + w * (pts[b]-pts[a]);
}
inline double Path2d::len(int index) const {
    return calc()==0 ? 0 : l[index==-1 ? size()-1 : index];
}

inline int Path2d::size() const
{
    return (int)pts.size();
}

inline const std::vector<P2d>& Path2d::points() const
{
    return pts;
}

inline P2d Path2d::get(int index) const
{
    assert(index>=0 && index<(int)pts.size());
    return pts[index];
}

inline void Path2d::set(int index,const P2d& p)
{
    assert(index>=0 && index<(int)pts.size());
    pts[index]=p;
}

inline P2d Path2d::operator[](int index) const
{
    assert(index>=0 && index<(int)pts.size());
    return pts[index];
}

inline P2d& Path2d::operator[](int index)
{
    l.clear();
    assert(index>=0 && index<(int)pts.size());
    return pts[index];
}

inline const P2d *Path2d::data() const
{
    return size()?&pts[0]:NULL;
}

inline P2d *Path2d::data()
{
    l.clear();
    return size()?&pts[0]:NULL;
}

inline void Path2d::push_back(const P2d& p)
{
    pts.push_back(p);
    l.clear();
}

inline P2d Path2d::back() const
{
    assert(pts.size()>0);
    return pts.back();
}

inline P2d& Path2d::back()
{
    assert(pts.size()>0);
    l.clear();
    return pts.back();
}

inline P2d Path2d::front() const
{
    assert(pts.size()>0);
    return pts.front();
}

inline P2d &Path2d::front()
{
    assert(pts.size()>0);
    l.clear();
    return pts.front();
}

inline void Path2d::pop_back()
{
    assert(pts.size() > 0);
    pts.resize(pts.size()-1);
}

inline void Path2d::insert(int index,const P2d& p)
{
    assert(index>=0 && index<=(int)pts.size());
    pts.insert(pts.begin()+index,p);
    l.clear();
}

inline void Path2d::erase(int index)
{
    assert(index>=0 && index<(int)pts.size());
    pts.erase(pts.begin()+index);
    l.clear();
}

inline void Path2d::clear()
{
    pts.clear();
    l.clear();
}

inline void Path2d::resize(int size)
{
    assert(size>=0);
    pts.resize(size);
    l.clear();
}

inline void Path2d::copy(const std::vector<P2d>& pv)
{
    pts = pv;
    l.clear();
}

inline void Path2d::swap(std::vector<P2d>& pv)
{
    pts.swap(pv);
    l.clear();
}

inline void Path2d::swap(Path2d& x)
{
    pts.swap(x.pts);
    l.swap(x.l);
}

inline void Path2d::boundingBox(P2d& p0, P2d& p1, bool expand) {
    /*
     * Calcola in bounding box del path, se expand==true espande il
     * bbox fornito (p0 e p1 devono essere estremi validi).
     * */
    double x0=1E30, y0=1E30, x1=-1E30, y1=-1E30;
    if (expand)
        x0=p0.x, y0=p0.y, x1=p1.x, y1=p1.y;
    if (pts.empty())
        return;
    for (int j=0,m=size(); j<m; j++) {
        const P2d& pt = pts[j];
        if (x0 > pt.x) x0 = pt.x;
        if (y0 > pt.y) y0 = pt.y;
        if (x1 < pt.x) x1 = pt.x;
        if (y1 < pt.y) y1 = pt.y;
    }
    p0.x = x0;
    p0.y = y0;
    p1.x = x1;
    p1.y = y1;
}

inline Path2d operator+(Path2d p,const P2d& x) {
    return p+=x;
}
inline Path2d operator-(Path2d p,const P2d& x) {
    return p-=x;
}
inline Path2d operator*(Path2d p,double k)     {
    return p*=k;
}
inline Path2d operator*(double k,Path2d p)     {
    return p*=k;
}
inline Path2d operator/(Path2d p,double k)     {
    return p/=k;
}
inline Path2d operator*(Path2d p,const X2d& m) {
    return p*=m;
}
inline Path2d operator/(Path2d p,const X2d& m) {
    return p/=m;
}

#ifdef _DEBUG

inline bool operator == (const Path2d& p,const Path2d& arg)
{
    if (p.size() != arg.size())
        return false;

    for (int i=0; i<p.size(); ++i)
        if (p[i] != arg[i])
            return false;

    return true;
}

inline bool operator != (const Path2d& p,const Path2d& arg)
{
    if (p.size() != arg.size())
        return true;

    for (int i=0; i<p.size(); ++i)
        if (p[i] != arg[i])
            return true;

    return false;
}

#endif

inline double dist(const P2d& p,const Path2d& l)
{
    assert(l.size()>0);
    return l.dist(p);
}

inline double dist(const Path2d& l,const P2d& p)
{
    assert(l.size()>0);
    return l.dist(p);
}

inline P2d project(const P2d& p,const Path2d& l,double* lc = 0)
{
    assert(l.size()>0);
    return l.project(p,lc);
}

inline void smooth_path(Path2d& pts, int n)
{
    Path2d res;
    for (int i=0; i<n; i++)
    {
        double s = i/(n-1.0);
        double w = (1-cos(s*3.141592654))/2;
        double wj = w*(pts.size()-1);
        int j = int(floor(wj));
        if (j > pts.size()-2) j = pts.size()-2;
        res.push_back(pts[j] + (wj-j)*(pts[j+1]-pts[j]));
    }
    res.swap(pts);
}

/*
 * Calcola la distanza tra due path *non* degeneri (cioè con lunghezza
 * maggiore di zero).
 *
 * In realtà calcola la distanza massima fra due punti che
 * si muovano parallelamente a velocita' costante lungo i due path.
 *
 */
double pathPathDist(const Path2d& p1, const Path2d& p2, int n=1000);

/*
 *  Calcola l'area di un poligono
 */
double area(const Path2d& p);

/*
 *  Calcola l'area di un poligono restituendone il valore negativo se
 *  i vertici sono ordinati in senso antiorario
 */
double sarea(const Path2d& poly);

/*
 *  Calcola l'area contenuta in una lista di Path2d, che rappresentano contorni
 *  di un poligono od un gruppo di poligono non semplicemente connessi.
 *
 *  IMPORTANTE: l'ipotesi che viene assunta e che l'algoritmo non controlla che
 *  che non ci siano intersezioni.
 */
double area(const std::vector<Path2d>& path_list, int num_checks=1);

/*
 * Calcola il clip di un poligono ad un semipiano specificato passando
 * un punto p del bordo del semipiano e un vettore n normale al bordo
 * (orientato verso il semipiano, non necessariamente unitario).
 */
Path2d clipPoly(const Path2d& poly, const P2d& p, const P2d& n);


void bezto(const P2d& a, const P2d& b, const P2d& c, const P2d& d,
           std::vector<P2d>& path, double err=0.1);

/*
 * Esegue uno smooth 1-2-1 in place sul path passato. Lo smooth 1-2-1
 * sostituisce ogni punto con la media pesata tra due volte il punto stesso,
 * una volta il punto precedente e una vola il punto successivo.
 */
void smooth121(Path2d &path);

/*
 * Data una lista di Path2d senza intersezioni, ritorna un vector di bool di pari
 * dimensione in cui ogni elemento e' false se il relativo path e' contenuto
 * in un numero pari di contorni, true altrimenti.
 *
 * Attenzione, l'implementazione attuale e' O(n^2)!
 *
 */
std::vector<bool> oddEven(const std::vector<Path2d>& paths);

/*
 * Usando la regola di oddEven(), fa in modo che i path odd abbiano senso
 * antiorario e quelli even senso orariorarioo
 */
void orient(std::vector<Path2d>& paths);

/*
 * Data una regione chiusa del piano delimitata dai percorsi contenuti in
 * boundary restituisce il punto piu` interno.
 */
P2d mostInternalP(const std::vector<Path2d> &boundary);

/*
 * Dato un path, restituisce un elenco di path che rappresentano il
 * tratteggio del path originale con i tratti disegnati lunghi l_pen e quelli
 * non disegnati space. Le misure passate sono solo approssimate, perche` la
 * funzione fa in modo che il tratteggio si concluda con un tratto disegnato
 * completo. Epsilon e` la max distanza per cui due punti vengono considerati
 * coincidenti.
 */
std::vector<Path2d> dash(const Path2d& path, double l_pen, double space, double epsilon = 0.01);

/*
 * Raffina un path generandone una versione in cui la distanza fra due punti
 * successivi non e' mai superiore al valore indicato; questo viene fatto
 * suddividendo eventuali segmenti troppo lunghi in parti uguali.
 */
Path2d refine(const Path2d& p, double maxstep);

/**
 * Calcola il bounding box per std::vector<Path2d>
 */
void boundingBox(const std::vector<Path2d>& regions, bool initPointsFirst,
                 P2d* bboxMin, P2d* bboxMax);
/**
 * Calcola il bounding box per std::vector<std:vector<Path2d> >
 */
void boundingBox(const std::vector<std::vector<Path2d> >& regions, bool initPointsFirst,
                 P2d* bboxMin, P2d* bboxMax);

/**
    Transforms an input geometry with a given transformation matrix.
    @param[in, out] paths   Input/output geometry.
    @param[in]      xf      Input transformation matrix.
    @return The input geometry transformed with the tMat parameter.
  */
inline std::vector<Path2d>& operator*=(std::vector<Path2d>& paths, const X2d& xf) {
    for (int i=0,n=paths.size(); i<n; i++) {
        paths[i] *= xf;
    }
    return paths;
}

/**
    Transforms an input geometry with a given transformation matrix.
    @param[in]  paths   Input geometry.
    @param[in]  xf      Input transformation.
    @return The input geometry transformed with the tMat parameter.
  */
inline std::vector<Path2d> operator*(std::vector<Path2d> paths, const X2d& xf) {
    return paths*=xf;
}

/**
 * Given a poly-polygon as a vector of (closed) path2d(s), returns a
 * single path that can be used by an odd-even filling algo for
 * the drawing.
 * @param paths Poly-polygon to draw
 * @return The Path2d to be used by the drawing algo
 */
Path2d bridgedPath(const std::vector<Path2d> &paths);

#endif
