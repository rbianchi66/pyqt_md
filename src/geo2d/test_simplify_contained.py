#!/usr/bin/env python
#-*- coding: utf-8 -*-
from __future__ import division
import math
import geo2d
from geo2d import intersect
from geo2d import simplify_contained
from geo2d import inside
import unittest

P = geo2d.P

class TestIntersect(unittest.TestCase):

    def setUp(self):
        # Lista dei poligoni da testare
        self.polys = []
        
        # Pentagono in senso ORARIO con primo e ultimo punto ripetuto
        self.polys.append(geo2d.Path([P(0,5), P(0,5), P(5,10), P(10, 5), P(7.5,0), P(2.5, 0), P(0,5), P(0,5)]))
        
        # Pentagono con punta rigirata verso l'interno (concavita') con ultimo punto ripetuto
        self.polys.append(geo2d.Path([P(0,5), P(5,2.5), P(10, 5), P(7.5,0), P(2.5, 0), P(0,5), P(0,5)]))
        
        # Cerchio
        self.polys.append(geo2d.Path(geo2d.circle(P(0,0), 30, maxerr=.0001)))
        
    def testSimplifyContain(self):
        """Dopo una semplificazione di default il poligono generato
        contiene o e' sovrapposto al poligono originale"""
        for poly in self.polys:
            # print "Original:", str(poly)
            poly_simplified = simplify_contained(poly, 0)
            # print "Simplified:", str(poly_simplified), "\n"
            for point in poly:
                self.assertTrue(inside(point, poly_simplified) or point in poly_simplified)

    def testSimplifyContained(self):
        """Dopo una semplificazione di default il poligono generato
        e' contenuto o sovrapposto al poligono originale"""
        for poly in self.polys:
            # print "Original:", str(poly)
            poly_simplified = simplify_contained(poly, 1)
            # print "Simplified:", str(poly_simplified), "\n"
            for point in poly_simplified:
                self.assertTrue(inside(point, poly) or point in poly)

if __name__ == "__main__":
    unittest.main()
