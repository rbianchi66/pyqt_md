#!/usr/bin/env python
#-*- coding: utf-8 -*-
import os
import unittest
from elementtree import ElementTree
from transupdate import FixEncoding, TSFile, VerifyError

def indent(s):
    return "\n".join(["\t" + L for L in s.split("\n")])

def genTS(*contexts):
    s = ("<!DOCTYPE TS>\n<TS>\n" +
            indent("\n".join(contexts)) +
            "\n</TS>")

    import cStringIO
    return TSFile(cStringIO.StringIO(s), False)

def genCTX(name, *msgs):
    return ('<context encoding="UTF-8"><name>%s</name>\n' % name +
            indent("\n".join(msgs)) +
            "\n</context>")

def genMSG(src, cmt, trans="", transtype=None):
    if transtype is None:
        if trans:
            transtype = "finished"
        else:
            transtype = "unfinished"

    return ('<message encoding="UTF-8">\n' +
            "\t<source>%s</source>\n" % src +
            "\t<comment>%s</comment>\n" % cmt +
            '\t<translation type="%s">%s</translation>\n' % (transtype, trans) +
            "</message>\n")

def genMSGOBS(src, cmt, trans=""):
    return genMSG(src, cmt, trans, "obsolete")


class TestMerge(unittest.TestCase):

    def assertTreeEqual(self, n1, n2):
        self.assertEqual(n1.text, n2.text)
        self.assertEqual(n1.attrib, n2.attrib)
        self.assertEqual(len(n1.getchildren()), len(n2.getchildren()))

        # Algoritmo quadratico per ora, me ne frega il giusto
        n2c = n2.getchildren()[:]
        for c1 in n1.getchildren():
            for c2 in n2c:
                try:
                    self.assertTreeEqual(c1, c2)
                except self.failureException:
                    continue
                else:
                    n2c.remove(c2)
                    break
            else:
                self.fail("the XML trees do not match:\nGOT:\n%s\nEXPECTED:\n%s" %
                          (ElementTree.tostring(n1), ElementTree.tostring(n2)))

    def assertMerge(self, clean_obsolete, cur, new, result):
        cur.merge(new, clean_obsolete)
        self.assertTreeEqual(cur.xml.getroot(), result.xml.getroot())

    def testObsolete(self):
        """Un messaggio deve essere marcato come obsoleto."""
        self.assertMerge(False,
            genTS(genCTX("prova",genMSG("Ciao", "CODE"))),

            genTS(genCTX("prova",genMSG("Ciao2", "CODE2"))),

            genTS(genCTX("prova",genMSGOBS("Ciao", "CODE"),
                                 genMSG("Ciao2", "CODE2")))
        )

    def testObsoleteClean(self):
        """Un messaggio deve venir cancellato quando obsoleto (e richiesto cosi')."""
        self.assertMerge(True,
            genTS(genCTX("prova",genMSG("Ciao", "CODE"))),

            genTS(genCTX("prova",genMSG("Ciao2", "CODE2"))),

            genTS(genCTX("prova",genMSG("Ciao2", "CODE2")))
        )

    def testUpdateText(self):
        """Un messaggio deve venire aggiornato correttamente."""
        self.assertMerge(True,
            genTS(genCTX("prova",genMSG("Ciao", "CODE", "tradotto"))),

            genTS(genCTX("prova",genMSG("Ciao2", "CODE"))),

            genTS(genCTX("prova",genMSG("Ciao2", "CODE", "tradotto")))
        )

    def testNewMessages(self):
        """Un messaggio deve essere aggiunto se nuovo."""
        self.assertMerge(True,
            genTS(genCTX("prova",genMSG("Ciao", "CODE", "tradotto"))),

            genTS(genCTX("prova",genMSG("Ciao", "CODE"),
                                 genMSG("Ciao2", "CODE2"))),

            genTS(genCTX("prova",genMSG("Ciao", "CODE", "tradotto"),
                                 genMSG("Ciao2", "CODE2")))
        )

    def testDeobsolete(self):
        """Un messaggio era obsoleto ma ora non lo e' piu' (senza traduzione)."""
        self.assertMerge(True,
            genTS(genCTX("prova",genMSGOBS("Ciao", "CODE"))),

            genTS(genCTX("prova",genMSG("Pippo", "CODE"))),

            genTS(genCTX("prova",genMSG("Pippo", "CODE")))
        )

    def testDeobsolete2(self):
        """Un messaggio era obsoleto ma ora non lo e' piu' (con traduzione)."""
        self.assertMerge(True,
            genTS(genCTX("prova",genMSGOBS("Ciao", "CODE", "tradotto"))),

            genTS(genCTX("prova",genMSG("Pippo", "CODE"))),

            genTS(genCTX("prova",genMSG("Pippo", "CODE", "tradotto", "unfinished")))
        )

    def testAddContext(self):
        """Aggiunta di nuovo contesto."""
        self.assertMerge(True,
            genTS(genCTX("prova",genMSG("Ciao", "CODE"))),

            genTS(genCTX("prova",genMSG("Ciao", "CODE")),
                  genCTX("prova2", genMSG("Ciao2", "CODE2"))),

            genTS(genCTX("prova",genMSG("Ciao", "CODE")),
                  genCTX("prova2", genMSG("Ciao2", "CODE2")))
        )


class TestFixEncoding(unittest.TestCase):
    filename = "test.xml"

    def setUp(self):
        if os.path.isfile(self.filename):
            self.fail("impossibile eseguire il test (cancellare il file '%s')" % self.filename)

    def runFixEncoding(self, data):
        f = file(self.filename, "w")
        f.write(data)
        f.close()

        FixEncoding(self.filename)

        self.assertValidUtf8(file(self.filename).read())

    def assertValidUtf8(self, data):
        try:
            data.decode("utf-8")
        except UnicodeDecodeError:
            self.fail("expected utf-8 data")

    def testSingleEntity(self):
        self.runFixEncoding("""
                            <comment>Si &#xc3;&#xa8; trovato un errore imprevisto!
                            L&apos;applicazione verr&#xc3;&#xa0; terminata.</comment>
                            """)

    def tearDown(self):
        if os.path.isfile(self.filename):
            os.remove(self.filename)


class TestDuplicates(unittest.TestCase):

    def testDup(self):
        """Controllo di messaggi con stesso codice."""
        ts = genTS(genCTX("prova",genMSG("Ciao", "CODE"),
                                  genMSG("Pippo", "CODE")))
        self.assertRaises(VerifyError, ts.verifyDuplicates)

    def testNoDup(self):
        """Controllo di messaggi con stesso codice ma in contesti diversi."""
        ts = genTS(genCTX("prova",genMSG("Ciao", "CODE")),
                   genCTX("prova2",genMSG("Pippo", "CODE")))
        try:
            ts.verifyDuplicates()
        except VerifyError:
            self.fail("non ci sono messaggi duplicati da segnalare")

if __name__ == "__main__":
    unittest.main()
