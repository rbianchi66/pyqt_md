#!/usr/bin/env python
#-*- coding: utf-8 -*-
import sys, os
import re
import optparse
import glob
import subprocess
import tempfile
from distutils.dep_util import newer

opj = os.path.join

def release(filename):
    """
    Esegue la release di un file .TS trasformandolo in un file binario .QM
    che può essere caricato a run-time da Caligola.
    """
    if sys.platform.startswith("darwin"):
        # FIXME: assume comunque che ci sia un'installazione locale di Qt
        tmpf = tempfile.NamedTemporaryFile(mode="r+w")
        os.system("qmake -query QT_INSTALL_BINS > %s" % (tmpf.name))
        TRANSLATION_TOOLS_DIR = tmpf.read()[:-1]
    else:
        TRANSLATION_TOOLS_DIR = opj(os.path.dirname(__file__), "..", "tools", "Qt4")
    assert os.path.isdir(TRANSLATION_TOOLS_DIR), "Dir %s not found" % TRANSLATION_TOOLS_DIR

    qmname = os.path.splitext(filename)[0] + ".qm"
    if opts.output:
        qmname = os.path.join(opts.output, os.path.basename(qmname))

    if opts.make and not newer(filename, qmname):
        if opts.verbose >= 1:
            print "Skipped %s because it is not changed" % filename
        return

    if opts.verbose >= 1: print "=== Launching lrelease ==================="
    if sys.platform.startswith("win"):
        executable = os.path.join(TRANSLATION_TOOLS_DIR, "lrelease.exe")
        assert os.path.isfile(executable), "File '%s' not found" % executable
    # Non è ben chiaro sei i .qm generati dai tool di qt4 sono completamente
    # compatibili con qt3 per fortuna però lo sono i file .ts, quindi nel
    # caso basta usare diversi lrelease.
    elif sys.platform.startswith("linux"):
        executable = "lrelease-qt4" # standard ubuntu name
    elif sys.platform.startswith("darwin"):
        executable = os.path.join(TRANSLATION_TOOLS_DIR, "lrelease")
        assert os.path.isfile(executable), "File '%s' not found" % executable
    else:
        executable = "lrelease"
    args = [executable, "-compress"]
    if opts.verbose < 1: args += ["-silent"]
    args += [filename, "-qm", qmname]
    lrelease = subprocess.Popen(args)
    lrelease.wait()
    retcode = lrelease.returncode
    if opts.verbose >= 1: print "=========================================="

    if retcode != 0:
        print "lrelease failed (code=%s):" % retcode
        return False

    #PORTING
    return

    return True

def parseProject(pro):
    """
    Analizza il contenuto di un file .PRO di Qmake. Questa funzione non è assolutamente
    completa (manca per esempio la gestione della sostituzione dei nomi di variabili,
    cioé $$THIS, e anche la gestione del quoting per i nomi dei file che contengonos
    paziature), però è sufficiente per un uso semplice come necessario qui.
    """
    pro = re.sub(r"#[^\n]*$", r" ", pro)      # elimina i commenti
    pro = re.sub(r"\\\s*\n",  r" ", pro)      # concatena linee terminate con backslash
    pro = re.sub(r"\n",       r";", pro)      # unifica i terminatori di linea con i \n

    tags = {}
    for L in pro.split(";"):
        if not L:
            continue
        L = L.strip()

        m = re.match(r"(?:.*:)?([a-zA-Z0-9_]+)\s*=(.*)", L)
        if m:
            key, value = m.groups()
            if key in tags:
                print "WARNING: la chiave '%s' e' stata definita due volte" % key
            tags[key] = value.strip()
            continue

        m = re.match(r"(?:.*:)?([a-zA-Z0-9_]+)\s*\+=(.*)", L)
        if m:
            key, value = m.groups()
            try:
                tags[key] += " " + value
            except KeyError:
                tags[key] = value.strip()
            continue

    # Elenco delle chiavi i cui valori sono un elenco multiplo di parole (da splittare)
    for k in "TRANSLATIONS", "SOURCES", "HEADERS":
        if k in tags:
            tags[k] = tags[k].split()

    return tags

def main():
    global opts

    p = optparse.OptionParser(usage="%prog [opts] [project|ts_files...]", version="0.1")
    p.add_option("-v", "--verbose", action="count", default=0,
                 help="mostra maggiori informazioni durante il processo")
    p.add_option("-m", "--make", action="store_true", default=False,
                 help="emula il make: agisce solo se i file .qm non esistono "
                      "o sono piu' vecchi dei corrispondenti .ts")
    p.add_option("-o", "--output", metavar="DIR", type="string", default="",
                 help="specifica la directory di output (default: i file "
                      ".qm vengono creati nella stessa directory dei file "
                      ".ts corrispondenti)")

    opts, args = p.parse_args()

    for pat in args:
        for f in glob.glob(pat):
            ext = os.path.splitext(f)[1]

            if os.path.normcase(ext) == os.path.normcase(".ts"):
                release(f)

            elif os.path.normcase(ext) == os.path.normcase(".pro"):
                tags = parseProject(file(f, "rU").read())
                for t in tags.get("TRANSLATIONS", []):
                    release(t)

            else:
                print "WARNING: ignorato un file di tipo sconosciuto: '%s'" % f


if __name__ == "__main__":
    sys.exit(main())
