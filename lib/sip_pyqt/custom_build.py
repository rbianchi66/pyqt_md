#!/usr/bin/env python
#-*- coding: utf-8 -*-
from __future__ import with_statement
from os.path import *
import os, sys
import fileinput
from tempfile import mkdtemp
from glob import glob

SYSTEM_ENV = os.getenv('SIPPYQT_SYSTEM_ENV', 0)
QT_VERSION = os.getenv('SIPPYQT_QT_VERSION', '4')

# In Python 2.6, grazie al casino dei manifest, è stata rotta la
# compatibilità binaria tra Python 2.6.4 e Python 2.6.5. Siccome qui
# utilizziamo i binari creati con 2.6.5, assicuriamoci che chi fa il
# build stia utilizzando la versione giusta.
if os.name == "nt" and sys.version_info[0:2] == (2, 6):
    if sys.version_info[2] < 5:
        print "BUILD ERROR: If you are using Python 2.6, you need at least Python 2.6.5"
        sys.exit(2)

# Funzioni prese da build.py del package sip_pyqt
def platform():
    import platform
    return platform.system() + "-" + platform.architecture()[0]
def pyversion():
    return str(sys.version_info[0]) + "." + str(sys.version_info[1])


def install(destdir=None):
    pyqt_name = "PyQt" + QT_VERSION

    # Copia il contenuto della directory <plat>/PyQt4 in src/PyQt4
    import shutil

    if not destdir:
        destdir = "../../src"
    destdir = abspath(destdir)

    if SYSTEM_ENV:
        pyqt = __import__(pyqt_name)
        import sipconfig as sipconf

        # Creiamo una directory temporanea in cui copiamo sip e pyqt
        srcdir = mkdtemp(suffix="caligola_env")

        site_package = pyqt.__path__[0].replace(pyqt_name, '')
        shutil.copytree(pyqt.__path__[0], join(srcdir, pyqt_name))
        os.makedirs(join(srcdir, "sipbin"))

        shutil.copy(sipconf._pkg_config['sip_bin'], join(srcdir, "sipbin"))
        shutil.copy(join(sipconf._pkg_config['py_inc_dir'], "sip.h"), join(srcdir, "sipbin"))

        for f in glob(join(site_package, "sip*")):
            shutil.copy(f, srcdir)

        shutil.copy(abspath(join(dirname(__file__), pyversion(), platform(), "sipbin", "stl.sip")), join(srcdir, "sipbin"))

    else:
        srcdir = abspath(join(dirname(__file__), pyversion(), platform()))

    commondir=abspath(join(dirname(__file__), "common"))
    for d in [commondir, srcdir]:
        os.chdir(d)
        for dirpath,dirs,files in os.walk("."):
            if ".svn" in dirs:
                dirs.remove(".svn")
            if not isdir(join(destdir, dirpath)):
                os.makedirs(join(destdir, dirpath))
            for f in files:
                print "From:", join(d, dirpath, f)
                print "  To:", join(destdir, dirpath, f)
                shutil.copy(join(dirpath, f),
                            join(destdir, dirpath, f))

    if os.name == "nt":
        with open(join(destdir, "pyuic4.bat"), "w") as f:
            f.write('@"%s" -m "%s.uic.pyuic" %%*\n' % (sys.executable, pyqt_name))
    else:
        pyuicbin = join(destdir, "pyuic4")
        with open(pyuicbin, "w") as f:
            f.write('#!/bin/sh\n')
            f.write('exec "%s" -m "%s.uic.pyuic" $*\n' % (sys.executable, pyqt_name))
        os.chmod(pyuicbin, 0755)

    if SYSTEM_ENV:
        # Modifichiamo sipconfig.py in modo che cerchi
        # l'eseguibile sip nella cartella sipbin
        data = {}
        execfile(join(destdir, "sipconfig.py"), data)
        for L in fileinput.FileInput(join(destdir, "sipconfig.py"), inplace=True):
            L = L.replace(data["_pkg_config"]["default_sip_dir"], join(destdir, "sipbin").encode("string-escape"))
            sys.stdout.write(L)
    else:
        # sipconfig.py contiene i path assoluti di quando è stato compilato
        # nel pacchetto sip_pyqt. Carichiamo il file per estrarre il path
        # e lo sostituiamo con il path nuovo corretto.
        data = {}
        execfile(join(destdir, "sipconfig.py"), data)
        wrong_path = data["_pkg_config"]["sip_mod_dir"].encode("string-escape")
        for L in fileinput.FileInput(join(destdir, "sipconfig.py"), inplace=True):
            L = L.replace(wrong_path, destdir.encode("string-escape"))
            sys.stdout.write(L)


if __name__ == "__main__":
    install(sys.argv[1] if len(sys.argv) > 1 else None)
