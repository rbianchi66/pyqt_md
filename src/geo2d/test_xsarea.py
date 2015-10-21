#!/usr/bin/env python
#-*- coding: utf-8 -*-
import unittest
from geo2d import Path, P, X, xsarea, xlate

def quadrato(p1, l):
    return Path([p1, p1 + P(0, l), p1 + P(l, l), p1 + P(l, 0), p1])

class TestXSArea(unittest.TestCase):
    def testAreeNonConnesse(self):
        """Due path si intersecano solo in un punto"""
        q1 = Path([P(0, 0), P(50, 50), P(100, 0), P(0,0)])
        q2 = Path([P(0, 0), P(-50, -50), P(-100, 0), P(0,0)])
        a = xsarea(q1, X(), q2, X())
        self.assertAlmostEqual(a, 0)

    def testQuadratoSovrapposto(self):
        """Quadrato di area 10000 sovrapposto a se stesso per 1/4."""
        q1 = quadrato(P(0, 0), 100)
        a = xsarea(q1, X(), q1, xlate(P(50, 50)))
        self.assertAlmostEqual(a, 2500)

    def testQuadratiSovrapposti(self):
        """Quadrati di area 10000 sovrapposti per 1/4."""
        q1 = quadrato(P( 0,  0), 100)
        q2 = quadrato(P(50, 50), 100)

        a = xsarea(q1, X(), q2, X())
        self.assertAlmostEqual(a, 2500)

    def testCornice(self):
        """Quadrato interno ad una cornice"""
        c = [quadrato(P( 0,  0), 100), quadrato(P(10, 10),  80)]
        q = [quadrato(P( 20, 20) ,20)]
        a = xsarea(c, X(), q, X())
        self.assertAlmostEqual(a, 0)

    def testCommutativo(self):
        """Verifica che il risulta non cambi invertendo i due path"""
        p1 = [Path([P(0,0), P(20,20), P(20,90), P(150, 90), P(200, 0), P(0,0)])]
        p2 = [Path([P(0,0), P(10,20), P(20,50), P(120, 70), P(150, 0), P(0,0)])]
        x1 = xlate(P(0, 10))
        x2 = xlate(P(20, 30))
        a1 = xsarea(p1, x1, p2, x2)
        a2 = xsarea(p2, x2, p1, x1)
        self.assertAlmostEqual(a1, a2)

if __name__ == "__main__":
    unittest.main()

