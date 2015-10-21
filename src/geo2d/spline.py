#!/usr/bin/env python
#-*- coding: utf-8 -*-
"""
Funzioni per calcolo di spline e affini.
"""
import geo2d

def spline(pts, levels=6, closed=0, iters=20, kk=1.62):
    """
    Calcolo spline interpolante:
      pts           lista dei punti da interpolare
      levels=6      numero livelli di suddivisione
      closed=false  true se la spline e' chiusa
      iters=20      numero iterazioni per calcolo interpolazione
      kk=1.62       coefficiente di rilassamento

    Il valore di ritorno sono una lista di coordinate.
    """
    return geo2d.splineX(pts, levels, closed, iters, kk)[0]

def complexSplineX(pts, spigoli, levels=6, closed=0, iters=20, kk=1.62, tipo_spline="CALIGOLA"):
    """
    Confronta realtime il risultato tra la versione C e quella PYTHON
    """
    pt = pts[:]

##    ap, bp = complexSplineXP(pt, spigoli, levels, closed, iters, kk, tipo_spline)
    ap, bp = geo2d.complexSplineXC(pts, spigoli, levels, closed, iters, kk, tipo_spline=="CALIGOLA")

    return ap, bp

def complexSplineXP(pts, spigoli, levels=6, closed=0, iters=20, kk=1.62, tipo_spline="CALIGOLA"):
    """
    Calcola una linea complessa:
      pts           lista punti da interpolare
      spigoli       lista degli indici dei punti spigolo
      levels=6      numero livelli di suddivisione tratti spline
      closed=false  true se la linea e' chiusa
      iters=20      numero iterazioni per calcolo interpolazione tratti spline
      kk=1.62       coefficiente di rilassamento tratti spline

    Restituisce una lista di punti e una lista di tanti interi quanti sono
    gli elementi di pts che contiene l'indice del punto risultante associato
    per ciascuno dei punti in ingresso.

    In input, una linea chiusa deve essere passata *senza* duplicazione del
    primo/ultimo punto. Nel valore di ritorno, invece, il punto risulterà
    duplicato. FIXME: potremmo correggere questa asimmetria.

    La lista di mapping ritornata è sicuramente ordinata, ma può contenere
    duplicati: infatti, è possibile che due punti geometricamente sovrapposti
    nella linea di input corrispondano ad un solo punto nella spline di output.
    """
    assert len(spigoli) <= len(pts), \
        "Numero di spigoli maggiore di quello dei punti: %r > %r" % (len(spigoli), len(pts))

    spigoli = sorted(spigoli)
    if not closed:
        # Se la linea e' aperta allora il primo e l'ultimo punto
        # sono sempre considerati punti spigolo
        if 0 not in spigoli:
            spigoli = [0] + spigoli
        if len(pts)-1 not in spigoli:
            spigoli = spigoli + [len(pts)-1]

    if not spigoli:
        # Linea morbida chiusa, usa una singola spline
        if tipo_spline == "CALCAD":
            pts.append(pts[0])  # aggiungo il primo punto in fondo
            res, ix = geo2d.oldSplineX(pts)
        else:
            res, ix = geo2d.splineX(pts, levels, closed, iters, kk)
    #
    else:
        if closed:
            spigoli = [spigoli[-1]] + spigoli
        i0 = spigoli.pop(0)
        res = [pts[i0]]
        ix = {i0: 0}
        while spigoli:
            i1 = spigoli.pop(0)
            pp = [pts[i0]]
            ii = [i0]
            i = (i0 + 1) % len(pts)
            while i != i1:
                pp.append(pts[i])
                ii.append(i)
                i = (i + 1) % len(pts)
            pp.append(pts[i1])
            ii.append(i1)
            if len(pp) == 2:
                # Tratto rettilineo
                ix[i1] = len(res)
                res.append(pts[i1])
            else:
                # Tratto spline
                if tipo_spline == "CALCAD":
                    if pp[0] == pp[-1]:     # se punto iniziale a spigolo
                        pp.append(pp[0])    # aggiungo (ancora) il primo punto in fondo
                    ts, tix = geo2d.oldSplineX(pp)
                else:
                    ts, tix = geo2d.splineX(pp, levels, 0, iters, kk)
                for t, i in zip(tix,ii):
                    v = len(res) + t - 1
                    ix[i] = v
                res.extend(ts[1:])
            i0 = i1

        # Ruota per allineare risultato con pts (necessario solo
        # per spline chiuse con spigoli in cui il primo punto non
        # sia un punto spigolo)
        xx = ix[0]
        if xx > 0:
            res = res[xx:-1] + res[:xx+1]
            print 'res s p', len(res)
            for i in xrange(len(pts)):
                a = (ix[i] - xx) % len(res)
                ix[i] = a

            # A seguito della rotazione, due punti sovrapposti possono risultare
            # essere il primo e l'ultimo, nel qual caso ix non sarebbe più
            # ordinata.
            if ix[len(pts)-1] == 0:
                ix[len(pts)-1] = len(res)-1

        # converte il dict in lista
        nix = [None] * len(pts)
        for i, v in ix.items():
            nix[i] = v
        ix = nix

    sres = [res[0]]
    six = [ix[0]]

    # L'algoritmo di semplificazione per tratto funziona solo se ix è ordinata
    assert sorted(ix) == ix
    for i in xrange(1, len(ix)):
        if ix[i-1] != ix[i]:
            del sres[-1]
            s = res[ix[i-1]:ix[i]+1]
            assert len(s) > 0
            sres.extend(geo2d.simplify(s))
        six.append(len(sres) - 1)
    if closed:
        del sres[-1]
        sres.extend(geo2d.simplify(res[ix[-1]:]))
    return sres, six

def complexSpline(pts, spigoli, levels=6, closed=0, iters=20, kk=1.62):
    """
    Calcola una linea complessa:
      pts           lista punti da interpolare
      spigoli       lista degli indici dei punti spigolo
      levels=6      numero livelli di suddivisione tratti spline
      closed=false  true se la linea e' chiusa
      iters=20      numero iterazioni per calcolo interpolazione tratti spline
      kk=1.62       coefficiente di rilassamento tratti spline

    Restituisce una lista di punti.
    """
    return complexSplineX(pts, spigoli, levels, closed, iters, kk)[0]

def xispline(pts, ams, eps=0.05, angle_on_straigth=True):
    """
    Calcola i punti base di una linea spline con spigoli che
    approssima i punti passati in pts, inserendo un punto spigolo quando
    l'angolo tra due segmenti è maggiore di ams, fino a raggiungere
    precisione eps. Restituisce la lista dei punti e base e una lista con gli
    indici relativi ai punti spigolo.
    """
    # Semplifica la linea in input (questo toglie anche i punti doppi)
    pts = geo2d.simplify(pts, eps/2)
    # Calcola gli indici dei punti spigolo interni
    sp = geo2d.getAnglePointsIndexes(pts, ams, angle_on_straigth)
    # Calcola il risultato finale
    res = [pts[0]]
    fsp = []
    for i in xrange(0, len(sp)-1):
        s = geo2d.ispline(geo2d.Path(pts[sp[i]:sp[i+1]+1]))
        res.extend(s[1:])
        fsp.append(len(res)-1) # Salva l'indice (in res!!) del punto spigolo

    del fsp[-1] # Elimina l'ultimo indice di spigolo

    auto_closed = geo2d.same(res[0], res[-1])

    # Se il primo e l'ultimo punto sono uguali, cancello l'ultimo
    if auto_closed:
        del res[-1]
        fsp.insert(0,0) # In questo caso l'inizio e' sempre punto spigolo

    return res, fsp, auto_closed

def bspline(pts, levels):
    while levels > 0:
        n = pts.__len__()
        pts2 = [pts[0]]
        for i in xrange(1, n-1):
            pts2.append((pts[i-1]+pts[i])/2)
            pts2.append((pts[i-1]+6*pts[i]+pts[i+1])/8)
        pts2.append((pts[-1]+pts[-2])/2)
        pts2.append(pts[-1])
        levels -= 1
        pts = pts2
    return pts
