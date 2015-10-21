#!/usr/bin/env python
#-*- coding: utf-8 -*-

import math
import unittest

import geo2d

def close(a,b):
    return abs(a-b) < 1E-10

class TestP2d(unittest.TestCase):
    def testGetter(self):
        p = geo2d.P(1.0, 2.0)
        self.assertEqual(p.x, 1.0)
        self.assertEqual(p.y, 2.0)

        self.assertEqual(p[0], 1.0)
        self.assertEqual(p[1], 2.0)

    def testRepr(self):
        P = geo2d.P
        p = P(0.44444444, 0.8888888888)
        self.assertEqual(p, eval(repr(p)))

class Geo2dTestCase(unittest.TestCase):
    def assertGeo2dSame(self, p1, p2, prec=1e-3):
        self.assertTrue(geo2d.same(p1, p2, prec), "%r != %r" % (p1, p2))

class TestFunctions(Geo2dTestCase):
    def setUp(self):
        self.a = geo2d.P(10,10)
        self.b = geo2d.P(10,20)
        self.c = geo2d.P(20,20)
        self.d = geo2d.P(20,10)

    def testLen(self):
        self.assert_(geo2d.len2(geo2d.P(0,0)) == 0)
        self.assert_(geo2d.len2(geo2d.P(-1,0)) == 1)
        self.assert_(geo2d.len2(geo2d.P(0,2)) == 4)
        self.assert_(geo2d.len2(geo2d.P(3,4)) == 25)
        self.assert_(abs(geo2d.P(0,0)) == 0)
        self.assert_(abs(geo2d.P(-1,0)) == 1)
        self.assert_(abs(geo2d.P(0,2)) == 2)
        self.assert_(abs(geo2d.P(3,4)) == 5)

    def testDist(self):
        self.assert_(geo2d.dist2(self.a,self.b) == 100)
        self.assert_(geo2d.dist2(self.a,self.c) == 200)
        self.assert_(geo2d.dist2(self.a,self.d) == 100)
        self.assert_(geo2d.dist2(self.d,self.a) == 100)
        self.assert_(geo2d.dist2(self.a,self.a) == 0)
        self.assert_(geo2d.dist(self.a,self.b) == 10)
        self.assert_(geo2d.dist(self.a,self.c) == math.sqrt(2)*10)
        self.assert_(geo2d.dist(self.a,self.d) == 10)
        self.assert_(geo2d.dist(self.d,self.a) == 10)
        self.assert_(geo2d.dist(self.a,self.a) == 0)

    def testDir(self):
        self.assert_(close(geo2d.dir((self.b - self.a)/2),
                           geo2d.dir(self.c - self.d )))
        self.assertRaises(ZeroDivisionError,
                          geo2d.dir, geo2d.P(0,0))

    def testProject(self):
        a = geo2d.P(-1,0)
        b = geo2d.P(1,0)
        p0 = geo2d.P(0,2)
        p1 = geo2d.P(-2,2)
        p2 = geo2d.P(2,2)
        # all'interno del segmento
        x = geo2d.project(p0, a, b) # cap=True
        self.assertEqual(x, geo2d.P(0,0))
        x = geo2d.project(p1, a, b)
        self.assertEqual(x, a)
        x = geo2d.project(p2, a, b)
        self.assertEqual(x, b)
        # sul prolungamento del segmento
        x = geo2d.project(p0, a, b, False)
        self.assertEqual(x, geo2d.P(0,0))
        x = geo2d.project(p1, a, b, False)
        self.assertEqual(x, geo2d.P(-2,0))
        x = geo2d.project(p2, a, b, False)
        self.assertEqual(x, geo2d.P(2,0))

    def testDistPSeg(self):
        coords = [geo2d.P(-1, 1), geo2d.P(0, 1), geo2d.P(0.5, 1), geo2d.P(1, 1), geo2d.P(2, 1)]
        coords_spec = [geo2d.P(-1, -1), geo2d.P(0, -1), geo2d.P(0.5, -1), geo2d.P(1, -1), geo2d.P(2, -1)]
        results = [math.sqrt(2), 1, 1, 1, math.sqrt(2)]

        for i, c in enumerate(coords):
            self.assertAlmostEqual(geo2d.dist_p_seg(c, geo2d.P(0, 0), geo2d.P(1, 0)), results[i])

        for i, c in enumerate(coords_spec):
            self.assertAlmostEqual(geo2d.dist_p_seg(c, geo2d.P(0, 0), geo2d.P(1, 0)), results[i])

    def testOrtho(self):
        self.assert_(self.a^geo2d.ortho(self.a) == geo2d.len2(self.a))
        x = self.a - self.b
        self.assert_(x^geo2d.ortho(x) == geo2d.len2(x))

    def testArg(self):
        self.assert_(close(geo2d.arg(self.b-self.a), math.pi/2))
        self.assert_(close(geo2d.arg(self.d - self.c), -math.pi/2))
        self.assert_(close(geo2d.arg(self.c - self.b), 0))

    def testTriangleArea(self):
        self.assertEqual(geo2d.triangleArea(geo2d.P(0, 0),
                                            geo2d.P(2, 0),
                                            geo2d.P(0, 3),
                                            ), 3.0)

    def testX(self):
        self.assert_(self.a == self.a * geo2d.X())
        T1 = geo2d.rot(math.pi/2)
        self.assert_(geo2d.dist(self.a * T1, geo2d.P(-10,10)) < 1E-10)
        self.assert_(geo2d.dist(self.b * T1*geo2d.inv(T1), self.b) < 1E-10)
        self.assert_(geo2d.dist(self.a * geo2d.scale(2), geo2d.P(20,20)) < 1E-10)
        self.assert_(geo2d.dist(self.d * geo2d.cRot(self.d,2.3), self.d) < 1E-10)
        M = geo2d.mirror(geo2d.P(0,5), geo2d.P(1,5))
        self.assert_(geo2d.dist(self.a * M, geo2d.P(10,0)) < 1E-10)
        self.assert_(geo2d.dist(self.b * M, geo2d.P(10,-10)) < 1E-10)
        X = geo2d.xform(self.a, self.b, self.a, self.d)
        self.assert_(geo2d.dist(self.b * X, self.d) < 1E-10)
        self.assert_(geo2d.dist(self.a * X, self.a ) < 1E-10)
        self.assert_(geo2d.dist(self.c * X, geo2d.P(20,0)) < 1E-10)

        X = geo2d.cScaleXY(geo2d.P(0, 0), 1.5, 2.0)
        self.assertFalse(X.isMirror())
        self.assertFalse(X.isIdentity())
        X = geo2d.cRot(geo2d.P(0, 0), 2.0)
        self.assertFalse(X.isMirror())
        self.assertFalse(X.isIdentity())
        X = geo2d.mirror(geo2d.P(0,5), geo2d.P(1,5))
        self.assertTrue(X.isMirror())
        self.assertFalse(X.isIdentity())
        X = geo2d.xlate(geo2d.P(0.01, 0.0))
        self.assertFalse(X.isMirror())
        self.assertFalse(X.isIdentity())
        X = geo2d.X()
        self.assertFalse(X.isMirror())
        self.assertTrue(X.isIdentity())

    def testL(self):
        L = geo2d.Path([ self.a, self.b, self.c, self.d ])
        (x,d,s,i) = L.project(geo2d.P(5,15))
        self.assert_(close(x.x, 10))
        self.assert_(close(x.y, 15))
        self.assert_(close(d,5))
        self.assert_(i == 0)
        self.assert_(geo2d.dist2(L(s),x) < 1E-10)
        (x,d,s,i) = L.project(geo2d.P(5,25))
        self.assert_(geo2d.dist2(L(s),x) < 1E-10)

        S = L.segment(5, 15)
        self.assert_(len(S)==3)
        self.assert_(close(S.len(), 10))
        self.assert_(close(geo2d.dist(S(5), self.b), 0))
        self.assert_(close(geo2d.dist(L(10), self.b), 0))

        # Testa la segment anche passando i parametri in ordine inverso
        SR = L.segment(15, 5)
        self.assert_(len(SR)==3)
        self.assert_(close(SR.len(), 10))
        self.assert_(close(geo2d.dist(SR(5), self.b), 0))
        self.assert_(close(geo2d.dist(L(10), self.b), 0))

        # Verifica che con le due chiamate a segment siano stati generati due
        # Path con gli stessi punti ma specificati in ordine inverso
        pts1 = list(S)
        pts2 = list(SR); pts2.reverse()
        self.assert_(len(pts1) == len(pts2))
        for i in xrange(len(pts1)):
            self.assert_(close(pts1[i], pts2[i]))

        S1 = L.segment(5, 5)
        self.assertEqual(len(S1), 1)
        self.assert_(close(S1(0), L(5)))

        S2 = L.segment(11,19)
        self.assert_(len(S2)==2)
        S3 = L.segment(10,19)
        self.assert_(len(S3)==2)
        S4 = L.segment(9.99999,19)
        self.assert_(len(S4)==3)

    def testXposinv(self):
        """Vettori ortogonali per trasposta di inversa mantiene ortogonalità"""

        p1 = geo2d.P(1, 0)
        p2 = geo2d.P(0, 1)

        # Ortogonali
        self.assertAlmostEqual(p1 * p2, 0.0)

        # Matrice di trasformazione che turba l'ortogonalità
        x = geo2d.xlate(geo2d.P(3, 4)) * geo2d.rot(math.pi) * geo2d.scalexy(2, 3)
        ort = (p1 * x) * (p2 * x)
        self.assertNotAlmostEqual(ort, 0.0)

        # Usando la trasposta dell'inversa della stessa matrice l'ortogonalità è
        # mantenuta (non necessariamente la lunghezza)
        xi = geo2d.xposinv(x)
        ort = (p1 * xi) * (p2 * xi)
        self.assertAlmostEqual(ort, 0.0)

    def testComplexSplineX668(self):
        """Testa il funzionamento della complexSplineX su una particolare linea chiusa (bug #668)"""
        pts = [geo2d.P(-200, -200), geo2d.P(-300, -300),
               geo2d.P(-200, -400), geo2d.P(-100, -300)]
        spigoli = [0, 1, 2, 3]
        chiusa = True
        self._assertComplexSplineX(pts, spigoli, chiusa)

        # Avendo come input un rettangolo, la complexSplineX non deve fare
        # nulla. La linea risultante comunque avrà un punto in più (perché la
        # complexSplineX gestisce in questo modo le linee chiuse).
        r, ix = geo2d.complexSplineX(pts, spigoli, closed=chiusa)
        self.assertEqual(len(r), 5)
        self.assertEqual(len(ix), 4)
        self.assertEqual(r, pts + [pts[0]])
        self.assertEqual(ix, range(4))

    def _assertComplexSplineX(self, pts, spigoli, chiusa):
        """
        Controlla che la complexSplineX produca risultati IX corretti per una
        linea e tutte le sue rotazioni.
        """
        pts = pts[:]
        spigoli = spigoli[:]

        for i in range(len(pts)):
            r, ix = geo2d.complexSplineX(pts, spigoli, closed=chiusa)
            self.assertEqual(len(ix), len(pts))

            # Controlla che ix sia in ordine
            list_ix = list(ix)
            list_ix.sort()
            self.assertEqual(list_ix, ix, "ix non in ordine: %r" % ix)

            # Controlla che il primo punto della nuova spline corrisponda
            # al primo punto originale
            self.assertEqual(ix[0], 0)

            # Ruota pts e spigoli
            pts = pts[1:] + pts[:1]
            spigoli = [(s-1)%len(pts) for s in spigoli]

    def testComplexSplineX674(self):
        """Testa il funzionamento della complexSplineX su una particolare linea chiusa (bug #674)"""
        spigoli=[2]
        pts = [geo2d.P(-12.541133, 161.869420),
               geo2d.P(-11.900312, 161.565482),
               geo2d.P(-10.956328, 162.279497),
               geo2d.P(-12.540806, 161.867670)]
        chiusa = True

        self._assertComplexSplineX(pts, spigoli, chiusa)

    def testComplexSplineXPuntiSovrapposti(self):
        """Testa il funzionamento della complexSplineX su una linea con punti sovrapposti"""

        spigoli=[]
        pts=[geo2d.P(344.209265, 436.774216),
             geo2d.P(344.209265, 436.774216),
             geo2d.P(344.209265, 436.774216),
             geo2d.P(-148.978225, 490.298440)]
        chiusa = False

        self._assertComplexSplineX(pts, spigoli, chiusa)

    def testSortedReturnSplineX(self):
        """Verifica che gli indici tornati da splineX siano ordinati (bug #2975)."""
        pp = [geo2d.P(-45.923308441639293, 264.05874315447534),
              geo2d.P(2.8907390712166574, 281.50030147715114),
              geo2d.P(2.1981189975267341, 281.33502240601945),
              geo2d.P(266.81087904681232, 315.74370549505477)]
        levels = 6
        closed = 0
        iters = 20
        kk = 1.62
        ts, tix = geo2d.splineX(pp, levels, closed, iters, kk)

        self.assertEqual(tix, sorted(tix))

    def testAngle(self):
        a = geo2d.angle(geo2d.P(1,0), geo2d.P(0,1))
        self.assertAlmostEqual(a, math.pi/2)
        a = geo2d.angle(geo2d.P(0,1), geo2d.P(1,0))
        self.assertAlmostEqual(a, -math.pi/2)

    def testAngleDegenere(self):
        """Controlla il caso di angolo degenere."""
        p = geo2d.P(-0.930156, 0.367166)
        a = geo2d.angle(p, p)
        self.assertAlmostEqual(a, 0.0)

    def testIndexing(self):
        """Controlla accesso numerico alle coordinate"""
        a = geo2d.P(1,2)
        self.assertEqual(len(a), 2)
        self.assertEqual(list(a), [1.0, 2.0])
        self.assertEqual(a[0], 1)
        self.assertEqual(a[1], 2)
        self.assertEqual(a[-1], 2)
        self.assertEqual(a[-2], 1)
        self.assertRaises(IndexError, a.__getitem__, -3)
        self.assertRaises(IndexError, a.__getitem__, 2)

    def testHash(self):
        """Controlla hashing"""
        a = geo2d.P(1,2)
        b = geo2d.P(2,4) - a
        self.assertEqual(a, b)
        self.assertEqual(a.__hash__(), b.__hash__())

    def testPathPathDist(self):
        """Controlla distanza path-path"""
        P = geo2d.P
        pbad1 = geo2d.Path([P(100,100)])
        pbad2 = geo2d.Path([P(100,100), P(100,100)])
        p1 = geo2d.Path([P(0,0), P(100,0), P(100,0), P(100,100)])
        p2 = geo2d.Path([P(0,10), P(90, 10), P(90, 100), P(90,100)])
        self.assert_(14.142 < geo2d.pathPathDist(p1, p2) < 14.143)
        self.assertAlmostEqual(geo2d.pathPathDist(p1, p1), 0.0)
        self.assertAlmostEqual(geo2d.pathPathDist(p2, p2), 0.0)
        self.assertRaises(RuntimeError, geo2d.pathPathDist, pbad1, p2)
        self.assertRaises(RuntimeError, geo2d.pathPathDist, pbad2, p2)

    def testArea(self):
        P = geo2d.P
        self.assertAlmostEqual(geo2d.area(geo2d.Path(geo2d.circle(P(0,0), 100, maxerr=.0001))),
                              100*100*math.pi, 1)

    def testSArea(self):
        P = geo2d.P
        c = geo2d.Path(geo2d.circle(P(0,0), 100, maxerr=.0001))
        c_r = c
        c_r.reverse()
        self.assertAlmostEqual(geo2d.area(c),
                              -geo2d.sarea(c_r), 1)

    def testIntersSegmenti(self):
        a1 = geo2d.P(0, 0)
        a2 = geo2d.P(10, 0)
        b1 = geo2d.P(5, 1)
        b2 = geo2d.P(5, -1)

        p = geo2d.intersSegmenti(a1, a2, b1, b2)
        self.assert_(geo2d.same(p, geo2d.P(5, 0)))

        a1 = geo2d.P(0, 0)
        a2 = geo2d.P(10, 0)
        b1 = geo2d.P(5, 1)
        b2 = geo2d.P(5, 2)

        p = geo2d.intersSegmenti(a1, a2, b1, b2)
        self.assertEqual(p, None)

        a1 = geo2d.P(0, 0)
        a2 = geo2d.P(10, 0)
        b1 = geo2d.P(0, 1)
        b2 = geo2d.P(10, 1)

        p = geo2d.intersSegmenti(a1, a2, b1, b2)
        self.assertEqual(p, None)

        a1 = geo2d.P(0, 0)
        a2 = geo2d.P(10, 0)
        b1 = geo2d.P(10, 0)
        b2 = geo2d.P(20, 10)

        p = geo2d.intersSegmenti(a1, a2, b1, b2)
        self.assertEqual(p, None)

        a1 = geo2d.P(0, 0)
        a2 = geo2d.P(10, 0)
        b1 = geo2d.P(20, 10)
        b2 = geo2d.P(10, 0)

        p = geo2d.intersSegmenti(a1, a2, b1, b2)
        self.assertEqual(p, None)

    def testInterRettaSegmento(self):
        r1, r2 = geo2d.P(0,0), geo2d.P(1, 0)
        a1, a2 = geo2d.P(2,1), geo2d.P(2,-1)

        p = geo2d.intersRettaSegmento(r1, r2, a1, a2)
        self.assertGeo2dSame(p, geo2d.P(2, 0))

    def testInterSegm(self):
        a1 = geo2d.P(0, 0)
        a2 = geo2d.P(10, 0)
        b1 = geo2d.P(10, 0)
        b2 = geo2d.P(20, 10)

        p, s, t = geo2d.interSegm(a1, a2, b1, b2)
        self.assertAlmostEqual(geo2d.dist(p, geo2d.P(10, 0)), 0.0)
        self.assertAlmostEqual(s, 1.0)
        self.assertAlmostEqual(t, 0.0)

        a1 = geo2d.P(0, 0)
        a2 = geo2d.P(10, 0)
        b1 = geo2d.P(20, 10)
        b2 = geo2d.P(10, 0)

        p, s, t = geo2d.interSegm(a1, a2, b1, b2)
        self.assertAlmostEqual(geo2d.dist(p, geo2d.P(10, 0)), 0.0)
        self.assertAlmostEqual(s, 1.0)
        self.assertAlmostEqual(t, 1.0)

    def testIntersCircleSegment(self):
        c = geo2d.P(200, 200)
        r = 100

        p1 = geo2d.P(50, 50)
        p2 = geo2d.P(300, 200)
        inters = geo2d.intersCircleSegment(c, r, p1, p2)
        self.assertEqual(len(inters), 2)

        res1 = geo2d.P(300.0, 200.0)
        res2 = geo2d.P(152.94117647058823, 111.76470588235293)
        self.assert_(geo2d.same(res1, inters[0]) or geo2d.same(res1, inters[1]))
        self.assert_(geo2d.same(res2, inters[0]) or geo2d.same(res2, inters[1]))

        p1 = geo2d.P(50, 50)
        p2 = geo2d.P(150, 150)
        inters = geo2d.intersCircleSegment(c, r, p1, p2)
        self.assertEqual(len(inters), 1)
        res1 = geo2d.P(129.28932188134524, 129.28932188134524)
        self.assert_(geo2d.same(res1, inters[0]))

        p1 = geo2d.P(300, 300)
        p2 = geo2d.P(150, 150)
        inters = geo2d.intersCircleSegment(c, r, p1, p2)
        self.assertEqual(len(inters), 1)
        res1 = geo2d.P(270.71067811865476, 270.71067811865476)
        self.assert_(geo2d.same(res1, inters[0]))

        p1 = geo2d.P(300, 300)
        p2 = geo2d.P(400, 400)
        inters = geo2d.intersCircleSegment(c, r, p1, p2)
        self.assertEqual(len(inters), 0)

    def testGetCircleCenter(self):
        p1 = geo2d.P(1, 1)
        p2 = geo2d.P(1, -1)
        p3 = geo2d.P(-1, 1)

        c = geo2d.getCircleCenter(p1, p2, p3)
        self.assert_(geo2d.same(c, geo2d.P(0, 0)))

    def testGetAnglePointsIndexes(self):
        pts = [geo2d.P(0.0, 0.0),
               geo2d.P(50.0, 0.0),
               geo2d.P(100.0, 0.0),
               geo2d.P(100.0, 25.0),
               geo2d.P(100.0, 50.0),
               geo2d.P(50.0, 50.0),
               geo2d.P(0.0, 50.0),
               geo2d.P(0.0, 25.0),
               geo2d.P(0.0, 0.0)]
        esa = [geo2d.P(0.0, 0.0),
               geo2d.P(10.0, 10.0),
               geo2d.P(10.0, 20.0),
               geo2d.P(0.0, 30.0),
               geo2d.P(-10.0, 20.0),
               geo2d.P(-10.0, 10.0),
               geo2d.P(0.0, 0.0)]
        sp = geo2d.getAnglePointsIndexes(pts, math.radians(30.0), angle_on_straigth=True)
        self.assertEqual(sp, [0, 2, 4, 6, 8])
        sp = geo2d.getAnglePointsIndexes(esa, math.radians(50.0), angle_on_straigth=True)
        self.assertEqual(sp, [0, 3, 6])
        sp = geo2d.getAnglePointsIndexes(esa, math.radians(30.0), angle_on_straigth=True)
        self.assertEqual(sp, [0, 1, 2, 3, 4, 5, 6])

    def testDashPath(self):
        path = geo2d.Path([geo2d.P(0.0, 0.0), geo2d.P(40.0, 0.0)])

        pieno, vuoto = 10.0, 5.0
        tratti = geo2d.dashPath(path, pieno, vuoto)
        self.assertEqual(len(tratti), 3)
        for t in tratti:
            self.assertAlmostEqual(geo2d.Path(t).len(), pieno)

        pieno, vuoto = 50.0, 5.0
        tratti = geo2d.dashPath(path, pieno, vuoto)
        self.assertEqual(len(tratti), 1)
        self.assertAlmostEqual(geo2d.Path(tratti[0]).len(), path.len())

    def testPointInTriangle(self):
        a, b, c = geo2d.P(0.0, 0.0), geo2d.P(1.0, 0.0), geo2d.P(0.5, 1.0)

        p = geo2d.P(0.5, 0.0)
        self.assertTrue(geo2d.pointInTriangle(p, a, b, c))

        p = geo2d.P(0.5, 0.5)
        self.assertTrue(geo2d.pointInTriangle(p, a, b, c))

        p = geo2d.P(0.2, 0.8)
        self.assertFalse(geo2d.pointInTriangle(p, a, b, c))

        p = geo2d.P(0.8, 0.8)
        self.assertFalse(geo2d.pointInTriangle(p, a, b, c))


        a, b, c = [geo2d.P(0.0, 0.0), geo2d.P(1.0, 0.0), geo2d.P(0.0, 1.0)]

        p = geo2d.P(0.5, 0.0)
        self.assertTrue(geo2d.pointInTriangle(p, a, b, c))

        p = geo2d.P(0.5, 0.5)
        self.assertTrue(geo2d.pointInTriangle(p, a, b, c))

        p = geo2d.P(0.2, 0.8)
        self.assertTrue(geo2d.pointInTriangle(p, a, b, c))

        p = geo2d.P(0.8, 0.8)
        self.assertFalse(geo2d.pointInTriangle(p, a, b, c))

        p = geo2d.P(-0.1, 0.0)
        self.assertFalse(geo2d.pointInTriangle(p, a, b, c))

    def testBaryCoords(self):
        tri = [geo2d.P(0.0, 0.0), geo2d.P(1.0, 0.0), geo2d.P(0.5, 1.0)]
        p = geo2d.P(0.5, 0.5)
        self.assertEqual(geo2d.baryCoords(p, *tri), (0.25, 0.5))

        p = geo2d.P(0.0, 0.5)
        self.assertEqual(geo2d.baryCoords(p, *tri), (-0.25, 0.5))

        p = geo2d.P(1.0, 0.0)
        self.assertEqual(geo2d.baryCoords(p, *tri), (1.0, 0.0))

    def testArco(self):

        points = {}
        for a in (0, 45, 90, 135, 180, 225, 270, 315):
            ra = math.radians(a)
            points[a] = geo2d.P(math.cos(ra), math.sin(ra))

        def ptOnArco(arco, on):
            arco = geo2d.Path(arco)

            last_t = None
            for angle in on:
                pa = arco.project(points[angle])
                if last_t is None:
                    last_t = pa.t
                else:
                    # Controlla sia il verso...
                    self.assert_(pa.t > last_t, (pa.t, last_t))

                # ...che l'appartenenza all'arco.
                self.assertAlmostEqual(pa.d, 0.0, 1)

            for angle in set(points.keys()) - set(on):
                d = arco.project(points[angle]).d
                self.assert_(d > 0.0)

        # Calotta superiore
        arco = geo2d.arco(geo2d.P(0, 0), 1, 0, math.pi)
        ptOnArco(arco, [0, 45, 90, 135, 180])

        # Calotta inferiore
        arco = geo2d.arco(geo2d.P(0, 0), 1, 0, -math.pi)
        ptOnArco(arco, [0, 315, 270, 225, 180])

        # Calotta superiore
        arco = geo2d.arco(geo2d.P(0, 0), 1, math.pi, -math.pi)
        ptOnArco(arco, [180, 135, 90, 45, 0])

        # Calotta destra
        arco = geo2d.arco(geo2d.P(0, 0), 1, -math.pi / 2.0, math.pi)
        ptOnArco(arco, [270, 315, 0, 45, 90])

        # Calotta sinistra
        arco = geo2d.arco(geo2d.P(0, 0), 1, -math.pi / 2.0, -math.pi)
        ptOnArco(arco, [270, 225, 180, 135, 90])

class TestMinimize(unittest.TestCase):
    def testBasic(self):
        def func(x, y):
            return geo2d.dist(geo2d.P(3, 5), geo2d.P(x,y))

        r = geo2d.minimize(func, geo2d.P(10,10), 30)
        self.assertAlmostEqual(r.p.x, 3.0, places=2)
        self.assertAlmostEqual(r.p.y, 5.0, places=2)

        r = geo2d.minimize(func, geo2d.P(10,10), 30, eps=0.000001)
        self.assertAlmostEqual(r.p.x, 3.0, places=6)
        self.assertAlmostEqual(r.p.y, 5.0, places=6)

    def testLocalMinimum(self):
        def func(x, y):
            return geo2d.dist(geo2d.P(3, 5), geo2d.P(x,y))

        r = geo2d.localMinimum(func, geo2d.P(10,10), 30)
        for xr in r:
            self.assertAlmostEqual(xr.p.x, 3.0, places=2)
            self.assertAlmostEqual(xr.p.y, 5.0, places=2)

        r = geo2d.localMinimum(func, geo2d.P(10,10), 30, eps=0.000001)
        for xr in r:
            self.assertAlmostEqual(xr.p.x, 3.0, places=6)
            self.assertAlmostEqual(xr.p.y, 5.0, places=6)

    def testDoubleLocalMinimum(self):
        c0 = geo2d.P(17,8)
        c1 = geo2d.P(2, 4)
        def func(x, y):
            return min(geo2d.dist(c0, geo2d.P(x,y)),
                       geo2d.dist(c1, geo2d.P(x,y)))

        for eps in (None, 0.000001):
            if eps is None:
                r = geo2d.localMinimum(func, geo2d.P(10,10), 30)
                pl = 2
            else:
                r = geo2d.localMinimum(func, geo2d.P(10,10), 30, eps=eps)
                pl = 5
            for xr in r:
                if abs(xr.p - c0) < abs(xr.p - c1):
                    self.assertAlmostEqual(xr.p.x, c0.x, places=pl)
                    self.assertAlmostEqual(xr.p.y, c0.y, places=pl)
                else:
                    self.assertAlmostEqual(xr.p.x, c1.x, places=pl)
                    self.assertAlmostEqual(xr.p.y, c1.y, places=pl)

class TestPath(unittest.TestCase):
    def setUp(self):
        self.pts = [geo2d.P(i,i) for i in range(8)]
        self.p = geo2d.Path(self.pts)
    def testLen(self):
        self.assertEqual(len(self.p), len(self.pts))
    def testLength(self):
        last = len(self.pts)-1
        self.assertAlmostEqual(self.p.len(), math.sqrt(last*last + last*last))
    def testGetItem(self):
        self.assertEqual(self.p[2], self.pts[2])
        self.assertEqual(self.p[-2], self.pts[-2])
        self.assertEqual(self.p[0:2], self.pts[0:2])
        self.assertEqual(self.p[4:10], self.pts[4:10])
    def testAdd(self):
        p1 = self.p
        p2 = geo2d.Path([geo2d.P(i,i) for i in range(8, 16)])
        res = p1 + p2
        self.assertEqual(list(res), list(p1) + list(p2))
        self.assertAlmostEqual(res.len(), p1.len() + p2.len() + abs(p1[-1] - p2[0]))
    def testMul(self):
        X = geo2d.mirror(geo2d.P(0,0), geo2d.P(-1,1))
        p = self.p * X
        self.assertAlmostEqual(abs(p[0] - geo2d.P(0,0)), 0)
        self.assertAlmostEqual(abs(p[7] - geo2d.P(-7,-7)), 0)
        self.assertRaises(TypeError, lambda: X * self.p)
        self.assertRaises(TypeError, lambda: geo2d.P(0,0) * self.p)
        self.assertRaises(TypeError, lambda: self.p * geo2d.P(0,0))
    def testTan(self):
        P = geo2d.P
        path = geo2d.Path([P(0,0), P(10,0), P(10,0), P(10,0), P(10,10)])

        self.assertEqual(path.tanl(5), P(1,0))
        self.assertEqual(path.tanr(5), P(1,0))
        self.assertEqual(path.tan(5), P(1,0))

        self.assertEqual(path.tanl(10), P(1,0))
        self.assertEqual(path.tanr(10), P(0,1))
        self.assertAlmostEqual(path.tan(10).x, geo2d.dir(P(1,1)).x)
        self.assertAlmostEqual(path.tan(10).y, geo2d.dir(P(1,1)).y)

        self.assertEqual(path.tanl(15), P(0,1))
        self.assertEqual(path.tanr(15), P(0,1))
        self.assertEqual(path.tan(15), P(0,1))

    def testTanDegenere1(self):
        P = geo2d.P
        path = geo2d.Path([P(10,0), P(10,0), P(10,0), P(10,0)])

        self.assertRaises(ValueError, lambda: path.tanl(0))
        self.assertRaises(ValueError, lambda: path.tanr(0))
        self.assertRaises(ValueError, lambda: path.tan(0))
        self.assertRaises(ValueError, lambda: path.tanl(5))
        self.assertRaises(ValueError, lambda: path.tanr(5))
        self.assertRaises(ValueError, lambda: path.tan(5))

    def testTanEstremiConSingolarita(self):
        P = geo2d.P
        path = geo2d.Path([P(0,0), P(0,0), P(10,0), P(10,0), P(10,0), P(10,10), P(10,10)])

        self.assertEqual(path.tanl(0), P(1,0))
        self.assertEqual(path.tanr(0), P(1,0))
        self.assertEqual(path.tan(0), P(1,0))

        self.assertEqual(path.tanl(-10), P(1,0))
        self.assertEqual(path.tanr(-10), P(1,0))
        self.assertEqual(path.tan(-10), P(1,0))

        self.assertEqual(path.tanl(20), P(0,1))
        self.assertEqual(path.tanr(20), P(0,1))
        self.assertEqual(path.tan(20), P(0,1))

        self.assertEqual(path.tanl(200), P(0,1))
        self.assertEqual(path.tanr(200), P(0,1))
        self.assertEqual(path.tan(290), P(0,1))

    def testDegenere(self):
        self.assertRaises(ValueError, lambda: geo2d.Path([]))

class TestSostituisci(unittest.TestCase):
    def setUp(self):
        self.pts = [geo2d.P(i,i) for i in range(8)]
        self.p = geo2d.Path(self.pts)

    def testSostituisciAperto(self):
        """Test di sostituzione di parte di un path aperto con un altro"""
        p_i = geo2d.Path([geo2d.P(1+i,7-i) for i in range(6)])
        self.assertRaises(AssertionError, geo2d.sostituisci, self.p, False, p_i)

class TestArea(unittest.TestCase):
    def setUp(self):
        self.ferro_di_cavallo = [ (1., 1.),
                                (1.23423423, 1.),
                                (3., 1.),
                                (3., 1.5),
                                (1.5, 1.5),
                                (1.5, 2.5),
                                (3., 2.5),
                                (3., 3.),
                                (1., 3.) ]

        self.quadrato_con_buchi = [ [ (0,0), (5,0), (5,5), (0,5) ],
                                    [ (1,1), (2,1), (2,2), (1,2) ],
                                    [ (3,3), (4,3), (4,4), (3,4) ],
                                    [ (3,1), (4,1), (4,2), (3,2) ] ]

        self.triangolo_con_buco = [ [(1,1), (4,5), (6,1)],
                                    [(4.5,1.5), (5,3), (3,3)] ]

    def testAreaSemplice(self):
        """Test della misura dell'area di un poligono concavo (ferro di cavallo)"""
        path = geo2d.Path([geo2d.P(p[0], p[1]) for p in self.ferro_di_cavallo])
        self.assertEqual(geo2d.area(path), 2.5)

    def testAreaConBuchi(self):
        """Test della misura dell'area di un quadrato con tre buchi"""
        lista_contorni = []
        for punti in self.quadrato_con_buchi:
            lista_punti = [ geo2d.P(p[0], p[1]) for p in punti ]
            path = geo2d.Path(lista_punti)
            lista_contorni.append(path)
        self.assertEqual(geo2d.area(lista_contorni), 22.0)

        lista_contorni = []
        for punti in self.triangolo_con_buco:
            lista_punti = [ geo2d.P(p[0], p[1]) for p in punti ]
            path = geo2d.Path(lista_punti)
            lista_contorni.append(path)
        self.assertEqual(geo2d.area(lista_contorni), 8.5)

    def testPoligoniDisgiunti(self):
        """Test della misura dell'area di tre poligoni disgiunti, due dei quali con buchi"""
        lista_contorni = []
        ferro = geo2d.Path([geo2d.P(p[0], p[1]) for p in self.ferro_di_cavallo])

        lista_contorni.append(ferro)

        for punti in self.quadrato_con_buchi:
            lista_punti = [ geo2d.P(p[0], p[1]) for p in punti ]
            path = geo2d.Path(lista_punti)
            path = path * geo2d.xlate(geo2d.P(20,10))
            lista_contorni.append(path)

        for punti in self.triangolo_con_buco:
            lista_punti = [ geo2d.P(p[0], p[1]) for p in punti ]
            path = geo2d.Path(lista_punti)
            path = path * geo2d.xlate(geo2d.P(-10,-20))
            lista_contorni.append(path)

        self.assertEqual(geo2d.area(lista_contorni), 33.0)

    def testOddEven(self):
        """Test di contenimento del quadrato con 3 buchi"""
        lista_contorni = []
        for punti in self.quadrato_con_buchi:
            lista_punti = [ geo2d.P(p[0], p[1]) for p in punti ]
            path = geo2d.Path(lista_punti)
            lista_contorni.append(path)
        self.assertEqual(geo2d.oddEven(lista_contorni), [False, True, True, True])

    def testOrient(self):
        """Test di orientamento dei path del quadrato con 3 buchi"""
        lista_contorni = []
        for punti in self.quadrato_con_buchi:
            lista_punti = [ geo2d.P(p[0], p[1]) for p in punti ]
            path = geo2d.Path(lista_punti)
            lista_contorni.append(path)
        res = geo2d.orient(lista_contorni)
        self.assertEqual(list(lista_contorni[0]), list(geo2d.Path(reversed(res[0]))))
        self.assertEqual(list(lista_contorni[1]), list(res[1]))
        self.assertEqual(list(lista_contorni[2]), list(res[2]))
        self.assertEqual(list(lista_contorni[3]), list(res[3]))

class TestOrdinaContorni(unittest.TestCase):
    def testOrdina(self):
        paths = []
        paths.append(geo2d.Path([geo2d.P(0, 0), geo2d.P(0, 10), geo2d.P(10, 10), geo2d.P(10, 0), geo2d.P(0, 0)]))
        paths.append(geo2d.Path([geo2d.P(2, 4), geo2d.P(4, 4), geo2d.P(4, 2), geo2d.P(2, 2), geo2d.P(2, 4)]))
        paths.append(geo2d.Path([geo2d.P(5, 7), geo2d.P(5, 7), geo2d.P(5, 7), geo2d.P(5, 7), geo2d.P(5, 7)]))
        paths.append(geo2d.Path([geo2d.P(20, 20), geo2d.P(20, 30), geo2d.P(30, 30), geo2d.P(30, 20), geo2d.P(20, 20)]))
        ord = geo2d.ordinaContorni(paths)

        self.assertEqual(len(ord), 2)
        self.assertEqual(set(ord[0]), set([0, 3]))
        self.assertEqual(set(ord[1]), set([1, 2]))

if __name__ == "__main__":
    unittest.main()
