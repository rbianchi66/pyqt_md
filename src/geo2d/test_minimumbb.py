#!/usr/bin/env python
#-*- coding: utf-8 -*-

from __future__ import division
import unittest
import random

from geo2d import P
import geo2d
import geo

class TestMinimumBB(unittest.TestCase):
    def testBBox(self):
        pts = [P(0, 6), P(10, 6), P(0, 0), P(10, 0)]
        bbox = geo2d.MinimumBB(pts)

        for p in bbox.getRect():
            p = geo2d.P(int(p.x), int(p.y))
            self.assert_(p in pts)

    def testExtra(self):
        pts = [P(0, 6), P(10, 6), P(10, 0), P(0, 0)]
        bbox = geo2d.MinimumBB(pts).getRect()
        
        for i in range(len(pts)):
            self.assertTrue(geo2d.same(pts[i], bbox[i]))

    def testContains(self):
        pts = [P(0, 6), P(10, 4), P(0, 0), P(10, -2)]
        bbox = geo2d.MinimumBB(pts)

        self.assertTrue(bbox.test(P(0, 0)))
        self.assertTrue(bbox.test(P(3, 3)))
        self.assertFalse(bbox.test(P(-1, 0)))

    def testRectangle(self):
        """
        Calcola il bounding box ottenuti su più rettangoli, in modo da controllare
        eventuali problemi nella costruzione del bounding box minimo
        """

        def ordinaPunti(pts):
            ps = pts[:]
            for i in reversed(range(4)):
                for j in range(1, i+1):
                    if ps[j-1].x > ps[j].x:
                        ps[j-1], ps[j] = ps[j], ps[j-1]
            if ps[0].y > ps[1].y:
                pts[0] = ps[0]
                pts[3] = ps[1]
            else:
                pts[0] = ps[1]
                pts[3] = ps[0]
            if ps[2].y > ps[3].y:
                pts[1] = ps[2]
                pts[2] = ps[3]
            else:
                pts[1] = ps[3]
                pts[2] = ps[2]

        def assertFEqual(p1, p2, precision=1e-6):
            self.assertTrue((abs(p1-p2) < precision), "%r != %r" % (p1, p2))

        coefficiente = random
        coefficiente.seed(1)
        lunghezza = random
        lunghezza.seed(2)
        x_finale = random
        x_finale.seed(3)
        y_finale = random
        y_finale.seed(4)
        p_iniziale = P(0, 0)
        for i in range(100): 
            x = lunghezza.randint(5, 30)
            y = coefficiente.randint(0, 50) * x
            p_finale1 = P(x, y)
            
            p_finale2 = P(x_finale.randint(10, 50), y_finale.randint(10, 50))
            dir = p_finale1 - p_iniziale
            p1 = p_finale2 - dir
            punto1 = p_finale1 - geo2d.ortho(dir)

            p1, _, _ = geo2d.intersRette(p1, p_finale2, p_finale1, punto1)
           
            p2 = 2 * p_finale1 - p1
            p3 = p2 - 2 * (p_finale1 - p_iniziale)
            p4 = p1 - 2 * (p_finale1 - p_iniziale)
            
            bbox = list(geo2d.MinimumBB([p1, p2, p3, p4]).getRect())
            ordinaPunti(bbox)
            pts = [p1, p2, p3, p4]
            ordinaPunti(pts)
            for j in range(4):
                dist_box = geo2d.dist(bbox[j], bbox[(j+1) % 4])
                dist_pts = geo2d.dist(pts[j], pts[(j+1) % 4])
                assertFEqual(dist_box / dist_pts, 1, 1e-2)



if __name__ == "__main__":
    unittest.main()
