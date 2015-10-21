#-*- coding: utf-8 -*-
"""
Plugin QResources
-----------------

Il plugin Qresources analizza i file `build.cfg`
(ad esempio `dir1/dir2/build.cfg`) cercando una lista:

    qresources = ["resource.file", "*.jpg", "-foo.jpg"]

Inserisce tutti i file che corrispondono ai glob specificati (ed usando la
sintassi "-[glob]" per forzare una esclusione) all'interno del qrc, cioè
delle risorse di Qt.

Il path dei file viene modificato in modo che contenga solo l'ultima componente
(cioè il nome della directory che contiene i file). Continuando l'esempio qui
sopra, i file verranno inclusi dentro le risorse con questi path:

    dir2/resource.file
    dir2/...
"""

from common import *

priority = 60
solid = True

import deploy

def find():
    if name in parseConfig("build.cfg"):
        return ["build.cfg"]
    return []

qrc_template = """<RCC version="1.0">
<qresource prefix="%(prefix)s">
%(files)s
</qresource>
</RCC>"""
file_template = """    <file alias="%(alias)s">%(filename)s</file>"""

def _get_qrc(qtresources):
    res = set()
    for r in qtresources:
        if r[0] != '-':
            for alias in glob(r):
                filename = abspath(alias)
                res.add(file_template % locals())
        else:
            for alias in glob(r[1:]):
                filename = abspath(alias)
                res.discard(file_template % locals())

    if res:
        files = "\n".join(sorted(res))
        prefix = basename(os.getcwd())
        return qrc_template % locals()
    else:
        return None

def _project_rcc():
    # Prende il nome del progetto
    project_name = getProjectName()
    return project_name.lower() + ".rcc"

def install(files):
    temp_dir = join(deploy_dir, "temp")
    if not isdir(temp_dir):
        os.makedirs(temp_dir)
    for cfg_file in files:
        os.chdir(dirname(cfg_file))
        qtresources = parseConfig("build.cfg").get("qtresources", [])
        if qtresources:
            filename = basename(os.getcwd()) + ".qrc"
            trace("Creating qrc file (%s)..." % filename, ".")
            open(join(temp_dir, filename), "w+").write(_get_qrc(qtresources))

    qrc_files = glob(join(temp_dir, "*.qrc"))
    if qrc_files:
        trace("Updading binary %s file..." % _project_rcc(), ".")
        cmd = "rcc"
        if os.name != "posix":
            cmd = join(deploy_dir, "tools", "Qt4", cmd)
        system3(cmd, "-binary", "-compress", "9", "-threshold", "20",
                     "-o", join(opts.source, _project_rcc()),
                     *qrc_files)
        return True
    else:
        trace("Warning: no Qt Resources definition file found!", ".")
        return False
