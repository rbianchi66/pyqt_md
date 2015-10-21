#!/usr/bin/env python
#-*- coding: utf-8 -*-
from __future__ import division
import math
import geo2d
from geo2d import intersect
import unittest

P = geo2d.P


class TestIntersect(unittest.TestCase):

    def setUp(self):
        # Quadrato chiuso in 0,0 di lato 10
        self.p1 = geo2d.Path([P(0,0), P(10,0), P(10,10), P(0,10), P(0,0)])
        # Quadrato aperto in 1,1 di lato 5
        self.p2 = geo2d.Path([P(1,1), P(6,1), P(6,6), P(1,6)])
        # Rombo chiuso in 0,0 di diagonali 10 e 5
        self.p3 = geo2d.Path([P(0,0), P(2.5,5), P(0,10), P(-2.5,5), P(0,0)])
        # Linea chiusa disegnata a mano
        self.p4 = geo2d.Path([
                              P(436,128),P(436,129),P(434,130),P(430,131),P(427,133),P(425,134),P(421,134),
                              P(419,135),P(416,135),P(412,135),P(409,135),P(407,135),P(405,135),P(403,135),
                              P(402,136),P(401,137),P(399,138),P(397,140),P(395,141),P(392,143),P(390,145),
                              P(386,148),P(384,150),P(383,152),P(381,154),P(379,156),P(379,157),P(378,159),
                              P(378,163),P(378,165),P(378,168),P(378,170),P(378,172),P(380,175),P(380,177),
                              P(381,179),P(383,181),P(385,183),P(387,185),P(389,187),P(391,189),P(393,191),
                              P(397,194),P(402,196),P(415,199),P(428,202),P(434,204),P(439,206),P(454,206),
                              P(460,206),P(464,206),P(468,206),P(472,206),P(474,206),P(475,206),P(477,205),
                              P(478,203),P(480,203),P(482,202),P(484,200),P(485,199),P(490,197),P(492,195),
                              P(493,193),P(495,191),P(496,190),P(497,188),P(498,186),P(499,184),P(499,182),
                              P(500,180),P(501,177),P(502,175),P(503,173),P(503,171),P(503,169),P(503,167),
                              P(503,166),P(502,164),P(502,163),P(501,162),P(501,161),P(499,161),P(498,159),
                              P(497,159),P(495,158),P(492,158),P(488,158),P(485,158),P(482,158),P(478,158),
                              P(475,158),P(472,158),P(470,158),P(468,158),P(466,158),P(465,158),P(463,158),
                              P(462,158),P(461,158),P(460,158),P(459,158),P(458,158),P(456,158),P(454,158),
                              P(453,158),P(451,158),P(450,158),P(449,158),P(448,158),P(447,158),P(447,157),
                              P(446,157),P(445,157),P(445,156),P(444,156),P(443,156),P(442,155),P(441,154),
                              P(440,153),P(439,153),P(438,152),P(437,151),P(436,151),P(436,150),P(435,149),
                              P(434,149),P(434,148),P(433,148),P(433,147),P(432,147),P(432,146),P(431,146),
                              P(430,145),P(429,143),P(428,142),P(428,141),P(427,141),P(427,140),P(426,140),
                              P(426,139),P(425,139),P(424,139),P(424,138),P(423,138),P(423,137),P(423,136),
                              P(422,136),P(422,135),P(422,134),P(422,133),P(422,132),P(422,131),P(422,130),
                              P(422,129),P(423,129),P(423,128),P(424,128),P(425,127),P(426,126),P(426,125),
                              P(427,124),P(428,123),P(428,122),P(429,122),P(430,121),P(432,120),P(433,120),
                              P(434,119),P(435,118),P(437,118),P(438,117),P(439,117)
                              ])
    
    def testIntersect(self):
        """Quadrato aperto dentro quadrato chiuso"""
        self.assertEqual(geo2d.intersect(self.p1, self.p2), False)
        
    def testVerticiSuLato(self):
        self.p2 = self.p2*geo2d.xlate(P(-1,0))
        self.assertEqual(geo2d.intersect(self.p1, self.p2), True)
        self.assertEqual(geo2d.intersect(self.p1, self.p2, False), False)

    def testVerticiQuasiSuLato(self):
        self.p2 = self.p2*geo2d.xlate(P(-0.9999999,0))
        self.assertEqual(geo2d.intersect(self.p1, self.p2), False)
        
    def testLatoSuLato(self):
        self.p2 = self.p2*geo2d.xlate(P(0,-1))
        self.assertEqual(geo2d.intersect(self.p1, self.p2, False), False)
        self.assertEqual(geo2d.intersect(self.p1, self.p2, True), True)

    def testLatoQuasiSuLato(self):
        self.p2 = self.p2*geo2d.xlate(P(0,-0.99999999))
        self.assertEqual(geo2d.intersect(self.p1, self.p2), False)
    
    def testVerticeInComune(self):
        self.p3 = self.p3*geo2d.xlate(P(10,10))
        self.assertEqual(geo2d.intersect(self.p1, self.p3, True), True)
        self.assertEqual(geo2d.intersect(self.p1, self.p3, False), False)
        
    def testVerticeQuasiInComune(self):
        self.p3 = self.p3*geo2d.xlate(P(10.00000001,10.00000001))
        self.assertEqual(geo2d.intersect(self.p1, self.p3), False)
        
    def testSeStesso(self):
        self.assertEqual(geo2d.intersect(self.p1, self.p1, True), True)
        self.assertEqual(geo2d.intersect(self.p1, self.p1, False), False)
        self.assertEqual(geo2d.intersect(self.p4), True)
        
    def testDiagonale(self):
        self.p3 = self.p3*geo2d.xlate(P(11,6))
        self.assertEqual(geo2d.intersect(self.p1, self.p3), True)
        self.assertEqual(geo2d.intersect(self.p1, self.p3, False), True)


if __name__ == "__main__":
    unittest.main()
