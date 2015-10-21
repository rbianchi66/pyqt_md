#!/usr/bin/env python
#-*- coding: utf-8 -*-
import sys
from collections import defaultdict
from common import *
import plugins

# Cambia questo valore per forzare un rebuild di tutte le librerie
REBUILD_ALL_COUNT = 6

rebuild_file = join(root_dir, "last.rebuild")

def getCompilerIdent():
    if os.name == "nt":
        if opts.compiler == "mingw":
            return getMinGWVersion()
        elif opts.compiler == "msvc":
            return getVSVersion()
        else:
            assert False, str(opts.compiler)
    else:
        return "gcc"

def check_need_clean():
    try:
        f = file(rebuild_file)
        x = long(f.readline().strip())
        plat = f.readline().strip()
        vers = f.readline().strip()
        url = f.readline().strip()
        compiler = f.readline().strip()
    except (IOError, ValueError):
        return True
    if plat != sys.platform:
        print "INFO: platform changed from %r to %r, cleaning" % (plat, sys.platform)
        return True
    elif x != REBUILD_ALL_COUNT:
        print "INFO: Rebuilding because rebuild count changed from %r to %r" % (x, REBUILD_ALL_COUNT)
        return True
    elif vers != repr(sys.version.strip()):
        print "INFO: Rebuilding because Python version changed from %s to %s" % (vers, repr(sys.version.strip()))
        return True
    elif url != root_url:
        print "INFO: Rebuilding because URL changed from %r to %r" % (url, root_url)
        return True
    elif compiler != getCompilerIdent():
        print "INFO: Rebuilding because compiler changed from %r to %r" % (compiler, getCompilerIdent())
        return True
    return False

def update_rebuild():
    f = file(rebuild_file, "w")
    print >>f, REBUILD_ALL_COUNT
    print >>f, sys.platform
    print >>f, repr(sys.version.strip())
    print >>f, root_url.strip()
    print >>f, getCompilerIdent()
    f.close()

def parseBuildCfg():
    # set defaults
    cfg = {
        "enabled": [],
        "disabled": [],
        "verbose": False,
        "discovery": True,
    }
    if os.path.isfile("build.cfg"):
        cfg.update(parseConfig("build.cfg", ambient=globals()))
    return cfg

def getPluginFromName(name):
    return plugins.get(name)

def _discovery(use_plugins, builds, start_dir, cfg):
    use_plugins = set(use_plugins)
    cfg = cfg.copy()
    # Legge il file di configurazione locale (se c'è), e sovrascrive le
    # vecchie configurazioni.
    cfg.update(parseBuildCfg())

    # Se il discovery è bloccato, non considera questa directory.
    if not cfg["discovery"]:
        return

    # Applica l'attivazione o disattivazione dei plugin da considerare in
    # questo tree.
    if cfg["disabled"] or cfg["enabled"]:
        for p in cfg["disabled"]:
            use_plugins.discard(plugins.get(p))
        for p in cfg["enabled"]:
            use_plugins.add(plugins.get(p))

        # Le opzioni a linea di comando vincono sempre su tutto il resto,
        # quindi qualunque modifica sia stata fatta, riapplichiamo quanto
        # richiesto dall'utente.
        for p in opts.disabled:
            use_plugins.discard(p)
        for p in opts.enabled:
            use_plugins.add(p)

    sys.stdout.write("."); sys.stdout.flush()

    # Per ogni plugin attivo, fa il find() per trovare i file interessanti
    for p in use_plugins:
        if os.getcwd().startswith(start_dir) or hasattr(p, "solid"):
            files = map(abspath, p.find())
            if files:
                priority = cfg.get("priority", p.priority)
                builds[(priority, p)] += files

    # Va in ricorsione su eventuali sotto-directory
    for fn in map(abspath, os.listdir(".")):
        if isdir(fn) and split(fn)[1] not in ["CVS", ".svn", ".hg", ".git"]:
            # Applica il trucco della modalità deploy: sostituisce la directory
            # dei sorgenti (/src) con quella specificata a linea di comando
            # (--source, di solito src_mirror). In questo modo continua il
            # discovery lì dentro.
            if opts.deploy and normpath(fn) == normpath(src_dir):
                fn = opts.source
            os.chdir(fn)
            _discovery(use_plugins, builds, start_dir, cfg)

def discovery():
    """
    Scopre quali azioni eseguire chiedendo ad ogni plugin di cercare dei file
    su cui agire all'interno di ogni directory di un intero tree di file.

    Parte dalla directory corrente. Lancia il find() di ogni plugin all'interno,
    di ogni directory, facendosi restituire l'elenco di file da processare.
    Analizza inoltre un eventuale build.cfg alla ricerca
    """
    trace("Discovery", "#")
    start_dir = os.getcwd()
    builds = defaultdict(list)
    os.chdir(root_dir)
    _discovery(plugins.all, builds, start_dir, cfg={})
    print
    return builds


if __name__ == "__main__":
    global opts
    import optparse
    if __debug__:
        trace("Build working in non-optimized mode", "+")
    p = optparse.OptionParser()
    p.add_option("--pydebug", action="store_true", default=False,
                 help="build with a Python debug build (Py_DEBUG)")
    p.add_option("-d", "--deploy", metavar="DIR", default="",
                 help="build in deploy mode, storing temporary files into DIR")
    p.add_option("-f", "--force-install", default=False, action="store_true",
                 help="if specified, force reinstallation of libraries")
    p.add_option("-s", "--source", metavar="DIR", default=src_dir,
                 help="source directory (default: %default)")
    p.add_option("-l", "--libs", metavar="DIR", default=lib_dir,
                 help="libs directory (default: %default)")
    if os.name == "nt":
        p.add_option("-c", "--compiler", default="auto", type="choice",
                     choices=["msvc","mingw","auto"],
                     help='Select compiler to user: "mingw" or "msvc" (default: auto)')
    p.add_option("-e", "--enable-plugin", default=[], action="append", dest="enabled",
                 help="Enable provided plugin (default: all)")
    p.add_option("-n", "--disable-plugin", default=[], action="append", dest="disabled",
                 help="Disable provided plugin (default: %default)")
    p.add_option("-i", "--info", default=False, action="store_true",
                 help="Print out discovery results and stop.")
    # old options
    p.add_option("-r", "--rebuild-qt", default=False, action="store_true",
                 help=optparse.SUPPRESS_HELP)

    # Add plugins options
    for plugin in plugins.all:
        if hasattr(plugin, "addOptionGroup"):
            plugin.addOptionGroup(p)

    opts, args = p.parse_args()

    if args:
        subdir = args[0]
        if os.path.isdir(subdir):
            os.chdir(subdir)

    if opts.deploy:
       os.environ["BUILD_DEPLOY"] = "1"

    # Deprecated options
    if opts.rebuild_qt:
        print "-r option has been removed, use: -e pyqt4"
        sys.exit(2)

    if os.name == "nt":
        if opts.compiler == "auto":
            v = getVSVersion()
            if v:
                opts.compiler = "msvc"
            else:
                v = getMinGWVersion()
                if v:
                    opts.compiler = "mingw"
                else:
                    opts.compiler = None
        elif opts.compiler == "msvc":
            v = getVSVersion()
            if not v:
                print "ERROR: msvc requested but not found in PATH"
                sys.exit(2)
        elif opts.compiler == "mingw":
            v = getMinGWVersion()
            if not v:
                print "ERROR: mingw requested but not found in PATH"
                sys.exit(2)
        else:
            assert False, "unhandled compiler option"
    else:
        opts.compiler = "gcc"

    # plugins need to have access to the options
    import common
    common.opts = opts
    for plugin in plugins.all:
        plugin.opts = opts

    for p in (opts.source, root_dir):
        if p not in os.getenv("PYTHONPATH", "").split(os.pathsep):
            os.environ["PYTHONPATH"] = os.pathsep.join((p, os.getenv("PYTHONPATH", "")))
    print "#"*70
    print "#", os.path.abspath(sys.executable), sys.version
    print "# PYTHONPATH=%s" % os.environ["PYTHONPATH"]
    print "#"*70

    opts.enabled = map(plugins.get, list(opts.enabled))
    opts.disabled = map(plugins.get, list(opts.disabled))

    clean = check_need_clean()

    def run():
        builds = discovery()
        for (priority, plugin) in sorted(builds):
            files = builds[(priority, plugin)]
            if not files:
                continue
            for afile in files:
                print "--> [%d] %s %s" % (priority, plugin.__name__, simplepath(afile, root_dir))
            if opts.info: # TODO: convert --info in --interactive
                continue
            if clean and hasattr(plugin, "clean"):
                plugin.clean(files)
            try:
                plugin.install(files)
            except:
                trace("ERROR: BUILD FAILED", ".")
                raise
        update_rebuild()
    run()
