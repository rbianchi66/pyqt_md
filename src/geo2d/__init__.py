#!/usr/bin/env python
#-*- coding: utf-8 -*-
from __future__ import division

import sys
if not hasattr(sys, "frozen"):
    import os
    from glob import glob

    d = os.path.dirname(__file__)
    if os.name == "posix":
        dlname = os.path.join(d, "_geo2dcpp.so")
    else:
        dlname = os.path.join(d, "_geo2dcpp.pyd")

    if not os.path.isfile(dlname):
        dltime = 0
    else:
        dltime = os.path.getmtime(dlname)

    # Directory e estensioni dei file da controllare
    _check = {'src': ['*.cpp', '*.h'],
              'sip': ['*.sip']}
    def what():
        for k, v in _check.items():
            for v1 in v:
                for f in glob(os.path.join(d, k, v1)):
                    yield f

    for f in what():
        if os.path.getmtime(f) >= dltime:
            print f
            raise RuntimeError, "%s non e' stato ricompilato -- lanciare Build.py" % os.path.basename(dlname)

import math
from offset import offset, para, para_smussa, simplify, interpola
from offset import getParallelPath, projectedLen, posFromDistOn

from _geo2dcpp import *

from trattopath import TrattoPath
from spline import *
import itertools

# Wrappa l'unisci del C++ perché non supporta i TrattoPath. Implementa inoltre
# il controllo di sanità sui path in ingresso (la versione in C++ fa
# semplicemente delle assert).
_unisci = unisci
def unisci(A, B, ta, tb):
    AP = geo2d.Path(A.path, True)
    BP = geo2d.Path(B.path, True)
    if abs(AP.len()) < 1E-20 or abs(BP.len()) < 1E-20:
        # FIXME: `RuntimeError` è coperto dal `from _geo2dcpp import *` (in mlbspi.sip)
        from exceptions import RuntimeError
        raise RuntimeError("path degenere")
    return _unisci(AP, A.t0, A.t1, BP, B.t0, B.t1, ta, tb)

# Usa ancora l'offset originale in Python, per ora
from offset import offset

__pychecker__ = "no-shadowbuiltin"

def dist_p_seg(p, a, b):
    """Distanza fra un punto p e il segmento a, b"""
    return dist(project(p, a, b), p)

def localMinimum(func, c, du, eps=0.01, n=1):
    """
    Cerca i minimi locali della funzione ``func`` in un intorno ``du``
    del punto ``c``.

    La funzione effettua una ricerca con un metodo di shooting e
    steepest descent. Il parametro n indica il numero di suddivisioni
    da assegnare a ``du`` per trovare i punti di partenza, con il
    default = 1 vengono fatti 9 tentativi (-du,0,+du)*(-du,0,+du).
    La funzione restituisce la lista di tutti i minimi locali trovati
    (alcuni di questi potrebbero essere coincidenti o quasi coincidenti).
    Per ogni minimo locale viene tornata la tupla (f(x, y), (x, y)).

    Per *ogni* punto iniziale viene effettuata una discesa per passi
    (ad ogni step vengono considerate 4 direzioni) con dimezzamento
    del passo al raggiungimento di un minimo locale sino a quando
    il passo diventa inferiore a ``eps``.

    :param eps: Errore massimo ammesso per il valore trovato.
    :type eps: float
    :param n: Numero di suddivisioni che formano la griglia.
    :type n: int
    :return: Lista minimi locali come NamedTuple (value=f(x,y), p=(x, y)).
    :rtype: lista di `NamedTuple` con attributi ``value`` e ``p``.
    """
    from misc.namedtuple import NamedTuple

    try:
        du = list(du)
    except TypeError:
        du = [du,du]
    if len(du) > 2:
        raise ValueError, 'piu\' di due valori per il parametro "du"'

    res = {}
    x0 = c.x - du[0]
    x1 = c.x + du[0]
    y0 = c.y - du[1]
    y1 = c.y + du[1]

    ND = 4
    dirs = [geo2d.P(math.cos(i*math.pi*2/ND), math.sin(i*math.pi*2/ND))
            for i in xrange(ND)]

    for y in xrange(-n, n+1):
        for x in xrange(-n, n+1):
            xx = c.x + x*du[0]/n
            yy = c.y + y*du[1]/n
            xs = du[0]/n/4
            ys = du[1]/n/4
            cp = (func(xx, yy), xx, yy)
            threshold = cp[0] - abs(cp[0])*0.0001
            while True:
                moved = False
                for dd in dirs:
                    dx = dd.x*xs
                    dy = dd.y*ys
                    tx = xx + dx
                    ty = yy + dy
                    if x0 <= tx < x1 and y0 <= ty <= y1:
                        t = func(tx, ty)
                        if t < threshold:
                            cp = (t, tx, ty)
                            threshold = cp[0] - abs(cp[0])*0.0001
                            moved = True
                if not moved:
                    if min(xs, ys) < eps:
                        break
                    xs *= 0.5
                    ys *= 0.5
                else:
                    xx, yy = cp[1:]

            k = (long(xx / eps), long(yy / eps))
            if k not in res or res[k].value > cp[0]:
                res[k] = NamedTuple(("value", cp[0]),
                                    ("p", geo2d.P(xx, yy)))
    return res.values()

def minimize(func, c, du, eps=0.01, n=1):
    """
    Trova il minimo assoluto scegliendolo fra i minimi locali
    restituiti da localMinimum
    """
    return min(localMinimum(func, c, du, eps, n))

def lineToPoints((a, b), step):
    """
    Crea una lista di punti appartenenti al segmento (a,b) distanziati di
    step.
    """
    r = b - a
    n = 1 + int(abs(r) / float(step))
    dr =  r / float(n)
    points = []
    for i in range(n):
        points.append(a + i * dr)
    points.append(b)
    return points

def len2(p):
    """Lunghezza di un vettore al quadrato"""
    return p*p

def dist2(a, b):
    """Distanza al quadrato fra due punti"""
    return len2(a-b)

def dist(a, b):
    """Distanza fra due punti"""
    return abs(a-b)

def same(p1, p2, prec=1E-6):
    """Vero se i punti sono coincidenti."""
    return dist(p1, p2) < prec

def sameSide(p1,p2, a,b):
    """
    Ritorna true se p1 e p2 sono dallo stesso lato, rispetto alla linea ab
    """
    cp1 = (b-a) ^ (p1-a)
    cp2 = (b-a) ^ (p2-a)
    return (cp1 * cp2) >= 0

def pointInTriangle(p, a,b,c):
    """
    Torna True se il punto p e' all'interno del triangolo A, B, C.
    """
    return sameSide(p,a,b,c) and sameSide(p,b,a,c) and sameSide(p,c,a,b)

def align(a1, a2, b1, b2):
    """
    Torna la matrice di allineamento, intesa come trasformazione che trasla
    rispetto ai punti medi di (a1, a2) e (b1, b2), ruota dell'angolo compreso
    tra i due vettori e non altera la scala.
    """

    am = (a1 + a2) / 2.0
    bm = (b1 + b2) / 2.0
    return xform(am, am + dir(a2 - a1),
                 bm, bm + dir(b2 - b1))

def geometriaCurvaX(C, t0, t1, minpts=100, maxerr=0.01):
    """
    Torna una spezzata che approssima la curva C fra i parametri
    t0 e t1 usando il numero minimo di suddivisioni indicato e
    aggiungendo punti quando la distanza fra il punto medio dei
    segmenti e il punto calcolato sulla media dei valori di parametro
    agli estremi del segmento supera la soglia indicata in maxerr.

        C        Curva (callable C(t) = punto 2d)
        t0       Valore iniziale del parametro
        t1       Valore finale del parametro
        minpts   Numero iniziale di punti da calcolare
        maxerr   Distanza massima ammessa

    La funzione restituisce due liste parallele; nella prima
    sono contenute le coordinate dei punti, nella seconda i
    valori di parametro t per cui i punti sono stati calcolati.
    """
    delta = float(t1 - t0)/minpts
    todo = [t0 + i*delta for i in xrange(minpts+1)]  # Valori di t da verificare
    todo.reverse()
    Xtodo = [C(t) for t in todo]  # Coordinate da verificare
    t = todo.pop()     # Valore corrente parametro
    T = [t]            # Lista valori parametro gia' elaborati
    X = [Xtodo.pop()]  # Lista coordinate gia' elaborate
    ERR2 = maxerr**2
    while todo:
        tm = (todo[-1] + t)*0.5
        pm = C(tm)
        if dist2(pm, (X[-1] + Xtodo[-1])*0.5) > ERR2:
            # Troppo grossolano, suddividi
            todo.append(tm)
            Xtodo.append(pm)
        else:
            X.append(Xtodo.pop())
            t = todo.pop()
            T.append(t)
    return X, T

def geometriaCurva(C, t0, t1, minpts=100, maxerr=0.01):
    """
    Come geometriaCurvaX ma restituisce solo la lista delle coordinate.
    """
    return geometriaCurvaX(C, t0, t1, minpts, maxerr)[0]

def circle(center, r, **kwargs):
    """
    Restituisce una spezzata che rappresenta un cerchio
    di centro `center` e raggio `r`.
    """
    def C(t):
        return center + r * geo2d.P(math.cos(t), math.sin(t))
    return geometriaCurva(C, 0, 2*math.pi, **kwargs)

def distRetta(p, a, b):
    """
    Distanza tra p e la retta passante per a, b con segno.
    Segno positivo se p sta alla sx della retta percorsa da a verso b,
    segno negativo se p sta alla dx.
    """
    delta = b - a
    l2 = len2(delta)
    if l2 < 1E-20:
        raise Exception("direzione indefinita: punti coincidenti")
    else:
        return (p - a)*dir(ortho(delta))

def retteParallele(p1, p2, p3, p4):
    """
    Torna True se le rette per p1, p2 e p3, p4 sono parallele
    """
    if abs((p2 - p1)^(p3 - p4)) < 1E-20:
        return True
    else:
        return False

def intersRette(p1, p2, p3, p4, eps=1E-20):
    """
    Torna il punto p di intersezione delle rette passanti per p1, p2 e p3, p4
    ed i parametri s e t rispettivamente della retta p2-p1 e p4-p3.
    s < 0.0 se p sta dal lato di p1 | t < 0.0 se p sta dal lato di p3
    s = 0.0 se p == p1              | t = 0.0 se p == p3
    0.0 < s < 1.0 se p1 < p < p2    | 0.0 < t < 1.0 se p3 < p < p4
    s = 1.0 se p == p2              | t = 1.0 se p == p4
    s > 1.0 se p sta dal lato di p2 | t > 1.0 se p sta dal lato di p4
    Vale per rette non parallele.
    """
    delta = (p2 - p1)^(p3 - p4)
    if abs(delta) < eps:
        raise ZeroDivisionError("intersezione indefinita: rette parallele")
    s = (p3 - p1)^(p3 - p4) / delta
    t = (p2 - p1)^(p3 - p2) / delta
    return (p1 + s*(p2 - p1), s, t)

def intersSegmenti(a1, a2, b1, b2):
    """
    Torna il punto di intersezione dei segmenti (a1, a2), (b1, b2), None se
    questo non esiste.
    """
    try:
        p, s, t = geo2d.intersRette(a1, a2, b1, b2)
    except ZeroDivisionError:
        return None

    if 0.0 < s < 1.0 and 0.0 < t < 1.0:
        return p
    return None

def intersRettaSegmento(r1, r2, a1, a2):
    """
    Torna il punto di intersezione della retta passante per (r1, r2) col
    segmento (a1, a2), None se questo non esiste.
    """
    try:
        p, s, t = geo2d.intersRette(r1, r2, a1, a2)
    except ZeroDivisionError:
        return None

    if 0.0 < t < 1.0:
        return p
    return None

def interSegm(a1, a2, b1, b2):
    """
    Torna il punto di intersezione dei segmenti (a1, a2), (b1, b2), e i parametri
    relativi s e t rispettivamente sui segmenti a2-a1 e b2-b1, None, s, t quando
    non esiste intersezione.
    NOTA. Considera intersezioni valide anche quando uno degli estremi sul primo
    segmento coincide con uno degli estremi sull'altro segmento.
    """
    try:
        p, s, t = geo2d.intersRette(a1, a2, b1, b2)
    except ZeroDivisionError:
        return None, None, None

    if 0.0 <= s <= 1.0 and 0.0 <= t <= 1.0:
        return p, s, t
    return None, s, t

def intersCircleSegment(cc, r, p1, p2):
    """
    Torna i punti di intersezione tra il cerchio di centro cc e raggio r
    con il segmento p1, p2.
    """
    dp = p2 - p1

    a = dp * dp
    b = 2 * (dp.x * (p1.x - cc.x) + dp.y * (p1.y - cc.y))
    c = ((cc * cc) + (p1 * p1)) - (2 * (cc * p1)) - (r * r)
    bb4ac = b * b - 4 * a * c

    EPS = 1E-10
    if abs(a) < EPS or bb4ac < 0:
        return []

    mu1 = (-b + math.sqrt(bb4ac)) / (2 * a)
    mu2 = (-b - math.sqrt(bb4ac)) / (2 * a)

    # No Intersection
    if (mu1 < 0 or mu1 > 1) and (mu2 < 0 or mu2 > 1):
        return []

    # One point on mu1
    if (mu1 > 0 and mu1 < 1) and (mu2 < 0 or mu2 > 1):
        return [p1 + (dp * mu1)]

    # One Point on m2
    if mu2 > 0 and mu2 < 1 and (mu1 < 0 or mu1 > 1):
        return [p1 + (dp * mu2)]

    if (abs(mu1 - mu2) < EPS):
        return [p1 + (dp * mu1)]

    return [p1 + (dp * mu1), p1 + (dp * mu2)]

def intersCirclePath(cc, r, path):
    """
    Torna i punti di intersezione tra il cerchio di centro cc e raggio r
    e il path specificato.
    """
    intersections = []
    for i in xrange(len(path) - 1):
        intersections.extend(intersCircleSegment(cc, r, path[i], path[i+1]))
    return intersections

def getCircleCenter(p1, p2, p3):
    """
    Torna il centro del cerchio che passa per i punti p1, p2, p3; se non è
    possibile calcolarlo solleva una ZeroDivisionError.
    Per evitare comportamenti strani nel caso in cui i 3 punti siano tutti su
    una retta controlla che questo non sia vero; in caso contrario sollava una
    ZeroDivisionError.
    """
    for perm1, perm2, perm3 in list(itertools.permutations([p1, p2, p3]))[::2]:

        # Case #21514: potrei avere i punti tutti sulla stessa retta, in questo
        # caso distRetta lancia un'eccezione.
        try:
            dist = distRetta(perm1, perm2, perm3)
        except Exception:
            continue

        if abs(distRetta(perm1, perm2, perm3)) <= 1e-9:
            raise ZeroDivisionError

    temp = p2.x * p2.x + p2.y * p2.y
    bc = (p1.x * p1.x + p1.y * p1.y - temp) / 2
    cd = (temp - p3.x * p3.x - p3.y * p3.y) / 2

    det = 1 / ( (p1.x - p2.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p2.y) )
    cx = (bc * (p2.y - p3.y) - cd * (p1.y - p2.y)) * det
    cy = ((p1.x - p2.x) * cd - (p2.x - p3.x) * bc) * det
    return geo2d.P(cx, cy)

_triangulate = triangulate
def triangulate(path):
    """
    Triangola ``path``, percorso chiuso concavo o convesso, ma senza nessuna
    auto-intersezione. Torna una lista di triple dei punti (vertici dei
    triangoli).
    La triangolazione usa l'algoritmo ``earcut`` ed è implementata nel module
    `earcut.cpp`.
    """
    tris = _triangulate(path)

    # Dereferenzia la lista di indici creando una lista di triple di punti.
    assert (len(tris) % 3) == 0
    ret = [None] * (len(tris) // 3)
    for n,i in enumerate(xrange(0, len(tris), 3)):
        ret[n] = path[tris[i]], path[tris[i+1]], path[tris[i+2]]
    return ret

def baryCoords(p, a, b, c):
    """
    Torna le coordinate baricentriche u, v (t = 1.0 - u - v)
    del punto p nel triangolo a, b, c.
            p = (1.0 - u - v) * a + u * b + v * c
    """

    x1, y1 = b.x, b.y
    x2, y2 = c.x, c.y
    x3, y3 = a.x, a.y
    x, y = p.x, p.y

    det = (x1 - x3)*(y2 - y3) - (y1 - y3)*(x2 - x3)
    u = ( (y2 - y3)*( x - x3) - (x2 - x3)*( y - y3) ) / det
    v = (-(y1 - y3)*( x - x3) + (x1 - x3)*( y - y3) ) / det

    return u, v

class Interp3:
    """
    Interpolatore spline cubica: data una serie di punti di coordinate
    xi (float) e yi (elementi di uno spazio vettoriale sui reali) una
    istanza di questa classe calcola una interpolazione dei valori
    tramite una spline cubica a tratti.
    Il metodo __call__(x) restituisce il valore associato al valore di
    ascissa specificato.

    Si assume che i valori xi siano distinti e crescenti.
    La funzione non estrapola (per x<xi[0] restituisce yi[0] e
    per x>xi[-1] restituisce yi[-1]).

    E' anche possibile specificare un parametro di rigidita' K della
    cubica utilizzata per interpolare.
    """
    def __init__(self, xi, yi, K=1.25):
        self.xi = xi[:]
        self.yi = yi[:]
        self.abcd = []
        for i in xrange(len(xi)-1):
            i0 = max(0, i-1)
            i1 = min(i+2, len(self.xi)-1)
            y0 = self.yi[i]
            y1 = self.yi[i+1]
            dy0 = (y1 - self.yi[i0]) / K
            dy1 = (self.yi[i1] - y0) / K
            if i == 0:
                dy0 *= 0
            if i == len(xi) - 2:
                dy1 *= 0
            a = dy1 + dy0 - 2*(y1 - y0)
            b = y1 - y0 - dy0 - a
            c = dy0
            d = y0
            self.abcd.append((a, b, c, d))

    def __call__(self, x):
        if x <= self.xi[0]:
            return self.yi[0]
        elif x >= self.xi[-1]:
            return self.yi[-1]
        i = 0
        while x > self.xi[i+1]:
            i += 1
        t = (x - self.xi[i])/(self.xi[i+1] - self.xi[i])
        a, b, c, d = self.abcd[i]
        return ((a*t + b)*t + c)*t + d


def monotoneCubicSpline(x, y):
    # adapted from:
    # http://stat.ethz.ch/R-manual/R-patched/library/stats/html/splinefun.html
    # R/src/library/stats/src/monoSpl.c

    n = len(x)
    assert n == len(y)
    if n == 0:
        return lambda xi: 0.0
    elif n == 1:
        const = y[0]
        return lambda xi: const

    # ordina per x,y
    xy = sorted(zip(x, y))
    x = [xi for xi,yi in xy]
    y = [yi for xi,yi in xy]

    # Calcola derivate e pendenze
    m = [0]*n
    delta = [0]*(n-1)
    for i in range(n-1):
        delta[i] = (y[i + 1] - y[i]) / (x[i + 1] - x[i])
        if i > 0:
            m[i] = (delta[i - 1] + delta[i]) / 2
    m[0] = delta[0]
    m[-1] = delta[-1]

    # Fix delle pendenze
    for i in range(n-1):
        if abs(delta[i]) < 0.01:
            m[i] = m[i+1] = 0.0
        else:
            alpha = m[i] / delta[i]
            beta  = m[i+1] / delta[i]
            a2b3 = 2*alpha + beta - 3
            ab23 = alpha + 2*beta - 3
            if (a2b3 > 0) and (ab23 > 0) and (alpha * (a2b3 + ab23) < a2b3*a2b3):
                tau = 3*delta[i] / math.sqrt(alpha**2 + beta**2)
                m[i]   = tau * alpha
                m[i+1] = tau * beta

    def interpolate(xi):
        for i in range(n - 1)[::-1]:
            if x[i] <= xi:
                break
        h = x[i + 1] - x[i]
        t = (xi - x[i]) / h
        t2 = t**2
        t3 = t**3
        h00 =  2 * t3 - 3 * t2 + 1
        h10 =      t3 - 2 * t2 + t
        h01 = -2 * t3 + 3 * t2
        h11 =      t3  -    t2
        yi = h00 * y[i] + \
             h10 * h * m[i] + \
             h01 * y[i + 1] + \
             h11 * h * m[i + 1]
        return yi

    return interpolate

def pathLC(c, s, p, closed=False):
    """
    Calcola le coordinate locali t/n di un punto rispetto
    ad un punto di un path
    """
    rp = p - c(s)
    try:
        tg = c.tan(s, closed)
    except ValueError:
        tg = c.tanl(s)
    return rp*tg, rp*ortho(tg)

def pathGC(c, s, u, v, closed=False):
    """
    Calcola la posizione assoluta di un punto specificato in
    coordinate locali t/n rispetto ad un punto di un path
    """
    try:
        tg = c.tan(s, closed)
    except ValueError:
        tg = c.tanl(s)
    return c(s) + u*tg + v*ortho(tg)

def fletti(pts, locked, spigoli, chiusa):
    """
    Calcolo linea spline modificata (funzione "fletti")
        pts     lista punti 2d in ingresso
        locked  dizionario indice->coordinate per i punti
                bloccati in una nuova posizione
        spigoli indici dei punti spigoli in pts
        chiusa  True se la linea é chiusa
    viene restituita una lista di punti 2d di lunghezza pari
    a pts contenente le nuove coordinate dei punti base.
    """
    if not locked:
        # Nessun punto bloccato -> curva originale
        pts2 = pts[:]
    elif len(locked) == 1:
        # Un solo punto bloccato -> spostamento costante in coordinate locali
        s, sx = complexSplineX(pts, spigoli, closed=chiusa)
        sp = Path(s)
        L = [sp.len(ix) for ix in sx]
        i0 = locked.keys()[0]
        p0 = locked[i0]
        u, v = pathLC(sp, L[i0], p0, closed=chiusa)
        pts2 = [pathGC(sp, L[i], u, v, closed=chiusa) for i in xrange(len(pts))]
    else:
        # Piu' punti bloccati -> interpola gli scostamenti in
        # coordinate locali di path e applica i valori interpolati
        # ai punti liberi
        si = locked.items()
        si.sort()
        s, sx = complexSplineX(pts, spigoli, closed=chiusa)
        sp = Path(s)
        L = [sp.len(ix) for ix in sx]
        lcs = [(i, pathLC(sp, L[i], p, closed=chiusa)) for i, p in si]
        ii = Interp3([L[i] for i,_ in lcs],
                     [P(u, v) for i,(u,v) in lcs])
        pts2 = []
        for i in xrange(len(pts)):
            t = L[i]
            v = ii(t)
            pts2.append(pathGC(sp, t, v.x, v.y, closed=chiusa))
    return pts2

def freccia(p1, p2, pd1=None, pd2=None):
    """
    Torna la geometria della freccia che congiunge p1 e p2, sotto forma di due
    liste di punti: la prima descrive la linea di connessione, la seconda
    descrive la punta (triangolino).

    I parametri opzionali pd1 e pd2 rappresentano le tangenti ai punti p1 e p2
    sulle rispettive linee di appartenenza.
    """

    # Se i due punti sono coincidenti è inutile proseguire.
    try:
        dir(p2 - p1)
    except ZeroDivisionError:
        return ([p1, p2], [p1, p2, p1])

    c = p1
    c_ = p2

    if pd1 is None:
        pd1 = p1
    if pd2 is None:
        pd2 = p2

    try:
        n12 = dir(ortho(pd1 - p1)) * abs(c - c_)/2
        n12_ = dir(ortho(pd2 - p2)) * abs(c - c_)/2
    except ZeroDivisionError:
        n12 = n12_ = dir(p2 - p1)

    linea = [c] + geo2d.bezto(c, c+n12, c_-n12_, c_, 10)
    linea = Path(linea)

    tp = linea.len() - 5

    # Se non ha abbastanza rincorsa per calcolare la direzione della punta, ne
    # torna una che tiene conto solo del punto iniziale e finale.
    if tp > 0.0:
        pt = linea(tp)
        pdir = ortho(linea.tan(tp))
    else:
        pt = linea[0]
        pdir = ortho(dir(linea[-1] - linea[0]))

    punta = [pt+pdir*3, linea[-1], pt-pdir*3]

    return linea, punta

def sostituisci(p, closed, t):
    """
    Sostituzione un tratto di un percorso eventualmente chiuso "p" con un altro
    tratto "t". La sostituzione avviene proiettando gli estremi di "t" su "p" ed
    eliminando la parte compresa oppure, nel caso che il contorno sia chiuso,
    la piu' corta fra le due parti delimitate da questi estremi.
    Restituisce un nuovo `geo2d.Path`.

    Nota. Il primo e l'ultimo punto della parte innestata vengono proiettati sul
        contorno e le coordinate di questi punti possono quindi cambiare.
    """
    assert isinstance(p, geo2d.Path)
    assert isinstance(t, geo2d.Path)
    assert closed, "Sostituisci non supporta la fusione su tratti aperti"

    pr_a = p.project(t[0])
    pr_b = p.project(t[-1])
    t = list(t)

    if pr_a.t > pr_b.t:
        t.reverse()
        temp = pr_a
        pr_a = pr_b
        pr_b = temp

    new_path = list(p)

    lab = abs(pr_b.t - pr_a.t)
    lba = p.len() - lab

    if lab >= lba:
        new_path = list(p.segment(pr_a.t, pr_b.t)) + t[-2:0:-1]
    else:
        new_path = list(p.segment(0, pr_a.t)) + t[1:-1] + list(p.segment(pr_b.t, p.len()))

    # Si assicura di ricreare un percorso chiuso
    if dist(new_path[0], new_path[-1]) > 1E-10:
        new_path.append(new_path[0])
    else:
        new_path[-1] = new_path[0]

    # Todo: valutare se e' veramente necessario semplificare il path
    new_path = simplify(new_path)

    return Path(new_path)

def check_overlaps(contorni1, contorni2, eps=0.5):
    """
    Controlla se due contorni sono sovrapposti
    """
    it1 = geo2d.InsideTester(contorni1, eps)
    it2 = geo2d.InsideTester(contorni2, eps)
    # Controlla se il contorno esterno di un pezzo è contenuto
    # (anche parzialmente) nei contorni di un altro con regola
    # odd-even, o viceversa.
    return it1.contains(contorni2[0], True) or it2.contains(contorni1[0], True)

def check_most_inside(punti, contorno):
    """
    Verifica che la maggior parte dei `punti` sia interna al `contorno`. Se si
    torna True, False altrimenti.
    """
    interno = 0
    for p in punti:
        if geo2d.inside(p, contorno):
            interno += 1
    if interno > int(len(punti) / 2):
        return True
    return False

def ordinaContorni(contorni, n=1):
    """
    Data una lista di contorni, ritorna una lista di liste degli indici dei
    contorni nella lista originale. Il primo elemento contiene la lista di
    tutti gli indici dei contorni non contenuti in nessun altro (livello 0).
    Il secondo contiene gli indici dei contorni contenuti in un solo contorno
    (livello 1), e così via.
    Notare che gli elementi dispari contengono i buchi, i pari gli altri.

    Il contorno A e' contenuto nel contorno B se lo e' la maggior parte degli `n`
    punti "equidistanti" presi sul bordo di A". In questo modo si riesce a dare
    una valutazione di contenimento anche per contorni che si intersecano.

    Esempio:
       ___________
     _|_______  B |     consideriamo A contenuto in B, anche se i contorni
    | | A     |   |     si intersecano, perche' la "maggior parte di A" e'
    |_|_______|   |     contenuta in B.
      |___________|

    """
    #FIXME: l'algoritmo andrebbe ottimizzato perchè quadratico. Dalle prove fatte
    # non sembrerebbe comunque un problema.

    assert type(n) == int
    assert n >= 1

    res = []
    for i, c in enumerate(contorni):
        nr_contenuto = 0
        punti = set()

        if n > 1:
            nc = geo2d.interpola(c, 5.0)
            step = len(nc) / n
            for j in xrange(n):
                punti.add(nc[int(j*step)])
        else:
            punti.add(c[0])

        for c2 in contorni:
            if c is not c2 and check_most_inside(punti, c2):
                nr_contenuto += 1
        while len(res) < nr_contenuto + 1:
            res.append([])
        res[nr_contenuto].append(i)
    return res

def arco(c, r, angolo=0, ampiezza=2*math.pi):
    # Calcola l'angolo di suddivisione che garantisce che la
    # distanza fra la corda e la circonferenza non sia mai
    # superiore a 0.01mm facendo comunque in modo che la
    # circonferenza abbia sempre almeno 12 lati.
    # Per derivarla si parte da r*(1-cos(a/2))<0.01.

    a = 2*math.acos(1 - 0.01/max(r, 0.3))

    N = max(abs(int(ampiezza/a)), 1)
    k = ampiezza/float(N)
    return [ c + r * geo2d.P(math.cos(i*k+angolo), math.sin(i*k+angolo))
             for i in xrange(N+1) ]

HUGE = 8e13
def barc(p1, p2, bulge, eps=0.02):
    if abs(p2 - p1)*abs(bulge) < eps:
        return [p1, p2]
    elif abs(bulge) > HUGE or abs(bulge) < eps:
        return [p1, p2]
    else:
        m = (p1 + p2) / 2 + geo2d.ortho(p2 - p1) * bulge / 2
        new_bulge = math.tan(math.atan(bulge) / 2)
        return (barc(p1, m, new_bulge, eps) +
                barc(m, p2, new_bulge, eps)[1:])

def circlearc(p1, v1, p2, v2):
    """
    Genera un raccordo ad arco circolare tra p1 e p2, usando come direzioni
    iniziali v1 e v2.
    """

    L = geo2d.dist(p1, p2)
    xx = geo2d.dir(p2 - p1)

    cosalpha1 = v1 * xx
    cosalpha2 = v2 * xx

    k1 = L * (2 / 3)*(1 - cosalpha1) / (1 - cosalpha1 * cosalpha1)
    k2 = L * (2 / 3)*(1 - cosalpha2) / (1 - cosalpha2 * cosalpha2)

    result = [p1]
    result += geo2d.bezto(p1,
                          p1 + v1 * k1,
                          p2 - v2 * k2,
                          p2)
    return result

def getAnglePointsIndexes(pts, ams, angle_on_straigth):
    """
    Data `pts`, lista di geo2d.P, calcola gli indici dei punti spigolo interni
    (angolo interno > `ams`). Se angle_on_straigth è True aggiunge ulteriori
    spigoli alla fine e all'inizio di lunghi rettilinei.
    Torna la lista degli indici.
    """

    def calcSpigoliAms(pts, ams):
        """
        Calcola solo gli indici dei punti spigolo interni (angolo interno > `ams`).
        """
        sp = [0]
        for i in xrange(1, len(pts)-1):
            dir0 = geo2d.dir(pts[i] - pts[i-1])
            dir1 = geo2d.dir(pts[i+1] - pts[i])
            a = geo2d.angle(dir0, dir1)
            if abs(a) > ams:
                sp.append(i)
        sp.append(len(pts)-1)
        return sp

    def calcSpigoli(pts, ams):
        """
        Calcola gli indici dei punti spigolo interni e aggiunge eventuali altri
        spigoli alla fine e all'inizio di lunghi rettilinei.
        """
        lung = 0.0
        prev_a = 0.0
        sp = [0]
        for i in xrange(1, len(pts)-1):

            dir0 = geo2d.dir(pts[i] - pts[i-1])
            dir1 = geo2d.dir(pts[i+1] - pts[i])
            a = geo2d.angle(dir0, dir1)

            # Se ci troviamo nell'intervallo tra 0.0 e ams, penalizza l'incremento
            # di lunghezza in modo da rendere meno probabile l'introduzione di
            # spigoli (siamo sul "lungo")
            incr = geo2d.dist(pts[i], pts[i-1])
            if 0.0 <= prev_a <= ams:
                incr *= (1 - (prev_a / ams))
            lung += incr
            prev_a = a

            val = abs(a) * 0.2 * lung / geo2d.dist(pts[i+1], pts[i])

            # Aggiunge uno spigolo se
            # A) abbiamo oltrepassato l'angolo limite ams, oppure
            # B) arriviamo da un tratto lungo, quasi dritto e stiamo iniziando a
            # curvare
            if abs(a) > ams or \
                val   > ams:
                sp.append(i)
                lung = 0.0
                prev_a = 0.0
                continue

        sp.append(len(pts)-1)
        return sp

    if (angle_on_straigth):
        sp1 = calcSpigoli(pts, ams)
        sp2 = calcSpigoli(list(reversed(pts)), ams)
        sp2 = [(len(pts)-1) - s for s in sp2]

        sp = sp1 + sp2
        sp = list(set(sp))
        sp.sort()
        return sp
    else:
        return calcSpigoliAms(pts, ams)

def dashPath(p, t, a):
    """
    Dato il geo2d.Path `p` lo spezza in tratti di ugual lunghezza e torna la
    lista dei tratti. `t` è la lunghezza massima dei tratti e `a` è la
    lughezza dell'interruzione (distanza tra un tratto e il successivo).
    *Nota*: I tratti tornati sono liste di geo2d.P.
    """
    assert isinstance(p, geo2d.Path)
    tp = []
    lp = p.len()
    n_tratti = max(1, int(round((lp + a) / (t + a))))
    c = t
    if n_tratti == 1:
        c = lp
    ti = 0.0
    for i in xrange(n_tratti):
        tp.append(geo2d.TrattoPath(p, ti, ti + c).calcPts())
        ti += (c + a)
    return tp

def clip(pts, n, k):
    """
    Clipping di un poligno convesso al semipiano definito
    da n * p + k >= 0.
    """
    npts = []
    for i in xrange(len(pts)):
        d0 = pts[i - 1] * n + k
        d1 = pts[i] * n + k
        if d0 >= 0:
            npts.append(pts[i - 1])
        if d0 < 0 <= d1 or d1 < 0 <= d0:
            t = d0 / (d0 - d1)
            npts.append(pts[i - 1] + t*(pts[i] - pts[i - 1]))
    return npts

def versoreDirezione(p0, p1):
    """
    Ritorna il versore della direzione tra p0 e p1
    Se i due punti sono uguali torna P(1, 0)
    """
    distanza = dist(p0, p1)
    if distanza == 0:
        return P(1, 0)

    return (p1 - p0) / distanza

def versoreOrtogonale(p0, p1):
    """
    Ritorna il versore ortogonale alla direzione che va tra p0 e p1
    Se i due punti sono uguali torna P(0, 1)
    """
    distanza = dist(p0, p1)
    if distanza == 0:
        return P(0, 1)

    return ortho(p1 - p0) / distanza

def puntiAllineati(p0, p1, p2, epsilon = 1e-10):
    """
    Ritorna se i tre punti passati sono allineati.
    """
    return abs(p0.x*(p1.y - p2.y) + p1.x*(p2.y - p0.y) + p2.x*(p0.y - p1.y)) < epsilon
