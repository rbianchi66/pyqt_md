#!/usr/bin/env python
#-*- coding: utf-8 -*-
import math
import geo2d
import unittest

class TestPart(unittest.TestCase):
    def testCostruttori(self):
        self.assertRaises(TypeError, lambda: geo2d.Placement.Part(312))
        p1 = geo2d.Path([geo2d.P(0,0), geo2d.P(1,0)])
        p2 = geo2d.Path([geo2d.P(1,0), geo2d.P(1,1)])
        part = geo2d.Placement.Part([p1, p2])

    def testShape(self):
        p1 = geo2d.Path([geo2d.P(0,0), geo2d.P(1,0)])
        p2 = geo2d.Path([geo2d.P(1,0), geo2d.P(1,1)])
        part = geo2d.Placement.Part([p1, p2])
        s = part.shape(0)
        self.assertEqual(type(s), geo2d.Placement.Shape)

class TestSheet(unittest.TestCase):
    def testCostruttori(self):
        self.assertRaises(TypeError, lambda: geo2d.Placement.Sheet([]))
        s1 = geo2d.Placement.Sheet(100, 200)
        s2 = geo2d.Placement.Sheet(100.0, 200.0)
        #self.assertEqual(s1, s2) #FIXME
        # manca __cmp__

    def testHeightWidth(self):
        s1 = geo2d.Placement.Sheet(100, 200)
        s2 = geo2d.Placement.Sheet(200, 100)
        self.assertEqual(s1.height(), s2.width())

    def testAddPartOK(self):
        s1 = geo2d.Placement.Sheet(100, 200)
        p1 = geo2d.Path([geo2d.P(0,0), geo2d.P(1,0),
                         geo2d.P(1,1), geo2d.P(0,0)])
        p2 = geo2d.Path([geo2d.P(0.5,0.5), geo2d.P(1,1)])
        part = geo2d.Placement.Part([p1, p2])
        res, xd2 = s1.addPart(part)
        self.assertEqual(res, True)
        self.assertEqual(type(xd2), geo2d.X)

    def testAddPartFAIL(self):
        s1 = geo2d.Placement.Sheet(1, 1)
        p1 = geo2d.Path([geo2d.P(0,0), geo2d.P(5,0),
                         geo2d.P(5,5), geo2d.P(0,0)])
        part = geo2d.Placement.Part([p1])
        res, xd2 = s1.addPart(part)
        self.assertEqual(res, False)
        self.assertEqual(repr(xd2), repr(geo2d.X())) #FIXME
        # manca __cmp__ ?

    def testAddPartBorderOK(self):
        s1 = geo2d.Placement.Sheet(11, 11)
        s1.setHborder(3)
        p1 = geo2d.Path([geo2d.P(0,0), geo2d.P(5,0),
                         geo2d.P(5,5), geo2d.P(0,0)])
        part = geo2d.Placement.Part([p1])
        res, xd2 = s1.addPart(part)
        self.assertEqual(res, True)
        self.assertEqual(type(xd2), geo2d.X)

    def testAddPartBorderFAIL(self):
        s1 = geo2d.Placement.Sheet(11, 11)
        s1.setHborder(5)
        p1 = geo2d.Path([geo2d.P(0,0), geo2d.P(5,0),
                         geo2d.P(5,5), geo2d.P(0,0)])
        part = geo2d.Placement.Part([p1])
        res, xd2 = s1.addPart(part)
        self.assertEqual(res, False)
        self.assertEqual(repr(xd2), repr(geo2d.X())) #FIXME
        # manca __cmp__ ?

if __name__ == "__main__":
    unittest.main()
