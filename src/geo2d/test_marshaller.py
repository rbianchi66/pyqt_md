import unittest

try:
    from comcom2 import C2Stream, LocalMemoryManager, registerMarshaller
except ImportError:
    pass
else:
    import geo2d.marshaller
    import geo2d.marshaller
    
    registerMarshaller(geo2d.marshaller)
    l = LocalMemoryManager()
    class TestGeo2dMarshallers(unittest.TestCase):
        def testP2d(self):
            s = C2Stream(l)
            s.write_P2d(geo2d.P(4,5))
            self.assertEqual(s.read_P2d(), geo2d.P(4,5))

        def testX2d(self):
            s = C2Stream(l)
            X = geo2d.mirror(geo2d.P(2,4), geo2d.P(3,90))
            s.write_X2d(X)
            X2 = s.read_X2d()
            self.assertEqual(X.a, X2.a)
            self.assertEqual(X.b, X2.b)
            self.assertEqual(X.c, X2.c)

        def testVectorP2d(self):
            s = C2Stream(l)
            s.write_std_vector_P2d([geo2d.P(4,5)])
            self.assertEqual(s.read_std_vector_P2d(), [geo2d.P(4,5)])

if __name__ == "__main__":
    unittest.main()
