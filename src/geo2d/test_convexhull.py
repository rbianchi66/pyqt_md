#!/usr/bin/env python
#-*- coding: utf-8 -*-
from __future__ import division
import math
import geo2d
import random
from geo2d import inside_equal
from geo2d import convex_hull
import unittest

P = geo2d.P
Path = geo2d.Path


# Quando possibile sono stati inseriti test, sia per la versione di convex_hull
# che prende in input una nuvola di punti, sia per la versione che prende
# in input un Path rappresentante un poligono semplice chiuso.
class TestConvexHull(unittest.TestCase):

    # Se passo una nuvola di 2 punti, il path deve essere composto
    # proprio di questi 2 punti
    def testTwoPoint(self):
        pts = [P(0,0), P(1,1)]
        CH = convex_hull(pts)
        for p in pts:
            self.assertTrue(p in CH)

    def testTwoPointByPath(self):
        pts = Path([P(0,0), P(1,1)])
        CH = convex_hull(pts)
        for p in pts:
            self.assertTrue(p in CH)


    # Se passo una nuvola di 3 punti, il path deve essere composto
    # proprio di questi 3 punti
    def testThreePoint(self):
        pts = [P(0,0), P(1,1), P(1,0)]
        CH = convex_hull(pts)
        for p in pts:
            self.assertTrue(p in CH)

    def testThreePointByPath(self):
        pts = Path([P(0,0), P(1,1), P(1,0)])
        CH = convex_hull(pts)
        for p in pts:
            self.assertTrue(p in CH)


    # Se passo una nuvola di 1 punto, il path deve essere composto
    # solo di questo punto
    def testOnePoint(self):
        CH = convex_hull([P(0,0)])
        self.assertTrue(len(CH) == 1)
        self.assertTrue(CH[0] == P(0,0))

    def testOnePointbyPath(self):
        CH = convex_hull(Path([P(0,0)]))
        self.assertTrue(len(CH) == 1)
        self.assertTrue(CH[0] == P(0,0))


    # Se passo una nuvola di punti vuota, il path risultante
    # dovra' essere vuoto
    def testEmptySetOfPoints(self):
        CH = convex_hull([])
        self.assertTrue(len(CH) == 0)

    # Calcolo della convex hull di un quadrato, come nuvola di punti
    def testSquareByPoints(self):
        pts = [P(0,0), P(1,0), P(1,1), P(0,1), P(0,0)]
        CH = convex_hull(pts)
        for p in pts:
            self.assertTrue(p in CH)

    # Calcolo della convex hull di un quadrato, come poligono semplice
    def testSquareByPath(self):
        pts = Path([P(0,0), P(1,0), P(1,1), P(0,1), P(0,0)])
        CH = convex_hull(pts)
        for p in pts:
            self.assertTrue(p in CH)

    # Calcolo della convex hull di un quadrato con una concavita',
    # come nuvola di punti. Il risultato deve essere un quadrato senza
    # la concavita'
    def testPittedSquareByPoints(self):
        square = [P(0,0), P(1,0), P(1,1), P(0,1), P(0,0)]
        pitted_square = [P(0,0), P(1,0), P(0.5,0.5), P(1,1), P(0,1), P(0,0)]
        CH = convex_hull(pitted_square)
        for p in square:
            self.assertTrue(p in CH)
        self.assertTrue(P(0.5,0.5) not in CH)

    # Calcolo della convex hull di un quadrato con una concavita',
    # come poligono semplice. Il risultato deve essere un quadrato senza
    # la concavita'
    def testPittedSquareByPath(self):
        square = Path([P(0,0), P(1,0), P(1,1), P(0,1), P(0,0)])
        pitted_square = Path([P(0,0), P(1,0), P(0.5,0.5), P(1,1), P(0,1), P(0,0)])
        CH = convex_hull(pitted_square)
        for p in square:
            self.assertTrue(p in CH)
        self.assertTrue(P(0.5,0.5) not in CH)

    # Calcolo della convex hull di una nuvola di punti generata casualmente.
    # Ogni punto deve essere contenuto nel poligono ritornato da convex_hull
    def testRandomSetOfPoint(self):
        random.seed(123)
        rand = random.random
        npoints = 100
        scale_f = 20.0
        pts = []
        for n in range(npoints):
            pt = P(scale_f*(rand()-0.5), scale_f*(rand()-0.5))
            pts.append(pt)
        CH = convex_hull(pts)
        for p in pts:
            is_inside_with_overlap = inside_equal(p, CH, True)
            is_inside_strict = inside_equal(p, CH, False)
            self.assertTrue(is_inside_with_overlap)
            if is_inside_with_overlap and not is_inside_strict:
                self.assertTrue(p in CH)

    def testKochSnowflakePath(self):
        # L'algoritmo di Malkman a volte restituisce un numero di punti diverso
        # dall'agoritmo Monotone chain e comunque in diverso ordine.
        for level in range(5):
            snowflake = snowflake_path(level)
            self.assertEqual(len(snowflake), 3*4**level+1)
            CHpath = convex_hull(snowflake)
            CHpoints = convex_hull(snowflake.points())
            for p in CHpath:
                self.assertTrue(inside_equal(p, CHpoints, True))
            for p in CHpoints:
                self.assertTrue(inside_equal(p, CHpath, True))

# http://popefelix.dreamwidth.org/330614.html
def snowflake_coords(level, slen=1.0, origin=(0,0)):
    def snowflake_code():
        code = "FRRFRRF"
        for i in range(level):
            code = code.replace("F", "FLFRRFLF")
        return code
    points = []
    angle = 0 # degrees
    angleinc = 60 # angle increment, degrees
    vertex = origin
    points.append(origin)
    for c in snowflake_code():
        if c is 'F':
            x = vertex[0] + slen * math.sin(math.radians(angle))
            y = vertex[1] + slen * math.cos(math.radians(angle))
            vertex = (x,y)
            points.append(vertex)
        elif c is 'R':
            angle += angleinc
        elif c is 'L':
            angle -= angleinc
    return points
def snowflake_points(level, slen=1.0, origin=(0,0)):
    return [P(x,y) for (x,y) in snowflake_coords(level, slen, origin)]
def snowflake_path(level, slen=1.0, origin=(0,0)):
    return Path(snowflake_points(level, slen, origin))


if __name__ == "__main__":
    unittest.main()
