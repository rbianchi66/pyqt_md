#!/usr/bin/env python
#-*- coding: utf-8 -*-
from __future__ import division
import unittest
from geo2d import Path, P
from geo2d import bump
from math import sin, cos

Bumper, PosRot, Shape = bump.Bumper, bump.PosRot, bump.Shape

def shape(r1, r2, nvertices, xl = P(0, 0)):
    res = Path()
    for i in range(nvertices):
        a = i * 2 * PI / nvertices;
        p1 = r1 * P(sin(a), cos(a)) + xl
        a += PI / nvertices
        p2 = r2 * P(sin(a), cos(a)) + xl
        res.append(p1)
        res.append(p2)
    res.append(res[0])
    return res

def quadrato(p, l):
    o = p - P(l / 2, l / 2)
    return Path([o, o + P(0, l), o + P(l, l), o + P(l, 0), o])

class TestBump(unittest.TestCase):
    def testShapeNonSovrapposte(self):
        """ Shape che non si toccano: il bump non deve fare nulla """
        c = [quadrato(P(0, 0), 100), quadrato(P(0, 0), 80)]
        s1 = Shape(c, c, 0)
        q = [quadrato(P(0, 0), 60)]
        s2 = Shape(q, q, 0)
        bumper = Bumper()
        pr, over = bumper.bump(s1, PosRot(), s2, PosRot())

        self.assertAlmostEqual(over, 0)
        self.assertAlmostEqual(abs(pr.pos - P(0, 0)), 0)
        self.assertAlmostEqual(pr.rot, 0)

    def testQuadratiSovrapposti(self):
        """ Due quadrati allineati che si sovrappongo di poco sul lato destro:
            devono risultare adiacenti sul lato destro """
        q = [quadrato(P(0, 0), 100)]
        s = Shape(q, q, 0)
        bumper = Bumper()
        pr, over = bumper.bump(s, PosRot(95, 0, 0), s, PosRot(0, 0, 0))

        self.assertAlmostEqual(over, 0)
        self.assert_(abs(pr.pos - P(100, 0)) < 2.001)
        self.assertAlmostEqual(pr.rot, 0, 3)

    def testBumpENettoSeparati(self):
        """ In questo test l'area di bump di una shape non coincide con il
            netto """
        q1 = [quadrato(P(0, 0),  50)]
        q2 = [quadrato(P(0, 0), 100)]
        s1 = Shape(q1, q2, 0)
        s2 = Shape(q2, q2, 0)
        bumper = Bumper()
        pr, over = bumper.bump(s1, PosRot(95, 0, 0), s2, PosRot(0, 0, 0))

        self.assertAlmostEqual(over, 0)
        self.assert_(abs(pr.pos - P(100, 0)) < 2.001)
        self.assertAlmostEqual(pr.rot, 0, 3)

    def testCornice(self):
        """ In questo test un quadrato deve essere incastrato in una cornice """
        q = [quadrato(P(0, 0), 79)]
        s1 = Shape(q, q, 0)
        c = [quadrato(P(0, 0), 100), quadrato(P(0, 0), 80)]
        s2 = Shape(c, c, 0)
        bumper = Bumper()
        pr, over = bumper.bump(s1, PosRot(2, 2, 0.1), s2, PosRot(0, 0, 0))

        self.assertAlmostEqual(over, 0)
        self.assert_(abs(pr.pos - P(0, 0)) < 2.001)
        self.assert_(abs(pr.rot) < 0.025)

if __name__ == "__main__":
    unittest.main()

