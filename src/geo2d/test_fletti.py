#!/usr/bin/env python
#-*- coding: utf-8 -*-
import unittest
import geo2d
import db
import test

P = geo2d.P

class TestFletti(test.Database):
    def setUp(self):
        super(TestFletti, self).setUp()
        # Definisco qui le linee sulle quali eseguire i test
        # Rettangolo
        self.Rettangolo = db.LineaBase2D([db.PuntoLibero2D(P( 100, 100)),
                                          db.PuntoLibero2D(P( 100,-100)),
                                          db.PuntoLibero2D(P(-100,-100)),
                                          db.PuntoLibero2D(P(-100, 100))],
                                          [0, 1, 2, 3],
                                          True, tool=self.creaToolLinea())
        self.rettangolo = [pt.p for pt in self.Rettangolo.pts]
        # Linea aperta diritta
        self.Linea = db.LineaBase2D([db.PuntoLibero2D(P(0, 0)),
                                     db.PuntoLibero2D(P(20,0)),
                                     db.PuntoLibero2D(P(40,0)),
                                     db.PuntoLibero2D(P(50,0)),
                                     db.PuntoLibero2D(P(60,0)),
                                     db.PuntoLibero2D(P(80,0)),
                                     db.PuntoLibero2D(P(100,0))],
                                     [],
                                     False, tool=self.creaToolLinea())
        self.linea = [pt.p for pt in self.Linea.pts]
        # Linea chiusa morbida
        self.Lcm = db.LineaBase2D([db.PuntoLibero2D(P(100, 0)),
                                     db.PuntoLibero2D(P(100,100)),
                                     db.PuntoLibero2D(P(-100,100)),
                                     db.PuntoLibero2D(P(-100,-100)),
                                     db.PuntoLibero2D(P(100,-100))],
                                     [],
                                     True, tool=self.creaToolLinea())
        self.lcm = [pt.p for pt in self.Lcm.pts]

    def testLineaChiusaMorbida(self):
        """Linea chiusa morbida. sposta un punto lungo la normale."""
        def normale(i):
            """Torna la normale alla linea Lcm nel punto di indice i."""
            (t1, t2) = self.Lcm.tan(i)
            t = (t1 + t2)/2.0
            return geo2d.ortho(t)

        dist = 10
        locked = {0: P(110,0)}
        pts = geo2d.fletti(self.lcm, locked, list(self.Lcm.spigoli), self.Lcm.chiusa)

        self.assert_(geo2d.same(pts[0], P(110,0)))
        for i in xrange(1, len(pts)):
            pt = self.Lcm.pts[i].p - dist * normale(i)
            self.assert_(geo2d.same(pts[i], pt, prec=0.2))

    def testFlettiRettangolo(self):
        """Test fletti su rettangolo."""
        locked = {0: P(200, 200)}
        pts = geo2d.fletti(self.rettangolo, locked, list(self.Rettangolo.spigoli),
                     self.Rettangolo.chiusa)

        prec = 1E-2

        self.assert_(geo2d.same(pts[0], P( 200, 200), prec))
        self.assert_(geo2d.same(pts[1], P( 200,-200), prec))
        self.assert_(geo2d.same(pts[2], P(-200,-200), prec))
        self.assert_(geo2d.same(pts[3], P(-200, 200), prec))

        locked = {0: P(200, 100)}
        pts = geo2d.fletti(self.rettangolo, locked, list(self.Rettangolo.spigoli),
                     self.Rettangolo.chiusa)

        self.assert_(geo2d.same(pts[0], P( 200, 100), prec))
        self.assert_(geo2d.same(pts[1], P( 100,-200), prec))
        self.assert_(geo2d.same(pts[2], P(-200,-100), prec))
        self.assert_(geo2d.same(pts[3], P(-100, 200), prec))

        locked = {0: P(100, 0)}
        pts = geo2d.fletti(self.rettangolo, locked, list(self.Rettangolo.spigoli),
                     self.Rettangolo.chiusa)

        self.assert_(geo2d.same(pts[0], P( 100, 0), prec))
        self.assert_(geo2d.same(pts[1], P( 0,-100), prec))
        self.assert_(geo2d.same(pts[2], P(-100,-0), prec))
        self.assert_(geo2d.same(pts[3], P(0, 100), prec))

        locked = {0: P(200, 200), 2: P(-200,-200), 3: P(-300, 300)}
        pts = geo2d.fletti(self.rettangolo, locked, list(self.Rettangolo.spigoli),
                     self.Rettangolo.chiusa)

        self.assert_(geo2d.same(pts[0], P( 200, 200), prec))
        self.assert_(geo2d.same(pts[2], P(-200,-200), prec))
        self.assert_(geo2d.same(pts[3], P(-300, 300), prec))
        self.assert_(pts[1].x < 200)
        self.assert_(pts[1].y > -200)

        locked = {0: P(200, 200), 2: P(-200,-200), 3: P(-150, 150)}
        pts = geo2d.fletti(self.rettangolo, locked, list(self.Rettangolo.spigoli),
                     self.Rettangolo.chiusa)

        self.assert_(geo2d.same(pts[0], P( 200, 200), prec))
        self.assert_(geo2d.same(pts[2], P(-200,-200), prec))
        self.assert_(geo2d.same(pts[3], P(-150, 150), prec))
        self.assert_(pts[1].x > 200)
        self.assert_(pts[1].y < -200)

    def testFlettiLinea(self):
        """Test fletti su linea aperta."""
        locked = {3: P(50, 10)}
        pts = geo2d.fletti(self.linea, locked, list(self.Linea.spigoli),
                     self.Linea.chiusa)

        self.assert_(geo2d.same(pts[0], P(0,10)))
        self.assert_(geo2d.same(pts[1], P(20,10)))
        self.assert_(geo2d.same(pts[2], P(40,10)))
        self.assert_(geo2d.same(pts[3], P(50,10)))
        self.assert_(geo2d.same(pts[4], P(60,10)))
        self.assert_(geo2d.same(pts[5], P(80,10)))
        self.assert_(geo2d.same(pts[6], P(100,10)))

        locked = {3: P(60, 10)}
        pts = geo2d.fletti(self.linea, locked, list(self.Linea.spigoli),
                     self.Linea.chiusa)

        self.assert_(geo2d.same(pts[0], P(10,10)))
        self.assert_(geo2d.same(pts[1], P(30,10)))
        self.assert_(geo2d.same(pts[2], P(50,10)))
        self.assert_(geo2d.same(pts[3], P(60,10)))
        self.assert_(geo2d.same(pts[4], P(70,10)))
        self.assert_(geo2d.same(pts[5], P(90,10)))
        self.assert_(geo2d.same(pts[6], P(110,10)))

        locked = {0: P(0,0), 3: P(50, 10), 6: P(100,0)}
        pts = geo2d.fletti(self.linea, locked, list(self.Linea.spigoli),
                     self.Linea.chiusa)

        self.assert_(geo2d.same(pts[0], P(0,0)))
        self.assert_(geo2d.same(pts[3], P(50,10)))
        self.assert_(geo2d.same(pts[6], P(100,0)))
        self.assertEqual(pts[1].x, 20)
        self.assert_(0 < pts[1].y < 10)
        self.assertEqual(pts[2].x, 40)
        self.assert_(0 < pts[2].y < 10)
        self.assertEqual(pts[4].x, 60)
        self.assert_(0 < pts[4].y < 10)
        self.assertEqual(pts[5].x, 80)
        self.assert_(0 < pts[5].y < 10)

if __name__ == "__main__":
    unittest.main()
