#!/usr/bin/env python
#-*- coding: utf-8 -*-

"""
transupdate.py - Gestione delle traduzioni per Caligola.

usage: transupdate.py [opts] [files...]

options:
  --version             show program's version number and exit
  -h, --help            show this help message and exit
  -p PROJECT, --project=PROJECT
                        nome del progetto, utilizzato come prefisso per i file
                        (default=project)
  -v, --verbose         mostra maggiori informazioni durante il processo
  -l LANG, --languages=LANG
                        specifica una lingua (o piu' lingue, separate da
                        virgole) di cui si vuole aggiornare il file di
                        traduzione (default=IT)

transupdate.py aggiunge un livello sopra pylupdate per automatizzare alcuni passaggi.

    1) Generazione automatica del file di progetto
    2) Aggioramento dei file .ts con pylupdate4
    3) Controllo della sintassi dei file xml generati (pylupdate può generare file 
       non validi che poi non potranno essere caricati da linguist)
"""

import os,sys
import subprocess
import optparse

try: set
except: from sets import Set as set    # 2.3 compatibility

from xml.etree import ElementTree

opj = os.path.join
TRANSLATION_TOOLS_DIR = opj(os.path.dirname(__file__), "..", "tools", "Qt4")
assert os.path.isdir(TRANSLATION_TOOLS_DIR), "Dir %s not found" % TRANSLATION_TOOLS_DIR

# Verbosità del programma
verbose = 0

class VerifyError(Exception):
    pass

def checkXML(filename):
    """
    Questa funzione controlla la validità del file XML
    generato da pylupdate.
    Questo è utile per il debug e per testare preventivamente
    i file che poi non potrebbero essere caricati correttamente
    da linguist.
    """
    f = open(filename, "rU")
    xml = f.read()
    f.close()
    try:
        ElementTree.fromstring(xml)
    except Exception, msg:
        lines = xml.split("\n")
        res = [str(msg)]
        res += lines[msg.lineno-3:msg.lineno]
        res += ["%s^" % (msg.offset*" ")]
        res += lines[msg.lineno:msg.lineno+2]
        raise Exception, "\n".join(res)

def findFiles(root, filter_func):
    if not os.path.isdir(root):
        yield root
        return

    for f in os.listdir(root):
        p = os.path.join(root, f)

        if os.path.isdir(p):
            for f in findFiles(p, filter_func):
                yield f
        else:
            if filter_func(root, f):
                yield p


def genProject(project, langs, files, excludes):
    """
    Genera un file di progetto in formato .PRO (qmake), che faccia da input
    a pylupdate per la traduzione nelle lingue specificate da "langs"
    di tutti i file sorgenti python (o directory) contenuti in "files"
    Torna una lista dei nomi di file di traduzione (.TS) che verrano creati,
    e il numero di file sorgenti python trovati.
    """
    def is_project_file(path, filename):
        excluded = [e for e in excludes if e in path]
        return filename.endswith(".py") and not excluded
    ts_files = []
    projname = os.path.splitext(project)[0]
    pro = file(project, "w")
    print >>pro, "CODECFORTR = UTF-8"
    print >>pro, "CODECFORSRC = UTF-8"
    print >>pro, "TRANSLATIONS = \\"
    for L in langs:
        f = "%s_%s.ts" % (projname, L)
        print >>pro, "\t%s \\" % f
        ts_files.append(f)
    print >>pro, "\t;"

    count = 0
    print >>pro, "SOURCES = \\"
    for d in files:
        for f in findFiles(d, is_project_file):
            print >>pro, "\t%s \\" % f
            count += 1
    print >>pro, "\t;"
    pro.close()

    return (ts_files, count)

def run_pylupdate(proj_file):
    if verbose >= 1: print "=== Launching pylupdate =================="
    executable = os.path.join(TRANSLATION_TOOLS_DIR, "pylupdate4.exe")
    assert os.path.isfile(executable), "File '%s' not found" % executable
    if sys.platform not in ["nt", "win32"]:
        executable = "pylupdate4"
    args = [executable, proj_file]
    if verbose >= 1: args.insert(len(args)-1, "-verbose")
    pylupdate = subprocess.Popen(args)
    pylupdate.wait()
    retcode = pylupdate.returncode
    if verbose >= 1: print "=========================================="

    if retcode != 0:
        print "pylupdate failed (code=%s):" % retcode
        return False

    return True

def main():
    global verbose

    p = optparse.OptionParser(usage="%prog [opts] [files...]", version="0.1")
    p.add_option("-p", "--project",
                 action="store", dest="project", default="project",
                 help="nome del progetto, utilizzato come prefisso per i file (default=project)")
    p.add_option("-v", "--verbose",
                 action="count", dest="verbose", default=0,
                 help="mostra maggiori informazioni durante il processo")
    p.add_option("-l", "--languages",
                 action="append", dest="langs", metavar="LANG",
                 help="specifica una lingua (o piu' lingue, separate da virgole) di cui si vuole aggiornare il file di traduzione (default=IT)")
    p.add_option("-x", "--exclude",
                 action="append", dest="excludes", default=[],
                 help="esclude i file che contengono nella path le stringhe fornite come parametro")
    opts, args = p.parse_args()

    verbose = opts.verbose

    # Non si usa il default di add_option() perché con append() si comporta in modo
    # strano (cioé in pratica è sempre presente nella lista)
    if not opts.langs:
        opts.langs = ["IT"]

    # Supporta liste di valori (--languages=en,de,ru)
    for L in filter(lambda s: "," in s, opts.langs):
        opts.langs.extend(L.split(","))
        opts.langs.remove(L)

    for L in opts.langs:
        if len(L) != 2:
            p.error("codice di linguaggio non valido: '%s'" % L)

    opts.langs = [L.upper() for L in opts.langs]
    opts.langs = set(opts.langs)

    if not args:
        args = [ os.getcwd() ]

    # Nei filesystem case-sensitive è necessario creare il file di
    # progetto con la giusta capitalizzazione. Per coerenza con altre
    # parti del codice generiamo nomi lowercase.
    proj_file = "%s.pro" % opts.project.lower()

    # Genera il file di progetto
    ts_files, num_py = genProject(proj_file, opts.langs, args, opts.excludes)

    if num_py == 0:
        print "nessun file python specificato come input"
        return 1

    if not run_pylupdate(proj_file):
        print "pylupdate failed, aborting"
        return 2

    for t in ts_files:
        try:
            checkXML(t)
        except Exception, msg:
            raise Exception, "Problems on file '%s':\n%s" % (t, msg)

if __name__ == "__main__":
    sys.exit(main())
