/*
 *
 *
 */

#include "geo2d.h"
#include "unisci.h"
#include <math.h>
#include <limits.h>
#include <assert.h>

#define DEBUG_UNISCI 0

#define MAX(a,b)    (((a) > (b)) ? (a) : (b))
#define MIN(a,b)    (((a) < (b)) ? (a) : (b))

namespace {

// Guarda se l'angolo di incidenza e' ragionevole (> 20 gradi)
// FIXME: Parametro ?
bool angleOk(P2d da, P2d db)
{
	double angle = sfmod((::angle(da, db) + PI/2), PI) - PI/2;
	return fabs(angle) > 20.0*PI/180.0;
}

// Elimina il ricciolino sul punto di unione. Se il punto di intersezione
// è opposto alla tangente del path in uscita, prova a tornare un po'
// indietro
double eliminaRicciolo(const Path2d& TP, double t0, double t1, 
					   double t, P2d p, double step)
{
	double dir = 1.0;
	if (t0 > t1)
	{
		step = -step;
        dir = -1.0;
	}

	P2d pt = TP(t);
	P2d tan = ::dir(pt - TP(t-step));
	double k = (p - pt) * tan;
#if DEBUG_UNISCI
	printf("\tElimina ricciolino: p=(%g,%g), pt=(%g,%g)\n", p.x, p.y, pt.x, pt.y);
	printf("\tElimina ricciolino: tan=(%g,%g), k=%g\n", tan.x, tan.y, k);
#endif
	if (k < 0)
	{
		// torna indietro di quanto necessario, più un 5% di sicurezza
		// per evitare problemi numerici
		t += k * dir * 1.05;
		if (t0 < t1)
			t = MIN(MAX(t, t0), t1);
		else
			t = MAX(MIN(t, t0), t1);
	}
	return t;
}

} // anonymous namespace



double EPSILON = 10E-8;

P2d unisci(Path2d A, double a0, double a1,
		   Path2d B, double b0, double b1,
		   double ta, double tb,
		   double *ai, double *bi)
{
	struct State {
		const Path2d *A, *B;
		double ta, tb;  // parametri primo e secondo punto
		double sgna, sgnb; // segno per percorrere i due tratti secondo la loro direzione

		P2d pa, pb;     // primo e secondo punto
		P2d da, db;     // direzione tangente uscente / entrante
		P2d p;          // intersezione delle tangenti
		double dd;      // distanza dell'intersezione (fattore di bontà)

		State() {}
		State(const Path2d& _A, const Path2d& _B,
			  double _ta, double _tb,
			  bool reva, bool revb)
			: A(&_A), B(&_B), ta(_ta), tb(_tb),
			  sgna(reva ? -1.0 : 1.0),
			  sgnb(revb ? -1.0 : 1.0)
		{
			pa = (*A)(_ta);
			pb = (*B)(_tb);
		}

		bool intersect(void)
		{
			const double eps = 0.1;

			da = dir((*A)(ta + eps * sgna) - pa);
			db = dir((*B)(tb - eps * sgnb) - pb);

			if (!angleOk(da, db))
				return false;

			double t = ((pb - pa) ^ db) / (da ^ db);
			p = pa + da * t;

			dd = len(pa - p) + len(pb - p);
			return true;
		}

		bool kiss(void)
		{
			// L'unione baciata riesce sempre
			dd = len(pa - pb);
			p = (pa + pb) / 2;
			return true;
		}

        bool operator<(const State& s) const
		{
            return dd - s.dd < -EPSILON;
        }

	};

#ifndef NDEBUG
	{
		Path2d A2(A), B2(B);
		A2.clean();
		B2.clean();
		assert(A2.len() != 0);
		assert(B2.len() != 0);
	}
#endif

	bool reva = (a1 < a0);
	bool revb = (b1 < b0);

	State cur(A, B, ta, tb, reva, revb);
	State best;

	// Esegue una discesa per trovare i valori di ta/tb che migliorano
	// il punto di intersezione trovato. Il punto di intersezione viene
	// calcolato come intersezione delle tangenti se l'angolo e'
	// ragionevole e come media se l'angolo delle tangenti diventa
	// irragionevolmente basso (unione "baciata"; quando cioe' calcolare
	// l'intersezione delle tangenti non ha senso numericamente)
	{
		// Prova l'intersezione e se non riesce usa la baciata
		cur.intersect() || cur.kiss();

		best = cur;

		double amin = MIN(a0, a1);
		double bmin = MIN(b0, b1);
		double amax = MAX(a0, a1);
		double bmax = MAX(b0, b1);
#if DEBUG_UNISCI
		printf("A min/max=%g,%g\n", amin, amax);
		printf("B min/max=%g,%g\n", bmin, bmax);
#endif

		double step = best.dd / 10.0;
		double step0 = step / 1000.0;
		while (step > step0)
		{
			double ta = best.ta;
			double tb = best.tb;
#if DEBUG_UNISCI
			printf("step=%g\n", step);
			printf("ta,tb=%g,%g\n", ta, tb);
			printf("pa,pb=(%g,%g),(%g,%g)\n", A(ta).x, A(ta).y, B(tb).x, B(tb).y);
#endif
			double ta_variants[3] = { MAX(amin, ta-step), ta, MIN(amax, ta+step) };
			double tb_variants[3] = { MAX(bmin, tb-step), tb, MIN(bmax, tb+step) };
			bool moved = false;

			for (int i=0;i<3;i++)
				for (int j=0;j<3;j++)
				{
					double xta = ta_variants[i];
					double xtb = tb_variants[j];

					if (xta == ta && xtb == tb)
						continue;

					State cur(A, B, xta, xtb, reva, revb);
					cur.intersect() || cur.kiss();
					if (cur < best)
					{
						best = cur;
						moved = true;
					}
				}

			if (!moved)
				step *= 0.5;
		}

		if (best.intersect())
		{
			// Nel caso che il risultato finale sia stato trovato tramite
			// intersezione e' necessario verificare che i due punti non
			// "superino" l'intersezione (neanche di pochissimo) perche' in
			// questo caso si creerebbero dei microscopici "riccioli" che
			// mandano *giustamente* in confusione eventuali elaborazioni
			// successive (es. offset).

			// Evita valori di step talmente piccoli che rischiano di generare
			// una divisone per zero sul calcolo della tangente.
			best.ta = eliminaRicciolo(A, a0, a1, best.ta, best.p, MAX(step0, 1E-8));
			best.tb = eliminaRicciolo(B, b0, b1, best.tb, best.p, MAX(step0, 1E-8));
			best.pa = A(best.ta);
			best.pb = B(best.tb);
		}
		else
		{
			// Nel caso che il risultato finale sia stato trovato
			// tramite unione baciata viene invece controllato se l'unione
			// si traduce in un "gradino" fra linee quasi parallele; in
			// questo caso viene applicato un "cerotto" per ammorbidire il
			// passaggio da una linea all'altra.
			if ((best.da * best.db) > 0)
			{
				// Se le tangenti si "guardano" lascia un "cerotto" di 3 volte
				// la distanza, tornando indietro lungo il tratto.
				// FIXME: Parametro ?
				double c = 1.5 * len(best.pa - best.pb);

				// Il "tornare indietro" è ovviamente in funzione del segno
				// del tratto (visto che a0 > a1 è permesso).
				if (!reva)
				{
					best.ta -= c;
					best.ta = MAX(best.ta, a0);
				}
				else
				{
					best.ta += c;
					best.ta = MIN(best.ta, a0);
				}

				if (!revb)
				{
					best.tb -= c;
					best.tb = MAX(best.tb, b0);
				}
				else
				{
					best.tb += c;
					best.tb = MIN(best.tb, b0);
				}

				best.pa = A(best.ta);
				best.pb = B(best.tb);
			}
		}
	}

	assert(len(A(best.ta) - best.pa) < 1E-4);
	assert(len(B(best.tb) - best.pb) < 1E-4);

	// Controlla che il punto ritornato sia contenuto entro gli estremi
	// dei tratti.
	if (!reva)
	{
		assert(best.ta >= a0);
		assert(best.ta <= a1);
	}
	else
	{
		assert(best.ta >= a1);
		assert(best.ta <= a0);
	}
	if (!revb)
	{
		assert(best.tb >= b0);
		assert(best.tb <= b1);
	}
	else
	{
		assert(best.tb >= b1);
		assert(best.tb <= b0);
	}

	if (ai) *ai = best.ta;
	if (bi) *bi = best.tb;
	return best.p;
}
