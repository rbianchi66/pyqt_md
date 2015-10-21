#!/usr/bin/env python
#-*- coding: utf-8 -*-
import math
from math import sqrt

import unittest
import geo2d

P = geo2d.P

class TestOffset(unittest.TestCase):

    def testMaxiDistanza(self):
        """Offset esteso rispetto alla lunghezza della linea (bug #8803)"""

        # Coordinate ad hoc per riprodurre il crash del bug #8803
        geometry = [geo2d.P(455.18190789527529,43.749739818848383),
                    geo2d.P(454.24637540679549,43.752226361217041),
                    geo2d.P(452.7734002544687,43.801475955309996),
                    geo2d.P(450.58853730854327,43.924461465483816)]

        paths = geo2d.offset(geometry, [170.0, -170.0])

        # Verifica che comunque riesca a produrre dei path semanticamente validi
        for path in paths:
            for pp in path:
                d = geo2d.Path(geometry).project(pp).d
                self.assertTrue(d - 170.0 < 0.1)

    def _verifyOffset(self, exp, pts, d, tipo="SPIGOLO_VIVO"):
        """
        assert che geo2d.offset(pts, d, tipo) somigli molto al path exp.
        """
        off = geo2d.offset(pts, d)

        for o in off:
            self.assert_(isinstance(o, geo2d.Path))
        off = off[0]
        if tipo == "SPIGOLO_VIVO":
            for pp in off:
                self.assertAlmostEqual(geo2d.Path(exp).project(pp).d, 0.0)
        else:
            assert tipo == "SMUSSATA"
            for pp in off:
                # Con la smussata alcuni punti necessariamente vengono
                # più lontani dall'offset atteso, basta che non siano
                # più lontani di d (l'offset specificato)
                self.assertTrue(geo2d.Path(exp).project(pp).d <= d)

    def testOffsetPiccolaDistanza1(self):
        """
        Controlla che l'offset per piccole distanze torni una lista di
        Path e che vengano filtrati i punti doppi.
        """
        d = 0.4
        pts =  [P(20.0, 40.0),
                P(60.0, 40.0),
                P(60.0, 40.0),  # doppio
                P(60.0, 40.0),  # triplo
                P(60.0, 10.0),
                P(20.0, 10.0),
                P(20.0, 40.0),
                P(20.0, 40.0)]  # doppio

        exp1 = [P(19.6, 40.4), P(60.4, 40.4), P(60.4, 9.6), P(19.6, 9.6), P(19.6, 40.4)]
        exp2 = [P(20.4, 39.6), P(59.6, 39.6), P(59.6, 10.4), P(20.4, 10.4), P(20.4, 39.6)]

        self._verifyOffset(exp1, pts, [0.4])
        self._verifyOffset(exp2, pts, [-0.4])

    def testOffsetPuntiAllineatiGrandeDistanza(self):

        pts = [P(0.0, 0.0), P(0.0, 5.0), P(0.0, 3.0)]

        exp1 = [P(-2.0, 0.0), P(-2.0, 4.8), # primo lato dell'offset
                P(0.0, 7.0),                # punto ricavato per unione
                P(2.0, 4.8), P(2.0, 3.0)]   # secondo lato dell'offset
        self._verifyOffset(exp1, pts, [2.0, -2.0])

    def testOffsetPuntiAllineatiPiccolaDistanza(self):
        """
        geo2d.offset su path con punti allineati, distanza piccola
        """

        pts = [P(0.0, 0.0), P(0.0, 10.0), P(0.0, 5.0)]
        exp1 = [P(-0.3, 0.0), P(0.0, 10.3), P(0.3, 5.0)]
        self._verifyOffset(exp1, pts, [0.3])

        exp2 = [P(0.3, 0.0), P(0.0, 9.7), P(-0.3, 5.0)]
        self._verifyOffset(exp2, pts, [-0.3])

    def testOffsetPuntiCoincidenti(self):
        """
        Bug #3317: offset su path lungo zero torna il path originale
        """
        pts = [P(472.57999999999998,156.506)] * 5

        exp = [P(472.57999999999998,156.506),
               P(472.57999999999998,156.506)]

        self._verifyOffset(exp, pts, [0.2])

    def testOffsetDistanza0(self):
        """
        Bug #3317: offset di un certo path a distanza 0
        """
        pts = [P(230.0028913896731,-1836.4128265784827),
               P(168.1394056335555,-1721.1678204720313),
               P(169.43049571855363,-1709.8479124079238),
               P(157.54131465514135,-1814.088846606988)]

        self._verifyOffset(pts, pts, [0])

    def testOffsetPathAperto(self):
        """
        Il calcolo dell'offset aperto tiene conto del verso della curva.
        """
        # Prova sia la versione C che quella Python, usata solo per
        # offset piccoli
        for d in (0.1, 1.0):
            pts = [P(0, 0), P(100, 0), P(100, 100), P(0, 100)]
            exp = [P(0.0, 100 - d), P(100 - d, 100 - d), P(100 - d, d), P(0.0, d)]
            self._verifyOffset(exp, pts, [d])

            pts.reverse()
            exp = [P(0.0, 100 + d), P(100 + d, 100 + d), P(100 + d, -d), P(0.0, -d)]
            self._verifyOffset(exp, pts, [d])

    def testOffsetPathChiuso(self):
        """
        L'offset positivo di una chiusa è sempre all'esterno (e viceversa).
        """
        for d in (0.1, 1.0):
            pts = [P(0, 0), P(100, 0), P(100, 100), P(0, 100), P(0, 0)]
            esterno = [P(100 + d, 100 + d), P(100 + d, -d), P(-d, -d), P(-d, 100 + d)]
            interno = [P(100 - d, 100 - d), P(100 - d, d), P(d, d), P(d, 100 - d)]

            self._verifyOffset(esterno, pts, [d])
            self._verifyOffset(interno, pts, [-d])

            pts.reverse()
            self._verifyOffset(esterno, pts, [d])
            self._verifyOffset(interno, pts, [-d])

    def testOffsetPathChiusoSmussato(self):
        """
        Il calcolo dell'offset chiuso smussato tiene conto del verso della curva.
        """
        for d in (0.1, 1.0):
            pts = [P(0, 0), P(100, 0), P(100, 100), P(0, 100), P(0, 0)]
            exp = [P(100 - d, 100 - d), P(100 - d, d), P(d, d), P(d, 100 - d)]

            self._verifyOffset(exp, pts, [d], tipo="SMUSSATA")

            pts.reverse()
            exp = [P(100 + d, 100 + d), P(-d, 100 + d), P(-d, -d), P(100 + d, -d)]

            self._verifyOffset(exp, pts, [d], tipo="SMUSSATA")

    def testDegenere1(self):
        """Controlla l'offset di una spline degenere (bug #664)."""
        d = 6.8243151335627594
        pts =  [P(135.795, 104.523),
                P(135.795, 104.523),
                P(139.930, 104.235),
                P(144.050, 103.887),
                P(148.154, 103.475),
                P(151.222, 103.120),
                P(155.295, 102.580),
                P(158.337, 102.123),
                P(161.367, 101.618),
                P(164.538, 101.035),
                P(167.386, 100.472)]

        # Testa sia la parallela a spigolo vivo che quella smussata
        for tp in ["SPIGOLO_VIVO", "SMUSSATA"]:
            geo2d.offset(pts, [d, -d], tp)

    def testSegmento(self):
        """
        Dato un segmento controlla che la parallela costruita sia corretta.
        """
        s = [P(0,0), P(100,0)]
        path = geo2d.Path(s)
        d = 5
        for tp in ["SMUSSATA", "SPIGOLO_VIVO"]:
            par = geo2d.offset(s, [d, -d], tp)
            precision = 1
            for l in par:
                for pt in l:
                     self.assertAlmostEqual(path.project(pt).dist,
                                            d, precision)

    def testSmusso(self):
        """
        Controlla che la parallela smussata ad una "L" sia corretta
        """
        elle = [P(0,0), P(100,0), P(100,100)]
        d = 5
        epsilon = 0.1
        par = geo2d.offset(elle, [d, -d], "SMUSSATA")
        for l in par:
            for pt in l:
                # 4 segmenti
                if (geo2d.dist_p_seg(pt, elle[0]+P(0,d), elle[1]+P(-d,d)) <
                        epsilon):
                    continue
                if (geo2d.dist_p_seg(pt, elle[0]+P(0,-d), elle[1]+P(0,-d)) <
                        epsilon):
                    continue
                if (geo2d.dist_p_seg(pt, elle[1]+P(-d,d), elle[2]+P(-d,0)) <
                        epsilon):
                    continue
                if (geo2d.dist_p_seg(pt, elle[1]+P(d,0), elle[2]+P(d,0)) <
                        epsilon):
                    continue
                # smusso
                self.assert_(abs(abs(pt-elle[1]) - d) < epsilon and
                        elle[1].x - epsilon < pt.x < elle[1].x + d + epsilon and
                        elle[1].y - epsilon - d < pt.y < elle[1].y + epsilon)

    def testOrario(self):
        """
        Controlla che sulle linee chiuse, i punti calcolati non cambino con
        l'ordinamento orario o antiorario dei punti della spezzata di
        partenza.
        """
        d = 5
        epsilon = 0.1
        pts1 = [P(0,0), P(0,100), P(50,50), P(0,0)]
        # Costruisce una spezzata uguale, ma con senso di percorrenza inverso
        pts2 = list(pts1)
        pts2.reverse()

        tp = "SPIGOLO_VIVO"
        for dd in [[d], [-d]]:
            par1 = geo2d.offset(pts1, dd, tp)
            par2 = geo2d.offset(pts2, dd, tp)
            # Controlla che area, perimetro e bounding box delle due figure
            # coincidano
            perim = [0]*2
            area = [0]*2
            min_x = [999999]*2
            min_y = [999999]*2
            max_x = [-999999]*2
            max_y = [-999999]*2
            for i, par in enumerate([par1, par2]):
                for j, pt in enumerate(par[0]):
                    perim[i] += abs(par[0][j] - par[0][j - 1])
                    area[i] += par[0][j]^par[0][j - 1] / 2
                    if pt.x < min_x[i]:
                        min_x[i] = pt.x
                    if pt.y < min_y[i]:
                        min_y[i] = pt.y
                    if pt.x > max_x[i]:
                        max_x[i] = pt.x
                    if pt.y > max_y[i]:
                        max_y[i] = pt.y
                area[i] = abs(area[i])

            for l in [perim, area, min_x, min_y, max_x, max_y]:
                self.assert_(abs(l[0] - l[1]) < epsilon)

    # Disabilitato in attesa del fix per il Bug 1414
    del testOrario

    def testDist(self):
        """
        Dato un quadrato di lato l e la parallela a distanza dist verifica
        che i punti della parallela siano distanti da dist a dist*sqrt(2) dal
        quadrato di partenza.
        """
        l = 100
        dist = 3
        epsilon = 0.1
        pts = [P(0,0), P(0,l), P(l,l), P(l,0), P(0,0)]
        path = geo2d.Path(pts)

        n_pts = len(pts)

        d = [dist, -dist]

        for tp in ["SPIGOLO_VIVO", "SMUSSATA"]:
            par = geo2d.offset(pts, d, tp)
            for l in par:
                for pt in l:
                    dmin = path.project(pt).dist
                    self.assert_(dist*sqrt(2) + epsilon > dmin > dist - epsilon)

    def testMultiPar(self):
        """
        Verifica che le parallele ad una figura a forma di otto
        siano due interne ed una esterna.
        """
        otto = [P(0,0), P(100, 0), P(0,100), P(100,100), P(0,0)]
        dist = [3, -3]
        epsilon = 0.1

        for tp in ["SPIGOLO_VIVO", "SMUSSATA"]:
            esterne = 0
            interne = 0
            par = geo2d.offset(otto, dist, tp)
            # Deve trovare 3 parallele e di queste, 2 interne all'otto e
            # una esterna
            for l in par:
                for pt in l:
                    # Qui controlla se tutti i punti sono interni ad una
                    # bounding box dell'otto
                    if not (0 <= pt.x <= 100 and 0 <= pt.y <= 100):
                        esterne = esterne + 1
                        break
                else:
                    interne = interne + 1
            self.assertEqual(esterne, 1)
            self.assertEqual(interne, 2)

    def testDegenere(self):
        """Controlla gli offset di tratti puntiformi."""
        path = geo2d.Path([P(0,0)])

        # L'offset completo si può fare (un punto -> un cerchio), ma la
        # parallela di un solo lato invece non ha senso.
        geo2d.offset(path, [10])
        self.assertRaises(ValueError, lambda: geo2d.para(path, 10, False))
        self.assertRaises(ValueError, lambda: geo2d.para(path, 10, True))
        self.assertRaises(ValueError, lambda: geo2d.para_smussa(path, 10, False))
        self.assertRaises(ValueError, lambda: geo2d.para_smussa(path, 10, True))

    def testInterpola(self):
        """interpola aggiunge i punti per distanza massima 3.0 (bug #3071)."""
        path = geo2d.Path([P(0, 0), P(5, 0), P(10, 0)])
        new_path = geo2d.interpola(path, 3.0)
        self.assertAlmostEqual(geo2d.pathPathDist(new_path, path), 0)

        # La soluzione ottimale per il path prevede 5 punti, quindi se ne sta
        # mettendo di più o di meno sta sbagliando.
        self.assert_(len(new_path) == 5)

        for i_pt, pt in enumerate(new_path):
            if i_pt == 0:
                continue
            self.assert_(geo2d.dist(pt, new_path[i_pt - 1]) < 3.0)


class Geo2dTestCase(unittest.TestCase):
    def assertGeo2dSame(self, p1, p2, prec=1e-3):
        self.assertTrue(geo2d.same(p1, p2, prec), "%r != %r" % (p1, p2))
    def assertFEqual(self, p1, p2, precision=1e-6):
        self.assertTrue((abs(p1-p2) < precision), "%r != %r" % (p1, p2))

class TestParallelPath(Geo2dTestCase):
    def testParallelPath(self):
        def snap_grid(p):
            return P(round(p.x), round(p.y))
        L = geo2d.Path([P(0,0), P(2,2), P(4,0)])
        parallel_path = geo2d.getParallelPath(L, sqrt(2))
        # Dal verso del path la direzione dell'offset è quella superiore
        for p1, p2 in zip(parallel_path, geo2d.Path([P(-1, 1), P(2, 4), P(5, 1)])):
            self.assertGeo2dSame(p1, p2)
        # Controlla che la trasformazione inversa torni sul path originario
        back_from_parallel = geo2d.getParallelPath(parallel_path, -sqrt(2))
        for p1, p2 in zip(L, back_from_parallel):
            self.assertGeo2dSame(p1, p2)


if __name__ == "__main__":
    unittest.main()
