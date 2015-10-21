#!/usr/bin/env python
# encoding: utf-8

from __future__ import division
import math
import random
from geo2d import vecpathOverVecpath, pathOverPath, P, Path, X, vecpathOverPath, points_matching, dist, scale, fixed_sampling, get_weights, circle, rot, xlate
import unittest
import itertools

# tolleranza fissata a 3 cifre dopo la virgola, ovvero
# 1 millesimo di millimetro
tollerance = 3

def pairwise(iterable, step = 1):
    "s -> (s0,s1), (s1,s2), (s2, s3), ..."
    a, b = itertools.tee(iterable)
    for s in range(step):
        next(b, None)
    return itertools.izip(a, b)

class TestUtils(unittest.TestCase):

    def testFixedSampling(self):
        """
        Controlla la correttezza del campionamento a step fisso su un segmento
        """
        # Path definito da 2 punti
        path = Path([P(0,0), P(100,0)])
        vecpoints = fixed_sampling(path, 10.0)
        self.assertEqual(len(vecpoints), 11)

        vecpoints = fixed_sampling(path, 101.0)
        self.assertEqual(len(vecpoints), 1)

        vecpoints = fixed_sampling(path, 100.0)
        self.assertEqual(len(vecpoints), 2)

        #Path definito da 101 punti
        path = Path([P(x,0) for x in xrange(101)])
        vecpoints = fixed_sampling(path, 10.0)
        self.assertEqual(len(vecpoints), 11)

        vecpoints = fixed_sampling(path, 101.0)
        self.assertEqual(len(vecpoints), 1)

        vecpoints = fixed_sampling(path, 100.0)
        self.assertEqual(len(vecpoints), 2)

    def testGetWeights(self):
        """
        Controlla la correttezza della funzione get_weights.
        Esegue un'interpolazione lineare dei pesi
        """
        path = Path([P(x,0) for x in xrange(101)])
        weight = [ x for x in xrange(101) ]
        dts = get_weights(path, weight, list(path))
        for d, w in zip(dts, weight):
            self.assertAlmostEqual(d, w)

        rx = random.random()*path.len()
        dts = get_weights(path, weight, [P(rx, 0)] )
        self.assertAlmostEqual(dts[0], rx)

class TestPointCloud(unittest.TestCase):

    cloudSize = 100
    sf = 1000.0

    def calcPCErr(self, pc1, pc2):
        """
        Calcola la distanza di un punto dal suo corrispondente fra due nuvole di punti
        """
        errore = 0.0
        for p1, p2 in zip(pc1, pc2):
            d = dist(p1,p2)
            errore += d*d
        return errore

    def testIdentity(self):
        """
        Date due identiche nuvole di punti, la trasformazione risultante dovra' essere l'identita'
        """
        pc1 = [ P( (random.random()-0.5)*self.sf, (random.random()-0.5)*self.sf) for x in xrange(self.cloudSize)]
        pc2 = list(pc1)
        mr = points_matching(pc1, pc2)
        A = X(mr.A)
        I = X()
        self.assertAlmostEquals(A.a.x, I.a.x)
        self.assertAlmostEquals(A.a.y, I.a.y)
        self.assertAlmostEquals(A.b.x, I.b.x)
        self.assertAlmostEquals(A.b.y, I.b.y)
        self.assertAlmostEquals(A.c.x, I.c.x)
        self.assertAlmostEquals(A.c.y, I.c.y)

        self.assertAlmostEquals(self.calcPCErr([ p*A for p in pc1 ], pc2), 0.0, tollerance)

    def testPerfectTrasformation(self):
        """
        Data una nuvola di punti e una sua copia alla quale viene applicata una rototraslazione,
        la funzione deve restituire esattamente la rototraslazione applicata alla seconda nuvola di punti
        """
        teta = math.pi*2*random.random()
        x1 = math.cos(teta)
        x2 = math.sin(teta)
        x3 = random.random()*self.sf
        x4 = random.random()*self.sf
        T = X(P(x1, x2), P(-x2, x1), P(x3, x4))
        pc1 = [ P( (random.random()-0.5)*self.sf, (random.random()-0.5)*self.sf) for x in xrange(self.cloudSize)]
        pc2 = [ p*T for p in pc1 ]
        mr = points_matching(pc1, pc2)
        A = X(mr.A)
        self.assertAlmostEquals(A.a.x, T.a.x)
        self.assertAlmostEquals(A.a.y, T.a.y)
        self.assertAlmostEquals(A.b.x, T.b.x)
        self.assertAlmostEquals(A.b.y, T.b.y)
        self.assertAlmostEquals(A.c.x, T.c.x)
        self.assertAlmostEquals(A.c.y, T.c.y)

        self.assertAlmostEquals(self.calcPCErr([ p*A for p in pc1 ], pc2), 0.0, tollerance)

    def testScaledTrasformation(self):
        """
        Data una nuovola di punti e una sua copia a cui viene applicata una matrice roto-scaling-traslazione
        la funzione dovra' restituire una matrice di rototraslazione che non applichi nessuno scaling
        alla prima nuvola.
        """
        teta = math.pi*2*random.random()
        sf = random.randrange(30,70) / 100
        x1 = math.cos(teta)
        x2 = math.sin(teta)
        x3 = random.random()*self.sf
        x4 = random.random()*self.sf
        T = X(P(x1, x2), P(-x2, x1), P(x3, x4)) * scale(sf)
        pc1 = [ P( (random.random()-0.5)*self.sf, (random.random()-0.5)*self.sf) for x in xrange(self.cloudSize)]
        pc2 = [ p*T for p in pc1 ]
        mr = points_matching(pc1, pc2)
        A = X(mr.A)
        pc1_bis = [ p*A for p in pc1 ]
        # La distanza tra i 2 punti originali deve essere la stessa tra i 2 punti trasformati.
        # In questo modo si e' sicuri che non ci sia stato nessuno scaling
        for (p1, p1bis) , (p2, p2bis) in pairwise( zip(pc1, pc1_bis) ):
            self.assertAlmostEquals(dist(p1,p2), dist(p1bis, p2bis), tollerance)

    def testWeightedPoints(self):
        """
        Data una nuvola di punti e una sua copia alla quale viene applicata una rototraslazione,
        vengono aggiunti dei punti differenti a peso nullo alle 2 nuvole.

        la funzione deve restituire esattamente la rototraslazione applicata alla seconda nuvola di punti
        """
        teta = math.pi*2*random.random()
        x1 = math.cos(teta)
        x2 = math.sin(teta)
        x3 = random.random()*self.sf
        x4 = random.random()*self.sf
        T = X(P(x1, x2), P(-x2, x1), P(x3, x4))

        pc1 = [ P( (random.random()-0.5)*self.sf, (random.random()-0.5)*self.sf) for x in xrange(self.cloudSize)]
        pc2 = [ p*T for p in pc1 ]

        rn = random.randint(10,100)
        rand_pc1 = [ P( (random.random()-0.5)*self.sf, (random.random()-0.5)*self.sf) for x in xrange(rn)]
        rand_pc2 = [ P( (random.random()-0.5)*self.sf, (random.random()-0.5)*self.sf) for x in xrange(rn)]

        weights = [1.0 for p in pc1] + [0.0 for x in xrange(rn)]

        mr = points_matching(pc1 + rand_pc1, pc2 + rand_pc2, weights)
        A = X(mr.A)
        self.assertAlmostEquals(A.a.x, T.a.x)
        self.assertAlmostEquals(A.a.y, T.a.y)
        self.assertAlmostEquals(A.b.x, T.b.x)
        self.assertAlmostEquals(A.b.y, T.b.y)
        self.assertAlmostEquals(A.c.x, T.c.x)
        self.assertAlmostEquals(A.c.y, T.c.y)

        self.assertAlmostEquals(self.calcPCErr([ p*A for p in pc1 ], pc2), 0.0, tollerance)
        self.assertNotAlmostEquals(self.calcPCErr([ p*A for p in pc1 + rand_pc1 ], pc2 + rand_pc2), 0.0)

class TestPathOverPath(unittest.TestCase):

    sf = 1000.0

    def testHalfCircleOverHalfCircle(self):
        """
        La funzione deve sovrapporre due semicerchi identici, dopo che uno di questi e'
        stato traslato e ruotato
        """
        O1 = P(0,0)
        O2 = P(100,100)
        c1 = Path( circle(O1, 100, minpts= 100, maxerr=100)[0:50] )
        c2 = Path( c1 ) * rot(random.random()*2*math.pi) * xlate(O2)
        mr = pathOverPath(c2, c1, 10)
        self.assertAlmostEquals(mr.sse/mr.n, 0.0)
        A = X(mr.A)
        for p1, p2 in zip(c1, c2): # controllo della distanza punto-path
            self.assertAlmostEquals(c1.project(p2*A).dist, 0, tollerance)

    def testQuarterCircleOverHalfCircle(self):
        """
        La funzione deve sovrapporre un quarto-di-cerchio a un semicerchio, dopo che
        il quarto-di-cerchio e' stato traslato e ruotato
        """
        O1 = P(0,0)
        O2 = P(100,100)
        c1 = Path( circle(O1, 100, minpts= 100, maxerr=100)[0:50] )
        c2 = Path( c1[0:25] ) * rot(random.random()*2*math.pi) * xlate(O2)
        mr = pathOverPath(c2, c1, 10)

        self.assertAlmostEquals(mr.sse/mr.n, 0.0)
        A = X(mr.A)
        for p2 in c2: # controllo della distanza punto-path
            self.assertAlmostEquals(c1.project(p2*A).dist, 0, tollerance)

    def testReverseQuarterCircleOverHalfCircle(self):
        """
        La funzione deve sovrapporre un quarto-di-cerchio a un semicerchio, dopo che
        il quarto-di-cerchio e' stato traslato e ruotato.
        L'ordine dei punti del quarto di cerchio viene invertito (senso antiorario).

        Si nota che l'errore di accostamento dei due path sale a causa di una non piu' perfetta
        corrispondenza nella scelta dei punti (dato che i path "cominciano" da due punti differenti)
        """
        O1 = P(0,0)
        O2 = P(100,100)
        c1 = Path( circle(O1, 100, minpts= 100, maxerr=100)[0:50] )
        tmp = c1[25:50]
        tmp.reverse()
        c2 = Path( tmp ) * rot(random.random()*2*math.pi) * xlate(O2)
        # Per passare questo test e' necessario alzare la risoluzione a 1mm
        mr = pathOverPath(c2, c1, 1)

        self.assertAlmostEquals(mr.sse/mr.n, 0.0)
        A = X(mr.A)
        for p2 in c2: # controllo della distanza punto-path
            # Per passare questo test e' necessario alzare la tolleranza
            self.assertAlmostEquals(c1.project(p2*A).dist,0, tollerance)

    def testHalfSquareOverSquare(self):
        """
        Sovrapposizione di un mezzo quadrato su un quadrato
        """
        sq = Path([P(0,0), P(10,0), P(17,0), P(80,0), P(100,0), P(100,100), P(0,100), P(0,0)])
        hsq = Path([P(100,100), P(0,100), P(0,0)]) * rot(random.random()*2*math.pi) * xlate(P(random.random()*self.sf, random.random()*self.sf))
        mr = pathOverPath(hsq, sq, 10)

        self.assertAlmostEquals(mr.sse/mr.n, 0.0)
        A = X(mr.A)
        for p1 in hsq: # controllo della distanza punto-path
            self.assertAlmostEquals(sq.project(p1*A).dist, 0, tollerance)

    def testReverseHalfSquareOverSquare(self):
        """
        Sovrapposizione di un mezzo quadrato su un quadrato.
        Viene invertito l'ordine dei punti del mezzo quadrato
        """
        sq = Path([P(0,0), P(10,0), P(17,0), P(80,0), P(100,0), P(100,100), P(0,100), P(0,0)])
        tmp = [P(100,100), P(0,100), P(0,0)]
        tmp.reverse()
        hsq = Path(tmp) * rot(random.random()*2*math.pi) * xlate(P(random.random()*self.sf, random.random()*self.sf))
        mr = pathOverPath(hsq, sq, 10)

        self.assertAlmostEquals(mr.sse/mr.n, 0.0)
        A = X(mr.A)
        for p1 in hsq: # controllo della distanza punto-path
            self.assertAlmostEquals(sq.project(p1*A).dist, 0, tollerance)

class TestVecpathOverPath(unittest.TestCase):

    sf = 1000

    def testTwoLinesOverSquare(self):
        """
        Dato un quadrato e 2 suoi lati, la funzione deve restituire la matrice di rototraslazione per
        sovrapporre i 2 lati al quadrato originale
        """
        square = Path([P(0,0), P(0, 100), P(100,100), P(100,0), P(0,0)])
        T = rot(random.random()*2*math.pi) * xlate(P(random.random()*self.sf, random.random()*self.sf))
        seg1 = Path([P(1,1), P(1,61)]) * T
        seg2 = Path([P(101,101), P(101,61)]) * T
        mr = vecpathOverPath([seg1, seg2], square, 10)
        A = X(mr.A)
        self.assertAlmostEquals(mr.sse/mr.n, 0.0)
        for p1 in list(seg1)+list(seg2): # controllo della distanza punto-path
            self.assertAlmostEquals(square.project(p1*A).dist, 0, tollerance)

    def testFourSquareOverSquare(self):
        """
        Dato un quadrato e 4 suoi lati accorciati, la funzione deve restituire
        la matrice di rototraslazione per sovrapporre i 2 lati al quadrato originale
        """
        square = Path([P(0,0), P(0, 100), P(100,100), P(100,0), P(0,0)])
        T = rot(random.random()*2*math.pi) * xlate(P(random.random()*self.sf, random.random()*self.sf))
        seg1 = Path([P(0,0), P(0,60)]) * T
        seg2 = Path([P(100,100), P(100,60)]) * T
        seg3 = Path([P(30,100), P(70,100)]) * T
        seg4 = Path([P(25,0), P(83,0)]) * T
        mr = vecpathOverPath([seg1, seg2, seg3, seg4], square, 10)
        A = X(mr.A)
        self.assertAlmostEquals(mr.sse/mr.n, 0.0)
        for p1 in list(seg1)+list(seg2)+list(seg3)+list(seg4): # controllo della distanza punto-path
            self.assertAlmostEquals(square.project(p1*A).dist, 0, tollerance)

    def testFourWeightedLinesOverSquare(self):
        """
        Dati 4 segmenti non tutti sovrapponibili contemporaneamente ad un quadrato a causa di uno solo segmento,
        se si assegna un raggio maggiore  al segmento "non sovrapponibile", l'errore diminuisce rispetto
        al caso in cui, allo stesso segmento, gli si assegni un peso pari a quello degli altri segmenti
        """
        square = Path([P(0,0), P(0, 100), P(100,100), P(100,0), P(0,0)])
        sqw = [1.0, 1.0, 1.0, 1.0, 1.0]

        T = rot(random.random()*2*math.pi) * xlate(P(random.random()*self.sf, random.random()*self.sf))

        seg1 = Path([P(0,0), P(0,60)]) * T
        w1 = [1.0, 1.0]

        seg2 = Path([P(110,100), P(110,60)]) * T
        w2 = [100.0, 100.0]

        seg3 = Path([P(0,100), P(70,100)]) * T
        w3 = [1.0, 1.0]

        seg4 = Path([P(0,0), P(83,0)]) * T
        w4 = [1.0, 1.0]

        mr1 = vecpathOverPath([seg1, seg2, seg3, seg4], square, 10)
        e1 = float(mr1.sse)
        # seg2 non puo' essere sovrapposto a square insieme agli altri segmenti.
        # Se aumentiamo il raggio di precisione di seg2 , gli
        # altri si dovrebbero allineare meglio, facendo diminuire l'errore
        mr2 = vecpathOverPath([seg1, seg2, seg3, seg4], [w1, w2, w3, w4], square, sqw, 10)
        e2 = float(mr2.sse)
        self.assertTrue(e2 <= e1)



class TestVecpathOverVecpath(unittest.TestCase):

    sf = 1000

    def testTwoLinesOverSquare(self):
        """
        Ripetizione del test del caso vecpathOverPath
        """
        square = Path([P(0,0), P(0, 100), P(100,100), P(100,0), P(0,0)])
        T = rot(random.random()*2*math.pi) * xlate(P(random.random()*self.sf, random.random()*self.sf))
        seg1 = Path([P(1,1), P(1,61)]) * T
        seg2 = Path([P(101,101), P(101,61)]) * T
        mr = vecpathOverVecpath([seg1, seg2], [square], 1, 10, 1)[0]
        A = X(mr.A)
        self.assertAlmostEquals(mr.sse/mr.n, 0.0)
        for p1 in list(seg1)+list(seg2): # controllo della distanza punto-path
            self.assertAlmostEquals(square.project(p1*A).dist, 0, tollerance)

    def testTwoLinesOverTwoLines(self):
        """
        La funzione deve tornare la matrice di rototraslazione per queste quatro linee sovrapponibili
        due a due
        """
        T = rot(random.random()*2*math.pi) * xlate(P(random.random()*self.sf, random.random()*self.sf))
        W = rot(random.random()*2*math.pi) * xlate(P(random.random()*self.sf, random.random()*self.sf))

        s1T = Path([P(1,1), P(1,61)]) * T
        s2T = Path([P(101,101), P(101,61)]) * T

        s1W = Path([P(1,1), P(1,61)]) * W
        s2W = Path([P(101,101), P(101,61)]) * W

        #Per avere una sovrapposizione piu' esatta possibile, diminuisco il passo a 1mm
        mr = vecpathOverVecpath([s1T, s2T], [s1W, s2W], 1, 10, 1)[0]
        self.assertNotEqual(mr.n, -1) # Risultato invalido
        A = X(mr.A)
        self.assertAlmostEquals(mr.sse/mr.n, 0.0)
        for p1 in s1T:
            self.assertAlmostEquals(s1W.project(p1*A).dist, 0, tollerance)
        for p2 in s2T:
            self.assertAlmostEquals(s2W.project(p2*A).dist, 0, tollerance)

    def testFourSquareOverSquare(self):
        """
        Ripetizione del test per il caso vecpathOverPath
        """
        square = Path([P(0,0), P(0, 100), P(100,100), P(100,0), P(0,0)])
        T = rot(random.random()*2*math.pi) * xlate(P(random.random()*self.sf, random.random()*self.sf))
        seg1 = Path([P(0,0), P(0,60)]) * T
        seg2 = Path([P(100,100), P(100,60)]) * T
        seg3 = Path([P(30,100), P(70,100)]) * T
        seg4 = Path([P(25,0), P(83,0)]) * T
        mr = vecpathOverVecpath([seg1, seg2, seg3, seg4], [square], 1, 10, 1)[0]
        A = X(mr.A)
        self.assertAlmostEquals(mr.sse/mr.n, 0.0)
        for p1 in list(seg1)+list(seg2)+list(seg3)+list(seg4): # controllo della distanza punto-path
            self.assertAlmostEquals(square.project(p1*A).dist, 0, tollerance)

if __name__ == "__main__":
    # r = random.randint(0, 2**32)
    # print "To reproduce this test use random seed:", r
    # random.seed(r)
    random.seed(100)
    unittest.main()
