#!/usr/bin/env python
#-*- coding: utf-8 -*-
from __future__ import division
import math

import geo2d
from _geo2dcpp import para_smussaf, paraf, simplifyf

def simplify(*args, **kwargs):
    return list(simplifyf(*args, **kwargs))

def para(pts, dist, chiusa):
    """
    Calcola la parallela "sporca" (con auto-intersezioni) alla
    spezzata `pts` in ingresso.
    """
    pts = geo2d.Path(pts)
    # TODO: questo controllo verrà spostato dentro il SIP
    if pts.len() == 0:
        raise ValueError("parallela di path puntiforme")
    return list(paraf(pts, dist, chiusa))

def para_smussa(pts, dist, chiusa):
    """
    Calcola la parallela "sporca" (con auto-intersezioni) alla
    spezzata `pts` in ingresso, smussando gli spigoli.
    """
    pts = geo2d.Path(pts)
    # TODO: questo controllo verrà spostato dentro il SIP
    if pts.len() == 0:
        raise ValueError("parallela smussata di path puntiforme")
    return list(para_smussaf(pts, dist, chiusa))

def interpola(pts, d):
    """
    Data una spezzata aggiunge i punti necessari a fare in modo che la
    risultante abbia tutti i punti a distanza al più `d` tra un punto e
    il successivo.
    """
    if d <= 0:
        raise ValueError
    pts = geo2d.simplify(pts, 1/100.0)
    res = []
    res.append(pts[0])
    for i in xrange(len(pts) - 1):
        delta = pts[i+1] - pts[i]
        n = 1 + int(abs(delta) / d)
        for j in xrange(n):
            t = (j + 1.0) / n
            res.append(pts[i] + t*delta)
    return geo2d.Path(res)

def offset(pts, d, tipo="SPIGOLO_VIVO", soglia_unibili=1.0):

    # Semplifica la linea in input (questo toglie anche i punti doppi)
    pts = geo2d.simplify(pts, soglia_unibili/50.0)

    if max(abs(dd) for dd in d) < soglia_unibili*0.6:
        # Caso speciale per offset a distanza piccolissima:
        #   calcola il risultato considerando semplicemente lo
        #   spostamento punto a punto lungo la bisettrice delle
        #   normali dei segmenti incidenti. La distanza usata
        #   viene modificata per evitare di "tagliare le curve".
        #   Il risultato finale e' una parallela a "spigolo vivo"
        #   corretta a meno delle auto-intersezioni e/o saltelli.

        # In realtà esiste un caso in cui a questo punto ci sono ancora
        # dei punti doppi consecutivi, ovvero quello in cui siamo
        # rimasti con due punti coincidenti.  In questo caso non è
        # chiaro cosa sia l'offset, per cui torniamo il path di partenza
        # come fa anche la offset di geo2dcpp
        if len(pts) < 3 and geo2d.same(pts[0], pts[1]):
            return [geo2d.Path(pts)]

        chiusa = abs(pts[0] - pts[-1]) < 0.0001

        res = []
        pts = geo2d.Path(pts)

        # Gli utilizzatori attuali della offset si aspettano che le curve
        # chiuse vengano sempre espanse verso l'esterno con offset positivo
        # e verso l'interno con offset negativo; ora, il lato scelto dalla
        # funzione offset dipende dal verso della curva, ma visto che non
        # vogliamo modificarlo solo per mantenere questa invariante, per
        # ottenere lo stesso risultato inverte il segno dell'offset richiesto.
        s = 1.0
        if chiusa and geo2d.sarea(pts) > 0:
            s = -1.0

        for dd in d:
            dd *= s

            off = []

            for i in xrange(0, len(pts)):
                ddir = geo2d.nd(pts, i, dd, chiusa)
                off.append(pts[i] + ddir)

            res.append(geo2d.Path(off))
    else:
        tipi = dict(SPIGOLO_VIVO=0, SMUSSATA=1)
        res = geo2d.offsetf(pts, d, tipi[tipo], soglia_unibili)

    return res

def getParallelPath(L, d):
    """
    Dato il Path `L` torna il Path `parallelo` ad L a distanza `d`.
    Costruisce la parallela proiettando i punti della spezzata L a distanza d
    (corretta per l'angolo tra i segmenti) in direzione normale a L.
    """
    EPS = 1E-3
    pts = []
    for i, p in enumerate(L):
        s = L.len(i)
        try:
            t1 = geo2d.dir(L(s) - L(s - EPS))
        except ZeroDivisionError:
            t1 = geo2d.P(1, 0)
        try:
            t2 = geo2d.dir(L(s + EPS) - L(s))
        except ZeroDivisionError:
            t2 = geo2d.P(1, 0)
        n1 = geo2d.ortho(t1)
        n2 = geo2d.ortho(t2)
        n = geo2d.dir(n1 + n2) * d * math.sqrt(2.0 / (1.0 + n1 * n2 * 0.999))
        pts.append(p + n)

    return geo2d.Path(pts)

def projectedLen(geo1, geo2, pA=None, pB=None): # => len(geo1[pA:pB]) su geo2
    """
    Restituisce la lunghezza della proiezione di `geo1` su `geo2`.
    Se `pA` e `pB` sono diversi da None, considera le proiezioni di questi su
    `geo1` come estremi.

    L'implementazione attuale considera solo le proiezioni degli estremi di
    `geo1` su `geo2`.

    Questa lunghezza è «l'ombra» di `geo2` su `geo1` solo se le due geometrie
    sono affset l'una dell'altra, perchè la proiezione è normale a `geo2`.
    In breve questa funzione va bene per il fianco guidato solo se gli estremi
    sono localmente paralleli, altrimenti serve la versione che considera la
    proiezione ortogonale a `geo1`.

    geo2 ---.__________________________.---  La proiezione su geo2 è nella
         |                                |  stessa posizione dell'intersezione
    geo1 x--------------------------------x  con la normale uscente da geo1

            ____________________________
    geo2 .-°                            °-.  La proiezione su geo2 non è nella
          \                              /   stessa posizione dell'intersezione
    geo1 --x----------------------------x--  con la normale uscente da geo1

    """
    if pA is None and pB is None:
        p1a = geo1[0]
        p1b = geo1[-1]
    elif pA is not None and pB is not None:
        p1a = geo1.project(pA).p
        p1b = geo1.project(pB).p
    else:
        raise ValueError("E' stato definito solo l'estremo '%s' del tratto" % "pA" if pB is None else "pB")
    t2a = geo2.project(p1a).len
    t2b = geo2.project(p1b).len
    return geo2.segment(t2a, t2b).len()

def posFromDistOn(geo1, pA, geo2, dist): # => pB[P2d] su geo1
    """
    Restituisce un punto `pB` che è ad una distanza da `pA` tale da avere una
    proiezione su `geo2` lunga `dist`.
            ____________________________
    geo2 .-°                            °-. Le due proiezioni sono la prima
          \                               | verso `geo2` e la seconda verso
    geo1 --x------------------------------x `geo1` quindi hanno senso solo
                                            per path localmente paralleli.
    """
    p1a = geo1.project(pA).p
    t2a = geo2.project(p1a).len
    p2b = geo2(t2a + dist)

    return geo1.project(p2b).p

