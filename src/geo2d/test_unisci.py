#!/usr/bin/env python
#-*- coding: utf-8 -*-
import unittest
import geo2d

TrattoPath = geo2d.TrattoPath
P = geo2d.P
Path = geo2d.Path

class TestUnisci(unittest.TestCase):
    """
    Test specifici di geo2d.unisci.

    La funzione è chiaramente esercitata anche da vari test nel DB.
    In particolare, db.test_possibile.py verifica tutta una serie di
    tratti in cui l'unione è considerata "ovvia".
    """
    def testCrashDirNull(self):
        A = TrattoPath(Path([P(200.0, 100.0), P(300.0, 0.0)]), 0.0, 141.421356237)
        B = TrattoPath(Path([P(0.0, 0.0), P(300.0, 0.0)]), 0.0, 300.0)
        ta = 141.421356237
        tb = 300.0
        geo2d.unisci(A, B, ta, tb)

    def testRaccordoInutileBug1747(self):
        """Controlla che non venga creato un raccordo inutilmente."""
        # Queste linee sono prese da testdata\rettangolo_smussato.cal,
        # e corrispondono a S1 (primo raccordo) e 1 (primo lato).
        A = TrattoPath(Path([P(200.04356107660465, 313.04761904761898), P(95.238095238095184, 313.04761904761898),
                             P(95.238095238095184, 186.5714285714285), P(323.80952380952385, 186.5714285714285),
                             P(323.80952380952385, 313.04761904761898), P(95.238095238095184, 313.04761904761898),
                             P(95.238095238095184, 186.5714285714285), P(323.80952380952385, 186.5714285714285),
                             P(323.80952380952385, 313.04761904761898),P(200.04356107660465, 313.04761904761898)]),
                       710.140952381, 1420.19047619)

        B = TrattoPath(Path([P(122.12532709866662, 313.04761904761898), P(121.70325272547056, 313.04210546112421),
                             P(121.28185866251293, 313.02565637090822), P(120.86121885317951, 312.99840928087349),
                             P(120.44140724085609, 312.96050169492258), P(120.0224977689285, 312.91207111695797),
                             P(119.60456438078252, 312.85325505088213), P(119.18768101980396, 312.78419100059762),
                             P(118.77192162937861, 312.70501647000691), P(118.35736015289227, 312.61586896301253),
                             P(117.94407053373074, 312.516885983517), P(117.53212671527983, 312.40820503542278),
                             P(117.12160264092532, 312.2899636226324), P(116.71257225405303, 312.16229924904837),
                             P(116.30510949804875, 312.02534941857317), P(115.89928831629828, 311.87925163510937),
                             P(115.49518265218742, 311.72414340255938), P(115.09286644910198, 311.56016222482577),
                             P(114.69241365042772, 311.38744560581102), P(114.2938981995505, 311.20613104941765),
                             P(113.89739403985607, 311.01635605954817), P(113.50297511473026, 310.81825814010506),
                             P(113.11071536755885, 310.61197479499089), P(112.72068874172766, 310.39764352810806),
                             P(112.33296918062247, 310.17540184335917), P(111.94763062762908, 309.94538724464667),
                             P(111.5647470261333, 309.70773723587308), P(111.18439231952092, 309.46258932094094),
                             P(110.80664045117776, 309.21008100375275), P(110.4315653644896, 308.95034978821093),
                             P(110.05924100284224, 308.68353317821806), P(109.68974130962148, 308.40976867767665),
                             P(109.32314022821313, 308.12919379048918), P(108.959511702003, 307.84194602055817),
                             P(108.59892967437685, 307.54816287178613), P(108.24146808872051, 307.24798184807554),
                             P(107.88720088841977, 306.94154045332897), P(107.53620201686044, 306.62897619144883),
                             P(107.18854541742832, 306.3104265663377), P(106.84430503350917, 305.98602908189804),
                             P(106.50355480848884, 305.65592124203232), P(106.16636868575311, 305.32024055064318),
                             P(105.83282060868778, 304.97912451163302), P(105.50298452067865, 304.63271062890436),
                             P(105.17693436511152, 304.28113640635974), P(104.85474408537219, 303.9245393479016),
                             P(104.53648762484646, 303.56305695743254), P(104.22223892692013, 303.19682673885501),
                             P(103.912071934979, 302.82598619607148), P(103.60606059240885, 302.45067283298448),
                             P(103.30427884259551, 302.07102415349658), P(103.00680062892476, 301.68717766151025),
                             P(102.71369989478242, 301.29927086092795), P(102.42505058355425, 300.9074412556522),
                             P(102.1409266386261, 300.51182634958553), P(101.86140200338374, 300.11256364663046),
                             P(101.58655062121298, 299.70979065068946), P(101.3164464354996, 299.30364486566504),
                             P(101.05116338962942, 298.89426379545972), P(100.79077542698823, 298.48178494397604),
                             P(100.53535649096185, 298.06634581511645), P(100.28498052493605, 297.64808391278342),
                             P(100.03972147229663, 297.22713674087959), P(99.799653276429424, 296.8036418033073),
                             P(99.564849880720203, 296.3777366039692), P(99.33538522855477, 295.94955864676768),
                             P(99.111333263318926, 295.51924543560534), P(98.892767928398484, 295.08693447438463),
                             P(98.679763167179217, 294.65276326700808), P(98.472392923046954, 294.21686931737821),
                             P(98.270731139387465, 293.77939012939748), P(98.074851759586579, 293.34046320696837),
                             P(97.884828727030069, 292.90022605399349), P(97.700735985103762, 292.45881617437527),
                             P(97.522647477193431, 292.01637107201628), P(97.350637146684889, 291.57302825081894),
                             P(97.184778936963937, 291.12892521468581), P(97.025146791416375, 290.68419946751936),
                             P(96.871814653428004, 290.23898851322213), P(96.724856466384608, 289.79342985569662),
                             P(96.584346173671989, 289.3476609988453), P(96.450357718675974, 288.90181944657076),
                             P(96.322965044782336, 288.45604270277539), P(96.202242095376874, 288.01046827136179),
                             P(96.088262813845404, 287.56523365623241), P(95.981101143573724, 287.12047636128978),
                             P(95.880831027947622, 286.67633389043641), P(95.78752641035291, 286.23294374757484),
                             P(95.701261234175362, 285.79044343660746), P(95.622109442800806, 285.34897046143692),
                             P(95.550144979615027, 284.90866232596562), P(95.48544178800384, 284.46965653409609),
                             P(95.42807381135303, 284.03209058973084), P(95.378114993048399, 283.5961019967724),
                             P(95.335639276475746, 283.16182825912324), P(95.300720605020871,282.72940688068593),
                             P(95.273432922069588, 282.29897536536288), P(95.253850171007684, 281.87067121705667),
                             P(95.242046295220945, 281.44463193966976), P(95.238095238095184, 281.02099503710468)]),
                    20.6335214636, 0.0)

        ta = 710.140952381
        tb = 20.6335214636

        # Prova ad unire. C'è un punto d'interesezione quasi perfetto, quindi
        # mi aspetterei che il punto risultante sia perfettamente adagiato
        # sui path (e quindi a distanza zero da entrambi i path).
        # In precedenza, l'algoritmo di unisci non riusciva a passare da
        # intersezione a baciata durante la discesa, e quindi si ostinava
        # a voler intersecare il raccordo.
        pi, ai, bi = geo2d.unisci(A, B, ta, tb)
        self.assertAlmostEqual(A.path.project(pi).d, 0, 4)
        self.assertAlmostEqual(B.path.project(pi).d, 0, 4)

    def testPathDegenere(self):
        """Controlla che la unisci() sollevi un RuntimeError sui path degeneri."""
        A = TrattoPath(
            Path([P(154.00206571178259, 136.95485136207836),
                  P(154.00206571178262, 136.95485136207833)]),
                  3.63291081585, 181.645540792)

        B = TrattoPath(Path([P(0,0), P(10,0)]), 5, 8)
        ta = 181.645540792
        tb = 181.645540792
        
        self.assertRaises(RuntimeError, lambda: geo2d.unisci(A, B, ta, tb))

if __name__ == "__main__":
    unittest.main()
