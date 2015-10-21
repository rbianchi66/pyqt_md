#!/usr/bin/env python
#-*- coding: utf-8 -*-
import unittest
import geo2d
import gc

P = geo2d.P
Path = geo2d.Path

class TestMemory(unittest.TestCase):

    def setUp(self):
        if Path.get_alloc_count() == -1L:
            try: #2.7 and later
                self.skipTest("geo2d sip extension is not compiled in DEBUG mode")
            except:
                print "Skipping memory test: geo2d sip extension is not compiled in DEBUG mode"

    def testMapVecPath(self):
        pathcnt = Path.get_alloc_count()
        tmp = geo2d.mapvecpath_test(1000,100)
        tmp = geo2d.mapvecpath_test(1000,100)
        del tmp
        self.assertEqual(Path.get_alloc_count(), pathcnt)
    def testVecVecPath(self):
        pathcnt = Path.get_alloc_count()
        tmp = geo2d.vecvecpath_test(1000,100)
        tmp = geo2d.vecvecpath_test(1000,100)
        del tmp
        self.assertEqual(Path.get_alloc_count(), pathcnt)

    def testVecPath(self):
        pathcnt = Path.get_alloc_count()
        tmp = geo2d.vecpath_test(1000,100)
        tmp = geo2d.vecpath_test(1000,100)
        del tmp
        self.assertEqual(Path.get_alloc_count(), pathcnt)

if __name__ == "__main__":
    unittest.main()
