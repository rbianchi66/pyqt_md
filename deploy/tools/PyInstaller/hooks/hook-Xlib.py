import os
import sys
from os.path import join, dirname, basename, splitext
from tempfile import NamedTemporaryFile

def listModulesInPackage(pkg):
    test = NamedTemporaryFile()
    test.write('import %s\n' % pkg)
    test.flush()

    data = {}
    try:
        execfile(test.name, data)
    except ImportError:
        return []

    hiddenimports = []
    try:
        package, submodules = pkg.split('.', 1)
        submodules =submodules.split('.')
    except ValueError:
        package = pkg
        submodules = []
    ppath = dirname(data[package].__file__)
    if submodules:
        ppath = join(ppath, *submodules)
        package += '.' + '.'.join(submodules)
    hiddenimports.append(package)
    for fpath in os.listdir(ppath):
        if fpath.startswith('_') or not fpath.endswith('.py'):
            continue
        hiddenimports.append(package + '.' + splitext(fpath)[0])
    return hiddenimports

if sys.platform != 'win32':
    hiddenimports = listModulesInPackage('Xlib.keysymdef') + \
        listModulesInPackage('Xlib.support') +  \
        listModulesInPackage('Xlib.ext')
