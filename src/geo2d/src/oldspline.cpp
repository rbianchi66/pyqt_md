#include "oldspline.h"

#include "geo3d.h"
#include <algorithm>

/*
 *****************  Vecchia spline usata in Calcad  *****************
*/

static void cbezier_rec(int cx[4], int cy[4], int *x, int *y, int *n);

static int myround(double x)
{
    return (int)(floor(x + 0.5));
}

static double myhypot(double x, double y)
{
    return sqrt(x*x + y*y);
}

static int i_distan(int x, int y, int xx, int yy)
{
    return myround(myhypot((double)(x - xx), (double)(y - yy)));
}

//double PI = 3.14159265358;  // Pigreco

static int max_o_pt = 10000;

static int err = 1;

static double wcos = 1.0;
static double wsin = 0.0;
static double adx = 0.0;
static double ady = 0.0;
static double mat1[4][4];
static double mat2[4][4];
static double mat3[3][3];

static void determ3(double *d)
{
    /*
    Calcolo determinante della matrice 3x3 mat3 (primo indice righe)
    */
    double a, b, c;

    a= mat3[0][0] * mat3[1][1] * mat3[2][2] - mat3[0][0] * mat3[1][2] * mat3[2][1];
    b= mat3[1][0] * mat3[2][1] * mat3[0][2] - mat3[1][0] * mat3[2][2] * mat3[0][1];
    c= mat3[2][0] * mat3[0][1] * mat3[1][2] - mat3[2][0] * mat3[0][2] * mat3[1][1];
    d[0] = a + b + c;
}

static void determ4(double *d)
{
    /*
    Calcolo determinante della matrice 4x4 mat2 (primo indice righe)
    */
    int i, j, k, l;
    double a[4];

    for (k = 0; k < 4; ++k)
    {
        for (j = 1; j < 4; ++j)
        {
            l = 0;
            for (i = 0; i < 4; ++i)
            {
                if (i == k)
                    continue;
                mat3[l][j-1] = mat2[i][j];
                l = l + 1;
            }
        }
        determ3(&a[k]);
    }

    d[0] = mat2[0][0]*a[0] - mat2[1][0]*a[1] + mat2[2][0]*a[2] - mat2[3][0]*a[3];
}

static void affine(int x0, int y0, int x1, int y1, int x0n, int y0n, int x1n, int y1n, int *ix, int *iy)
{
    /*
    Calcolo coefficienti della trasformazione affine che porta i punti
    x0,y0 e x1,y1 nei nuovi punti x0n,y0n e x1n,y1n ed esegue la trasformazione
    sulle coordinate intere ix, iy.
    */
    int i, j;
    double delta, a;
    double xx, yy, x, y;

    // soluzione eq. lineare 4eq. 4inc., con i determinanti!
    // primo indice orizzontale

    mat1[0][0] = (double)(x0);
    mat1[0][1] = (double)(y0);
    mat1[1][0] = (double)(-y0);
    mat1[1][1] = (double)(x0);
    mat1[2][0] = 1.0;
    mat1[2][1] = 0.0;
    mat1[3][0] = 0.0;
    mat1[3][1] = 1.0;

    mat1[0][2] = (double)(x1);
    mat1[0][3] = (double)(y1);
    mat1[1][2] = (double)(-y1);
    mat1[1][3] = (double)(x1);
    mat1[2][2] = 1.0;
    mat1[2][3] = 0.0;
    mat1[3][2] = 0.0;
    mat1[3][3] = 1.0;

    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            mat2[i][j] = mat1[i][j];

    determ4(&delta);
    if (fabs(delta) < 0.00001) // determinante virtualmente nullo
    {
        wcos = 1.0;
        wsin = 0.0;
        adx = 0.0;
        ady = 0.0;
        return;
    }

    mat2[0][0] = (double)(x0n);
    mat2[0][1] = (double)(y0n);
    mat2[0][2] = (double)(x1n);
    mat2[0][3] = (double)(y1n);
    determ4(&a);
    wcos = a / delta;                   // WCOS

    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            mat2[i][j] = mat1[i][j];

    mat2[1][0] = (double)(x0n);
    mat2[1][1] = (double)(y0n);
    mat2[1][2] = (double)(x1n);
    mat2[1][3] = (double)(y1n);
    determ4(&a);
    wsin = a / delta;                   // WSIN

    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
           mat2[i][j] = mat1[i][j];

    mat2[2][0] = (double)(x0n);
    mat2[2][1] = (double)(y0n);
    mat2[2][2] = (double)(x1n);
    mat2[2][3] = (double)(y1n);
    determ4(&a);
    adx = a / delta;                    // ADX

    for (i = 0; i < 4; ++i)
        for (j = 0; j < 4; ++j)
            mat2[i][j] = mat1[i][j];

    mat2[3][0] = (double)(x0n);
    mat2[3][1] = (double)(y0n);
    mat2[3][2] = (double)(x1n);
    mat2[3][3] = (double)(y1n);
    determ4(&a);
    ady = a / delta;                    // ADY

    // Eseguo la trasformazione affine (vale per un singolo punto INTERO).
    x = ix[0];
    y = iy[0];
    xx = x*wcos - y*wsin + adx;
    yy = x*wsin + y*wcos + ady;
    ix[0] = myround(xx);
    iy[0] = myround(yy);
}

static void find_angle(int xs, int ys, int xc, int yc, int x, int y, double *angle)
{
    /*
    Torna l'angolo fra i segmenti (xs,ys - xc,yc) e (xc,yc - x,y) in angle,
    l'ordine dei segmenti conta. Il valore di ritorno e` fra -PI e PI.
    Se sono allineati torna 0, senno' si suppone di girare il primo fino
    a farlo coincidere con l'asse x (xc,yc punta ai valori positivi), si
    ruota il secondo in modo uguale e si mette con xc,yc in 0, torna
    l'angolo antiorario fra l'asse x e il secondo vettore.
    */
    double c, a, alfa1, alfa2, alfa;
    int deltax, deltay;

    deltay = yc - ys;
    deltax = xc - xs;
    c = (double)(deltay);
    a = (double)(deltax);
    angle[0] = 0.0;
    if (deltay == 0 && deltax == 0)
        return;

    alfa1 = atan2(c, a); // angolo orientato 1 segm. fra -PI e PI

    deltay = y - yc;
    deltax = x - xc;
    c = (double)(deltay);
    a = (double)(deltax);
    if (deltay == 0 && deltax == 0)
        return;

    alfa2 = atan2(c, a); // angolo orientato 2 segm. fra -PI e PI

    alfa = alfa2 - alfa1;
    if (alfa < -PI)
        alfa = alfa + 2.0*PI;
    if (alfa > PI)
        alfa = alfa - 2.0*PI;
    angle[0] = alfa;
}

static void get_aa(int *x, int *y, int *a1, int *a2)
{
    /*
    Dati i quattro punti di un tratto (0 e 3 estremi, 1 e 2 punti di
    tangente), torna gli angoli fra i due punti tangenti e la
    congiungente. In 0 antiorario, in 3 orario. Angoli in gradi
    per 1000 (fra - 180000 e + 180000)
    */
    double z;

    if (x[0] == x[3] && y[0] == y[3])
    {
        a1[0] = 0;
        a2[0] = 0;
        return;
    }

    if (x[0] == x[1] && y[0] == y[1])
        a1[0] = 0;
    else
    {
        find_angle(x[3], y[3], x[0], y[0], x[1], y[1], &z);
        if (z > 0.0)
            z = z - PI;
        else
            z = PI + z;
        a1[0] = myround(z*180000.0 / PI);
    }

    if (x[2] == x[3] && y[2] == y[3])
        a2[0] = 0;
    else
    {
        find_angle(x[0], y[0], x[3], y[3], x[2], y[2], &z);
        if (z > 0.0)
            z =  PI - z;
        else
            z = -PI - z;
        a2[0] = myround(z*180000.0 / PI);
    }
}

static void set_a1(int *x, int *y, int a)
{
    /*
    Modifica di a (gradi * 1000) l'angolo del punto 1 (vedi sopra)
    */
    int xx, yy, l;
    double z;

    if ((x[0] == x[3] && y[0] == y[3]) || (x[0] == x[1] && y[0] == y[1]))
        return;
    l = i_distan(x[0], y[0], x[1], y[1]);
    z = (double)(a);
    z = z * PI / 180000.0;
    xx = myround(l * cos(z));
    yy = myround(l * sin(z));
    affine(0, 0, l, 0, x[0], y[0], x[1], y[1], &xx, &yy);
    x[1] = xx;
    y[1] = yy;
}

static void set_a2(int *x, int *y, int a)
{
    /*
    Modifica di a (gradi * 1000) l'angolo del punto 2 (vedi sopra)
    */
    int xx, yy, l;
    double z;

    if ((x[0] == x[3] && y[0] == y[3]) || (x[2] == x[3] && y[2] == y[3]))
        return;
    l = i_distan(x[2], y[2], x[3], y[3]);
    z = - (double)(a);
    z = z * PI / 180000.0;
    xx = myround(l * cos(z));
    yy = myround(l * sin(z));
    affine(0, 0, l, 0, x[3], y[3], x[2], y[2], &xx, &yy);
    x[2] = xx;
    y[2] = yy;
}

static void noa_oscill(int *x, int *y, int n)
{
    /*
    Aggiustamento tangenti: per ogni tratto si cerca di minimizzare
    i cambi di curvatura rendendo simmetriche le tangenti.
    Per quanto possibile rende quindi i tratti fra i punti a curvatura
    costante

    Ingresso come prodotto da all_bz_tg. Uscita come ingresso ed in place.
    n numero valori. Tratti m, punti n, relazione n = m * 4!
    */
    int i, a1, a2, aa1, aa2, va, vaa, v;

    i = 0;
    while (i < (n-4))
    {
        get_aa(&x[i], &y[i], &a1, &a2);
        if (abs(a1 - a2) < 500)
        {
            i = i + 4;
            continue;
        }
        get_aa(&x[i+4], &y[i+4], &aa1, &aa2);
        va = a2 - a1;
        vaa = aa1 - aa2;
        if (va > 0 && vaa >= 0)
        {
            i = i + 4;
            continue;
        }
        if (va < 0 && vaa <= 0)
        {
            i = i + 4;
            continue;
        }
        if (va > 0)
        {
            if (va > -vaa)
                v = -vaa;
            else
                v = va;
        }
        else
        {
            if (va > -vaa)
                v = va;
            else
                v = -vaa;
        }

        set_a2(&x[i], &y[i], -v);
        set_a1(&x[i+4], &y[i+4], v);
        i = i + 4;
    }
}

static void aA_tg_par(int xi[3], int yi[3], int q, int xt[4], int yt[4])
{
    /*
    Entrano 3 punti della linea (iniziali, finali o con spigolo).
    Entra pure q: 0 se si vuole il primo tratto, 1 il secondo.
    Tornano i quattro punti di controllo del tratto voluto.
    */
    double x0, x1, y1, x2, y2, x3, y3, xc, yc, l1, l2;
    double cosfi, sinfi, x[3], y[3];
    double A, B, d, t;
    double tx0, tx1, ty0, ty1, XX, YY, dx;
    double tX2, tX1, tY2, tY1, xX, yY, Dx;

    // Valori di uscita per casi particolari (3 punti collineari)
    if (q == 0)
    {
        xt[0] = xi[0];
        yt[0] = yi[0];
        xt[3] = xi[1];
        yt[3] = yi[1];
        xt[1] = xi[0];
        yt[1] = yi[0];
        xt[2] = xi[1];
        yt[2] = yi[1];
    }
    else
    {
        xt[0] = xi[1];
        yt[0] = yi[1];
        xt[3] = xi[2];
        yt[3] = yi[2];
        xt[1] = xi[1];
        yt[1] = yi[1];
        xt[2] = xi[2];
        yt[2] = yi[2];
    }

    // Calcolo di fi, angolo di rotazione (come seno e coseno!)
    x1 = (double)(xi[0]);
    x2 = (double)(xi[1]);
    x3 = (double)(xi[2]);
    y1 = (double)(yi[0]);
    y2 = (double)(yi[1]);
    y3 = (double)(yi[2]);
    l1 = myhypot(x2-x1, y2-y1);
    l2 = myhypot(x3-x2, y3-y2);
    xc = (x3-x2) * l1 / l2 + x2;
    yc = (y3-y2) * l1 / l2 + y2;
    xc = (xc + x1) / 2.0;
    yc = (yc + y1) / 2.0;

    l1 = myhypot(x2-xc, y2-yc);
    if (l1 == 0.0) // se i punti sono allineati...
        return;

    cosfi = (y2 - yc) / l1;
    sinfi = (xc - x2) / l1;

    // Punti raddrizzati, la bisettrice dell'angolo compreso in verticale.
    x[0] = (cosfi * (x1-xc)) + (sinfi * (y1-yc)) + xc;
    x[1] = (cosfi * (x2-xc)) + (sinfi * (y2-yc)) + xc;
    x[2] = (cosfi * (x3-xc)) + (sinfi * (y3-yc)) + xc;
    y[0] = (-sinfi * (x1-xc)) + (cosfi * (y1-yc)) + yc;
    y[1] = (-sinfi * (x2-xc)) + (cosfi * (y2-yc)) + yc;
    y[2] = (-sinfi * (x3-xc)) + (cosfi * (y3-yc)) + yc;

    // Calcolo coefficienti parabola raddrizzata y = Ax2 + Bx + C
    // mediante risoluzione sistema con i tre punti raddrizzati
    x0 = x[0]*x[0];
    x1 = x[1]*x[1];
    x2 = x[2]*x[2];

    d = x0*x[1] - x0*x[2] + x1*x[2] - x1*x[0] + x2*x[0] - x2*x[1];

    if (fabs(d) < 0.0001) // determinante nullo
        return;

    t = y[0]*x[1] - y[0]*x[2] + y[1]*x[2] - y[1]*x[0] + y[2]*x[0] - y[2]*x[1];
    A = t / d;

    t = x0*y[1] - x0*y[2] + x1*y[2] - x1*y[0] + x2*y[0] - x2*y[1];
    B = t / d;

    // Calcolo tangenti per spline di Bezier
    if (q == 0)      // Prima meta`
    {
        dx = x[1] - x[0];
        tx0 = tx1 = dx;
        ty0 = 2.0*A*dx*x[0] + B*dx;
        ty1 = -2.0*y[0] + 2.0*y[1] - 2.0*A*dx*x[0] - B*dx;

        tx0 = tx0 / 3.0;
        tx1 = tx1 / - 3.0;
        ty0 = ty0 / 3.0;
        ty1 = ty1 / - 3.0;
        tx0 = tx0 + x[0];
        ty0 = ty0 + y[0];
        tx1 = tx1 + x[1];
        ty1 = ty1 + y[1];

        XX = (cosfi * (tx0-xc)) + (-sinfi * (ty0-yc)) + xc;
        YY = (sinfi * (tx0-xc)) + ( cosfi * (ty0-yc)) + yc;
        tx0 = XX;
        ty0 = YY;
        XX = (cosfi * (tx1-xc)) + (-sinfi * (ty1-yc)) + xc;
        YY = (sinfi * (tx1-xc)) + ( cosfi * (ty1-yc)) + yc;
        tx1 = XX;
        ty1 = YY;

        xt[1] =  (int)(tx0);
        yt[1] =  (int)(ty0);
        xt[2] =  (int)(tx1);
        yt[2] =  (int)(ty1);
    }
    else       // Seconda parte
    {
        Dx = x[2] - x[1];
        tX1 = tX2 = Dx;
        tY1 = 2.0*A*Dx*x[1] + B*Dx;
        tY2 = -2.0*y[1] + 2.0*y[2] - 2.0*A*Dx*x[1] - B*Dx;

        tX1 = tX1 / 3.0;
        tX2 = tX2 / - 3.0;
        tY1 = tY1 / 3.0;
        tY2 = tY2 / - 3.0;
        tX1 = tX1 + x[1];
        tY1 = tY1 + y[1];
        tX2 = tX2 + x[2];
        tY2 = tY2 + y[2];

        xX = (cosfi * (tX1-xc)) + (-sinfi * (tY1-yc)) + xc;
        yY = (sinfi * (tX1-xc)) + ( cosfi * (tY1-yc)) + yc;
        tX1 = xX;
        tY1 = yY;
        xX = (cosfi * (tX2-xc)) + (-sinfi * (tY2-yc)) + xc;
        yY = (sinfi * (tX2-xc)) + ( cosfi * (tY2-yc)) + yc;
        tX2 = xX;
        tY2 = yY;

        xt[1] =  (int)(tX1);
        yt[1] =  (int)(tY1);
        xt[2] =  (int)(tX2);
        yt[2] =  (int)(tY2);
    }
}

static void aA_tg(int xi[4], int yi[4], int xo[4], int yo[4])
{
    /*
    Dati 4 punti della linea torna i quattro punti di controllo del
    tratto intermedio (Bezier). Vale anche per primi due od ultimi
    due uguali (spigoli od estremi), nel qual caso chiama aA_tg_par.
    Per intermedi uguali genera 4 punti identici!
    Calcolo "alla ANGELO" rivisto per spigoli ed estremi.
    */
    double x0, y0, x1, y1, d32, d21, d43;
    double t0x, t0y, t1x, t1y, a, z;

    xo[0] = xo[1] = xi[1];
    xo[3] = xo[2] = xi[2];
    yo[0] = yo[1] = yi[1];
    yo[3] = yo[2] = yi[2];

    if (xi[1] == xi[2] && yi[1] == yi[2])
        return;
    if (xi[0] == xi[1] && yi[0] == yi[1] && xi[2] == xi[3] && yi[2] == yi[3])
        return;

    x1 = (double)(xi[2] - xi[1]);
    y1 = (double)(yi[2] - yi[1]);
    d32 = myhypot(x1, y1);

    if (xi[0] != xi[1] || yi[0] != yi[1])
    {
        x0 = (double)(xi[1] - xi[0]);
        y0 = (double)(yi[1] - yi[0]);
        d21 = myhypot(x0, y0);
        x1 = (double)(xi[2] - xi[0]);
        y1 = (double)(yi[2] - yi[0]);
        a = d21 / (d32 + d21);
        z = ( d32 - d21 ) / d21;
        t0x = z * x0 + a * x1;
        t0y = z * y0 + a * y1;
        xo[1] = xi[1] + myround(t0x / 3.0);
        yo[1] = yi[1] + myround(t0y / 3.0);
    }
    else
    {
        aA_tg_par(&xi[1], &yi[1], 0, xo, yo);
        return;
    }

    if (xi[2] != xi[3] || yi[2] != yi[3])
    {
        x0 = (double)(xi[3] - xi[2]);
        y0 = (double)(yi[3] - yi[2]);
        d43 = myhypot(x0, y0);
        x1 = (double)(xi[3] - xi[1]);
        y1 = (double)(yi[3] - yi[1]);
        a = ( d43 - d32 ) / d43;
        z = ( d32 * d32 ) / ( d43 * (d43 + d32) );
        x0 = (double)(xi[2] - xi[1]);
        y0 = (double)(yi[2] - yi[1]);
        t1x = a * x0 + z * x1;
        t1y = a * y0 + z * y1;
        xo[2] = xi[2] - myround(t1x / 3.0);
        yo[2] = yi[2] - myround(t1y / 3.0);
    }
    else
    {
        aA_tg_par(xi, yi, 1, xo, yo);
        return;
    }
}

static void all_aA_tg(int *x, int *y, int n, int **xo, int **yo, int *no)
{
    /*
    Data una sequenza di punti-guida per le splines, genera in uscita
    i punti per bezier, aggiungendo due punti per ciascun punto in ingresso,
    tranne che per i terminali (ne aggiunge uno solo). Per n punti in
    ingresso genera quindi n-1 tratti, per ciascun tratto 4 punti dupli-
    cando anche quelli di ingresso. Ogni 4 punti il primo e l'ultimo
    sono quelli di ingresso, gli altri le tangenti. Alloca i vettori
    di uscita (che vanno disallocati). Per errori torna no = 0.
    Almeno 3 punti di ingresso !
    Se due punti coincidono le tangenti non sono significative e
    vengono messe a zero (4 punti a zero !).
    */
    int i, j, nn, *xx, *yy, xv[4], yv[4];

    nn = (n-1) * 4;
    no[0] = 0;
    if (n <= 2)
        return;

    xx = (int *)malloc(nn*sizeof(int));
    yy = (int *)malloc(nn*sizeof(int));

    // Primo tratto
    if (x[0] == x[n-1] && y[0] == y[n-1]) // closed
    {
        xv[0] = x[n-2];
        yv[0] = y[n-2];
    }
    else
    {
        xv[0] = x[0];
        yv[0] = y[0];
    }

    for (j = 1; j < 4; ++j)
    {
        xv[j] = x[j-1];
        yv[j] = y[j-1];
    }

    aA_tg(xv, yv, xx, yy);

    // Tratti intermedi
    for (i = 0; i < (n-3); ++i)
    {
        for (j = 0; j < 4; ++j)
        {
            xv[j] = x[i+j];
            yv[j] = y[i+j];
        }
        aA_tg(xv, yv, &xx[4+i*4], &yy[4+i*4]);
    }

    // Ultimo tratto
    for (j = 0; j < 3; ++j)
    {
        xv[j] = x[n-3+j];
        yv[j] = y[n-3+j];
    }

    xv[3] = xv[2];
    yv[3] = yv[2];
    if (x[0] == x[n-1] && y[0] == y[n-1]) // closed
    {
        xv[3] = x[1];
        yv[3] = y[1];
    }

    aA_tg(xv, yv, &xx[(n-2)*4], &yy[(n-2)*4]);

    xo[0] = xx;
    yo[0] = yy;
    no[0] = nn;

    // Correzioni delle tangenti
    noa_oscill(xx, yy, nn);
}

Path2d oldSplineX(const std::vector<P2d>& pts,
                  std::vector<int> *indices)
{
    int npt = (int)pts.size();
    assert(npt >= 2);

    int i, j, n, nt;
    int *xt = NULL, *yt = NULL, *xi, *yi, *xo, *yo;

    if (npt == 2)
    {
        Path2d res;
        std::vector<int> res2;

        res.push_back(pts[0]);
        res2.push_back(0);

        res.push_back(pts[1]);
        res2.push_back(1);

        if (indices)
            std::swap(*indices, res2);
        return res;
    }

    xi = (int *)malloc(npt*sizeof(int));
    yi = (int *)malloc(npt*sizeof(int));
    xo = (int *)malloc(max_o_pt*sizeof(int));
    yo = (int *)malloc(max_o_pt*sizeof(int));

    for (i = 0; i < npt; ++i)
    {
        xi[i] = myround(pts[i].x*100); // metto in 1/100 mm (intero)
        yi[i] = myround(pts[i].y*100); // metto in 1/100 mm (intero)
    }

    all_aA_tg(xi, yi, npt, &xt, &yt, &nt);

    std::vector<int> res2;
    n = 0;
    for (i = 0; i < npt-1; ++i)
    {
        j = i * 4;
        res2.push_back(n);
        cbezier_rec(&xt[j], &yt[j], xo, yo, &n);
    }
    res2.push_back(n);
    xo[n] = xi[npt-1];
    yo[n] = yi[npt-1];
    n = n + 1;

    Path2d res;
    for (j=0; j < n; ++j)
        res.push_back(P2d((double)(xo[j]/100.0), (double)(yo[j]/100.0)));

    free(xt);
    free(yt);
    free(xi);
    free(yi);
    free(xo);
    free(yo);

    if (indices)
        std::swap(*indices, res2);

    return res;
}

static void subdiv_bezier(double xi[4], double yi[4], double xl[4], double yl[4], double xr[4], double yr[4])
{
    /*
    Dato un tratto di bezier in xi,yi, torna due tratti
    (destro e sinistro) suddividendo a meta`.
    */
    double h;

    xl[1] = (xi[0] + xi[1]) / 2.0;
    h     = (xi[1] + xi[2]) / 2.0;
    xl[2] = (xl[1] +  h   ) / 2.0;
    xr[2] = (xi[2] + xi[3]) / 2.0;
    xr[1] = (h     + xr[2]) / 2.0;
    xl[3] = xr[0] = (xl[2] + xr[1]) / 2.0;
    xl[0] = xi[0];
    xr[3] = xi[3];

    yl[1] = (yi[0] + yi[1]) / 2.0;
    h     = (yi[1] + yi[2]) / 2.0;
    yl[2] = (yl[1] +  h   ) / 2.0;
    yr[2] = (yi[2] + yi[3]) / 2.0;
    yr[1] = (h     + yr[2]) / 2.0;
    yl[3] = yr[0] = (yl[2] + yr[1]) / 2.0;
    yl[0] = yi[0];
    yr[3] = yi[3];
}

static double ld_p_s(double x0, double y0, double x1, double y1, double xa, double ya)
{
    /*
    Torna la distanza fra xa,ya ed il SEGMENTO individuato da x0,y0 e x1,y1.
    */
    double a, b, c, d, t, ds, xc, yc;

    if (myhypot(y1-y0, x1-x0) < 0.6)
        return myhypot(xa-x0, ya-y0);

    if (y1 == y0)
    {
        ds = fabs(ya - y0);
        yc = y0;
        xc = xa;
    }
    else
    {
        if (x1 == x0)
        {
            ds = fabs(xa - x0);
            xc = x0;
            yc = ya;
        }
        else
        {
            t = (x1-x0) / (y1-y0);
            a = 1.0;
            b = -t;
            c = -x0 + y0*t;     // retta fra 0 e 1
            d = xa + b*ya + c;
            t = a + b*b;        // a*a + b*b
            xc = xa - d / t;
            yc = ya - b*d / t;  // coord. intersezione
            if (d < 0.0)
                d = -d;
            t = sqrt(t);
            ds = d / t;
        }
    }

    // ds = distanza fra punto e retta, cioe` fra xa,ya e xc,yc;
    // da stabilire se l'intersezione e` nel segmento

    if (fabs(x1-x0) > fabs(y1-y0))
        t = (xc-x0)/(x1-x0);
    else
        t = (yc-y0)/(y1-y0);

    if (t < 0.0 || t > 1.0)
    {
        d  = myhypot(xa-x0, ya-y0);
        ds = myhypot(xa-x1, ya-y1);
        if (d < ds)
            ds = d;
    }

    return ds;
}

static int straight_bezier(double xi[4], double yi[4], double e)
{
    /*
    Torna SI se il tratto di bezier e` rettilineo entro e.
    */
    double d;

    if (fabs(xi[0] - xi[3]) < e && fabs(yi[0] - yi[3]) < e)
        return 1;

    d = ld_p_s(xi[0], yi[0], xi[3], yi[3], xi[1], yi[1]);
    if (d > e)
        return 0;
    d = ld_p_s(xi[0], yi[0], xi[3], yi[3], xi[2], yi[2]);
    if (d > e)
        return 0;

    return 1;
}

static void cb_rec(double cx[4], double cy[4], double e, int *x, int *y, int *n)
{
    double xr[4], yr[4], xl[4], yl[4];

    if (straight_bezier(cx, cy, e) == 1)
    {
        if (n[0] < max_o_pt)
        {
            x[n[0]] = myround(cx[0]);
            y[n[0]] = myround(cy[0]);
            n[0] = n[0] + 1;
        }
    }
    else
    {
        subdiv_bezier(cx, cy, xl, yl, xr, yr);
        cb_rec(xl, yl, e, x, y, n);
        cb_rec(xr, yr, e, x, y, n);
    }
}

static void cbezier_rec(int cx[4], int cy[4], int *x, int *y, int *n)
{
    /*
    Funzione ricorsiva per il calcolo delle spline (usando quella SOPRA!).
    Entra il tratto da calcolare, l'errore e il vettore da riempire.
    */
    int i;
    double xx[4], yy[4], e;

    for (i = 0; i < 4; ++i)
    {
        xx[i] = cx[i];
        yy[i] = cy[i];
    }

    e = (double)err;

    cb_rec(xx, yy, e, x, y, n);
}
