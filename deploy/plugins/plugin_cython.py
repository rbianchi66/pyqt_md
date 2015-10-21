#!/usr/bin/env python
#-*- coding: utf-8 -*-
"""
Plugin per compilare con cython i file .pyx
"""
import os.path
import shutil
import sys
from glob import glob

import common

priority = 5
pxd_include_dirs = []

def find():
    files = glob('*.pyx')
    if files:
        try:
            import Cython
        except ImportError:
            common.fatal('Cython not installed')
        if not hasattr(Cython, "__version__"):
            common.fatal('Unknown Cython version: assuming too old. Minimum version is "0.14"')
        version = Cython.__version__.split('.', 2)
        major, minor = map(int, version[0:2])
        if major * 100 + minor < 14:
            common.fatal('Cython version "%s", minimum version is "0.14"' % Cython.__version__)
    return files

def _build_dir(source):
    return os.path.join(os.path.dirname(source), 'build')

def clean(files):
    for fpath in set(map(_build_dir, files)):
        if os.path.exists(fpath):
            shutil.rmtree(fpath)

class Conf(object):
    def __init__(self, data):
        self.data = data
        plat = {
            'win': '_nt',
            'lin': '_linux',
            'dar': '_darwin',
        }
        try:
            self.platform = plat[sys.platform[:3]]
        except KeyError:
            raise RuntimeError('platform "%s" not supported' % sys.platform)

    def __getitem__(self, key):
        common = self.data.get(key)
        platform = self.data.get(key + self.platform)
        if common and platform:
            return common + platform
        else:
            try:
                return filter(None, (common, platform))[0]
            except IndexError:
                return None

def pyx_to_dll_wrapper(filename, ext=None, force_rebuild=0, build_in_temp=False, pyxbuild_dir=None, setup_args={}, reload_support=False):
    """
    compila il file .pyx passato; ha la stessa signature della funzione
    pyximport.pyx_to_dll ma utilizza, se presente, il file build.cfg posto
    nella stessa directory del file .pyx.

    il file .cfg deve definire un dizionario "cython" con, opzionale, una
    chiave per ogni file .pyx, esempio:

    cython = {
        'foo': {
        }
    }

    Il dizionario associato a 'foo' verrà utilizzato per la compilazione del
    file foo.pyx; questo dizionario può contenere le seguenti chiave:

        * include_dirs
        * library_dirs
        * libraries

    che hanno lo stesso significato dei corrispettivi argomenti nella classe
    distutils.core.Extension

        http://docs.python.org/distutils/apiref.html#distutils.core.Extension

    unica differenza, include_dirs viene utilizzato anche da cython come elenco
    di path dove cercare eventuali .pxd

    la configurazione di un modulo può essere specificata per le varie
    piattaforme dove verrà compilato aggiungendo ad ogni attributo i suffissi:
    _linux, _nt, _darwin

    Per ogni attributo possono essere presenti sia la versione specifica per la
    piattaforma che quella generica, in questo caso i due valori vengono
    concatenati:

    cython = {
        'foo': {
            'include_dirs': ('/usr/include',),
            'include_dirs_linux': ('/usr/local/include',),
        }
    }

    Le directory presenti in `lib_dir`/pxd/ vengono aggiunte automaticamente al
    path di ricerca di cython *e* alle include_dirs passate al compilatore, in
    questo modo possiamo avere in un solo posto i file .pxd insieme ai file .h
    necessari a cython.
    """
    from distutils.extension import Extension
    try:
        from pyximport.pyxbuild import pyx_to_dll
    except TypeError: # unsupported operand type(s) for +: 'NoneType' and 'str'
        # Questo è un workaround per una incompatibilità con python -O
        # dell'estensione a distutils di cython che cerca fare
        # Extension.__doc__ + "altra documentazione", solo
        # che con l'ottimizzazione accesa Extension.__doc__ è None.
        assert Extension.__doc__ is None
        Extension.__doc__ = ""
        from pyximport.pyxbuild import pyx_to_dll

    if not pxd_include_dirs:
        pxd_include_dirs.extend(x for x in glob(os.path.join(common.opts.libs, 'pxd', '*')) if os.path.isdir(x))

    cfgpath = os.path.join(os.path.dirname(filename), 'build.cfg')
    modname = os.path.splitext(os.path.basename(filename))[0]
    cfg = common.parseConfig(cfgpath).get('cython', {})
    conf = Conf(cfg.get(modname, {}))

    if ext is None:
        # il nome dell'estensione deve essere un dotted name relativo alla
        # directory dei sorgenti
        name = os.path.normpath(filename)[len(common.opts.source):]
        if name[0] in '/\\':
            name = name[1:]
        name = os.path.splitext(name)[0].replace('/', '.').replace('\\', '.')
        ext = Extension(name=name, sources=[filename])

    include_dirs = pxd_include_dirs + ( conf['include_dirs'] or [] )
    library_dirs = conf['library_dirs'] or []
    libraries = conf['libraries'] or []

    def _s(name, value):
        if hasattr(ext, name):
            setattr(ext, name, getattr(ext, name) + value)
        else:
            setattr(ext, name, value)

    _s('pyrex_include_dirs', pxd_include_dirs)
    _s('include_dirs', include_dirs)
    _s('library_dirs', library_dirs)
    _s('libraries', libraries)

    return pyx_to_dll(
        filename,
        ext=ext,
        force_rebuild=True,
        build_in_temp=build_in_temp,
        pyxbuild_dir=pyxbuild_dir,
        setup_args=setup_args,
        reload_support=reload_support,
    )

def install(files):
    for fpath in files:
        tmp = pyx_to_dll_wrapper(fpath, pyxbuild_dir=_build_dir(fpath))
        shutil.copy(tmp, os.path.dirname(fpath))
