#!/usr/bin/env python
#-*- coding: utf-8 -*-
import unittest
import transrelease

class TestParseProject(unittest.TestCase):

    def parse(self, s):
        return transrelease.parseProject(s)

    def testBasic(self):
        """Controlla un semplice progetto su una linea."""
        d = self.parse("A = a ; B=b ;")
        self.assertEqual(len(d), 2)
        self.assert_("A" in d)
        self.assert_("B" in d)
        self.assertEqual(d["A"], "a")
        self.assertEqual(d["B"], "b")

    def testNewline(self):
        """Controlla l'utilizzo di newline al posto dei terminatori."""
        d = self.parse("A = a \nB=b \n")
        self.assertEqual(len(d), 2)
        self.assert_("A" in d)
        self.assert_("B" in d)
        self.assertEqual(d["A"], "a")
        self.assertEqual(d["B"], "b")

    def testBackslash(self):
        """Controlla l'utilizzo di backslash per continuare le linee."""
        d = self.parse("A = a \  \n b c ;")
        self.assertEqual(len(d), 1)
        self.assert_("A" in d)
        self.assertEqual(d["A"].split(), ["a", "b", "c"])

    def testLeadingSpaces(self):
        """Controlla gli spazi a inizio linea."""
        d = self.parse(" A = a;")
        self.assertEqual(len(d), 1)
        self.assert_("A" in d)
        self.assertEqual(d["A"], "a")

    def testTargets(self):
        """Controlla la presenza di target (ignorati)."""
        d = self.parse(" unix:A = a;")
        self.assertEqual(len(d), 1)
        self.assert_("A" in d)
        self.assertEqual(d["A"], "a")

    def testAddition(self):
        """Controlla l'utilizzo di +=."""
        d = self.parse("A=a; A+=b")
        self.assertEqual(len(d), 1)
        self.assert_("A" in d)
        self.assertEqual(d["A"].split(), ["a", "b"])


if __name__ == "__main__":
    unittest.main()
