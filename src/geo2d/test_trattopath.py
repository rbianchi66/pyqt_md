#!/usr/bin/env python
#-*- coding: utf-8 -*-
import unittest
import geo2d

# FIXME: non dovremmo scrivere test di geo2d che dipendono da db/test. Dovrebbero
# essere totalmente indipendenti!
import db
import test

P = geo2d.P

class TestTrattoPath(test.Database):
    def setUp(self):
        super(TestTrattoPath, self).setUp()
        path = geo2d.Path([P(0, 0), P(0, 1), P(0, 2)])
        t0 = 0.5
        t1 = 1.2
        self.tp = geo2d.TrattoPath(path, t0, t1)

    def testReversed(self):
        tprev = self.tp.reversed()
        expected = [P(0, 2), P(0, 1), P(0, 0)]
        for p in expected:
            self.assert_(geo2d.same(p, tprev.path(expected.index(p))))
        self.assertAlmostEqual(tprev.t0, 0.8)
        self.assertAlmostEqual(tprev.t1, 1.5)

    def testPt(self):
        self.assert_(geo2d.same(self.tp.p0, P(0, 0.5)))
        self.assert_(geo2d.same(self.tp.p1, P(0, 1.2)))

    def testContains(self):
        self.failUnless(self.tp.contains(P(0, 1.1)))
        self.failIf(self.tp.contains(P(0, 1.3)))
        self.failIf(self.tp.contains(P(0.5, 0)))
        self.failUnless(self.tp.contains(self.tp.t0))
        self.failUnless(self.tp.contains(self.tp.t1))

        self.failUnless(self.tp.contains(0.6))
        self.failIf(self.tp.contains(0.4))
        self.assertRaises(TypeError, self.tp.contains, None)

    def testTrattoACavalloLineaChiusa(self):
        linea = db.LineaBase2D([db.PuntoLibero2D(geo2d.P(0, 0)),
                                db.PuntoLibero2D(geo2d.P(100, -100)),
                                db.PuntoLibero2D(geo2d.P(-100, -100))],[],True,
                                tool=self.creaToolLinea())

        tratto = db.Tratto(linea, -0.5, 0.5)
        tp = geo2d.TrattoPath.fromDbTratto(tratto)

        path = tp.calcPath()
        self.assertAlmostEqual(path.project(geo2d.P(0, 0)).d, 0)

    def testFromDbTrattoConDelta(self):
        linea = db.LineaBase2D(pts=[db.PuntoLibero2D(geo2d.P(0, 0)),
                                db.PuntoLibero2D(geo2d.P(100, -100)),
                                db.PuntoLibero2D(geo2d.P(-100, -100))],
                               spigoli=[0,1,2],chiusa=True,
                               tool=self.creaToolLinea())

        tratto = db.Tratto(linea, -0.5, 0.5)
        tp = geo2d.TrattoPath.fromDbTratto(tratto, delta=8.0)

        self.assertAlmostEqual(tp.len(), tratto.geometria().len())
        self.assertAlmostEqual(tp.path.len(), tratto.geometria().len() + 8.0*2)
        self.assertAlmostEqual(tp.p0.x, tratto.linea.pt(tratto.t0).x)
        self.assertAlmostEqual(tp.p0.y, tratto.linea.pt(tratto.t0).y)
        self.assertAlmostEqual(tp.p1.x, tratto.linea.pt(tratto.t1).x)
        self.assertAlmostEqual(tp.p1.y, tratto.linea.pt(tratto.t1).y)

    def testFromDbTrattoConDeltaAperta(self):
        linea = db.LineaBase2D(pts=[db.PuntoLibero2D(geo2d.P(0, 0)),
                                db.PuntoLibero2D(geo2d.P(100, -100)),
                                db.PuntoLibero2D(geo2d.P(-100, -100))],
                               spigoli=[1],chiusa=False,
                               tool=self.creaToolLinea())

        tratto = db.Tratto(linea, 0, 2)
        tp = geo2d.TrattoPath.fromDbTratto(tratto, delta=8.0)

        self.assertAlmostEqual(tp.p0.x, linea.pts[0].p.x)
        self.assertAlmostEqual(tp.p0.y, linea.pts[0].p.y)
        self.assertAlmostEqual(tp.p1.x, linea.pts[2].p.x)
        self.assertAlmostEqual(tp.p1.y, linea.pts[2].p.y)

    def testCerchio(self):
        linea = db.Cerchio2D(centro=db.PuntoLibero2D(geo2d.P(100,100)),
                             diametro=10, tool=self.creaToolLinea())

        assert linea.pt(linea.range()[0]) ==linea.pt(linea.range()[1])
        tratto = db.Tratto(linea=linea, t0=linea.range()[0], t1=linea.range()[1])

        tp = geo2d.TrattoPath.fromDbTratto(tratto, delta=None)
        self.assertAlmostEqual(tp.p0.x, tp.p1.x)
        self.assertAlmostEqual(tp.p0.y, tp.p1.y)

        tp = geo2d.TrattoPath.fromDbTratto(tratto, delta=10)
        self.assertAlmostEqual(tp.p0.x, tp.p1.x)
        self.assertAlmostEqual(tp.p0.y, tp.p1.y)

    def testSplitBasic(self):
        p = geo2d.Path([geo2d.P(0,0), geo2d.P(1,0), geo2d.P(2,0),
                        geo2d.P(3,0), geo2d.P(4,0), geo2d.P(5,0),
                        geo2d.P(6,0)])

        tp = geo2d.TrattoPath(p, 1.0, 5.0)
        tp1, tp2 = tp.split(2.5)

        self.assertAlmostEqual(tp1.path.len() + tp2.path.len(), tp.path.len())
        self.assertAlmostEqual(tp1.t0, 0)
        self.assertAlmostEqual(tp2.t0, 0)

        self.assertAlmostEqual(tp1.p0.x, 2.5)
        self.assertAlmostEqual(tp2.p0.x, 2.5)

        self.assertAlmostEqual(tp1.p1.x, tp.p0.x)
        self.assertAlmostEqual(tp1.p1.y, tp.p0.y)

        self.assertAlmostEqual(tp2.p1.x, tp.p1.x)
        self.assertAlmostEqual(tp2.p1.y, tp.p1.y)

    def testSplitCerchio(self):
        # Bug #891: il cerchio con prolungamenti veniva splittato in modo sbagliato
        linea = db.Cerchio2D(centro=db.PuntoLibero2D(geo2d.P(100,100)),
                                  diametro=10, tool=self.creaToolLinea())
        tratto = db.Tratto(linea=linea, t0=linea.range()[0], t1=linea.range()[1])
        tp = geo2d.TrattoPath.fromDbTratto(tratto, delta=10)

        tp1, tp2 = tp.split((tp.t0 + tp.t1) / 2)
        self.assertAlmostEqual(tp1.path.len(), tp2.path.len())

    def testFromDbTrattoLineaChiusa(self):
        linea = db.LineaBase2D(pts=[db.PuntoLibero2D(p=geo2d.P(0,0)),
                                    db.PuntoLibero2D(p=geo2d.P(1,1)),
                                    db.PuntoLibero2D(p=geo2d.P(2,2)),
                                    db.PuntoLibero2D(p=geo2d.P(3,3)),
                                    db.PuntoLibero2D(p=geo2d.P(4,4)),
                                    db.PuntoLibero2D(p=geo2d.P(5,5)),
                                    db.PuntoLibero2D(p=geo2d.P(6,6)),
                                    db.PuntoLibero2D(p=geo2d.P(7,7)),
                                    db.PuntoLibero2D(p=geo2d.P(8,8)),
                                    db.PuntoLibero2D(p=geo2d.P(9,9)),
                                    db.PuntoLibero2D(p=geo2d.P(9,0))],
                               spigoli=[], chiusa=True, tool=self.creaToolLinea())
        tratto = db.Tratto.fromLinea(linea)
        tp = geo2d.TrattoPath.fromDbTratto(tratto, delta=4)
        self.assertAlmostEqual(tp.len(), linea.geometria().len())

    def testFromDbTrattoErrorePrecisione(self):
        t = db.Tratto(t0=0.59431391600489236,
                      t1=2.2893139160048928,
                      linea=db.LineaBase2D(
                         spigoli=set([]), tool=self.creaToolLinea(),
                         pts=[
                              db.PuntoLibero2D(
                                 p=geo2d.P(16.87591240875912,
                                     129.44525547445249)),
                              db.PuntoLibero2D(
                                 p=geo2d.P(103.76642335766422,
                                     184.56934306569337)),
                              db.PuntoLibero2D(
                                 p=geo2d.P(207.00729927007299,
                                     229.41605839416053)),
                              db.PuntoLibero2D(
                                 p=geo2d.P(214.94890510948903,
                                     232.68613138686123))]))

        tp = geo2d.TrattoPath.fromDbTratto(t, delta=30)

    def assertSameTrattoPath(self, tp1, tp2):
        self.assertAlmostEqual(tp1.t0, tp2.t0, 7)
        self.assertAlmostEqual(tp1.t1, tp2.t1, 7)
        self.assertAlmostEqual(tp1.p0.x, tp2.p0.x, 7)
        self.assertAlmostEqual(tp1.p0.y, tp2.p0.y, 7)
        self.assertAlmostEqual(tp1.p1.x, tp2.p1.x, 7)
        self.assertAlmostEqual(tp1.p1.y, tp2.p1.y, 7)

        self.assertAlmostEqual(tp1.path(0.0).x, tp2.path(0.0).x, 7)
        self.assertAlmostEqual(tp1.path(0.0).y, tp2.path(0.0).y, 7)
        self.assertAlmostEqual(tp1.path(2.0).x, tp2.path(2.0).x, 7)
        self.assertAlmostEqual(tp1.path(2.0).y, tp2.path(2.0).y, 7)
        self.assertAlmostEqual(tp1.path(14.0).x, tp2.path(14.0).x, 7)
        self.assertAlmostEqual(tp1.path(14.0).y, tp2.path(14.0).y, 7)

    def testFromDbTrattoReversed(self):
        linea = db.Cerchio2D(centro=db.PuntoLibero2D(geo2d.P(100,100)),
                                  diametro=10, tool=self.creaToolLinea())
        tratto1 = db.Tratto(linea=linea, t0=1.67, t1=2.12)
        tratto2 = db.Tratto(linea=linea, t0=2.12, t1=1.67)

        tp1 = geo2d.TrattoPath.fromDbTratto(tratto1, delta=10).reversed()
        tp2 = geo2d.TrattoPath.fromDbTratto(tratto2, delta=10)

        self.assertSameTrattoPath(tp1, tp2)

    def testFromDbTrattoApertaReversed(self):
        linea = db.LineaBase2D(pts=[db.PuntoLibero2D(p=geo2d.P(0,0)),
                                    db.PuntoLibero2D(p=geo2d.P(1,1)),
                                    db.PuntoLibero2D(p=geo2d.P(2,2)),
                                    db.PuntoLibero2D(p=geo2d.P(3,3)),
                                    db.PuntoLibero2D(p=geo2d.P(4,4)),
                                    db.PuntoLibero2D(p=geo2d.P(5,5)),
                                    db.PuntoLibero2D(p=geo2d.P(6,6))],
                               chiusa=False, spigoli=[], tool=self.creaToolLinea())

        tratto1 = db.Tratto(linea=linea, t0=0.67, t1=2.12)
        tratto2 = db.Tratto(linea=linea, t0=2.12, t1=0.67)

        tp1 = geo2d.TrattoPath.fromDbTratto(tratto1, delta=5).reversed()
        tp2 = geo2d.TrattoPath.fromDbTratto(tratto2, delta=5)

        self.assertSameTrattoPath(tp1, tp2)

    def testProlunga(self):
        tp1 = self.tp.prolunga(10)
        self.assert_(self.tp.path is tp1.path)
        self.assertAlmostEqual(self.tp.len() + 10, tp1.len())
        self.assertAlmostEqual(self.tp.t0, tp1.t0)
        self.assert_(tp1.contains(self.tp.t1))

class TestTrattoPathRovesciato(test.Database):
    def setUp(self):
        super(TestTrattoPathRovesciato, self).setUp()
        path = geo2d.Path([P(0, 0), P(0, 1), P(0, 2)])
        t0 = 0.5
        t1 = 1.2
        self.tp = geo2d.TrattoPath(path, t1, t0)

    def testLength(self):
        len = self.tp.len()
        self.assertAlmostEqual(len, 0.7)

    def testTrim(self):
        tpt = self.tp.trim()
        self.assertAlmostEqual(tpt.len(), 0.7)
        self.assertAlmostEqual(tpt.path.len(), 0.7)

        # L'orientamento del path è indipendente da quello del tratto costruito
        # su di esso.
        self.assertAlmostEqual(tpt.path(0).x, 0)
        self.assertAlmostEqual(tpt.path(0).y, 0.5)
        self.assertAlmostEqual(tpt.path(0.7).x, 0)
        self.assertAlmostEqual(tpt.path(0.7).y, 1.2)

        self.assertAlmostEqual(tpt.t0, 0.7)
        self.assertAlmostEqual(tpt.t1, 0)

    def testContains(self):
        t = 1.0
        p = P(0, 1)
        self.assert_(self.tp.contains(t))
        self.assert_(self.tp.contains(p))

        t = 0.4
        p = P(0, 0.4)
        self.assert_(not self.tp.contains(t))
        self.assert_(not self.tp.contains(p))

        t = 1.3
        p = P(0, 1.3)
        self.assert_(not self.tp.contains(t))
        self.assert_(not self.tp.contains(p))

    def testReversed(self):
        tpr = self.tp.reversed()
        self.assertAlmostEqual(tpr.len(), 0.7)
        self.assertAlmostEqual(tpr.path.len(), 2)

        # L'orientamento del path è indipendente da quello del tratto costruito
        # su di esso.
        self.assertAlmostEqual(tpr.path(0).x, 0)
        self.assertAlmostEqual(tpr.path(0).y, 2)
        self.assertAlmostEqual(tpr.path(2).x, 0)
        self.assertAlmostEqual(tpr.path(2).y, 0)

        self.assertAlmostEqual(tpr.t0, 1.5)
        self.assertAlmostEqual(tpr.t1, 0.8)

    def testSplit(self):
        tp1, tp2 = self.tp.split(0.8)

        self.assertAlmostEqual(tp1.len(), 0.3)
        self.assertAlmostEqual(tp1.path.len(), 0.8)

        self.assertAlmostEqual(tp1.path(0).x, 0)
        self.assertAlmostEqual(tp1.path(0).y, 0.8)
        self.assertAlmostEqual(tp1.path(0.8).x, 0)
        self.assertAlmostEqual(tp1.path(0.8).y, 0)

        self.assertAlmostEqual(tp1.t0, 0)
        self.assertAlmostEqual(tp1.t1, 0.3)

        self.assertAlmostEqual(tp2.len(), 0.4)
        self.assertAlmostEqual(tp2.path.len(), 1.2)

        self.assertAlmostEqual(tp2.path(0).x, 0)
        self.assertAlmostEqual(tp2.path(0).y, 0.8)
        self.assertAlmostEqual(tp2.path(0.8).x, 0)
        self.assertAlmostEqual(tp2.path(1.2).y, 2)

        self.assertAlmostEqual(tp2.t0, 0)
        self.assertAlmostEqual(tp2.t1, 0.4)

    def testProlunga(self):
        tp1 = self.tp.prolunga(10)
        self.assert_(self.tp.path is tp1.path)
        self.assertAlmostEqual(self.tp.len() + 10, tp1.len())
        self.assertAlmostEqual(self.tp.t0, tp1.t0)
        self.assert_(tp1.contains(self.tp.t1))

    def testFromDbTrattoConTrattoApparentementeInnocente(self):
        """Controlla un tratto che causava problemi numerici (bug #893)."""
        t = db.Tratto(t0=0.074755218161420958,
                    t1=2.8045989681614247,
                    linea=db.LineaBase2D(
                       spigoli=set([]), tool=self.creaToolLinea(),
                       pts=[
                            db.PuntoLibero2D(
                               p=geo2d.P(661.99164635321631,
                                   120.00072419323487)),
                            db.PuntoLibero2D(
                               p=geo2d.P(663.00277861788663,
                                   120.99567834167053)),
                            db.PuntoLibero2D(
                               p=geo2d.P(661.96737917886423,
                                   121.00376739978789)),
                            db.PuntoLibero2D(
                               p=geo2d.P(662.99468955976931,
                                   119.98454607700015)),
                            db.PuntoLibero2D(
                               p=geo2d.P(669.6324836912022,
                                   120.0437404366953))
                           ]))

        tp = geo2d.TrattoPath.fromDbTratto(t, delta=30)
        self.assertAlmostEqual(tp.len(), t.geometria().len())

if __name__ == "__main__":
    unittest.main()
