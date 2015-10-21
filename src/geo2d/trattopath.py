#!/usr/bin/env python
#-*- coding: utf-8 -*-
import operator
import geo2d

class TrattoPath(tuple):
    """
    Classe immutabile che implementa la rappresentazione di un tratto su Path
    nella forma (path, t0, t1), dove:

    * path è un geo2d.Path
    * (t0, t1) sono i parametri di lunghezza relativi al geo2d.Path che
      individuano gli estremi del tratto sul Path

    Si può costruire un TrattoPath specificandone gli estremi tramite parametri
    di lunghezza sul path. E' possibile accedere a (path, t0, t1), oltre che
    coi modi delle tuple, anche tramite property.

    :note: Bisogna fare molta attenzione all'uso della path.project nel codice.
    Nel caso delle unioni, accade molto spesso di lavorare con tratti
    con sovrapposizione interna. In quei casi, l'uso di project è sempre
    errata.

    :note: Sarebbe bello imporre l'invariante t0 < t1, che semplificherebbe
    notevolmente i calcoli. C'è però da fare attenzione: è vero che è sempre
    possibile imporla (perchè, in caso di problemi, basta rovesciare il path
    sottostante per ottenere un TrattoPath di geometria equivalente ma che
    rispetti t0 < t1), ma ci sono dei casi in cui rovesciare il path rompe
    alcune parti del codice. Per esempio, l'inseguimento del percorso assume
    di poter sempre fare t2len/len2t, ma questo non è vero se i path sottostanti
    ai TrattoPath vengono rovesciati "a piacere".
    """
    def __new__(cls, *args):
        assert isinstance(args[0], geo2d.Path)
        if len(args) == 1:
            path = args[0]
            return tuple.__new__(cls, (path, 0, path.len()))
        elif len(args) == 3:
            path, t0, t1 = args
            return tuple.__new__(cls, (path, t0, t1))
        else:
            raise TypeError, "Combinazione di parametri %r non supportata" % args

    path = property(operator.itemgetter(0))
    t0 = property(operator.itemgetter(1))
    t1 = property(operator.itemgetter(2))
    p0 = property(lambda self: self.path(self.t0))
    p1 = property(lambda self: self.path(self.t1))

    def __repr__(self):
        return "TrattoPath(%s, %s, %s)" % (repr(self.path), self.t0, self.t1)

    def trim(self):
        """
        Restituisce un TrattoPath che contiene solo la porzione di path
        specificata dai parametri di lunghezza
        """
        a = min(self.t0, self.t1)
        b = max(self.t0, self.t1)
        path = self.path.segment(a, b)
        c1 = 0
        c2 = path.len()
        if self.t0 < self.t1:
            return TrattoPath(path, c1, c2)
        else:
            return TrattoPath(path, c2, c1)

    def calcPath(self):
        """
        Restituisce il geo2d.Path rappresentato dal TrattoPath
        """
        return self.path.segment(self.t0, self.t1)

    def calcPts(self):
        """
        Restituisce la lista dei punti che compongono questo TrattoPath
        """
        g = self.path.segment(self.t0, self.t1)
        pts = list(g)
        return pts

    def close(self):
        """
        Restituisce un nuovo TrattoPath ricavato dalla chiusura sugli estremi
        dell'attuale.
        """
        pts = self.calcPts()
        pts.append(pts[0])
        retp = geo2d.Path(pts)
        return TrattoPath(retp)

    def reversed(self):
        """
        Restituisce un nuovo TrattoPath geometricamente equivalente all'attuale,
        ma orientato nella direzione opposta.
        """
        path = self.path.reversed()
        t0 = path.len() - self.t0
        t1 = path.len() - self.t1
        return TrattoPath(path, t1, t0)

    def contains(self, p):
        """
        Se p è di tipo geo2d.P_: torna True se il punto p sta sul tratto, False
        altrimenti.

        Se p e' un parametro: torna True se il parametro e' compreso
        tra i parametri del tratto.
        """
        t0, t1 = min(self.t0, self.t1), max(self.t0, self.t1)
        if isinstance(p, geo2d.P):
            proj = self.path.project(p)
            return abs(proj.dist) < 1E-20 and t0 <= proj.t <= t1
        elif isinstance(p, float):
            return t0 <= p <= t1
        else:
            raise TypeError, "parametro di tipo %r non supportato" % type(p)

    def split(self, t):
        """
        Spezza questo `TrattoPath` in corrispondenza del parametro di lunghezza
        ``t``, e torna i due `TrattoPath` risultanti orientati dal punto in cui si
        è spezzato verso l'esterno.
        """
        t0, t1 = min(self.t0, self.t1), max(self.t0, self.t1)
        assert t0 <= t <= t1

        path1 = self.path.segment(t, 0)
        path2 = self.path.segment(t, self.path.len())

        return TrattoPath(path1, 0, t-t0), TrattoPath(path2, 0, t1-t)

    def len(self):
        """
        Torna la lunghezza di questo `TrattoPath`.
        """
        return abs(self.t1 - self.t0)

    def prolunga(self, du):
        """
        Torna un TrattoPath prolungato dalla parte di ``t1`` di un ``du``
        specificato.
        """
        if self.t0 < self.t1:
            return TrattoPath(self.path, self.t0, self.t1 + du)
        else:
            return TrattoPath(self.path, self.t0, self.t1 - du)

    @staticmethod
    def fromDbTratto(t, delta=None):
        """
        Factory method che dato un `db.Tratto` ne ricava la rappresentazione
        `TrattoPath`. Se specificato, `delta` rappresenta (in millimetri)
        quanta parte di linea originale in eccesso rispetto al tratto
        bisogna conservare.
        """
        if delta is None:
            return TrattoPath(t.geometria())

        linea = t.linea
        swap = False
        t0, t1 = t.t0, t.t1
        if t0 > t1:
            t0,t1 = t1,t0
            swap = True
        lenl = linea.geometria().len()
        lent = linea.lunghezzaTratto(t0, t1)

        # Normalizza il primo parametro e lo converte in lunghezza
        t0 = linea.normalize(t0)
        t0 = linea.t2len(t0)

        margin0 = delta
        margin1 = delta

        # Calcola le lunghezze rappresentanti il tratto esteso di "delta" mm.
        # Bisogna fare attenzione perché il dominio parametrico di una linea
        # aperta *non* ammette valori fuori dal range. Dobbiamo quindi
        # fare clamp per evitare di sbucare dalla linea.
        nt0 = t0 - margin0
        if not linea.chiusa and nt0 < 0:
            margin0 = t0
            nt0 = 0

        nt1 = t0 + lent + margin1
        if not linea.chiusa and nt1 >= lenl:
            margin1 = lenl - lent - t0
            nt1 = lenl

        nt0 = linea.len2t(nt0)
        nt1 = linea.len2t(nt1)

        if swap:
            nt0,nt1 = nt1,nt0
            margin0,margin1 = margin1,margin0
        path = geo2d.Path(linea.geometriaTratto(nt0, nt1))
        return TrattoPath(path, margin0, path.len() - margin1)
