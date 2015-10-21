#!/usr/bin/env python
#-*- coding: utf-8 -*-
from __future__ import division
import math
import geo2d
from random import *
from geo2d import inside_equal, rects_cover, circle
import unittest

P = geo2d.P
Path = geo2d.Path

# Genera il Path equivalente ad un rettangolo coprente (cr)
def crToPath(cr):
    return Path([P(cr.x,cr.y), P(cr.x+cr.w, cr.y), P(cr.x+cr.w, cr.y+cr.h), P(cr.x, cr.y+cr.h), P(cr.x,cr.y)])

class TestRectsCover(unittest.TestCase):
    
    # Cerca 2 rettangoli ricoprenti un area rettangolare.
    # Entrambi devono essere contenuti nell'area originale
    def testTwoSquareInSquare(self):
        sq = Path([P(0,0), P(500,0), P(500,500), P(0,500), P(0,0)]);
        res_squares = rects_cover(sq, 2, 10, True)
        self.assertTrue(len(res_squares) == 2)
        for sqs in res_squares:
            for p in crToPath(sqs):
                self.assertTrue( sqs.w == 0 or sqs.h == 0 or inside_equal(p, sq) )
    
    # Cerca il piu' grande rettangolo ricoprente per un'area rettangolare
    def testSquareInSquare(self):
        sq = Path([P(0,0), P(500,0), P(500,500), P(0,500), P(0,0)]);
        res_squares = rects_cover(sq, 1)
        self.assertTrue(len(res_squares) == 1)
        for p in crToPath(res_squares[0]):
            self.assertTrue( inside_equal(p, sq) )
            
    # Chiama la funzione, richiedendo 0 rettangoli ricoprenti:
    # il vettore di rettangoli ricoprenti risultante dovra essere vuoto
    def testZeroSquareCall(self):
        sq = Path([P(0,0), P(500,0), P(500,500), P(0,500), P(0,0)]);
        res_squares = rects_cover(sq, 0)
        self.assertTrue(len(res_squares) == 0)
    
    
    # Ricerca due rettangoli contenuti in un cerchio di raggio 10
    def testSquareInCircleSafe(self):
        ci = Path(circle(P(0,0),10))
        res_squares = rects_cover(ci, 2, 10, True)
        for sqs in res_squares:
            for p in crToPath(sqs):
                #Se il rettangolo e' degenere, allora non e' detto che sia interno al cerchio
                self.assertTrue( sqs.w == 0 or sqs.h == 0 or inside_equal(p, ci) )
                    
                
if __name__ == "__main__":
    unittest.main()
