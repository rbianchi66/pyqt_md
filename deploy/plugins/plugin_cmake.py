#-*- coding: utf-8 -*-
"""
Plugin che esegue il build con cmake, e' possibile specificare il generatore.
"""
import os
import sys
import shutil
from common import *
from os.path import *

priority = 15
solid = True

# Mappa di cosa sappiamo buildare da riga di comando.
_builders = {
    'NMake Makefiles': ['nmake.exe'],
    'Unix Makefiles': ['make', '-j%d' % numCpuCores()],
    'MinGW Makefiles': ['mingw32-make'],
    'KDevelop3 - Unix Makefiles': ['make', '-j%d' % numCpuCores()],
}

if sys.platform == 'win32' and getoutput("jom.exe", "/?") is not None:
    _builders['NMake Makefiles'] = ['jom.exe', '-j%d' % numCpuCores()]

def _defGenerator():
    if os.name == "nt":
        if opts.compiler == "msvc":
            return 'NMake Makefiles'
        elif opts.compiler == "mingw":
            mingwForceStandardExe()
            return 'MinGW Makefiles'
        else:
            assert False, str(opts.compiler)
    else:
        return 'Unix Makefiles'

def addOptionGroup(parser):
    import optparse
    group = optparse.OptionGroup(parser, "plugin %s options" % name)
    group.add_option("--%s-generator" % name, default="",
                      help="As cmake -G generator, [value stored in CMakeCache or default for selected compiler]")
    group.add_option("--%s-build_type" % name, default="Release",
                      help="Define CMAKE_BUILD_TYPE, [value stored in CMakeCache or %default]")
    group.add_option("--%s-lsb" % name, default="", metavar="LSB_VERSION",
                      help="Compile using the specified LSB SDK, [default: don't use LSB]")

    if sys.platform == 'darwin':
        group.add_option("--%s-arch" % name, default="i386;ppc",
                          help="Darwin architectures, [default:%default]")

    parser.add_option_group(group)

def _generator():
    option = getattr(opts, "%s_generator" % name)
    return option

def _buildType():
    option = getattr(opts, "%s_build_type" % name)
    return option

def _lsbVersion():
    option = getattr(opts, "%s_lsb" % name)
    return option

def _buildArch():
    option = getattr(opts, "%s_arch" % name)
    return option

def find():
    return glob('CMakeLists.txt')

def clean(files):
    p = _generator() or _defGenerator()
    if p in _builders and p != 'devenv':
        for configure in _roots(files):
            os.chdir(dirname(configure))
            # potrebbe non essere mai stato generato niente ancora
            if os.path.exists("CMakeFiles"):
                trace("CMake cleaning in %s" % os.getcwd())
                try:
                    os.remove("CMakeCache.txt")
                except:
                    pass
                shutil.rmtree("CMakeFiles", ignore_errors=True)
    else:
        # non sappiamo eseguire il clean da riga di comando
        trace("WARNING: SKIPPING CMAKE CLEAN in %s\nPLEASE CLEAN THE PROJECT BY YOURSELF USING: %s" % (os.getcwd(), _generator()), "!")

def _roots(files):
    # Filtra i cmakelists in modo che cmake venga invocato solo su quelli toplevel.
    r = [dirname(f).split(sep) for f in files]
    r.sort(key=lambda x: len(x))
    for i in xrange(len(r)):
        f = r[i]
        while f:
            if f in r[:i]: break
            f = f[:-1]
        else:
            yield sep.join(r[i] + ['CMakeLists.txt'])

def install(files):
    import sys
    for cm in _roots(files):
        os.chdir(dirname(cm))

        trace("Running cmake in %s" % os.getcwd(), "~")

        # Se vengono passati da riga di comando  usa il generatore eo il build_type richiesti
        # altrimenti se disponibili li estrae da CMakeCache altrimenti usa i default
        passato_g = reduce(lambda x, y: x or y, ["--cmake-generator" in z for z in sys.argv], False)
        passato_bt = reduce(lambda x, y: x or y, ["--cmake-build_type" in z for z in sys.argv], False)

        b = _buildType()
        p = _generator()
        if not p:
            p = _defGenerator()

        if not passato_g or not passato_bt:
            try:
                cm = open("CMakeCache.txt", "r").readlines()
                for z in cm:
                    if not passato_g and z.find("CMAKE_GENERATOR:INTERNAL=") != -1:
                        p = z.split('=')[1].split('\n')[0]
                    if not passato_bt and z.find("CMAKE_BUILD_TYPE:STRING=") != -1:
                        b = z.split('=')[1].split('\n')[0]
            except IOError:
                pass

        if _lsbVersion():
            # devo provare a compilare con LSB
            if os.path.exists("/opt/lsb/bin/lsbcc") \
            and os.path.exists("/opt/lsb/bin/lsbc++"):
                # esistono i compilatori LSB
                if _lsbVersion() in os.popen('/opt/lsb/bin/lsbcc --lsb-version').read() \
                and _lsbVersion() in os.popen('/opt/lsb/bin/lsbc++ --lsb-version').read():
                    # la versione _lsb() e' installata
                    systemCMake("cmake", "-G", p, "-DCMAKE_BUILD_TYPE=%s" % b, 
                    '-DCMAKE_C_COMPILER="/opt/lsb/bin/lsbcc --lsb-target-version=%s"' % _lsbVersion(),
                    '-DCMAKE_CXX_COMPILER=/opt/lsb/bin/lsbc++ --lsb-target-version=%s"' % _lsbVersion(),
                    '-DCMAKE_EXE_LINKER_FLAGS=--lsb-besteffort')
                else:
                    trace("ERROR: found LSB compilers, but not version %s!" % _lsbVersion())
                    sys.exit(1)

            else:
                trace("ERROR: LSB compilers not installed!")
                sys.exit(1)

        else:
            # build "normale"
            if p != "devenv":
                systemCMake("cmake", "-G", p, "-DCMAKE_BUILD_TYPE=%s" % b)
            else:
                systemCMake("cmake", "-DCMAKE_BUILD_TYPE=%s" % b)

        if p in _builders:
            trace("Building using %s in %s" % (_builders[p][0], os.getcwd()), ".")
            systemCMake(*_builders[p])
        else:
            # non sappiamo buildare da riga di comando tutti i progetti generabili
            trace("WARNING: SKIPPING CMAKE BUILIDING in %s\nPLEASE BUILD THE PROJECT BY YOURSELF USING: %s" % (os.getcwd(), _generator()), "!")
            trace("PRESS ENTER TO CONTINUE")
            import sys
            sys.stdin.readline()

def systemCMake(*args, **kwargs):
    if sys.platform == 'darwin':
        e = dict(os.environ)
        e['CMAKE_OSX_ARCHITECTURES'] = _buildArch()
        kwargs['env'] = e
    system3(*args, **kwargs)


