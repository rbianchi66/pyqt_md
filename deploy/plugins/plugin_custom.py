#-*- coding: utf-8 -*-

from common import *

priority = 50

def find():
    return [name for name in glob('custom_build.*') if not name.endswith('~')]

def install(files):
    for cbuild in files:
        os.chdir(dirname(cbuild))
        trace("Running %s in %s" % (cbuild, os.getcwd()), "~")
        if ".pyc" in cbuild or ".pyo" in cbuild:
            continue
        if cbuild.endswith(".py"):
            system3(sys.executable, cbuild, opts.source)
        else:
            system3(cbuild, opts.source)
