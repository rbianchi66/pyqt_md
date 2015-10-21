#!/usr/bin/env python
#-*- coding: utf-8 -*-
"""
Plugin che installa eventuali dipendenze tramite easyinstall.

Le dipendenze sono contenute all'interno del solito build.cfg (formato Python).
Bisogna definire una lista chiamata "requires" contenente l'elenco dei pacchetti
richiesti (con definizione in formato EasyInstall, cioè comprendente anche un
vincolo opzionale sul numero di versione).

Eventuali dipendenze specifiche di una piattaforma sono definite usando normale
codice Python.

Esempio di frammento di build.cfg:
     =====================
     import os
     requires = [
        "ElementTree",
        "PyOpenGL >= 3.0",
        os.name == "nt" and "PyWin32" or "",
     ]
     =====================
"""
import sys
from common import *

priority = 5
solid = True

def find():
    return []   # ***** DISABILITA

    if "requires" in parseConfig("build.cfg"):
        return ["build.cfg"]
    return []

def addOptionGroup(parser):
    import optparse
    group = optparse.OptionGroup(parser, "plugin %s options" % name)
    group.add_option("--%s-reinstall" % name, default=False, action="store_true",
          help="Reinstall dependencies even if available")
    parser.add_option_group(group)

def install(files):
    infos = {}
    for f in files:
        ns = {}
        execfile(f, ns)
        for L in ns["requires"]:
            if not L: continue
            pkg_name = L.split()[0]
            infos.setdefault(pkg_name, []).append((f, L))

    todo = []
    for pkg in sorted(infos):
        reqs = infos[pkg]
        if len(set(L for (f,L) in reqs)) != 1:
            raise RuntimeError("different requirements for package %s: %r" % (pkg_name, reqs))
        f, L = reqs[0]
        todo.append(L)

    try:
        from ez_setup import main as ez_setup
        ez_setup([])
    except SystemExit:
        print
        print
        print "Aggiornato setuptools - eseguire nuovamente build o deploy"
        print
        print
        sys.exit(1)  # forza codice di errore, altrimenti deploy continua...

    import pkg_resources
    print sys.argv[0]

    for req in todo:
        try:
            pkt = pkg_resources.require(req)
        except pkg_resources.DistributionNotFound:
            # Il pacchetto non esiste, proviamo a scaricarlo
            system3(sys.executable, "-m", "easy_install",
                    "--always-unzip", req)
            # Controlla che la dipendenza sia soddisfatta in una nuova
            # sessione (quella corrente non è stata aggiornata dopo
            # l'installazione)
            system3(sys.executable, "-c",
                    "from pkg_resources import require; require('%s')" % req)
