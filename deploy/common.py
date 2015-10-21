#!/usr/bin/env python
#-*- coding: utf-8 -*-
"""
Utilità varie per gli script di build/deploy.
"""
import sys
import os
import re
from glob import glob
import subprocess2 as subprocess
from os.path import *
import shutil
from distutils.dep_util import newer

startdir = os.getcwd()
deploy_dir = abspath(split(__file__)[0])
root_dir = abspath(join(deploy_dir, ".."))
src_dir = join(root_dir, "src")
lib_dir = join(root_dir, "lib")
tools_dir = join(deploy_dir, "tools")

def platform():
    """Return a platform name including machine type (eg: "Linux-32bit")"""
    import platform
    return platform.system() + "-" + platform.architecture()[0]

def gccmrt(v):
    system3("gccmrt", v, shell=True)

def getoutput(*args):
    """
    Esegue il comando specificato, torna lo stdout o None in caso di errore
    (programma non trovato, o esecuzione fallita).
    """
    # Usiamo shell=False perché non ci serve la shell (il PATH viene utilizzato
    # anche senza shell su tutti gli OS).
    # Se l'eseguibile non esiste, viene sollevato un OSError.
    try:
        p = subprocess.Popen(args, shell=False,
                             stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        out = p.communicate()[0]
        if p.returncode != 0:
            return None
        return out
    except OSError:
        return None

def getMinGWVersion():
    """
    Torna la versione del MinGW installata nel PATH
    """
    p = subprocess.Popen(['gcc', '-dumpversion'], stdout=subprocess.PIPE)
    version = p.communicate()[0]
    if p.returncode != 0:
        return None

    p = subprocess.Popen(['gccmrt'], shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    mrt = p.communicate()[0]
    if p.returncode != 0:
        # Se non c'è GCCMRT, non è un MinGW Develer e quindi non può
        # essere usato per la compilazione di estensioni Python
        return "mingw"

    if version.startswith("4.3"):
        return "mingw-develer-43"
    elif version.startswith("4.1"):
        return "mingw-develer-41"
    else:
        return "mingw-develer-unknown"

def getVSVersion():
    """
    Torna la versione di Visual Studio installata nel PATH
    """
    version = getoutput("cl")
    if not version:
        return None
    m = re.search("[vV]ersione{0,1} (\d+)", version)
    if not m:
        return None
    vconv = {
        "13": "VS2003",
        "14": "VS2005",
        "15": "VS2008",
        "16": "VS2010",
    }
    return vconv.get(m.group(1), None)

def msvcForcePythonMatch():
    py = sys.version_info[:2]
    vs = getVSVersion()

    if py <= (2,3) and vs == "VS98":
        return
    if py >= (2,4) and py <= (2,5) and vs == "VS2003":
        return
    if py >= (2,6) and vs == "VS2008":
        return

    print
    print "Invalid Visual Studio for your Python version!"
    print "Visual Studio version detected: %s" % vs
    print
    print "Build.py cannot build a Python extension with Visual Studio unless"
    print "it is the correct version.  Please configure the correct Visual"
    print "Studio in your PATH and then run build.py again; alternatively,"
    print "try with MinGW-Develer (build.py --compiler mingw)"
    print
    print "Compatibility table:"
    print "   * Python 2.3 or previous: Visual Studio 98 (VC 6)"
    print "   * Python 2.4, 2.5: Visual Studio 2003 (VC 7.1)"
    print "   * Python 2.6, 2.7, 3.x: Visual Studio 2008 (VC 9.0)"
    sys.exit(2)

def mingwForcePythonMatch():
    py = sys.version_info[:2]
    vs = getMinGWVersion()

    if vs.startswith("mingw-develer"):
        if py <= (2,3):
            gccmrt('60')
        elif py >= (2,4) and py <= (2,5):
            gccmrt('71')
        elif py >= (2,6):
            gccmrt('90')
        else:
            assert False, str(py)
        return

    print
    print "Invalid MinGW version for your Python version!"
    print
    print "To compile a Python extension, you need to install"
    print "Develer's MinGW:"
    print "   http://www.develer.com/oss/GccWinBinaries"
    print
    print "Standard MinGW builds are unable to correctly compile "
    print "Python extensions, so build.py cannot continue."
    sys.exit(2)

def mingwForceStandardExe():
    vs = getMinGWVersion()
    # Se si compila un eseguibile con MinGW-Develer, usiamo come target la version 6.0
    # cioè MSVCRT.DLL, in modo che l'eseguibile funzioni su tutte le versioni di Windows
    # (tutti hanno MSVCRT.DLL preinstallato).
    # Le versioni standard di MinGW usano sempre MSVCRT.DLL, quindi non c'è nulla da fare.
    if vs.startswith("mingw-develer"):
        gccmrt('60')

def getUrl(wc):
    p = subprocess.Popen(['svn', 'info', wc], stdout=subprocess.PIPE)
    svn_info = p.communicate()[0]
    if p.returncode != 0:
        return ""
    for L in svn_info.split("\n"):
        k,v = L.split(":", 1)
        if k == "URL":
            return v.strip()
    else:
        print "WARNING: cannot compute root URL"
        return ""
root_url = getUrl(root_dir)

if not hasattr(subprocess, "check_call"):
    # Usa un po' di codice del Python 2.5 per avere a disposizione la check_call

    class CalledProcessError(Exception):
        def __init__(self, returncode, cmd):
            self.returncode = returncode
            self.cmd = cmd
        def __str__(self):
            return "Command '%s' returned non-zero exit status %d" % (self.cmd, self.returncode)

    def check_call(*popenargs, **kwargs):
        """Run command with arguments.  Wait for command to complete.  If
        the exit code was zero then return, otherwise raise
        CalledProcessError.  The CalledProcessError object will have the
        return code in the returncode attribute.

        The arguments are the same as for the Popen constructor.  Example:

        check_call(["ls", "-l"])
        """
        retcode = subprocess.call(*popenargs, **kwargs)
        cmd = kwargs.get("args")
        if cmd is None:
            cmd = popenargs[0]
        if retcode:
            raise subprocess.CalledProcessError(retcode, cmd)
        return retcode

    subprocess.CalledProcessError = CalledProcessError
    subprocess.check_call = check_call

def quote(s):
    return '"' + s + '"'

def trace(s, sep='-'):
    s = (" %s " % s).center(70, sep)
    print sep*len(s)
    print s
    print sep*len(s)

def fatal(s, sep='-'):
    trace(s, sep)
    sys.exit(2)

def system(*args):
    s = " ".join(map(quote, args))
    system2(s)

def system2(s):
    print s

    # "echo on" è un work-around per un bug della shell di NT che si confonde
    # se la command-line comincia con le virgolette (le rimuove, lasciando
    # il resto della stringa sbilanciata).
    if sys.platform.startswith("win"):
        s = 'echo on && ' + s

    if os.system(s) != 0:
        print "%s: execution failure, aborting" % os.path.basename(sys.argv[0])
        sys.exit(2)

def system3(*parms, **kwargs):
    print subprocess.list2cmdline(parms)
    try:
        subprocess.check_call(parms, **kwargs)
    except (OSError, subprocess.CalledProcessError), err:
        print "execution failure, aborting:", parms
        print err
        sys.exit(2)

def parseConfig(fn=None, cfg=None, ambient=None):
    """
    Legge la configurazione dal file specificato.
    Usa `cfg` come `locals` e `ambient` come `globals`.
    """
    cfg = cfg or {}
    ambient = ambient or {}
    if fn and os.path.isfile(fn):
        execfile(fn, ambient, cfg)
    return cfg

def dirsContaining(name, levels=None):
    """
    Restituisce un insieme di dir che contengono almeno un file
    il cui nome soddisfa il glob fornito in 'name'
    """
    res = set()
    if levels == 0:
        if glob(name):
            res.add(os.getcwd())
    else:
        for root, dirs, files in os.walk(os.getcwd()):
           if ".svn" in root:
               continue
           if glob(join(root, name)):
                res.add(root)
    return res

def numCpuCores():
    try:
        import multiprocessing
        return multiprocessing.cpu_count()
    except:
        return 1

def make(*args):
    if "clean" in args and not exists("Makefile"):
        return
    if os.name == "nt":
        if opts.compiler == "mingw":
            system3('mingw32-make', *args)
        elif opts.compiler == "msvc":
            system3('nmake', *args)
        else:
            assert False
    else:
        if "clean" not in args and "install" not in args:
            args += ("-j%d" % numCpuCores(),)
        system3('make', *args)

def simplepath(apath, base=None):
    """
    Return a simplified path in easy cases:
    >>> simplepath("/a/b/c", base="/a/b")
    'c'
    >>> simplepath("/a/b/c/d", base="/b/c")
    '/a/b/c/d'
    """
    if not base:
        base = os.getcwd()
    if apath.startswith(base):
        return normpath(apath.replace(base, "."))
    else:
        return normpath(apath)

def getProjectName(cache = []):
    if cache:
        return cache[0]
    build_name = parseConfig(join(root_dir, "build.cfg")).get("name", '')
    deploy_name = parseConfig(join(root_dir, "deploy.cfg")).get("name", '')
    if not build_name and not deploy_name:
        print ''
        print 'WARNING: no project name in build.cfg or deploy.cfg'
    if build_name and deploy_name and build_name != deploy_name:
        fatal('"name" in build.cfg differs from "name" in deploy.cfg')
    cache.append(build_name and build_name or deploy_name)
    return cache[0]

def _test():
    import doctest
    doctest.testmod()

if __name__ == "__main__":
    _test()
