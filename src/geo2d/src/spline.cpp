#include "sysdep.h"
#include "spline.h"
#include "geo3d.h"
#include <algorithm>

Path2d splineX(const std::vector<P2d>& pts,
               std::vector<int> *indices,
               int levels,
               bool closed,
               int iters,
               double kk)
{
    int n0 = (int)pts.size();
    assert(n0 >= 2);

    int n = closed ? n0*3 : n0;
    int nn = ((n-1) << levels) + 1;

    // curva finale (nn punti)
    std::vector<P3d> c(nn);
        P3d *pc = &c[0];

    // punti da interpolare (n punti)
    std::vector<P3d> p(n);
    for (int i=0;i<n0;i++)
    {
        p[i].x = pts[i].x;
        p[i].y = pts[i].y;
    }
    for (int i=n0;i<n;i++)
    {
        p[i].x = p[i-n0].x;
        p[i].y = p[i-n0].y;
    }
    p[0].z = 0;
    for (int i=1;i<n;i++)
        p[i].z = p[i-1].z + dist(p[i].xy(), p[i-1].xy());

    // offset di interpolazione (n punti)
    std::vector<P3d> d(n, P3d(0,0,0));

    // mappatura punti di interpolazione su curva (n punti)
    std::vector<int> ix(n);
    for (int i=0;i<n;i++)
        ix[i] = i*(1<<levels);
    --ix[n-1];

    while (iters--)
    {
        // Calcolo B-Spline
        int step = 1<<levels;
        for (int i=0; i<n; i++)
            c[i*step] = p[i] + d[i];
        while (step > 1)
        {
            int h = step >> 1;
//			asm("// HANDMADE INLINE");
#if 0
            for (int i=h; i<nn-h; i+=step)
                        {
                            double ax = pc[i+h].x;
                            double ay = pc[i+h].y;
                            double az = pc[i+h].z;
                            double bx = pc[i-h].x;
                            double by = pc[i-h].y;
                            double bz = pc[i-h].z;
                            pc[i].x = (ax + bx)*0.5;
                            pc[i].y = (ay + by)*0.5;
                            pc[i].z = (az + bz)*0.5;
                        }
                        double ox = pc->x;
                        double oy = pc->y;
                        double oz = pc->z;
            for (int i=step; i<nn-step; i+=step)
                        {
                            double px = pc[i].x;
                            double py = pc[i].y;
                            double pz = pc[i].z;
                            double psx = pc[i+step].x;
                            double psy = pc[i+step].y;
                            double psz = pc[i+step].z;
                            double nx = (ox + 6*px + psx)*0.125;
                            double ny = (oy + 6*py + psy)*0.125;
                            double nz = (oz + 6*pz + psz)*0.125;
                            ox = px;
                            oy = py;
                            oz = pz;
                            pc[i].x = nx;
                            pc[i].y = ny;
                            pc[i].z = nz;
                        }
#else
    //		asm("// NORMALE NOINLINE");
            for (int i=h; i<nn-h; i+=step)
                pc[i] = (pc[i+h] + pc[i-h])*0.5;
            P3d o = pc[0];
            for (int i=step; i<nn-step; i+=step)
            {
                P3d n = (o + 6*pc[i] + pc[i+step])*0.125;
                o = pc[i];
                pc[i] = n;
            }
#endif
            step = h;
        }

        // Aggiornamento delta
        for (int i=1;i<n-1;i++)
        {
            int s = ix[i];

            P3d besto = p[i] - project(p[i], c[s], c[s+1]);
            double bestd = len2(besto);

            while (s > ix[i-1])
            {
                P3d o = p[i] - project(p[i], c[s-1], c[s]);
                double d = len2(o);
                if (d < bestd)
                {
                    besto = o;
                    bestd = d;
                    --s;
                }
                else
                    break;
            }

            while (s < ix[i+1])
            {
                P3d o = p[i] - project(p[i], c[s+1], c[s+2]);
                double d = len2(o);
                if (d < bestd)
                {
                    besto = o;
                    bestd = d;
                    ++s;
                }
                else
                    break;
            }

            ix[i] = s;
            d[i] += besto*kk;
        }
    }

    // In quest'ultima parte dell'algoritmo forziamo la presenza di alcuni punti
    // del vettore originale (p) anche in quello risultante (res). Serve tenere
    // aggiornato un vettore di indici (res2) che memorizzi quali dei punti
    // originali sono stati inseriti.

    std::vector<P2d> res;
    std::vector<int> res2;
    res2.reserve(n);

    // Se inserisco direttamente un punto in coda a res è facile aggiornare
    // subito res2. Alcuni inserimenti però vengono fatti sul vettore intermedio
    // c (che viene successivamente riversato in massa dentro res) quindi è
    // necessario ricordarsi per quali punti bisognerà inserire un indice in
    // res2 al momento della copia in res.
    std::vector<int> tmpstore;
    std::vector<int>::const_iterator ti;

    if (closed)
    {
        // L'output è il tratto fra n0 e n0*2
        int i=n0;
        int j=ix[i]+1;
        double t = _project(p[i].xy(), c[j-1].xy(), c[j].xy());

        if (t < 0.99)
        {
            res2.push_back(0);
            res.push_back(p[i].xy());
        }
        else
        {
            tmpstore.push_back(j);
            c[j] = p[i];
        }

        for (i=n0+1;i<n0*2;i++)
        {
            while (j <= ix[i])
            {
                for(ti=tmpstore.begin();ti!=tmpstore.end();++ti)
                    if ((*ti) == j)
                        res2.push_back(res.size());

                res.push_back(c[j++].xy());
            }

            double t = _project(p[i].xy(), c[j-1].xy(), c[j].xy());
            if (t < 0.01)
            {
                res2.push_back(res.size() - 1);
                res[res.size() - 1] = p[i].xy();
            }
            else
            {
                if (t < 0.99)
                {
                    res2.push_back(res.size());
                    res.push_back(p[i].xy());
                }
                else
                {
                    tmpstore.push_back(j);
                    c[j] = p[i];
                }
            }
        }

        while (j < ix[n0*2])
        {
            for(ti=tmpstore.begin(); ti!=tmpstore.end(); ++ti)
                if ((*ti) == j)
                    res2.push_back(res.size());

            res.push_back(c[j++].xy());
        }
        res.push_back(res[0]);
    }
    else
    {
        int j=0;
        for (int i=0;i<n-1;i++)
        {
            while (j <= ix[i])
            {
                for(ti=tmpstore.begin();ti!=tmpstore.end();++ti)
                    if ((*ti) == j)
                        res2.push_back(res.size());

                res.push_back(c[j++].xy());
            }

            double t = _project(p[i].xy(), c[j-1].xy(), c[j].xy());
            if (t < 0.01)
            {
                res2.push_back(res.size() - 1);
                res[res.size() - 1] = p[i].xy();
            }
            else
            {
                if (t < 0.99)
                {
                    res2.push_back(res.size());
                    res.push_back(p[i].xy());
                }
                else
                {
                    tmpstore.push_back(j);
                    c[j] = p[i];
                }
            }
        }

        while (j < nn)
        {
            for(ti=tmpstore.begin(); ti!=tmpstore.end(); ++ti)
                if ((*ti) == j)
                    res2.push_back(res.size());

            res.push_back(c[j++].xy());
        }
        res2.push_back(res.size() - 1);
    }

    assert(int(res2.size()) == n0);
    if (indices)
        std::swap(*indices, res2);
    return res;
}

Path2d hermite(const P2d& p0, const P2d& p1, const P2d& t0, const P2d& t1, int np)
{
    std::vector<P2d> pts(np);

    double z = 1.0 / (np - 1);
    double ax = 2.0*p0.x - 2.0*p1.x + t0.x + t1.x;
    double bx = 3.0*p1.x - 3.0*p0.x - 2.0*t0.x - t1.x;
    double cx = t0.x;
    double dx = p0.x;
    double ay = 2.0*p0.y - 2.0*p1.y + t0.y + t1.y;
    double by = 3.0*p1.y - 3.0*p0.y - 2.0*t0.y - t1.y;
    double cy = t0.y;
    double dy = p0.y;

    for (int i=0;i<np;i++)
    {
        double t = i*z;
        double t2 = t*t;
        double t3 = t2*t;

        pts[i] = P2d(t3*ax + t2*bx + t*cx + dx, t3*ay + t2*by + t*cy + dy);
    }
    return pts;
}

std::vector<P2d> ispline(const Path2d& pts, double eps, unsigned maxpt)
{
    int numpts = pts.size();

    std::vector<int> r(numpts-2);
    for (int i=0;i<numpts-2;i++)
        r[i] = i+1;

    std::vector<int> kpi;
    kpi.reserve(numpts*2);
    kpi.push_back(0);
    kpi.push_back(numpts - 1);

    if (maxpt == 0)
        maxpt = numpts;

    std::vector<P2d> kpts;
    kpts.reserve(pts.size());

    while (r.size() && kpi.size() < maxpt)
    {
        int kpilen = (int)kpi.size();
        kpts.resize(kpilen);
        for (int i=0;i<kpilen;i++)
            kpts[i] = pts[kpi[i]];

        std::vector<int> idx;
        Path2d sp = splineX(kpts, &idx);

        idx.insert(idx.begin(), 0);
        idx.push_back(sp.size());

        double baddist = -1E+20;
        int badidx = 0;
        int kpidx = 0;
        int rlen = (int)r.size();
        for (int j=0;j<rlen;j++)
        {
            int k = r[j];

            // HACK: scrivendo semplicemente kpi[kpipx] si causa una
            // miscompilazione (scatta l'assert subito sotto).
            while (k >= (&kpi[0])[kpidx])
                ++kpidx;

            assert(kpidx < (int)kpi.size());
            assert(kpidx+1 < (int)idx.size());

            double d;
            sp.project(pts[k], NULL, &d, NULL, idx[kpidx-1], idx[kpidx+1]);
            if (baddist < d)
            {
                baddist = d;
                badidx = k;
            }
        }

        if (baddist > eps)
        {
            // Sia kpi che r sono array ordinati di indici, per cui possiamo
            // fare ricerche dicotomiche.
            kpi.insert(std::upper_bound(kpi.begin(), kpi.end(), badidx), badidx);
            r.erase(std::lower_bound(r.begin(), r.end(), badidx));
        }
        else
            break;
    }

    int kpilen = (int)kpi.size();
    kpts.resize(kpilen);
    for (int i=0;i<kpilen;i++)
        kpts[i] = pts[kpi[i]];
    return kpts;
}
