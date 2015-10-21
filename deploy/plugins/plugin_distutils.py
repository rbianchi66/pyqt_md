#-*- coding: utf-8 -*-
import re
from common import *

priority = 20

def addOptionGroup(parser):
    import optparse
    group = optparse.OptionGroup(parser, "plugin %s options" % name)
    group.add_option("--%s-dest" % name, metavar="DIR", default="",
          help="As setup.py --install-lib DIR, defaults to src if outside "
               "the source tree, or to '.' if inside the source tree.")
    parser.add_option_group(group)

def _dest():
    option = getattr(opts, "%s_dest" % name)
    # Controlla che il path corrente sia contenuto nel tree dei sorgenti
    # normalizza il path per evitare casi come C:/src e c:/src/geo2d
    if os.path.normcase(os.getcwd()).startswith(os.path.normcase(opts.source)):
        # if we are in the source dir, build inplace
        default = "."
    else:
        default = opts.source
    return option or default

def install(files):
    for setup_py in files:
        os.chdir(dirname(setup_py))
        trace("Building distutils in %s" % os.getcwd(), ".")
        # FIXME: in attesa che la configurazione sia automaticamente giusta
        # a livello gerarchico e fusa con le opzioni a linea di comando...
        cfg = opts.__dict__.copy()
        if isfile("build.cfg"):
            cfg = parseConfig("build.cfg", cfg)
        args = ["build"]
        if opts.pydebug:
            args += ["--debug"]
        if opts.deploy:
            args += ["--build-base", join(opts.deploy, basename(os.getcwd()))]
        if os.name == "nt":
            if opts.compiler == "mingw":
                mingwForcePythonMatch()
                args += ["--compiler", "mingw32"]
            elif opts.compiler == "msvc":
                msvcForcePythonMatch()
                args += ["--compiler", "msvc"]
            else:
                assert False, str(opts.compiler)
        args += ["install", "--install-lib", _dest()]
        if opts.force_install:
            args += ["--force"]
        # Prova a controllare se il package usa setuptools. Se usa setuptools,
        # è necessario installarlo con una flag che disabilata la modalità
        # che permette l'installazione di più versioni in parallelo (ma richiede
        # l'uso di file .pth, e quindi non funziona nella directory src).
        setup_py_source = open("setup.py").read()
        use_setuptools = re.search("^import setuptools", setup_py_source, re.M) or \
                         re.search("^from setuptools", setup_py_source, re.M)
        if use_setuptools:
            args += ["--single-version-externally-managed", "--root", "."]
        system3(sys.executable, 'setup.py', *args)
    return True

def find():
    return glob("setup.py")

def clean(files):
    for setup_py in files:
        os.chdir(dirname(setup_py))
        if os.path.exists(join(os.getcwd(), "build")):
            shutil.rmtree(join(os.getcwd(), "build"))

