#!/usr/bin/env python
#-*- coding: utf-8 -*-
import os
import sys
import time
from os.path import *
import xml.etree.ElementTree as ET

from subprocess2 import Popen, PIPE
from common import *

def getExternals(path, env=None):
    if env is None:
        env = os.environ.copy()
        env["LC_MESSAGES"] = "C"

    svn_status = Popen(["svn", "status", path, "--xml"], stdout=PIPE, env=env).stdout.read()
    st = ET.fromstring(svn_status)
    return [e.get("path") for e in st.find("target") if e.find("wc-status").get('item') == 'external']

def checkIsSVNRepository(env=None, path=None):
    if env is None:
        env = os.environ.copy()
        env["LC_MESSAGES"] = "C"

    args = ["svn", "info"]
    if path is not None:
        args.append(path)

    svninfo = Popen(args, stdout=PIPE, env=env)
    svninfo.wait()
    return not bool(svninfo.returncode)

def GetBuildNumber(svn_path=None, escludi_externals=False):
    # Usa svnversion per estrarre il build number. Parsare .svn/entries è
    # sconsigliato ufficialmente perché il formato può cambiare tra una versione
    # e l'altra di Subversion.

    svn_path = svn_path or "."

    def executeCommand(arg, shell=False):
        env = os.environ.copy()
        env["LC_MESSAGES"] = "C"

        execution = Popen(arg, stdout=PIPE, env=env, shell=shell)
        execution.wait()

        if execution.returncode:
            return None

        resp = ""
        for L in execution.stdout:
            L = L.strip()
            resp += L + "\n"
        return resp.strip()

    def gitsvn_version(path):

        # Cerca l'ultima revisione del path passato
        shell = sys.platform.startswith("win")
        log = executeCommand(['git', 'svn', 'log', '--limit=1', path], shell=shell)

        if log is not None:

            # Prendo la prima riga dove so esserci la revisione
            rev_string = log.split("\n")[1]

            # Prendo il primo elemento dove c'è la revisione del file
            rev = rev_string.split(" ")[0][1:]

            return rev

        return ""

    def svnversion(*args):
        return executeCommand(["svnversion"] + list(args))

    def svn_info(args):
        svn_string = executeCommand(['svn', 'info', '--xml', args])

        if svn_string is None:
            return None

        info = ET.fromstring(svn_string)

        commits = info.findall(".//commit")
        assert len(commits) == 1
        return commits[0].attrib['revision']

    if checkIsSVNRepository(path=svn_path):

        # A seconda che il path sia un file o una directory chiama funzioni diverse
        if os.path.isfile(svn_path):
            rev = svn_info(svn_path)
        else:
            rev = svnversion(svn_path)
    else:
        return gitsvn_version(svn_path)

    if rev is None:
        return ""

    if escludi_externals:
        return rev

    # Chiede il build number anche sugli external; se ne trova uno modificato
    # appende una "X" per far capire che la modifica è su un external.
    for ext in getExternals("."):
        if "M" in svnversion(ext):
            rev = rev + "X-%s" % basename(ext)
            break

    # Se il numero di revisione è pulito (una sola revisione senza modifiche
    # locali), possiamo "ottimizzarlo" prendendo il numero di revisione
    # dell'ultimo commit in *questa* directory, che è più significativo
    # (perché non cambia in caso di commit esterni al progetto).
    try:
        int(rev)
    except ValueError:
        return rev

    last_rev = -1
    for dir in getExternals(".") + ["."]:
        rev = svnversion("-c", dir)
        if ":" in rev:
            rev = rev.split(":")[1]
        if rev > last_rev:
            last_rev = rev
    return last_rev

def getRelativePath():
    # Usa svn info per estrarre il path relativo alla root del repositorio in
    # cui si trova il programma.
    env = os.environ.copy()
    env["LC_MESSAGES"] = "C"
    info = {}

    # Prova a cercare le informazioni con svn, se non è un repository svn prova
    # con git svn
    if not checkIsSVNRepository(env):
        shell = sys.platform.startswith('win')
        svninfo = Popen(["git", "svn", "info"], stdout=PIPE, env=env, shell=shell)
    else:
        svninfo = Popen(["svn", "info"], stdout=PIPE, env=env)

    svninfo.wait()
    for L in svninfo.stdout:
        L = L.strip()
        if L:
            k,v = L.split(": ", 1)
            info[k] = v

    assert info["URL"].startswith(info["Repository Root"])
    return info["URL"][len(info["Repository Root"]):]

def getPathDescription():
    rp = getRelativePath()
    assert rp[0] == "/"
    comp = rp[1:].split("/")

    if "trunk" in comp:
        return "trunk"

    try:
        idx = comp.index("branches")
    except ValueError:
        pass
    else:
        return "branch %s" % comp[idx+1]

    try:
        idx = comp.index("tags")
    except ValueError:
        pass
    else:
        return "tag %s" % comp[idx+1]

    # Se non riusciamo a parsare, torniamo il path relativo completo (meglio
    # di nulla)
    return rp

def isFinal():
    # Per semplicità, assumiamo che le release finali vengano buildate solo
    # a partire da tag.
    rp = getRelativePath()
    assert rp[0] == "/"
    comp = rp[1:].split("/")
    return "tags" in comp

def _GenerateBuildNrModule(fn, infos={}):
    nr = GetBuildNumber()
    tm = time.time()

    helpers = """
def systemDate():
    '''
    Torna la data e l'ora di build in formato GMT ISO.
    '''
    import time
    return time.strftime("%Y-%m-%d %H:%M", time.gmtime(date))

def localeDate():
    '''
    Torna una stringa contenente la data e l'ora di build, formattata secondo
    il locale corrente. L'ora è anche aggiustata in base al fuso orario.
    '''
    import time
    return time.strftime("%c", time.localtime(date))

def localeYear():
    '''
    Torna una stringa contenente l'anno in cui è stata fatta la build.
    Utile per mostrare il copyright.
    '''
    import time
    return time.strftime("%Y", time.localtime(date))

def strictRevNum():
    '''
    Se la working copy era in uno stato consistente, torna il numero di
    versione. Torna invece None se c'erano modifiche locali, sottodirectory
    non aggiornate, o directory switchate.
    '''
    try:
        return int(revnum)
    except ValueError:
        return None

def versionString():
    '''
    Torna il numero di versione con aggiunto "-pre" se non è una versione
    finale (presa da una tag).
    '''

    if final:
        return version
    else:
        return version + "-pre"
"""

    if fn == "-":
        f = sys.stdout
    else:
        f = file(fn, "w")

    f.write("#-*- coding: utf-8 -*-\n")
    infos.setdefault('lts', False)
    for k,v in sorted(infos.items()):
        f.write("%s = %r\n" % (k,v))
    f.write("revnum = %r\n" % nr)
    f.write("date = %r\n" % tm)
    f.write("path = %r\n" % getPathDescription())
    f.write("final = %s\n" % isFinal())
    f.write(helpers)
    f.close()

def GenerateBuildNrModule(fn):
    cfg = parseConfig(join(root_dir, "deploy.cfg"))
    infos = dict(cfg["infos"])
    infos["name"] = cfg["name"]
    return _GenerateBuildNrModule(fn, infos)

if __name__ == "__main__":
    import sys
    if len(sys.argv) < 2:
        print "Usage: genbuildnr.py [filename]"
        sys.exit(2)

    GenerateBuildNrModule(sys.argv[1])
