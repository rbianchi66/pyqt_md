#include "complexspline.h"
#include "spline.h"
#include "oldspline.h"
#include "offset.h"
#include <algorithm>
#include <map>

// tipo_spline
const int CALIGOLA = 1;
const int CALCAD = 0;

std::vector<P2d> complexSplineXC(const std::vector<P2d>& ipts,
			   std::vector<int> spigoli,
			   int levels,
			   bool closed,
			   int iters,
			   double kk,
               int tipo_spline,
               std::vector<int> *indices)
{
    const std::vector<P2d> *pts = &ipts;
    std::vector<P2d> *pts2 = NULL;

    // Numero di spigoli maggiore di quello dei punti
    int len_pts = (int)pts->size();

	assert((int)spigoli.size() <= len_pts);

    if (!closed)
    {
        // Se la linea e' aperta allora il primo e l'ultimo punto
        // sono sempre considerati punti spigolo
        if (std::find(spigoli.begin(), spigoli.end(), 0) == spigoli.end())
        {
            spigoli.push_back(0);
        }
        if (std::find(spigoli.begin(), spigoli.end(), len_pts-1) == spigoli.end())
        {
            spigoli.push_back(len_pts-1);
        }
    }

    std::sort(spigoli.begin(), spigoli.end());

	std::vector<P2d> res;
	std::vector<int> ix;

    if (spigoli.empty())
    {
        // Linea morbida chiusa, usa una singola spline
        if (tipo_spline == CALCAD)
        {
            pts2 = new std::vector<P2d>;

            for (int t=0; t<len_pts; ++t)
                pts2->push_back((*pts)[t]);

            pts2->push_back((*pts)[0]); // aggiungo il primo punto in fondo

            // nuovi punti
            pts = pts2;
            len_pts = (int)pts->size();

            oldSplineX(*pts, &ix).swap(res);
        }
        else
        {
            splineX(*pts, &ix, levels, closed, iters, kk).swap(res);
        }
    }
    else
    {
        if (closed)
            spigoli.insert(spigoli.begin(), spigoli[(int)spigoli.size()-1]);

        int i0 = spigoli[0];
        res.push_back((*pts)[i0]);

        std::map<int, int> dix;
        dix[i0] = 0;

        std::vector<P2d> pp;
        std::vector<int> ii;
        pp.reserve(len_pts);
        ii.reserve(len_pts);

        for (int si=1; si<(int)spigoli.size(); ++si)
        {
            int i1 = spigoli[si];

            pp.clear();
            ii.clear();
            pp.push_back((*pts)[i0]);
            ii.push_back(i0);

            int i = i0+1;
            assert (i >= 0 && i <= len_pts);
            if (i == len_pts)
                i = 0;
            while (i != i1)
            {
                pp.push_back((*pts)[i]);
                ii.push_back(i);

                ++i;
                assert (i >= 0 && i <= len_pts);
                if (i == len_pts)
                    i = 0;
            }

            pp.push_back((*pts)[i1]);
            ii.push_back(i1);

            if ((int)pp.size() == 2)
            {
                // Tratto rettilineo
                dix[i1] = (int)res.size();
                res.push_back((*pts)[i1]);
            }
            else
            {
                // Tratto spline
                std::vector<P2d> ts;
                std::vector<int> tix;

                if (tipo_spline == CALCAD)
                {
                    if (pp[0] == pp[(int)pp.size()-1])  // se punto iniziale a spigolo
                        pp.push_back(pp[0]);            // aggiungo (ancora) il primo punto in fondo

                    oldSplineX(pp, &tix).swap(ts);
                }
                else
                {
                    splineX(pp, &tix, levels, 0, iters, kk).swap(ts);
                }

                int zip_min = tix.size() < ii.size() ? int(tix.size()) : int(ii.size());
                for (int index = 0; index < zip_min; ++index)
                {
                    int i = ii[index];
                    int t = tix[index];
                    int v = (int)res.size() + t - 1;

                    dix[i] = v;
                }

                res.insert(res.end(), ts.begin()+1, ts.end());
            }

            i0 = i1;
        }

        // Ruota per allineare risultato con pts (necessario solo
        // per spline chiuse con spigoli in cui il primo punto non
        // sia un punto spigolo)
        int xx = dix[0];

        if (xx > 0)
        {
            std::rotate(res.begin(), res.begin()+xx, res.end());
            res.push_back(res[0]);

            for (int i=0; i<len_pts; ++i)
            {
                int v = imod(dix[i] - xx, (int)res.size());
                dix[i] = v;
            }

            // A seguito della rotazione, due punti sovrapposti possono risultare
            // essere il primo e l'ultimo, nel qual caso ix non sarebbe più
            // ordinata.
            if (dix[len_pts-1] == 0)
                dix[len_pts-1] = (int)res.size()-1;
        }

        std::vector<int> nix;

        // converte il dict in lista
        for (std::map<int, int>::iterator ite=dix.begin(); ite != dix.end(); ++ite)
        {
            // int i = (*ite).first;
            int v = (*ite).second;

            nix.push_back(v);
        }

        ix.swap(nix);
    }

    std::vector<P2d> sres;
    std::vector<int> six;

    sres.push_back(res[0]);
    six.push_back(ix[0]);

    // L'algoritmo di semplificazione per tratto funziona solo se ix è ordinata
    for (int i=1; i<(int)ix.size(); ++i)
    {
        if (ix[i-1] != ix[i])
        {
            sres.pop_back();
            std::vector<P2d> s;

            for (int z=ix[i-1]; z<ix[i]+1; ++z)
                s.push_back(res[z]);

            assert((int)s.size() > 0);

            Path2d simp = Path2d(s);
            simplify(simp);

            for (int z=0; z<(int)simp.size(); ++z)
                sres.push_back(simp.get(z));
        }

        six.push_back((int)sres.size() - 1);
    }

    if (closed)
    {
        sres.pop_back();
        std::vector<P2d> s;

        for (int z=ix[ix.size()-1]; z<(int)res.size(); ++z)
            s.push_back(res[z]);

        Path2d simp = Path2d(s);
        simplify(simp);

        for (int z=0; z<(int)simp.size(); ++z)
            sres.push_back(simp.get(z));
    }

	if (indices)
		std::swap(*indices, six);

    if (pts2)
        delete pts2;

	return sres;
}
