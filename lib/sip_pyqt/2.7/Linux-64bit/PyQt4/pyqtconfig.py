# Copyright (c) 2010 Riverbank Computing Limited <info@riverbankcomputing.com>
# 
# This file is part of PyQt.
# 
# This copy of PyQt is licensed for use under the terms of the PyQt
# Commercial License Agreement version 3.6.  See the file LICENSE for more
# details.
# 
# PyQt is supplied WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#
# This module is intended to be used by the configuration scripts of extension
# modules that %Import PyQt4 modules.


import sipconfig


# These are installation specific values created when PyQt4 was configured.
_pkg_config = {
    'pyqt_bin_dir':      '/root/Qt-4.6.2-x86_64/sip_pyqt/bin/2.7/Linux-64bit',
    'pyqt_config_args':  '--bindir /root/Qt-4.6.2-x86_64/sip_pyqt/bin/2.7/Linux-64bit --destdir /root/Qt-4.6.2-x86_64/sip_pyqt/bin/2.7/Linux-64bit --consolidate --confirm-license --verbose --no-docstrings --no-sip-files --concatenate --concatenate-split 6 --enable QtCore --enable QtGui --enable QtOpenGL --enable QtSvg --enable QtWebKit --enable QtNetwork --plugin qjpeg --plugin qsvg',
    'pyqt_mod_dir':      '/root/Qt-4.6.2-x86_64/sip_pyqt/bin/2.7/Linux-64bit/PyQt4',
    'pyqt_modules':      'QtCore QtGui QtNetwork QtOpenGL QtSvg QtWebKit',
    'pyqt_sip_dir':      '/root/Qt-4.6.2-x86_64/sip_pyqt/bin/2.7/Linux-64bit/sipbin/PyQt4',
    'pyqt_sip_flags':    '-x VendorID -t WS_X11 -x PyQt_Accessibility -x PyQt_SessionManager -x PyQt_OpenSSL -x PyQt_NoPrintRangeBug -t Qt_4_6_2 -x Py_v3 -g',
    'pyqt_version':      0x040703,
    'pyqt_version_str':  '4.7.3',
    'qt_data_dir':       '/usr/local/Trolltech/Qt-4.6.2',
    'qt_dir':            '/usr/local/Trolltech/Qt-4.6.2',
    'qt_edition':        'Desktop',
    'qt_framework':      0,
    'qt_inc_dir':        '/usr/local/Trolltech/Qt-4.6.2/include',
    'qt_lib_dir':        '/usr/local/Trolltech/Qt-4.6.2/lib',
    'qt_threaded':       1,
    'qt_version':        0x040602,
    'qt_winconfig':      ''
}

_default_macros = {
    'AIX_SHLIB':                '',
    'AR':                       'ar cqs',
    'CC':                       'gcc',
    'CFLAGS':                   '-pipe',
    'CFLAGS_CONSOLE':           '',
    'CFLAGS_DEBUG':             '-g',
    'CFLAGS_EXCEPTIONS_OFF':    '',
    'CFLAGS_EXCEPTIONS_ON':     '',
    'CFLAGS_MT':                '',
    'CFLAGS_MT_DBG':            '',
    'CFLAGS_MT_DLL':            '',
    'CFLAGS_MT_DLLDBG':         '',
    'CFLAGS_RELEASE':           '-O2',
    'CFLAGS_RTTI_OFF':          '',
    'CFLAGS_RTTI_ON':           '',
    'CFLAGS_SHLIB':             '-fPIC',
    'CFLAGS_STL_OFF':           '',
    'CFLAGS_STL_ON':            '',
    'CFLAGS_THREAD':            '-D_REENTRANT',
    'CFLAGS_WARN_OFF':          '-w',
    'CFLAGS_WARN_ON':           '-Wall -W',
    'CHK_DIR_EXISTS':           'test -d',
    'CONFIG':                   'qt warn_on release incremental link_prl',
    'COPY':                     'cp -f',
    'CXX':                      'g++',
    'CXXFLAGS':                 '-pipe',
    'CXXFLAGS_CONSOLE':         '',
    'CXXFLAGS_DEBUG':           '-g',
    'CXXFLAGS_EXCEPTIONS_OFF':  '',
    'CXXFLAGS_EXCEPTIONS_ON':   '',
    'CXXFLAGS_MT':              '',
    'CXXFLAGS_MT_DBG':          '',
    'CXXFLAGS_MT_DLL':          '',
    'CXXFLAGS_MT_DLLDBG':       '',
    'CXXFLAGS_RELEASE':         '-O2',
    'CXXFLAGS_RTTI_OFF':        '',
    'CXXFLAGS_RTTI_ON':         '',
    'CXXFLAGS_SHLIB':           '-fPIC',
    'CXXFLAGS_STL_OFF':         '',
    'CXXFLAGS_STL_ON':          '',
    'CXXFLAGS_THREAD':          '-D_REENTRANT',
    'CXXFLAGS_WARN_OFF':        '-w',
    'CXXFLAGS_WARN_ON':         '-Wall -W',
    'DEFINES':                  '',
    'DEL_FILE':                 'rm -f',
    'EXTENSION_PLUGIN':         '',
    'EXTENSION_SHLIB':          '',
    'INCDIR':                   '',
    'INCDIR_OPENGL':            '/usr/X11R6/include',
    'INCDIR_QT':                '/usr/local/Trolltech/Qt-4.6.2/include',
    'INCDIR_X11':               '/usr/X11R6/include',
    'LFLAGS':                   '',
    'LFLAGS_CONSOLE':           '',
    'LFLAGS_CONSOLE_DLL':       '',
    'LFLAGS_DEBUG':             '',
    'LFLAGS_DLL':               '',
    'LFLAGS_OPENGL':            '',
    'LFLAGS_PLUGIN':            '-shared',
    'LFLAGS_RELEASE':           '-Wl,-O1',
    'LFLAGS_SHLIB':             '-shared',
    'LFLAGS_SONAME':            '-Wl,-soname,',
    'LFLAGS_THREAD':            '',
    'LFLAGS_WINDOWS':           '',
    'LFLAGS_WINDOWS_DLL':       '',
    'LIB':                      '',
    'LIBDIR':                   '',
    'LIBDIR_OPENGL':            '/usr/X11R6/lib64',
    'LIBDIR_QT':                '/usr/local/Trolltech/Qt-4.6.2/lib',
    'LIBDIR_X11':               '/usr/X11R6/lib64',
    'LIBS':                     '',
    'LIBS_CONSOLE':             '',
    'LIBS_CORE':                '',
    'LIBS_GUI':                 '',
    'LIBS_NETWORK':             '',
    'LIBS_OPENGL':              '-lGLU -lGL',
    'LIBS_RT':                  '',
    'LIBS_RTMT':                '',
    'LIBS_THREAD':              '-lpthread',
    'LIBS_WEBKIT':              '',
    'LIBS_WINDOWS':             '',
    'LIBS_X11':                 '-lXext -lX11 -lm',
    'LINK':                     'g++',
    'LINK_SHLIB':               'g++',
    'LINK_SHLIB_CMD':           '',
    'MAKEFILE_GENERATOR':       'UNIX',
    'MKDIR':                    'mkdir -p',
    'MOC':                      '/usr/local/Trolltech/Qt-4.6.2/bin/moc',
    'RANLIB':                   '',
    'RPATH':                    '-Wl,-rpath,',
    'STRIP':                    'strip'
}


class Configuration(sipconfig.Configuration):
    """The class that represents PyQt configuration values.
    """
    def __init__(self, sub_cfg=None):
        """Initialise an instance of the class.

        sub_cfg is the list of sub-class configurations.  It should be None
        when called normally.
        """
        if sub_cfg:
            cfg = sub_cfg
        else:
            cfg = []

        cfg.append(_pkg_config)

        sipconfig.Configuration.__init__(self, cfg)


class QtCoreModuleMakefile(sipconfig.SIPModuleMakefile):
    """The Makefile class for modules that %Import QtCore.
    """
    def __init__(self, *args, **kw):
        """Initialise an instance of a module Makefile.
        """
        if "qt" not in kw:
            kw["qt"] = ["QtCore"]

        sipconfig.SIPModuleMakefile.__init__(self, *args, **kw)


class QtGuiModuleMakefile(QtCoreModuleMakefile):
    """The Makefile class for modules that %Import QtGui.
    """
    def __init__(self, *args, **kw):
        """Initialise an instance of a module Makefile.
        """
        if "qt" not in kw:
            kw["qt"] = ["QtCore", "QtGui"]

        QtCoreModuleMakefile.__init__(self, *args, **kw)


class QtHelpModuleMakefile(QtGuiModuleMakefile):
    """The Makefile class for modules that %Import QtHelp.
    """
    def __init__(self, *args, **kw):
        """Initialise an instance of a module Makefile.
        """
        if "qt" not in kw:
            kw["qt"] = ["QtCore", "QtGui", "QtHelp"]

        QtGuiModuleMakefile.__init__(self, *args, **kw)


class QtMultimediaModuleMakefile(QtGuiModuleMakefile):
    """The Makefile class for modules that %Import QtMultimedia.
    """
    def __init__(self, *args, **kw):
        """Initialise an instance of a module Makefile.
        """
        if "qt" not in kw:
            kw["qt"] = ["QtCore", "QtGui", "QtMultimedia"]

        QtGuiModuleMakefile.__init__(self, *args, **kw)


class QtNetworkModuleMakefile(QtCoreModuleMakefile):
    """The Makefile class for modules that %Import QtNetwork.
    """
    def __init__(self, *args, **kw):
        """Initialise an instance of a module Makefile.
        """
        if "qt" not in kw:
            kw["qt"] = ["QtCore", "QtNetwork"]

        QtCoreModuleMakefile.__init__(self, *args, **kw)


class QtAssistantModuleMakefile(QtNetworkModuleMakefile):
    """The Makefile class for modules that %Import QtAssistant.
    """
    def __init__(self, *args, **kw):
        """Initialise an instance of a module Makefile.
        """
        if "qt" not in kw:
            kw["qt"] = ["QtCore", "QtGui", "QtNetwork", "QtAssistant"]

        QtNetworkModuleMakefile.__init__(self, *args, **kw)


class QtOpenGLModuleMakefile(QtGuiModuleMakefile):
    """The Makefile class for modules that %Import QtOpenGL.
    """
    def __init__(self, *args, **kw):
        """Initialise an instance of a module Makefile.
        """
        kw["opengl"] = 1

        if "qt" not in kw:
            kw["qt"] = ["QtCore", "QtGui", "QtOpenGL"]

        QtGuiModuleMakefile.__init__(self, *args, **kw)


class QtScriptModuleMakefile(QtCoreModuleMakefile):
    """The Makefile class for modules that %Import QtScript.
    """
    def __init__(self, *args, **kw):
        """Initialise an instance of a module Makefile.
        """
        if "qt" not in kw:
            kw["qt"] = ["QtCore", "QtScript"]

        QtCoreModuleMakefile.__init__(self, *args, **kw)


class QtScriptToolsModuleMakefile(QtScriptModuleMakefile):
    """The Makefile class for modules that %Import QtScriptTools.
    """
    def __init__(self, *args, **kw):
        """Initialise an instance of a module Makefile.
        """
        if "qt" not in kw:
            kw["qt"] = ["QtCore", "QtGui", "QtScript", "QtScriptTools"]

        QtScriptModuleMakefile.__init__(self, *args, **kw)


class QtSqlModuleMakefile(QtGuiModuleMakefile):
    """The Makefile class for modules that %Import QtSql.
    """
    def __init__(self, *args, **kw):
        """Initialise an instance of a module Makefile.
        """
        if "qt" not in kw:
            kw["qt"] = ["QtCore", "QtGui", "QtSql"]

        QtGuiModuleMakefile.__init__(self, *args, **kw)


class QtSvgModuleMakefile(QtGuiModuleMakefile):
    """The Makefile class for modules that %Import QtSvg.
    """
    def __init__(self, *args, **kw):
        """Initialise an instance of a module Makefile.
        """
        if "qt" not in kw:
            kw["qt"] = ["QtCore", "QtGui", "QtSvg"]

        QtGuiModuleMakefile.__init__(self, *args, **kw)


class QtTestModuleMakefile(QtGuiModuleMakefile):
    """The Makefile class for modules that %Import QtTest.
    """
    def __init__(self, *args, **kw):
        """Initialise an instance of a module Makefile.
        """
        if "qt" not in kw:
            kw["qt"] = ["QtCore", "QtGui", "QtTest"]

        QtGuiModuleMakefile.__init__(self, *args, **kw)


class QtWebKitModuleMakefile(QtNetworkModuleMakefile):
    """The Makefile class for modules that %Import QtWebKit.
    """
    def __init__(self, *args, **kw):
        """Initialise an instance of a module Makefile.
        """
        if "qt" not in kw:
            kw["qt"] = ["QtCore", "QtGui", "QtNetwork", "QtWebKit"]

        QtNetworkModuleMakefile.__init__(self, *args, **kw)


class QtXmlModuleMakefile(QtCoreModuleMakefile):
    """The Makefile class for modules that %Import QtXml.
    """
    def __init__(self, *args, **kw):
        """Initialise an instance of a module Makefile.
        """
        if "qt" not in kw:
            kw["qt"] = ["QtCore", "QtXml"]

        QtCoreModuleMakefile.__init__(self, *args, **kw)


class QtXmlPatternsModuleMakefile(QtCoreModuleMakefile):
    """The Makefile class for modules that %Import QtXmlPatterns.
    """
    def __init__(self, *args, **kw):
        """Initialise an instance of a module Makefile.
        """
        if "qt" not in kw:
            kw["qt"] = ["QtCore", "QtXmlPatterns"]

        QtCoreModuleMakefile.__init__(self, *args, **kw)


class phononModuleMakefile(QtGuiModuleMakefile):
    """The Makefile class for modules that %Import phonon.
    """
    def __init__(self, *args, **kw):
        """Initialise an instance of a module Makefile.
        """
        if "qt" not in kw:
            kw["qt"] = ["QtCore", "QtGui", "phonon"]

        QtGuiModuleMakefile.__init__(self, *args, **kw)


class QtDesignerModuleMakefile(QtGuiModuleMakefile):
    """The Makefile class for modules that %Import QtDesigner.
    """
    def __init__(self, *args, **kw):
        """Initialise an instance of a module Makefile.
        """
        if "qt" not in kw:
            kw["qt"] = ["QtCore", "QtGui", "QtDesigner"]

        QtGuiModuleMakefile.__init__(self, *args, **kw)


class QAxContainerModuleMakefile(QtGuiModuleMakefile):
    """The Makefile class for modules that %Import QAxContainer.
    """
    def __init__(self, *args, **kw):
        """Initialise an instance of a module Makefile.
        """
        if "qt" not in kw:
            kw["qt"] = ["QtCore", "QtGui", "QAxContainer"]

        QtGuiModuleMakefile.__init__(self, *args, **kw)
