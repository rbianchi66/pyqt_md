#!/usr/bin/env python
#-*- coding: utf-8 -*-

from __future__ import division
import unittest

from geo2d import P
import geo2d

class TestOrientedBB(unittest.TestCase):
    def testBBox(self):
        pts = [P(0, 6), P(10, 6), P(0, 0), P(10, 0)]
        bbox = geo2d.OrientedBB(pts, 0)

        for p in bbox.getRect():
            p = geo2d.P(int(p.x), int(p.y))
            self.assert_(p in pts)

    def testExtra(self):
        pts = [P(0, 6), P(10, 6), P(0, 0), P(10, 0)]
        bbox = geo2d.OrientedBB(pts, 2)

        epts = [P(12, 8), P(-2, 7), P(-1, -2), P(12, -1)]
        for p in bbox.getRect():
            p = geo2d.P(int(p.x), int(p.y))
            self.assert_(p in epts)

    def testMerge(self):
        pts1 = [P(0, 6), P(10, 6), P(0, 0), P(10, 0)]
        bbox1 = geo2d.OrientedBB(pts1, 0)

        pts2 = [P(12, 6), P(16, 6), P(16, 0), P(12, 0)]
        bbox2 = geo2d.OrientedBB(pts2, 0)

        pts = [P(0, 0), P(0, 6), P(16, 0), P(16, 6)]
        box = geo2d.OrientedBB([bbox1, bbox2])
        for p in box.getRect():
            p = geo2d.P(int(p.x), int(p.y))
            self.assert_(p in pts)

    def testXSect(self):
        pts1 = [P(0, 6), P(10, 6), P(0, 0), P(10, 0)]
        bbox1 = geo2d.OrientedBB(pts1, 0)

        pts2 = [P(12, 6), P(16, 6), P(16, 0), P(12, 0)]
        bbox2 = geo2d.OrientedBB(pts2, 0)
        self.assertFalse(bbox1.xsect(bbox2))

        pts2 = [P(8, 6), P(16, 6), P(16, 0), P(8, 0)]
        bbox2 = geo2d.OrientedBB(pts2, 0)
        self.assertTrue(bbox1.xsect(bbox2))

    def testContains(self):
        pts = [P(0, 6), P(10, 4), P(0, 0), P(10, -2)]
        bbox = geo2d.OrientedBB(pts, 0)

        self.assertTrue(bbox.test(P(0, 0)))
        self.assertTrue(bbox.test(P(3, 3)))
        self.assertFalse(bbox.test(P(-1, 0)))

if __name__ == "__main__":
    unittest.main()
