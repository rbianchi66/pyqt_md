#!/usr/bin/env python
# -*- encoding: utf-8 -*-
import sys
import os
import re
from common import *
import which
import subprocess2 as subprocess

errors = 0

def error_conflict_default(name, default_ver, cur_ver, resolution):
    print "ERROR: Found wrong version"
    print
    print "Program name:       ", name
    print "Default version:    ", default_ver
    print "Current version:    ", cur_ver
    print
    print resolution
    print "-"*78
    print
    global errors
    errors += 1


def error_conflict(name, ver, exp_ver, resolution):
    if ver is not None:
        print "ERROR: Found wrong version"
    else:
        print "ERROR: Program not found"
    print
    print "Program name:       ", name
    print "Expected version:   ", exp_ver
    print "Version found:      ", ver
    print
    print resolution
    print "-"*78
    print
    global errors
    errors += 1

def check_python(exp_ver):
    exp_ver_t = tuple(map(int, exp_ver.split(".")))
    ver_t = sys.version_info
    ver = ".".join(map(str,ver_t[:3]))
    if ver_t < exp_ver_t:
        error_conflict("Python", ver, exp_ver,
            "Install a newer Python interpreter")
    # Give a warning only if the major version is newer (eg: 2.7 vs 2.5).
    # Ignore newer minor versions (eg: 2.6.5 vs 2.6.3)
    elif ver_t[:2] > exp_ver_t:
        error_conflict("Python", ver, exp_ver,
            "Your Python version is newer than the officially supported one. "
            "Since Python is usually backward compatible, you can try to build "
            "the project anyway, but this version has never been tested by "
            "the developers.")

    check_default_python()

def check_default_python():
    # Teoricamente, l'intero sistema di build utilizza la versione di Python
    # con cui viene eseguito Build.py (e si spera Configure.py). In realtà però
    # è possibile che qualche script di build (da considerarsi *buggato*) lanci
    # un qualche script esterno Python utilizzando il Python nel PATH, invece della
    # versione utilizzata per lanciare Build.py.
    # Di conseguenza, controlliamo che, se c'è una versione di Python nel PATH,
    # sia la stessa con cui è stato eseguito Configure.py, e avvertiamo eventualmente
    # del problema
    ver_t = sys.version_info
    ver = ".".join(map(str,ver_t[:3]))

    dver = getoutput("python", "-V")
    if not dver:
        return
    if not dver.startswith("Python "):
        raise ValueError("python -V has a unexpected output: %r\n" % ver);
    dver = dver[7:].strip()
    if not sys.version.startswith(dver + " "):
        error_conflict_default("Default Python installation", dver, ver,
            "Your PATH contains a default Python version which is different "
            "from the one you are using to run Configure.py. Theoretically, Build.py will ignore this "
            "default Python version, but there might be bugs in the build scripts "
            "of this project.\n"
            "Suggested fix: add the correct Python version at the beginning of PATH. "
            "Or you can try to build anyway, and it should work most of the times.")

def check_visual_studio(exp_ver):
    ver = getVSVersion()
    if ver != exp_ver:
        error_conflict("Visual Studio", ver, exp_ver,
            "Use the Visual Studio Command Prompt, or run the environment script "
            "(eg: VSVARS32.BAT, exact name changes across different versions)")

def check_cmake(exp_ver):
    exp_ver_t = tuple(map(int,exp_ver.split(".")))
    ver = getoutput("cmake", "--version")
    if ver:
        r = re.search("cmake version ([0-9.]+)", ver)
        if not r:
            raise ValueError("cmake --version has a unexpected output: %r\n" % ver);
        ver = r.group(1)
        ver_t = tuple(map(int,ver.split(".")))

    if not ver or ver_t < exp_ver_t:
        error_conflict("CMake", ver, exp_ver,
            "Add the correct CMake binary to your PATH")

def check_system_path():
    assert os.name == "nt"
    path = os.environ["PATH"]

    # In Windows, PATH non deve *mai* contenere dei quotes. Le virgolette per
    # fare escaping delle spaziature sono gestite solo dalla shell (CMD), ma
    # PATH viene utilizzato anche da funzioni kernel tipo CreateProcess(), che non
    # sono in grado di gestirle.
    # Una directory nel PATH con virgolette sarà quindi gestita diversamente da
    # subprocess.Popen(shell=True) rispetto a shell=False, e vogliamo evitare questo
    # casino.
    if '"' in path:
        print "ERROR: Your PATH environment variable contains quotes (\")"
        print
        print "PATH should never contains quotes, because there is no need to escape spaces, "
        print "and can cause confusion to Windows/Python functions that do not expect them "
        print "(eg: CreateProcess will fail). "
        print "Please edit your PATH and remove any quotes from it."
        print "-"*78
        global errors
        errors += 1

def configure():
    cfg = join(root_dir, "configure.cfg")
    cfg2 = join(root_dir, "configure-%s.cfg" % sys.platform)

    if os.path.isfile(cfg):
        execfile(cfg)
    if os.path.isfile(cfg2):
        execfile(cfg2)

    if os.name == "nt":
        check_system_path()

    if not errors:
        print "Configuration looks OK!"
        print "Now run Build.py to build the project."
        return 0
    return 1

if __name__ == "__main__":
    sys.exit(configure())
