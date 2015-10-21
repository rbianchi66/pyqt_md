#include "path_matching.h"

#include <assert.h>
#include <algorithm>
#include <math.h>
#include <ctime>

#define EPS 1e-7 // Soglia di troncamento a 0
#define EQUAL(X, Y) (fabs(X-Y) < EPS)

inline bool SAME_POINT(const P2d& P1, const P2d& P2)
{
    return (fabs(P1.x-P2.x) < 0.01 && fabs(P1.y-P2.y) < 0.01);
}


inline double getScore(const match_result& m)
{
    return (m.D*m.D)/(1+m.sse);
}

struct IndexCompare
{
    IndexCompare() {}
    inline bool operator()(const match_result& x, const match_result& y) const
    {
        return (x.id < y.id);
    }
};

struct SseCompare
{
    SseCompare() {}
    inline bool operator()(const match_result& x, const match_result& y) const
    {
        return !(x.sse < y.sse);
    }
};

struct ScoreCompare
{
    ScoreCompare() {}
    inline bool operator()(const match_result& x, const match_result& y) const
    {
        return getScore(x) < getScore(y);
    }
};

inline double lin_interp(const double& v1, const double& t1, const double& v2, const double& t2, const double& t)
{
    return v1 + ( (t-t1) * (v2-v1)/(t2-t1) );
}

inline double combine_radius(const double& w1, const double& w2)
{
    return 2.0 / (1.0 + w1*w2);
}

std::vector<double> fast_weights(const std::vector<Path2d>& vecpath, const std::vector< std::vector<double> >& vecweights, const std::vector<double>& points_t, const std::vector<int>& corr)
{
    int n = points_t.size();
#ifndef NDEBUG
    int sz = vecpath.size();
#endif
    assert(n > 0);
    assert(sz > 0);
    assert(vecpath.size() == vecweights.size());
    
    std::vector<double> dts(n, 0.0);
    double v1, v2, t1, t2;
    int segment, segment0, segment1;
    for (int i = 0; i < n; ++i)
    {
        const Path2d& path = vecpath[corr[i]];
        const std::vector<double>& weights = vecweights[corr[i]];
        const double& t = points_t[i];
        
        assert( (t > path.len(0) || EQUAL(t, path.len(0)) ) && (t < path.len(-1) || EQUAL(t, path.len())) );
        
        segment0 = 0;
        segment1 = path.size()-1;
        while(segment1 - segment0 > 1) // Ricerca binaria
        {
            segment = (segment0+segment1)/2;
            if (path.len(segment) > t)
                segment1 = segment;
            else if (path.len(segment) < t)
                segment0 = segment;
            else
            {
                segment0 = segment;
                segment1 = segment+1;
            }
                
        }
        
        t1 = path.len(segment0);
        t2 = path.len(segment1);
        v1 = weights[segment0];
        v2 = weights[segment1];
        dts[i] = lin_interp(v1, t1, v2, t2, t);
    }
    return dts;
}

std::vector<double> get_weights(const Path2d& path, const std::vector<double>& weights, const std::vector<P2d>& points)
{
    int n = points.size();
    assert(n > 0);
    assert(path.size() == int(weights.size()));
    
    std::vector<double> dts(n, 0.0);
    double v1, v2, t1, t2, t, d;
    int segment;
    for (int i = 0; i < n; ++i)
    {
        const P2d& p = points[i];
        path.project(p, &t, &d, &segment);
        t1 = path.len(segment);
        t2 = path.len(segment+1);
        v1 = weights[segment];
        v2 = weights[segment+1];
        dts[i] = lin_interp(v1, t1, v2, t2, t);
    }
    return dts;
}

std::vector<P2d> fixed_sampling(const Path2d& path, double step_len, double start_t)
{
    assert(path.size() > 1);
    assert(start_t >= 0.0);
    int pn = int( floor((path.len()-start_t)/step_len) + 1 );
    std::vector<P2d> points(pn);
    for (int i = 0; i < pn; ++i)
        points[i] = path(start_t+step_len*i);
    return points;
}

std::vector<P2d> uniform_sampling(const Path2d& path, int npoints, double* step_len, double start_t, double end_t)
{
    assert(path.size() > 1);
    assert(npoints > 1);
    assert(start_t >= 0.0);
    
    if (end_t <= 0.0)
        end_t = path.len();
        
    double sl = (end_t-start_t)/(npoints-1);
    
    std::vector<P2d> points(npoints);
    for (int i = 0; i < npoints; ++i)
        points[i] = path(start_t+sl*i);
    
    if (step_len != NULL)
        *step_len = sl;
    return points;
}

inline match_result points_matching(const std::vector<P2d>& pts, const std::vector<P2d>& vts, const std::vector<double>& dts, bool fast)
{
    assert(pts.size() == vts.size());
    
    if ( pts.size() < 2 ) // Invalid data
        return match_result();
        
    assert(pts.size() >= 2);
    
    int sz = pts.size();
    
    double Sx = 0.0;    double Sy = 0.0;
    double Wx = 0.0;    double Wy = 0.0;
    double Isx = 0.0;    double Isy = 0.0;
    double Idx = 0.0;    double Idy = 0.0;
    double D = 0.0;
    for(int i = 0; i < sz; ++i)
    {
        const P2d& p = pts[i];
        const P2d& v = vts[i];
        double d = dts[i];
        
        Sx += p.x * d;
        Sy += p.y * d;
        Wx += v.x * d;
        Wy += v.y * d;
        Isx += p.x * v.x * d;
        Isy += p.y * v.y * d;
        Idx += p.x * v.y * d;
        Idy += p.y * v.x * d;
        D += d;
    }
    assert( !EQUAL(D, 0.0) );
    
    double a = -Wx*Sx -Wy*Sy +D*Isx +D*Isy;
    double b =  Wx*Sy -Wy*Sx -D*Idy +D*Idx;
    double den = sqrt(a*a+b*b);
    
    if ( EQUAL(den, 0.0) ) // Invalid data
        return match_result();
        
    assert( !EQUAL(den, 0.0) );
    double x1 = a / den;
    double x2 = b / den;
    double x3 = (Wx+x2*Sy-x1*Sx) / D;
    double x4 = (Wy-x2*Sx-x1*Sy) / D;
    
    double err = 0.0;
    double max_err = 0.0;
    
    if(!fast)
    {
        for(int i = 0; i < sz; ++i)
        {
            const P2d& p = pts[i];
            const P2d& v = vts[i];
            double d = dts[i];
            double m1 = x1 * p.x -x2 * p.y + x3 -v.x;
            double m2 = x2 * p.x +x1 * p.y + x4 -v.y;
            double tmp = d * (m1*m1+m2*m2);
            err += tmp;
            if (tmp > max_err)
                max_err = tmp;
        }
    }
    return match_result( X2d(P2d(x1,x2), P2d(-x2, x1), P2d(x3, x4)), err, max_err, sz, D );
}

match_result points_matching(const std::vector<P2d>& pts, const std::vector<P2d>& vts, bool fast)
{
    return points_matching(pts, vts, std::vector<double>(pts.size(), 1.0), fast);
}


match_result pathOverPath(const Path2d& path1, const Path2d& path2, double res)
{
    std::vector<double> radius1;
    std::vector<double> radius2;
    if (path1.len() < path2.len())
    {
        match_result mr = __pathOverPath(path2, radius2, path1, radius1, res);
        mr.A = inv(mr.A);
        return mr;
    }
    else
    {
        return __pathOverPath(path1, radius1, path2, radius2, res);
    }
}

match_result pathOverPath(const Path2d& path1, std::vector<double>& radius1, const Path2d& path2, std::vector<double>& radius2, double res)
{
    if (path1.len() < path2.len())
    {
        match_result mr = __pathOverPath(path2, radius2, path1, radius1, res);
        mr.A = inv(mr.A);
        return mr;
    }
    else
    {
        return __pathOverPath(path1, radius1, path2, radius2, res);
    }
}

match_result __pathOverPath(const Path2d& path1, const std::vector<double>& radius1, const Path2d& path2, const std::vector<double>& radius2, double res, std::vector<match_result>* mrs)
{
    assert(path1.len() >= path2.len());
    assert(res > 0);
    assert(res <= path2.len());
    assert(int(radius1.size()) == 0 || int(radius1.size()) == path1.size());
    assert(int(radius2.size()) == 0 || int(radius2.size()) == path2.size());
    
    std::vector<P2d> vts = fixed_sampling(path2, res);
    std::vector<P2d> pts = fixed_sampling(path1, res);
    
    int m = pts.size();
    int n = vts.size();
    assert( n <= m );

    std::vector<double> r_vts(n, 1.0);
    std::vector<double> r_pts(m, 1.0);

    if(radius2.size() > 0)
        get_weights(path2, radius2, vts).swap(r_vts);
        
    if(radius1.size() > 0)
        get_weights(path1, radius1, pts).swap(r_pts);
    
    std::vector<double> best_dts(n);
    std::vector<double> dts(n);
    std::vector<double> rdts(n);
    
    std::vector<P2d> best_points(n);
    std::vector<P2d> points(n);
    std::vector<P2d> rpoints(n);
    std::vector<match_result> results(2*m);
    
    match_result best_match;
    for (int i = 0; i < m; ++i)
    {
        int left_points = m-i;
        if (left_points < n)
        {
            std::copy(pts.begin()+i, pts.end(), points.begin()); // Punti
            std::copy(pts.begin(), pts.begin()+n-left_points, points.begin()+left_points);
            
            std::copy(r_pts.begin()+i, r_pts.end(), dts.begin()); // Raggi
            std::copy(r_pts.begin(), r_pts.begin()+n-left_points, dts.begin()+left_points);
        }
        else
        {
            std::copy(pts.begin()+i, pts.begin()+i+n, points.begin()); // Punti
            std::copy(r_pts.begin()+i, r_pts.begin()+i+n, dts.begin()); // Raggi
        }
        std::copy(points.rbegin(), points.rend(), rpoints.begin()); // Punti
        std::copy(dts.rbegin(), dts.rend(), rdts.begin()); // Raggi
        
        for(int j = 0; j < n; ++j)
        {
            dts[j] = combine_radius(dts[j], r_vts[j]);
            rdts[j] = combine_radius(rdts[j], r_vts[j]);
        }
        
        match_result mr1 = points_matching(points, vts, dts);
        match_result mr2 = points_matching(rpoints, vts, rdts);
    
        results[2*i] = mr1;
        results[2*i+1] = mr2;
        
        if (best_match.sse > mr1.sse)
        {
            best_match = mr1;
            best_points.swap(points);
            best_dts.swap(dts);
        }
        
        if (best_match.sse > mr2.sse)
        {
            best_match = mr2;
            best_points.swap(rpoints);
            best_dts.swap(rdts);
        }
    }
    
    double last_sse = best_match.sse * 10.0;
    for (int i = 0; i < 100 && (last_sse/best_match.sse - 1.0) > 0.001; ++i)
    {
        last_sse = best_match.sse;
        Path2d newpath = path1 * best_match.A;
        for (int j=0; j < n; ++j)
            best_points[j] = newpath.project(vts[j]);
        
        if(radius1.size() > 0)
        {
            get_weights(newpath, radius1, best_points).swap(best_dts);
            for(int j = 0; j < n; ++j)
                best_dts[j] = combine_radius(best_dts[j], r_vts[j]);
        }
        
        match_result tmp_match = points_matching(best_points, vts, best_dts);
        
        if (tmp_match.sse < best_match.sse)
        {
            best_match.A *= tmp_match.A;
            best_match.sse = tmp_match.sse;
            best_match.max_se = tmp_match.max_se;
        }
    }

    if (mrs != NULL)
        results.swap(*mrs);
        
    return best_match;
}

match_result vecpathOverPath(const std::vector<Path2d>& vecpath, const Path2d& path2, double step)
{
    std::vector< std::vector<double> > vecradius;
    std::vector<double> radius2;
    return vecpathOverPath(vecpath, vecradius, path2, radius2, step);
}

match_result vecpathOverPath(const std::vector<Path2d>& vecpath, const std::vector< std::vector<double> >& vecradius, const Path2d& path2, const std::vector<double>& radius2, double step)
{
    int sz = vecpath.size();
    assert( sz > 0 );
    assert( step > 0 );
    assert( int(vecradius.size()) == 0 || int(vecradius.size()) == sz);
    
    int longest_path_id = 0;
    double tot_len = 0.0;
    for (int i = 0; i < int(vecpath.size()); ++i)
    {
        double tmp_len = vecpath[i].len();
        tot_len += tmp_len;
        if(tmp_len > vecpath[longest_path_id].len())
            longest_path_id = i;
        if(vecradius.size() > 0)
            assert(int(vecradius[i].size()) == vecpath[i].size());
    }
    
    const Path2d& fp = vecpath[longest_path_id];
    
    std::vector<double> fp_rad;
    if (vecradius.size() > 0)
        fp_rad = vecradius[longest_path_id];
    
    std::vector<match_result> first_matches; // Possibili allineamenti iniziali
    
    int inversion_flag = 0;
    if (fp.len() >= path2.len())
    {
        __pathOverPath(fp, fp_rad, path2, radius2, step, &first_matches);
        inversion_flag = 1;
    }
    else
    {
        __pathOverPath(path2, radius2, fp, fp_rad, step, &first_matches);
        inversion_flag = 0;
    }
    
    
    std::vector<P2d> orig_points( int(ceil(tot_len/step)+sz) );
    std::vector<P2d> points( int(ceil(tot_len/step)+sz) );
    std::vector<P2d> vts( int(ceil(tot_len/step)+sz) );

    std::vector<double> orig_dts( int(ceil(tot_len/step)+sz), 1.0 );
    std::vector<double> dts( int(ceil(tot_len/step)+sz), 1.0 );
    std::vector<double> r_vts( int(ceil(tot_len/step)+sz), 1.0 );
    
    int points_number = 0;
    std::vector<P2d> tmp_pts;
    std::vector<double> tmp_dts;
    for (int i = 0; i < sz; ++i) //Campionamento del vecpath
    {
        fixed_sampling(vecpath[i], step).swap(tmp_pts);
        // Prima di copiare mi assicuro di non andare fuori
        assert(points_number + tmp_pts.size() <= orig_points.size());
        std::copy(tmp_pts.begin(), tmp_pts.end(), orig_points.begin()+points_number); // Punti
        
        if (vecradius.size() > 0)
        {
            get_weights(vecpath[i], vecradius[i], tmp_pts).swap(tmp_dts);
            std::copy(tmp_dts.begin(), tmp_dts.end(), orig_dts.begin()+points_number); // Raggi
        }
        
        points_number += tmp_pts.size();
    }
    // Punti
    orig_points.resize(points_number);
    points.resize(points_number);
    vts.resize(points_number);
    
    // Raggi
    orig_dts.resize(points_number);
    dts.resize(points_number);
    r_vts.resize(points_number);
    
    match_result best_result;
    double current_err;
    double last_err;
    for (int k = 0; k < int(first_matches.size()); ++k) // Per ogni allinamento iniziale
    {
        match_result tmp_result = first_matches[k];
        tmp_result.A = (inversion_flag == 0) ? inv(tmp_result.A) : tmp_result.A;
        
        current_err = DBL_MAX/10;
        last_err = DBL_MAX;
        for (int i = 0; i < 100 && (last_err/current_err - 1.0) > 0.001; ++i)
        {
            last_err = current_err;
            for (int j = 0; j < points_number; ++j)
            {
                points[j] = orig_points[j] * tmp_result.A;
                vts[j] = path2.project(points[j]);
            }
            
            if (radius2.size() > 0)
            {
                get_weights(path2, radius2, vts).swap(r_vts);            
             
                for (int j = 0; j < points_number; ++j)
                    dts[j] = combine_radius(orig_dts[j], r_vts[j]);
            }
            
            match_result m = points_matching(points, vts, dts);
            if (m.sse < tmp_result.sse || i == 0)
            {
                tmp_result.A *= m.A;
                tmp_result.sse = m.sse;
                tmp_result.max_se = m.max_se;
                current_err = (tmp_result.sse/tmp_result.D);
            }
        }
        
        if (tmp_result.sse < best_result.sse)
            best_result = tmp_result;
    }
    return best_result;
}

std::vector<match_result> vecpathOverVecpath(const std::vector<Path2d>& vecpath1, const std::vector<Path2d>& vecpath2, double step, double overlap, double filter)
{
    double len1 = 0.0;
    double len2 = 0.0;
    int sz1 = vecpath1.size();
    int sz2 = vecpath2.size();
    std::vector< std::vector<double> > vecradius;
    std::vector<match_result> mrs;
    
    for (int i = 0; i < sz1; ++i)
        len1 += vecpath1[i].len();
        
    for (int i = 0; i < sz2; ++i)
        len2 += vecpath2[i].len();
        
    if (len2 < len1)
    {
        mrs = __vecpathOverVecpath(vecpath2, vecradius, vecpath1, vecradius, step, overlap, filter);
        for (int i = 0; i < int(mrs.size()); ++i)
        {
            mrs[i].A = inv(mrs[i].A);
            mrs[i].S = inv(mrs[i].S);
            mrs[i].S1 = inv(mrs[i].S1);
            for(int j = 0; j < int(mrs[i].pts.size()); ++j)
            {
                mrs[i].pts[j] = mrs[i].pts[j] * mrs[i].A;
                mrs[i].vts[j] = mrs[i].vts[j] * mrs[i].A;
            }
        }
        return mrs;
    }
    else
    {
        return __vecpathOverVecpath(vecpath1, vecradius, vecpath2, vecradius, step, overlap, filter);
    }
}

std::vector<match_result> vecpathOverVecpath(const std::vector<Path2d>& vecpath1, const std::vector< std::vector<double> >& vecradius1, const std::vector<Path2d>& vecpath2, const std::vector< std::vector<double> >& vecradius2, double step, double overlap, double filter)
{
    double len1 = 0.0;
    double len2 = 0.0;
    int sz1 = vecpath1.size();
    int sz2 = vecpath2.size();
    std::vector<match_result> mrs;
    
    for (int i = 0; i < sz1; ++i)
        len1 += vecpath1[i].len();
        
    for (int i = 0; i < sz2; ++i)
        len2 += vecpath2[i].len();
        
    if (len2 < len1)
    {
        mrs = __vecpathOverVecpath(vecpath2, vecradius2, vecpath1, vecradius1, step, overlap, filter);
        for (int i = 0; i < int(mrs.size()); ++i)
        {
            mrs[i].A = inv(mrs[i].A);
            mrs[i].S = inv(mrs[i].S);
            mrs[i].S1 = inv(mrs[i].S1);
            for(int j = 0; j < int(mrs[i].pts.size()); ++j)
            {
                mrs[i].pts[j] = mrs[i].pts[j] * mrs[i].A;
                mrs[i].vts[j] = mrs[i].vts[j] * mrs[i].A;
            }
        }
        return mrs;
    }
    else
    {
        return __vecpathOverVecpath(vecpath1, vecradius1, vecpath2, vecradius2, step, overlap, filter);
    }
}


std::vector<match_result> __vecpathOverVecpath(const std::vector<Path2d>& vecpath1, const std::vector< std::vector<double> >& vecradius1, const std::vector<Path2d>& vecpath2, const std::vector< std::vector<double> >& vecradius2, double step, double overlap, double filter)
{
    int max_solutions = 10; // Massimo numero di soluzioni da ritornare
    int sz1 = vecpath1.size();
    int sz2 = vecpath2.size();
    int rsz1 = vecradius1.size();
#ifndef NDEBUG
    int rsz2 = vecradius2.size();
#endif
    int overlap_window = int( overlap / step);
    
    if (sz1 == 0 || sz2 == 0 || step <= 0 || overlap_window < 2) // Invalid Input
        return std::vector<match_result>(1);
    
    assert( sz1 > 0 );
    assert( sz2 > 0 );
    assert( step > 0 );
    assert( overlap_window >= 2 );
    
    // O ci sono tutti i raggi di accuratezza o non ce n'e' nessuno
    assert( (rsz1 == sz1 && rsz2 == sz2) || (rsz1 == 0 && rsz2 == 0) );
    
    // Vettori per campionamento a risoluzione piena
    std::vector< std::vector<P2d> > vecpoint1(sz1);
    std::vector< std::vector<P2d> > vecpoint2(sz2);
    std::vector< std::vector<double> > vecray1(sz1);
    
    // Vettori per campionamento a risoluzione ridotta
    std::vector< std::vector<P2d> > reduced_vecpoint1(sz1);
    std::vector< std::vector<P2d> > reduced_vecpoint2(sz2);
    std::vector< std::vector<double> > reduced_vecray1(sz1);
        
    std::vector< int > reduced_veclens2(sz2);
    std::vector<P2d> extreme_pts(sz1*2);
    
    // Campionamento vecpath1: punti e pesi
    int pts_len = 0;
    int reduced_pts_len = 0;
    for(int i = 0; i < sz1; ++i)
    {
        fixed_sampling(vecpath1[i], step).swap(vecpoint1[i]);
        fixed_sampling(vecpath1[i], step * 2).swap(reduced_vecpoint1[i]);
        if (rsz1 > 0)
        {
            get_weights(vecpath1[i], vecradius1[i], vecpoint1[i]).swap(vecray1[i]);
            get_weights(vecpath1[i], vecradius1[i], reduced_vecpoint1[i]).swap(reduced_vecray1[i]);
        }
        
        extreme_pts[i*2] = vecpath1[i].points().front();
        extreme_pts[i*2+1] = vecpath1[i].points().back();
        
        pts_len += vecpoint1[i].size();
        reduced_pts_len += reduced_vecpoint1[i].size();
    }
    
    if (pts_len < overlap_window) // Risoluzione troppo bassa
        return std::vector<match_result>(1);
    assert(pts_len >= overlap_window);
    
    // Campionamento vecpath2: solo punti (i pesi vengono campionati in seguito in base alle proiezioni del vecpath1)
    int vts_len = 0;
    int reduced_vts_len = 0;
    for(int i = 0; i < sz2; ++i)
    {
        fixed_sampling(vecpath2[i], step).swap(vecpoint2[i]);
        fixed_sampling(vecpath2[i], step * 2).swap(reduced_vecpoint2[i]);
        reduced_vecpoint2[i].pop_back(); // Tolgo l'ultimo punto che riacquisto con lo shift
        
        vts_len += vecpoint2[i].size();
        reduced_veclens2[i] = reduced_vecpoint2[i].size(); // lunghezza originale
        reduced_vts_len += reduced_vecpoint2[i].size();
    }

    if (vts_len < overlap_window) // Risoluzione troppo bassa
        return std::vector<match_result>(1);
    assert(vts_len >= overlap_window);
    
    // Vettori per il primo allineamento di punti
    std::vector<P2d> first_vts(overlap_window);
    std::vector<P2d> first_pts(overlap_window);
    
    // Voglio usare il push_back, ma senza dover riallocare la memoria
    std::vector<P2d> pts(pts_len);
    std::vector<P2d> vts(pts_len);
    std::vector<double> wts(pts_len); // Distanze iniziali
    
    std::vector<int> cts(pts_len); // Corrispondenza punto-path
    std::vector<double> tts(pts_len); // Arco dei punti
    std::vector<double> dts(pts_len); // Pesi
    std::vector<double> r_pts(pts_len); // Raggi pts
    std::vector<double> r_vts(pts_len); // Raggi vts
    
    const IndexCompare index_comparator;
    const SseCompare sse_comparator;
    const ScoreCompare score_comparator;
    
    std::vector<match_result> first_solutions(reduced_pts_len*reduced_vts_len); // Allinamento alla tangente
    std::vector<match_result> second_solutions(reduced_pts_len*reduced_vts_len); // Allineamento agli estremi
    std::vector<match_result> third_solutions(reduced_pts_len*reduced_vts_len); // Allineamento completo
    std::vector<match_result> final_solutions(max_solutions); // Soluzioni finali
    
    second_solutions.resize(0);
    third_solutions.resize(0);
    
    // Con l'heap posso processare direttamente i migliori risultati senza ordinare il vettore
    std::make_heap(third_solutions.begin(), third_solutions.end(), score_comparator);
    
    P2d best_p0;
    P2d best_v0;
    P2d best_p1;
    P2d best_v1;
    
    P2d tmp_v0;
    P2d tmp_p0;
    P2d tmp_v1;
    P2d tmp_p1;
    
    double tmp_dist0;
    double tmp_dist1;
    
    double last_score;
    double current_score;
    
    double best_dist;
    double tmp_t;
    double best_t;
    int best_c;
    
    int pushed;
    int limit = 0;
    
    P2d start_p;
    P2d end_p;
    
    X2d inverse;
    
    // Allineamento alla tangente
    for (int f = 0; f < int(step * 2); ++f)
    {
        int c = 0;
        for(int m = 0; f > 0 && m < sz2; ++m) // salto se f=0 perche' l'ho gia' fatto prima
        {
            fixed_sampling(vecpath2[m], step * 2, double(f)).swap(reduced_vecpoint2[m]);
            if (int(reduced_vecpoint2[m].size()) > reduced_veclens2[m])
                reduced_vecpoint2[m].pop_back();
            assert(int(reduced_vecpoint2[m].size()) == int(reduced_veclens2[m]));
        }
        
        for (int k = 0; k < sz2; ++k)
        {
            const std::vector<P2d>& tmp_vts = reduced_vecpoint2[k];
                            
            for (int i = overlap_window; i <= int(tmp_vts.size()); ++i)
            {
                std::copy(tmp_vts.begin()+i-overlap_window, tmp_vts.begin()+i, first_vts.begin());
                
                for (int l = 0; l < sz1; ++l)
                {
                    const std::vector<P2d>& tmp_pts = reduced_vecpoint1[l];
                    
                    for(int j = overlap_window; j <= int(tmp_pts.size()); ++j)
                    {
                        std::copy(tmp_pts.begin()+j-overlap_window, tmp_pts.begin()+j, first_pts.begin());
                        
                        match_result first_result = points_matching(first_pts, first_vts);
                        first_result.id = limit;
                        first_result.S = first_result.A;
                        
                        if (first_result.sse < first_solutions[c].sse)
                            first_solutions[c] = first_result;
                        ++c;
                    }
                    
                }
                
            }
        }
    }
    
    if (first_solutions.empty()) // No acceptable results
        return std::vector<match_result>(1);
    assert(first_solutions.size() > 0);
    
    //Allineamento agli estremi
    limit = first_solutions.size();
    for (int i = 0; i < limit; ++i)
    {
        match_result& tmp_result = first_solutions[i++];
        
        if (tmp_result.sse > filter * overlap_window * 2)
            continue;
        
        pts.resize(0);
        vts.resize(0);
        inverse = inv(tmp_result.A);
        
        for (int h = 0; h < int(extreme_pts.size()); h+=2)
        {
            start_p = extreme_pts[h] * tmp_result.A;
            end_p = extreme_pts[h+1] * tmp_result.A;
            
            best_dist = DBL_MAX;
            for (int t = 0; t < sz2; ++t) // Prendo la proiezione piu' vicina
            {
                tmp_v0 = vecpath2[t].project(start_p, NULL, NULL);
                tmp_v1 = vecpath2[t].project(end_p, NULL, NULL);
                tmp_p0 = vecpath1[h/2].project( tmp_v0 * inverse, NULL, &tmp_dist0 );
                tmp_p1 = vecpath1[h/2].project( tmp_v1 * inverse, NULL, &tmp_dist1 );
                
                if (tmp_dist0 + tmp_dist1 < best_dist)
                {
                    best_v0 = tmp_v0;
                    best_p0 = tmp_p0;
                    best_v1 = tmp_v1;
                    best_p1 = tmp_p1;
                    best_dist = tmp_dist0 + tmp_dist1;
                }
            }
            
            if (!SAME_POINT(best_v0, best_v1) && !SAME_POINT(best_p0, best_p1) && best_dist < sin(filter * 3.14159265/180.0)*dist(best_v0, best_v1) )
            {
                vts.push_back(best_v0);
                pts.push_back(best_p0 * tmp_result.A);
                vts.push_back(best_v1);
                pts.push_back(best_p1 * tmp_result.A);
            }
        }
                    
        match_result m = points_matching(pts, vts);
        
        // Implementare una soluzione piu' sofisticata per la ricerca dei segmenti sovrapposti di 2 path
        if (m.n == -1) // Se fallisco l'allineamento agli estremi conservo l'allineamento originale
        {
            m = tmp_result;
            m.A = X2d();
        }
            
        if( m.sse < filter * m.n * 2)
        {
            m.A = tmp_result.A * m.A;
            m.id = m.id;
            m.S = m.S;
            m.S1 = m.A;
            m.pts = pts;
            m.vts = vts;
            second_solutions.push_back(m);
        }
    }
    
    if (second_solutions.empty()) // No acceptable results
        return std::vector<match_result>(1);
    assert(second_solutions.size() > 0);

    limit = second_solutions.size();
    for (int i = 0; i < limit; ++i)
    {
        match_result& tmp_result = second_solutions[i];

        pts.resize(0);
        vts.resize(0);
        wts.resize(0);
        
        if (rsz1 > 0)
        {
            tts.resize(0);
            cts.resize(0);
            dts.resize(0);
            r_pts.resize(0);
            r_vts.resize(0);
        }
        
        for (int m = 0; m < sz1; ++m)
        {
            const std::vector<P2d>& tmp_pts = vecpoint1[m];
            pushed = 0; // Numero dei punti corrispondenti per l'm-esimo path
            
            for (int h = 0; h < int(tmp_pts.size()); ++h)
            {
                best_p0 = tmp_pts[h] * tmp_result.A;
                
                best_dist = DBL_MAX;
                for (int c = 0; c < sz2; ++c) // Prendo la proiezione piu' vicina
                {
                    tmp_v0 = vecpath2[c].project(best_p0, &tmp_t, &tmp_dist0);
                    if (tmp_dist0 < best_dist)
                    {
                        best_c = c;
                        best_t = tmp_t;
                        best_v0 = tmp_v0;
                        best_dist = tmp_dist0;
                    }
                }
                
                if ( vts.size() > 0 && SAME_POINT(best_v0, vts.back()) ) // Se il punto e' lo stesso 
                {
                    if (best_dist < wts.back()) // Se la distanza e' minore
                    {
                        wts.back() = best_dist;
                        vts.back() = best_v0;
                        pts.back() = best_p0;
                        if (rsz1 > 0)
                        {
                            tts.back() = best_t;
                            cts.back() = best_c;
                            r_pts.back() = vecray1[m][h];
                        }
                    }
                }
                else if (best_dist < filter * overlap_window) // FILTRO SULLA DISTANZA MASSIMA DI UNA PROIEZIONE
                {
                    ++pushed;
                    pts.push_back(best_p0);
                    vts.push_back(best_v0);
                    wts.push_back(best_dist);
                    if (rsz1 > 0)
                    {
                        tts.push_back(best_t);
                        cts.push_back(best_c);
                        r_pts.push_back(vecray1[m][h]);
                    }
                }
            }
            
            if (pushed == 1) // Se c'e' 1 solo punto (probabilmente una corrispondenza con un estremo) allora lo tolgo
            {
                pts.pop_back();
                vts.pop_back();
                wts.pop_back();
                if (rsz1 > 0)
                {
                    tts.pop_back();
                    cts.pop_back();
                    r_pts.pop_back();
                }
            }
                
        }
        
        match_result mr;
        
        if (rsz1 > 0) // Se ci sono i pesi
        {
            fast_weights(vecpath2, vecradius2, tts, cts).swap(r_vts);
            dts.resize(r_vts.size());
            for (int j = 0; j < int(dts.size()); ++j)
                dts[j] = combine_radius(r_pts[j], r_vts[j]);
            
            mr = points_matching(pts, vts, dts);
        }
        else
            mr = points_matching(pts, vts);
        
        if( mr.n != -1 && mr.sse < filter * filter * mr.n )
        {
            mr.A = tmp_result.A * mr.A;
            mr.id = tmp_result.id;
            mr.S = tmp_result.S;
            mr.S1 = tmp_result.S1;
            mr.pts = tmp_result.pts;
            mr.vts = tmp_result.vts;
            third_solutions.push_back(mr);
            std::push_heap(third_solutions.begin(), third_solutions.end(), score_comparator);
        }
    }
    
    if (third_solutions.empty()) // No acceptable results
        return std::vector<match_result>(1);
    assert(third_solutions.size() > 0);
    
    // Adattamento delle corrispondenze tra punti
    int z;
    limit = std::min(max_solutions, (int)third_solutions.size()); // Accosto i migliori 5
    for (int i = 0; i < limit; ++i)
    {
        std::pop_heap(third_solutions.begin(), third_solutions.end(), score_comparator);
        match_result& tmp_result = third_solutions.back();
        
        // Passi di adattamento
        z = 0;
        current_score = getScore(tmp_result);
        last_score = current_score / 10;
        for (z = 0; z < 100 && (current_score/last_score - 1.0) > 0.001 ; ++z) // Miglioramento minimo dell 1x1000. Altrimenti termino
        {
            last_score = current_score;
            pts.resize(0);
            vts.resize(0);
            wts.resize(0);
            
            if (rsz1 > 0)
            {
                tts.resize(0);
                cts.resize(0);
                dts.resize(0);
                r_pts.resize(0);
                r_vts.resize(0);
            }
            
            for (int m = 0; m < sz1; ++m)
            {
                const std::vector<P2d>& tmp_pts = vecpoint1[m];
                pushed = 0;
            
                for(int h = 0; h < int(tmp_pts.size()); ++h)
                {
                    best_p0 = tmp_pts[h] * tmp_result.A;
                    
                    best_dist = DBL_MAX;
                    for (int c = 0; c < sz2; ++c) // Prendo la proiezione piu' vicina
                    {
                        tmp_v0 = vecpath2[c].project(best_p0, &tmp_t, &tmp_dist0);
                        if (tmp_dist0 < best_dist)
                        {
                            best_c = c;
                            best_t = tmp_t;
                            best_v0 = tmp_v0;
                            best_dist = tmp_dist0;
                        }
                    }
                    
                    if ( vts.size() > 0 && SAME_POINT(best_v0, vts.back()) ) // Se il punto e' lo stesso 
                    {
                        if (best_dist < wts.back()) // Se la distanza e' minore
                        {
                            wts.back() = best_dist;
                            vts.back() = best_v0;
                            pts.back() = best_p0;
                            if (rsz1 > 0)
                            {
                                tts.back() = best_t;
                                cts.back() = best_c;
                                r_pts.back() = vecray1[m][h];
                            }
                        }
                    }
                    else if (best_dist < filter * overlap_window) // FILTRO SULLA DISTANZA MASSIMA DI UNA PROIEZIONE
                    {
                        ++pushed;
                        pts.push_back(best_p0);
                        vts.push_back(best_v0);
                        wts.push_back(best_dist);
                        if (rsz1 > 0)
                        {
                            tts.push_back(best_t);
                            cts.push_back(best_c);
                            r_pts.push_back(vecray1[m][h]);
                        }
                    }
                }
                
                if (pushed == 1)
                {
                    pts.pop_back();
                    vts.pop_back();
                    wts.pop_back();
                    if (rsz1 > 0)
                    {
                        tts.pop_back();
                        cts.pop_back();
                        r_pts.pop_back();
                    }
                }
            }
            
            
            match_result mr;
            
            if (rsz1 > 0) // Se ci sono i pesi
            {
                fast_weights(vecpath2, vecradius2, tts, cts).swap(r_vts);
                dts.resize(r_vts.size());
                for (int j = 0; j < int(dts.size()); ++j)
                    dts[j] = combine_radius(r_pts[j], r_vts[j]);
                
                mr = points_matching(pts, vts, dts);
            }
            else
                mr = points_matching(pts, vts);

            if ( mr.n != -1 && !score_comparator(mr, tmp_result) ) // Aggiorno solo se il punteggio e' minore
            {
                tmp_result.A *= mr.A;
                tmp_result.sse = mr.sse;
                tmp_result.max_se = mr.max_se;
                tmp_result.n = mr.n;
                tmp_result.D = mr.D;
                //tmp_result.id = i;
                current_score = getScore(mr);
            }
            // Se non aggiorno, alla prossima verifica della condizione di uscita, il ciclo termina
        }
        
        final_solutions[i] = tmp_result;        
        third_solutions.pop_back(); // Lo tolgo solo ora perche' c'e' un riferimento!!!
    }
    
    std::sort(final_solutions.rbegin(), final_solutions.rend(), score_comparator); // Ordine decrescente
    return final_solutions;
}
