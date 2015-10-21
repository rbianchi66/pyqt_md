#!/usr/bin/env python
#-*- coding: utf-8 -*-
"""
Plugin che esegue semplici compilazioni di eseguibili o librerie dinamiche. Da
usare come alternativa a Makefile/CMake nel caso si voglia semplicemente fare
l'equivalente di "compile *.cpp".

Per utilizzarlo, inserire codice come questo in build.cfg:

compile = {
    "name": "iolib",
    "flags": "exe static",  # shlib lib shared debug
    "sources": ["*.cpp", "-serial_linux.cpp"],
    "headers": ["*.h"],
    "libs": [],
    "include_dirs": [],
    "sources_posix": ["serial_linux.cpp"]
}
"""
import sys
from glob import glob
from common import *
import distutils
import distutils.log
import distutils.ccompiler
priority = 5

def find():
    if "compile" in parseConfig("build.cfg"):
        return ["build.cfg"]
    return []

def _expand(files, desc):
    for s in desc:
        if s[0] == '-':
            files.discard(abspath(s[1:]))
        else:
            files.update(map(abspath, glob(s)))

def _compile(cfg, force=0, verbose=0, compiler=None):
    basedir = abspath(".")
    extra_link_args = []
    output_dir = join(basedir, "build")
    flags = set(cfg["flags"].split())
    exename = cfg["name"]
    debug = "debug" in flags
    headers = set()
    sources = set()
    if sys.platform.startswith("win"):
        libs = cfg.get("libs_nt", [])
        libdirs = cfg.get("libdirs_nt", [])
        extra_link_args = cfg.get("extra_link_args_nt", [])
        include_dirs = cfg.get("include_dirs_nt", [])
        _expand(headers, cfg.get("headers_nt", []))
        _expand(sources, cfg.get("sources_nt", []))
    elif sys.platform.startswith("lin"):
        libs = cfg.get("libs_linux", [] )
        libdirs = cfg.get("libdirs_linux", [])
        extra_link_args = cfg.get("extra_link_args_linux", [])
        include_dirs = cfg.get("include_dirs_linux", [])
        _expand(headers, cfg.get("headers_linux", []))
        _expand(sources, cfg.get("sources_linux", []))
    elif sys.platform.startswith("darwin"):
        libs = cfg.get("libs_darwin", [])
        libdirs = cfg.get("libdirs_darwin", [])
        extra_link_args = cfg.get("extra_link_args_darwin", [])
        include_dirs = cfg.get("include_dirs_darwin", [])
        _expand(headers, cfg.get("headers_darwin", []))
        _expand(sources, cfg.get("sources_darwin", []))

    if verbose:
        distutils.log.set_threshold(distutils.log.DEBUG)

    if len(sources) == 0:
        trace("Warning: no " + sys.platform + " source file provided for " + os.path.split(os.path.abspath(os.path.curdir))[1], ".")
        return

    if opts.compiler == "mingw":
        mingwForceStandardExe()
        compiler = "mingw32"
    elif opts.compiler == "msvc":
        compiler = "msvc"
    elif opts.compiler == "gcc":
        compiler = "unix"
    else:
        assert None, "unknown configured compiler: %s" % opts.compiler

    cc = distutils.ccompiler.new_compiler(force=force, compiler=compiler, verbose=verbose)
    if __debug__:
        cc.define_macro("DEBUG")

    if "static" in flags:
        if os.name == "nt" and opts.compiler == "msvc":
            # HACK: la lista di opzioni viene creata al primo compile(), quindi
            # dobbiamo fare un compile vuoto prima di poterla modificare.
            cc.compile(sources=[], debug=debug, output_dir=output_dir, depends=[], include_dirs=include_dirs)
            cc.compile_options.remove("/MD")

            # Durante il linking statico, non viene generato nessun manifest.
            # Il codice di distutils pero' pensa che venga sempre generato e fallisce
            # se poi non riesce ad embeddarlo chiamando mt.exe. Dobbiamo evitare
            # questo errore ignorando la chiamata a mt.exe
            # (vedi distutils\msvc9compiler.py, metodo link(), linea 673)
            rspawn = cc.spawn
            def spawn(args):
                print "SPAWN", args
                if args[0] == "mt.exe":
                    return
                rspawn(args)
            cc.spawn = spawn

    if sys.platform.startswith("darwin"):
        extra_link_args += ['-framework','CoreFoundation']

        # Sotto Mac usiamo g++ per compilare e linkare: evita un errore dovuto
        # ad un simbolo "__Unwind_Resume" non definito:
        # http://lists.apple.com/archives/unix-porting/2006/May/msg00073.html
        cc.set_executable("compiler_cxx", "g++")
        cc.set_executable("linker_exe", "g++")

    objs = cc.compile(sources=sorted(sources), debug=debug, output_dir=output_dir,
                      depends=sorted(headers), include_dirs=include_dirs)

    if "exe" in flags:
        cc.link_executable(objs, exename, debug=debug, output_dir=output_dir,
                           target_lang='c++', libraries=libs, library_dirs=libdirs, extra_postargs=extra_link_args)
    elif "shlib" in flags:
        raise NotImplementedError("compilazione di libreria dinamica non ancora supportata")
    elif "lib" in flags:
        raise NotImplementedError("compilazione di libreria dinamica non ancora supportata")
    else:
        raise RuntimeError("nessuna definizione di tipo di target")

    exetmp = cc.executable_filename(exename, output_dir=output_dir)

    if os.name == "nt" and not os.path.splitext(exename)[1]:
        exename += ".exe"
    shutil.copy(exetmp, abspath(exename))

def install(files):
    for cfg in files:
        os.chdir(os.path.dirname(cfg))
        cfg = parseConfig(cfg)["compile"]
        _compile(cfg)
