#-*- coding: utf-8 -*-

# TODO splittare il modulo
# TODO ridurre l'accoppiamento con le variabili di common

from common import *
import deploy

priority = 30

def _project_pro():
    return getProjectName().lower() + ".pro"

def find():
    return glob(_project_pro())

def install(files):
    for pro in files:
        os.chdir(dirname(pro))
        trace("Building translations...", "~")
        lang_dir = join(opts.source, "Lang")
        if not isdir(lang_dir):
            os.makedirs(lang_dir)
        transrelease_py = join(deploy_dir, "translation", "transrelease.py")
        system3(sys.executable, transrelease_py,
                "--verbose", "--make",
                "--output", lang_dir,
                _project_pro())
        system3(sys.executable, transrelease_py,
                "--verbose", "--make",
                "--output", lang_dir,
                join(deploy_dir, "translation/qtmsg/*.ts"))

