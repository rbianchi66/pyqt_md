#!/usr/bin/env python
#-*- coding: utf-8 -*-
from __future__ import division
import geo2d
import unittest

P = geo2d.P
Path = geo2d.Path

ams = 0.392699081699

class TestXispline(unittest.TestCase):
    def assertGeo2dSame(self, p1, p2, prec=1e-6):
        self.assertTrue(geo2d.same(p1, p2, prec), "%r != %r" % (p1, p2))
    def assertGeo2dNotSame(self, p1, p2, prec=1e-6):
        self.assertFalse(geo2d.same(p1, p2, prec), "%r == %r" % (p1, p2))
    def setUp(self):
        self.linea_chiusa = Path(geo2d.circle(P(0,0), 100))
        self.linea_aperta = Path([p for p in self.linea_chiusa if p.x <= 0])
        self.assertGeo2dNotSame(self.linea_aperta[0], self.linea_aperta[-1])
    def testAutoLineaAperta(self):
        pts, spigoli, chiusa = geo2d.xispline(self.linea_aperta, ams)
        self.assertFalse(chiusa)
        ipts, ix = geo2d.complexSplineX(pts, spigoli, closed=chiusa)
        ilinea = Path(ipts)
        self.assertGeo2dNotSame(ilinea[0], ilinea[-1])
        self.assertLess(geo2d.pathPathDist(self.linea_aperta, ilinea), 0.06)
    def testAutoLineaChiusa(self):
        pts, spigoli, chiusa = geo2d.xispline(self.linea_chiusa, ams)
        self.assertTrue(chiusa)
        ipts, ix = geo2d.complexSplineX(pts, spigoli, closed=chiusa)
        ilinea = Path(ipts)
        self.assertGeo2dSame(ilinea[0], ilinea[-1])
        self.assertLess(geo2d.pathPathDist(self.linea_chiusa, ilinea), 0.06)
    def testChiudiLineaAperta(self):
        pass
    def testApriLineaAperta(self):
        pass
    def testChiudiLineaChiusa(self):
        pass
    def testApriLineaChiusa(self):
        pass

