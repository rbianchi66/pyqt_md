// Workaround ad un bug sull'inclusione di iostream per gcc-4.0 sotto Mac OS X
// (http://lists.apple.com/archives/xcode-users/2007/Apr/msg00040.html)
#include <cctype>

#include <iostream>
#include "packing.h"
#include <ctime>

/* Dato un vector di Path2d, ne calcola la bounding box risultante */
void bBox(const std::vector<Path2d> &paths, P2d &minp, P2d &maxp, bool initp = true)
{
    for (unsigned int i=0; i<paths.size(); ++i)
    {
        const Path2d &path = paths[i];
        const int sz = path.size();
        for (int p=0; p < sz; ++p)
        {
            bool first = !(p|i) && initp;

            if (first || minp.x > path[p].x) minp.x = path[p].x;
            if (first || minp.y > path[p].y) minp.y = path[p].y;
            if (first || maxp.x < path[p].x) maxp.x = path[p].x;
            if (first || maxp.y < path[p].y) maxp.y = path[p].y;
        }
    }
}

/* Contiene le rappresentazioni a scaline di un'area, per ogni rotazione */
namespace { struct PQRect{

    PQRect(const std::vector<Path2d>& path1, int rot) : refer(&path1), xs1(rot), xs2(rot), h(rot), w(rot), t(rot), rotations(rot)
    {
        initPQRect(path1);
    };

    void initPQRect(const std::vector<Path2d>& path1)
    {
        P2d minp, maxp, path1_center;
        bBox(path1, minp, maxp);
        path1_center = (minp+maxp) / 2.0;

        for (int r = 0; r < rotations; ++r)
        {
            // Crea una copia dell'area
            std::vector<Path2d> path1_copy(path1.size());
            double angle = (((2.0 * PI) / ((double)rotations))) * r;

            // Ruota
            t[r] = X2d::xlate(-path1_center) * X2d::rot(angle);
            for (int i = 0; i < int(path1_copy.size()); ++i)
                path1_copy[i] = path1[i] * X2d::xlate(-path1_center) * X2d::rot(angle);

            //Traslo l'area per rasterizzarla correttamente
            P2d minp_copy, maxp_copy;
            bBox(path1_copy, minp_copy, maxp_copy);

            t[r] = t[r] * X2d::xlate(-minp_copy);
            for (int i = 0; i < int(path1_copy.size()); ++i)
                path1_copy[i] = path1_copy[i] * X2d::xlate(-minp_copy);

            int path1_copy_w = int(maxp_copy.x-minp_copy.x+1);
            int path1_copy_h = int(maxp_copy.y-minp_copy.y+1);

            Image<int> bitmap(path1_copy_w, path1_copy_h);

            // Disegno il bordo
            polyDraw(bitmap, 1, path1_copy);

            // Dalla bitmap ricavo la rappresentazione a scanline
            for (int y = 0; y < path1_copy_h; ++y)
            {
                int x1 = 0;
                int x2 = path1_copy_w-1;

                while( x1 < path1_copy_w && bitmap.pixel(x1, y) == 0)
                {
                    ++x1;
                }
                while( x2 > 0 && bitmap.pixel(x2, y) != 1)
                {
                    --x2;
                }
                xs1[r].push_back(x1);
                xs2[r].push_back(x2);
            }
            h[r] = path1_copy_h;
            w[r] = path1_copy_w;
        }

    }

    // referred path (tengo un riferimento per passarlo ad un eventuale comparator personalizzato)
    const std::vector<Path2d>* refer;

    // Ogni rotazione, ha il suo vector di inizio-fine scanline
    std::vector< std::vector< int > > xs1;
    std::vector< std::vector< int > > xs2;

    std::vector<int> h; // Associa ad ogni rotazione il numero di scanline
    std::vector<int> w; // Associa ad ogni rotazione la larghezza dell'immagine

    std::vector<X2d> t; //Ad ogni rotazione associa una matrice di rotazione
    int rotations; // Rotazioni totali
}; }

template< class C > double pack_t(PQRect& p1, X2d& x1, PQRect& p2, X2d& x2, int r1, int r2, C comparator)
{
    double best_score = INT_MAX;
    int best_deltax = INT_MAX;
    int best_deltay = 0;
    int best_r1 = 0;
    //int best_r2 = 0;

    for (int z = 0; z < r1; ++z)
    {
        //std::vector<int>& x11 = p1.xs1[z];
        std::vector<int>& x12 = p1.xs2[z];
        int h1 = p1.h[z];
        int w1 = p1.w[z];
        std::vector<int>& x21 = p2.xs1[z];
        //std::vector<int>& x22 = p2.xs2[z];
        int h2 = p2.h[z];
        int w2 = p2.w[z];
/*
    for (int z = 0; z < r1; ++z)
    {
        // Riferimenti ai dati del primo pezzo
        std::vector<int>& x11 = p1.xs1[z];
        std::vector<int>& x12 = p1.xs2[z];
        int h1 = p1.h[z];
        int w1 = p1.w[z];

        // Per ogni rotazione del secondo pezzo
        for (int r = 0; r < r2; ++r)
        {
            std::vector<int>& x21 = p2.xs1[r];
            std::vector<int>& x22 = p2.xs2[r];
            int h2 = p2.h[r];
            int w2 = p2.w[r];
*/
            // Fa scorrere la cima del secondo pezzo lungo l'altezza del primo pezzo
            for (int y1 = 0; y1 < h1+h2+2; ++y1)
            {
                //Scostamento iniziale
                int tmp_delta = 0;

                const int start_s = max(0, y1-h2);
                const int start_s2 = y1 - start_s;
                const int stop_s = min(h1, y1);

                //Calcola l'offset
                for(int y2 = start_s; y2 < stop_s; ++y2)
                {
                    int mm = x12[y2]-x21[ h2-start_s2 + (y2-start_s) ];
                    if (mm > tmp_delta)
                        tmp_delta = mm;
                }
                tmp_delta += 1;

                double score = comparator(h1, w1, h2, w2, tmp_delta, y1-h2, *(p1.refer), X2d(), *(p2.refer), X2d());

                if (score < best_score) //Salva il punteggio piu' basso
                {
                    best_r1 = z;
                    best_deltax = tmp_delta;
                    best_deltay = y1;
                    best_score = score;
                }
            }
/*
        }
    }
*/
    }
    x1 = p1.t[best_r1] * X2d::xlate(P2d(0, 0));
    x2 = p2.t[best_r1] * X2d::xlate(P2d(best_deltax, best_deltay - p2.h[best_r1]));

    //if (best_r1 == 1)
    {
        x1 *= X2d::rot(-PI/2.0*best_r1);
        x2 *= X2d::rot(-PI/2.0*best_r1);
    }

    return best_score;
}

/*
 * Date 2 aree ne mantiene una fissa, mentre l'altra viene accostata sulle x, con diverse
 * rotazioni a diverse altezze y. Sceglie poi l'accostamento che produce la bounding box minore
 */
double pack(const std::vector<Path2d>& path1, X2d& x1, const std::vector<Path2d>& path2, X2d& x2, int rots)
{
    PQRect p1(path1, rots);
    PQRect p2(path2, rots);
    return pack_t(p1, x1, p2, x2, rots, rots, BBoxAreaCompare());
}

template <class C> double pack(const std::vector<Path2d>& path1, X2d& x1, const std::vector<Path2d>& path2, X2d& x2, int rots, C comparator)
{
    PQRect p1(path1, rots);
    PQRect p2(path2, rots);
    return pack_t(p1, x1, p2, x2, rots, rots, comparator);
}


/*
 * Ritorna l'indice del path scelto fra [other_paths]
 */
int pack(const std::vector<Path2d>& path1, X2d& x1, std::vector< std::vector<Path2d> >& other_paths, X2d& x2, int rots)
{
    PQRect p1(path1, rots);

    int best_path = -1;
    double best_gain = INT_MIN;
    P2d minp, maxp, e;

    bBox(path1, minp, maxp);
    e = maxp-minp;

    double path1_box_area = e.x*e.y;
    double path1_real_area = area( path1 ); //area( convex_hull(path1) );
    double path1_usage = path1_real_area/path1_box_area;


    for (int i = 0; i < int(other_paths.size()); ++i)
    {
        std::vector<Path2d>& path2 = other_paths[i];

        bBox(path2, minp, maxp);
        e = maxp-minp;

        double path2_box_area = e.x*e.y;
        double path2_real_area = area(path2); //area(convex_hull(path2));
        double path2_usage = path2_real_area/path2_box_area;

        PQRect p2(path2, rots);
        X2d tmp_x1, tmp_x2;
        double new_box_area = pack_t(p1, tmp_x1, p2, tmp_x2, rots, rots, BBoxAreaCompare());
        double new_usage = (path1_real_area + path2_real_area) / new_box_area;

        /*
         * USA L'AREA DELLA CONVEX HULL
        */
        //~ std::vector<Path2d> tmp_res(path1.size()+path2.size());
        //~ for (int k = 0; k < path1.size(); ++k)
            //~ tmp_res[k] = path1[k]*tmp_x1;
        //~ for (int k = 0; k < path2.size(); ++k)
            //~ tmp_res[path1.size() + k] = path2[k]*tmp_x2;
        //~ double new_usage = (path1_real_area + path2_real_area) / area(convex_hull(tmp_res));


        /*
         * USA L'AREA DELLA BOUNDING BOX
        */

        double gain = new_usage - max(path1_usage, path2_usage);
        if ( gain > best_gain)
        {
            best_gain = gain;
            best_path = i;
            x1 = tmp_x1;
            x2 = tmp_x2;
        }
    }
    if (best_gain > 0)
        return best_path;
    else
        return -best_path;
}

struct PackingScoreComparator
{
    inline bool operator () (const Packing& p1, const Packing& p2)
    {
        return (p1.score < p2.score);
    }
};

template< class C > std::vector< Packing > pack_t(std::vector< std::vector<Path2d> >& paths, int rots, C comparator)
{
    std::vector< Packing > packs;
    const int sz = paths.size();

    std::vector<PQRect> qrects;

    //Pre-caching delle strutture dati
    for (int i = 0; i < int(paths.size()); ++i)
        qrects.push_back(PQRect(paths[i], rots));

    for (int i = 0; i < sz; ++i)
    {
        PQRect& p1 = qrects[i];
        //std::cerr << "packing: " << i << " of " << sz << "\n";

        for (int j = i+1; j < sz; ++j)
        {
            PQRect& p2 = qrects[j];
            X2d tmp_x1, tmp_x2;

            double score = pack_t(p1, tmp_x1, p2, tmp_x2, rots, rots, comparator);
            packs.push_back( Packing(i, j, tmp_x1, tmp_x2, score) );
        }
    }

    sort(packs.begin(), packs.end(), PackingScoreComparator());

    return packs;
}

std::vector< Packing > pack(std::vector< std::vector<Path2d> >& paths, int rots)
{
    return pack_t(paths, rots, BBoxAreaCompare());
}
