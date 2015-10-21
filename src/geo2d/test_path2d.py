#!/usr/bin/env python
#-*- coding: utf-8 -*-
from __future__ import division
import math
import geo2d
import unittest

P = geo2d.P

class TestPath2dVsPath(unittest.TestCase):
    def testCostruttori(self):
        self.assertRaises(TypeError, lambda: geo2d.Path(213))
        p1 = geo2d.Path([geo2d.P(0,0), geo2d.P(1,0)])
        p2 = geo2d.Path((geo2d.P(0,0), geo2d.P(1,0)))
        p3 = geo2d.Path(iter([geo2d.P(0,0), geo2d.P(1,0)]))
        self.assertRaises(TypeError, lambda: geo2d.Path(iter([geo2d.P(0,0), 1])))
        self.assertRaises(TypeError, lambda: geo2d.Path([None]))

    def testEvaluation(self):
        path = geo2d.Path([geo2d.P(0,0), geo2d.P(0,0), geo2d.P(0,0),
                           geo2d.P(1,0),
                           geo2d.P(2,0),
                           geo2d.P(3,0), geo2d.P(3,0), geo2d.P(3,0),
                           geo2d.P(4,0),
                           geo2d.P(5,0),
                           geo2d.P(6,0), geo2d.P(6,0), geo2d.P(6,0)])

        for i in range(-20, 80):
            i /= 10
            self.assertEqual(path(i), geo2d.P(i,0))

    def testGetItem(self):
        p1 = geo2d.Path([geo2d.P(0,0), geo2d.P(1,0)])
        self.assertEqual(p1[0], geo2d.P(0,0))
        self.assertEqual(p1[1], geo2d.P(1,0))
        self.assertEqual(p1[-1], geo2d.P(1,0))
        self.assertEqual(p1[-2], geo2d.P(0,0))
        self.assertRaises(IndexError, lambda: p1[5])
        self.assertRaises(TypeError, lambda: p1["ciao"])

    def testSlice(self):
        p1 = geo2d.Path([geo2d.P(0,0), geo2d.P(1,0)])
        self.assertEqual(p1[0:2], list(p1))

    def testLen(self):
        p1 = geo2d.Path([geo2d.P(0,0), geo2d.P(1,0)])
        p2 = geo2d.Path([geo2d.P(0,0)])
        self.assertEqual(len(p1), 2)
        self.assertEqual(len(p2), 1)

    def testIterable(self):
        p1 = geo2d.Path([geo2d.P(0,0), geo2d.P(1,0)])
        self.assertEqual(list(p1), [geo2d.P(0,0), geo2d.P(1,0)])

    def testRepr(self):
        p1 = geo2d.Path([geo2d.P(0,0), geo2d.P(1,0)])
        self.assertEqual(list(eval(repr(p1), geo2d.__dict__)), list(p1))

    def testConcat(self):
        p1 = geo2d.Path([geo2d.P(0,0), geo2d.P(1,0)])
        p2 = geo2d.Path([geo2d.P(2,0), geo2d.P(3,0)])

        p = p1 + p2
        self.assertEqual(len(p), 4)
        self.assertEqual(p[:2], list(p1))
        self.assertEqual(p[2:], list(p2))

    def testMatMul(self):
        p1 = geo2d.Path([geo2d.P(0,0), geo2d.P(1,0)])
        X = geo2d.xlate(P(100,100))

        p2 = p1 * X
        self.assertEqual(list(p1), [geo2d.P(0,0), geo2d.P(1,0)])
        self.assertEqual(list(p2), [geo2d.P(100,100), geo2d.P(101,100)])

    def testProjectResult(self):
        self.assert_(issubclass(geo2d.ProjectResult, tuple))
        pr = geo2d.ProjectResult((2,3,4,5))
        self.assertEqual(pr.p, 2)
        self.assertEqual(pr.pt, 2)
        self.assertEqual(pr.point, 2)
        self.assertEqual(pr.d, 3)
        self.assertEqual(pr.dist, 3)
        self.assertEqual(pr.distance, 3)
        self.assertEqual(pr.t, 4)
        self.assertEqual(pr.len, 4)
        self.assertEqual(pr.length, 4)
        self.assertEqual(pr.s, 5)
        self.assertEqual(pr.seg, 5)
        self.assertEqual(pr.segment, 5)

    def testProject(self):
        p1 = geo2d.Path([geo2d.P(0,0), geo2d.P(1,0), geo2d.P(2,0)])

        res1 = p1.project(geo2d.P(0.5, 8))
        res2 = p1.project(geo2d.P(1.5, -8))

        self.assertEqual(res1.pt, geo2d.P(0.5, 0))
        self.assertEqual(res1.dist, 8)
        self.assertEqual(res1.t, 0.5)
        self.assertEqual(res1.seg, 0)

        self.assertEqual(res2.pt, geo2d.P(1.5, 0))
        self.assertEqual(res2.dist, 8)
        self.assertEqual(res2.t, 1.5)
        self.assertEqual(res2.seg, 1)

    def testProjectSuPathPuntiforme(self):
        """project() su path puntiforme (parte del bug #3118)."""
        p1 = geo2d.Path([geo2d.P(500, 500)])

        res1 = p1.project(geo2d.P(600, 500))

        self.assertEqual(res1.pt, geo2d.P(500, 500))
        self.assertEqual(res1.dist, 100)
        self.assertEqual(res1.t, 0)
        self.assertEqual(res1.seg, 0)

    def testSegment(self):
        p1 = geo2d.Path([geo2d.P(0,0), geo2d.P(1,0), geo2d.P(2,0)])
        p1 = p1.segment(0.5, 1.5)
        self.assertEqual(p1.len(), 1.0)
        self.assertEqual(list(p1), [geo2d.P(0.5,0), geo2d.P(1.0,0), geo2d.P(1.5,0)])

    def testProjectResultGc(self):
        # Controlla che ProjectResult supporti il garbage collector
        # ciclico. Questo codice inoltre riproduceva un crash che è
        # costato molto sangue...
        import gc
        import weakref

        for i in range(3):
            gc.collect()

        class A(object):
            pass
        a = A()
        t = geo2d.ProjectResult((0,a))
        a.t = t
        wr = weakref.ref(a)
        del a
        del t

        for i in range(3):
            gc.collect()

        self.assert_(wr() is None)

    def testReversed(self):
        p1 = geo2d.Path([geo2d.P(0,0), geo2d.P(1,1), geo2d.P(2,2), geo2d.P(3,3)])
        p2 = p1.reversed()
        self.assertEqual(list(p1), list(reversed(p2)))

    def testString(self):
        p1 = geo2d.Path([geo2d.P(i,i) for i in range(100)])
        s = p1.tostring()
        self.assertEqual(list(p1), list(geo2d.Path.fromstring(s)))

        p1 = geo2d.Path(geo2d.circle(geo2d.P(15,34), 89))
        s = p1.tostring()
        self.assertEqual(list(p1), list(geo2d.Path.fromstring(s)))

    def testBoundingBox(self):
        p1 = geo2d.Path([geo2d.P(0,0), geo2d.P(1,1), geo2d.P(2,2), geo2d.P(3,3)])
        tl, br = p1.boundingBox()

        self.assertEqual(tl, geo2d.P(0, 0))
        self.assertEqual(br, geo2d.P(3, 3))

    def testProlungamentoConPuntiVicini(self):
        # Cerco un punto (s) esterno alla linea

        # K                      M  N           s
        # +----------------------+--+           +
        for i in xrange(30):
            path = geo2d.Path([geo2d.P(0, 0), geo2d.P(10, 0), geo2d.P(10 + (1.0 / 10 ** i), 0)])
            self.assertEqual(path(10), geo2d.P(10, 0))
            self.assertEqual(path(11), geo2d.P(11, 0))
            self.assertEqual(path( 1), geo2d.P( 1, 0))
            self.assertEqual(path(-5), geo2d.P(-5, 0))

        # s           M  N                      K
        # +           +--+----------------------+
        for i in xrange(30):
            path = geo2d.Path([geo2d.P(1, 0), geo2d.P(1 + (1.0 / 10 ** i), 0), geo2d.P(10, 0)])
            self.assertEqual(path(  0), geo2d.P( 1, 0))
            self.assertEqual(path(  1), geo2d.P( 2, 0))
            self.assertEqual(path( -5), geo2d.P(-4, 0))
            self.assertEqual(path(-10), geo2d.P(-9, 0))

if __name__ == "__main__":
    unittest.main()
