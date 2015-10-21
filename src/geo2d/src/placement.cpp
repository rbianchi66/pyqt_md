#include "sysdep.h"
#include "placement.h"
#include <math.h>

namespace Placement {

class Shape
{
    // Skyline superiori e inferiori di una singola sagoma piazzabile,
    // ottenute da un boundary, una risoluzione e un angolo di rotazione.
    // Quando una shape una volta assegnata l'ascissa viene piazzata
    // la sua skyline inferiore viene usata per stabilire l'appoggio
    // alla skyline corrente del foglio e quella superiore per
    // aggiornare quest'ultima.
    public:
        int x0; // traslazione X in pixel rispetto alla pura rotazione
        std::vector<double> upper;
        std::vector<double> lower;
};

double res = 1.0;     // 1 pixel / mm
int hborder = 2;      // 2 pixel (mm) di semi-distanza fra i pezzi
int angle_res = 256;  // 256 = angolo giro (angle_res DEVE essere 2^n)
int angle_step = 32;  // prove di drop del pezzo ogni 32 step angolari
int xstep = 1;        // incrementi di drop di 8 pixel

Part::Part(std::vector<Path2d> boundary)
  : boundary(boundary), shapes(angle_res)
{ }

Part::~Part()
{
    for (int i=0; i<angle_res; i++)
        delete shapes[i];
}

Shape *Part::shape(int alpha)
{
    // Normalizza angolo
    alpha = alpha & (angle_res - 1);

    // Shape gia' calcolata in precedenza ?
    if (shapes[alpha])
        return shapes[alpha];

    // Calcola coefficienti per rotazione
    double cs = cos(alpha*2*3.141592654/angle_res);
    double sn = sin(alpha*2*3.141592654/angle_res);

    // Calcola intervallo X
    int ix0=1<<30, ix1=-(1<<30);
    for (int i=0,ni=int(boundary.size()); i<ni; i++)
    {
        Path2d& p=boundary[i];
        for (int j=0,nj=p.size(); j<nj; j++)
        {
            int x = int(floor((p[j].x*cs + p[j].y*sn) * res));
            if (x < ix0) ix0 = x;
            if (x > ix1) ix1 = x;
        }
    }

    // Inizializza skyline
    int n = ix1 - ix0 + 1;
    Shape *s = new Shape;
    s->x0 = ix0;
    std::vector<double> t_upper(n);
    std::vector<double> t_lower(n);
    double *upper = &t_upper[0];
    double *lower = &t_lower[0];
    for (int i=0; i<n; i++)
    {
        upper[i] = -1E20;
        lower[i] = 1E20;
    }

    // Calcolo effettivo
    for (int i=0,ni=int(boundary.size()); i<ni; i++)
    {
        Path2d& p=boundary[i];
        for (int j=0,nj=p.size(),oj=nj-1; j<nj; oj=j++)
        {
            double x0 = (p[oj].x * cs + p[oj].y * sn)*res;
            double y0 = p[oj].y * cs - p[oj].x * sn;
            double x1 = (p[j].x * cs + p[j].y * sn)*res;
            double y1 = p[j].y * cs - p[j].x * sn;
            double xa = x0 < x1 ? x0 : x1;
            double xb = x0 + x1 - xa;
            int ix = int(floor(xa));
            double xx = ix + 0.5;
            if (xx < xa)
            {
                xx += 1.0;
                ix += 1;
            }
            if (xx < xb)
            {
                double dy = (y1 - y0) / (x1 - x0);
                double y = y0 + (xx - x0) * dy;
                while (xx < xb)
                {
                    if (lower[ix - ix0] > y) lower[ix - ix0] = y;
                    if (upper[ix - ix0] < y) upper[ix - ix0] = y;
                    ix += 1;
                    xx += 1.0;
                    y += dy;
                }
            }
        }
    }

    // Bordatura Y
    double hbmm = hborder/res;
    for (int i=0; i<n; i++)
    {
        if (upper[i] >= lower[i])
        {
            upper[i] += hbmm;
            lower[i] -= hbmm;
        }
    }

    // Trim eventuali elementi non usati
    while (upper[n-1] < lower[n-1])
        --n;
    int i0 = 0;
    while (upper[i0] < lower[i0])
            i0++;
    for (int j=i0; j<n; j++)
    {
        upper[j-i0] = upper[j];
        lower[j-i0] = lower[j];
    }
    n -= i0;

    // Copia risultato e bordatura X
    s->upper.resize(n + 2*hborder);
    s->lower.resize(n + 2*hborder);
    for (int i=0; i<n; i++)
    {
        double v0 = lower[i];
        double v1 = upper[i];
        int j0 = i-hborder; if (j0<0) j0 = 0;
        int j1 = i+hborder+1; if (j1>n) j1 = n;
        for (int j=j0; j<j1; j++)
        {
            if (v0 > lower[j]) v0 = lower[j];
            if (v1 < upper[j]) v1 = upper[j];
        }
        s->upper[i+hborder] = v1;
        s->lower[i+hborder] = v0;
    }
    for (int i=0; i<hborder; i++)
    {
        s->lower[i] = s->lower[hborder];
        s->lower[n+hborder+i] = s->lower[n+hborder-1];
        s->upper[i] = s->upper[hborder];
        s->upper[n+hborder+i] = s->upper[n+hborder-1];
    }
    s->x0 += i0 - hborder;

    // Memorizza in cache e torna il risultato
    shapes[alpha] = s;
    return s;
}

Sheet::Sheet(double w, double h)
    : w(w), h(h), iw(int(w*res)), skyline(iw)
{
}

// Lascia "cadere" la shape indicata dall'alto
// e calcola il valore di appoggio. Torna false
// se la parte superiore della shape supera
// l'altezza del foglio di piazzamento.
bool Sheet::drop(Shape *s, int x, double& d)
{
    int pw = int(s->upper.size());
    double *upper = &s->upper[0];
    double *lower = &s->lower[0];
    double *y0 = &skyline[x];
    d = -1E20;
    for (int i=0; i<pw; i++)
    {
        double v = y0[i] - lower[i];
        if (d < v) d = v;
    }
    for (int i=0; i<pw; i++)
    {
        double y = upper[i] + d;
        if (y > h)
            return false;
    }
    return true;
}

//
// Calcolo bonta' dell'incastro; considera sia la Y media
// del pezzo che quanto il profilo inferiore "combacia"
// con la skyline del piazzato
//
// Nota:
//    non ricordo come sono arrivato a questa formula che
//    ora mi appare come totalmente priva di logica :-(
//
// Nota:
//    e' comunque molto facile inventare formule logiche
//    che pero' fanno nesting orrendi ... quindi copio
//
// Nota:
//    cosi' a occhio c'e' un po' di roba che si potrebbe
//    calcolare una-tantum per shape (h/tw sembra la Y
//    del baricentro della shape piazzata, si potrebbe
//    ottenere con ym + d precalcolando ym)
//
double Sheet::weight(Shape *s, int x, double d)
{
    int pw = int(s->upper.size());
    double *upper = &s->upper[0];
    double *lower = &s->lower[0];
    double *y0 = &skyline[x];

    double tw = 0.0;
    double h = 0.0;
    double p = d * pw;
    double vmax = 100 /* MAGIC ? */ - d;
    for (int i=0; i<pw; i++)
    {
        double yy0 = y0[i];
        double yy1 = upper[i] + d;
        double dy = yy1 - yy0;
        double my = (yy0 + yy1) * 0.5;
        h += dy*my;
        tw += dy;
        double v = lower[i] - y0[i];
        if (v > vmax) v = vmax;
        p += v;
    }
    return h/tw + p/pw;
}

void Sheet::update(Shape *s, int x, double d)
{
    int pw = int(s->upper.size());
    double *upper = &s->upper[0];
    double *y0 = &skyline[x];
    for (int i=0; i<pw; i++)
    {
        double y = upper[i] + d;
        if (y0[i] < y) y0[i] = y;
    }
}

bool Sheet::addPart(Part *p, X2d& xf)
{
    // Best result
    int bx = -1;
    int ba = 0;
    double bw = 0;
    double bd = 0;

    // Bitmap di check posizioni gia' incontrate
    int bsz = (iw * angle_res + 7) >> 3;
    std::vector<unsigned char> bitmap(bsz);

    for (int x=0; x<iw; x+=xstep)
    {
        for (int a=0; a<angle_res; a+=angle_step)
        {
            // xx/aa = posizione di drop
            int aa = a;
            Shape *s = p->shape(aa);
            int pw = int(s->upper.size());
            if (pw > iw)
                // FIXME:
                //   Per essere corretti bisognerebbe cercare
                //   se esiste almeno un angolo accettabile invece
                //   di rinunciare se non esistono angoli *di drop*
                //   accettabili.
                continue;
            int xx = x - pw / 2;
            if (xx < 0) xx = 0;
            if (xx + pw > iw) xx = iw - pw;

            double dd = 0;
            if (!drop(s, xx, dd))
                // FIXME:
                //   Idem
                continue;
            double ww = weight(s, xx, dd);

            // Loop di discesa
            bool bottom = false;
            const double EPS = 0.0001;
            while (!bottom)
            {
                // Controlla se la posizione e' gia' stata visitata
                int bpos = xx * angle_res + (aa & (angle_res - 1));
                int ibpos = bpos >> 3;
                int bmask = 1 << (ibpos & 7);
                if (bitmap[ibpos] & bmask)
                    break;
                bitmap[ibpos] |= bmask;

                bottom = true;
                // Prova a ruotare da un lato
                {
                    int a1 = aa-1;
                    Shape *s1 = p->shape(a1);
                    int pw1 = int(s1->upper.size());
                    if (pw1 < iw)
                    {
                        int x1 = xx+pw/2-pw1/2;
                        if (x1 < 0) x1 = 0;
                        if (x1 + pw1 > iw) x1 = iw - pw1;
                        double d1;
                        if (drop(s1, x1, d1))
                        {
                            double w1 = weight(s1, x1, d1);
                            if (w1 < ww - EPS)
                            {
                                bottom = false;
                                xx = x1;
                                aa = a1;
                                ww = w1;
                                dd = d1;
                                pw = pw1;
                                s = s1;
                            }
                        }
                    }
                }

                // Prova a ruotare dall'altro
                if (bottom)
                {
                    int a1 = aa + 1;
                    Shape *s1 = p->shape(a1);
                    int pw1 = int(s1->upper.size());
                    if (pw1 < iw)
                    {
                        int x1 = xx+pw/2-pw1/2;
                        if (x1 < 0) x1 = 0;
                        if (x1 + pw1 > iw) x1 = iw - pw1;
                        double d1;
                        if (drop(s1, x1, d1))
                        {
                            double w1 = weight(s1, x1, d1);
                            if (w1 < ww - EPS)
                            {
                                bottom = false;
                                xx = x1;
                                aa = a1;
                                ww = w1;
                                dd = d1;
                                pw = pw1;
                                s = s1;
                            }
                        }
                    }
                }

                // Prova a traslare a sinistra
                if (xx > 0)
                {
                    int x1 = xx-1;
                    double d1;
                    if (drop(s, x1, d1))
                    {
                        double w1 = weight(s, x1, d1);
                        if (w1 < ww - EPS)
                        {
                            bottom = false;
                            xx = x1;
                            ww = w1;
                            dd = d1;
                        }
                    }
                }
                // Prova a traslare a destra
                if (xx+pw < iw)
                {
                    int x1 = xx+1;
                    double d1;
                    if (drop(s, x1, d1))
                    {
                        double w1 = weight(s, x1, d1);
                        if (w1 < ww - EPS)
                        {
                            bottom = false;
                            xx = x1;
                            ww = w1;
                            dd = d1;
                        }
                    }
                }
            }

            // Trovato il fondo; guarda se e' meglio del best-so-far
            if (bx == -1 || ww < bw)
            {
                bx = xx;
                ba = aa;
                bw = ww;
                bd = dd;
            }
        }
    }

    if (bx == -1)
        // Nessuna soluzione accettabile e' stata trovata
        return false;

    // Aggiorna skyline con il nuovo piazzato
    Shape *s = p->shape(ba);
    update(s, bx, bd);

    // Calcola la trasformazione che e' stata usata
    double cs = cos(ba*2*3.141592654/angle_res);
    double sn = sin(ba*2*3.141592654/angle_res);
    xf.a = P2d(cs, -sn);
    xf.b = P2d(sn, cs);
    xf.c = P2d((bx - s->x0)/res, bd);

    return true;
}

void Sheet::setHborder(int hb)
{
    hborder = hb;
}

}
