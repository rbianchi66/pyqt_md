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
    'pyqt_bin_dir':      'D:\\Work\\sip_pyqt\\bin\\2.6\\Windows-32bit',
    'pyqt_config_args':  '--bindir D:\\Work\\sip_pyqt\\bin\\2.6\\Windows-32bit --destdir D:\\Work\\sip_pyqt\\bin\\2.6\\Windows-32bit --consolidate --confirm-license --verbose --no-docstrings --no-sip-files --concatenate --concatenate-split 1 --enable QtCore --enable QtGui --enable QtOpenGL --enable QtSvg --enable QtWebKit --enable QtNetwork --plugin qjpeg --plugin qsvg CXXFLAGS+=/bigobj',
    'pyqt_mod_dir':      'D:\\Work\\sip_pyqt\\bin\\2.6\\Windows-32bit\\PyQt4',
    'pyqt_modules':      'QtCore QtGui QtNetwork QtOpenGL QtSvg QtWebKit',
    'pyqt_sip_dir':      'D:\\Work\\sip_pyqt\\bin\\2.6\\Windows-32bit\\sipbin\\PyQt4',
    'pyqt_sip_flags':    '-x VendorID -t WS_WIN -x PyQt_Accessibility -x PyQt_OpenSSL -x PyQt_NoPrintRangeBug -t Qt_4_6_2 -x Py_v3 -g',
    'pyqt_version':      0x040703,
    'pyqt_version_str':  '4.7.3',
    'qt_data_dir':       'D:\\Qt\\4.6.2-static-vc90',
    'qt_dir':            'D:\\Qt\\4.6.2-static-vc90',
    'qt_edition':        'Desktop',
    'qt_framework':      0,
    'qt_inc_dir':        'D:\\Qt\\4.6.2-static-vc90\\include',
    'qt_lib_dir':        'D:\\Qt\\4.6.2-static-vc90\\lib',
    'qt_threaded':       1,
    'qt_version':        0x040602,
    'qt_winconfig':      ''
}

_default_macros = {
    'AIX_SHLIB':                '',
    'AR':                       '',
    'CC':                       'cl',
    'CFLAGS':                   '-nologo -Zm200 -Zc:wchar_t-',
    'CFLAGS_CONSOLE':           '',
    'CFLAGS_DEBUG':             '-Zi -MD',
    'CFLAGS_EXCEPTIONS_OFF':    '',
    'CFLAGS_EXCEPTIONS_ON':     '',
    'CFLAGS_MT':                '',
    'CFLAGS_MT_DBG':            '',
    'CFLAGS_MT_DLL':            '',
    'CFLAGS_MT_DLLDBG':         '',
    'CFLAGS_RELEASE':           '-O2 -MD',
    'CFLAGS_RTTI_OFF':          '',
    'CFLAGS_RTTI_ON':           '',
    'CFLAGS_SHLIB':             '',
    'CFLAGS_STL_OFF':           '',
    'CFLAGS_STL_ON':            '',
    'CFLAGS_THREAD':            '',
    'CFLAGS_WARN_OFF':          '-W0',
    'CFLAGS_WARN_ON':           '-W3',
    'CHK_DIR_EXISTS':           'if not exist',
    'CONFIG':                   'qt warn_on release incremental flat link_prl precompile_header autogen_precompile_source copy_dir_files debug_and_release debug_and_release_target embed_manifest_dll embed_manifest_exe',
    'COPY':                     'copy /y',
    'CXX':                      'cl',
    'CXXFLAGS':                 '-nologo -Zm200 -Zc:wchar_t- -Dprotected=public /bigobj',
    'CXXFLAGS_CONSOLE':         '',
    'CXXFLAGS_DEBUG':           '-Zi -MD',
    'CXXFLAGS_EXCEPTIONS_OFF':  '',
    'CXXFLAGS_EXCEPTIONS_ON':   '-EHsc',
    'CXXFLAGS_MT':              '',
    'CXXFLAGS_MT_DBG':          '',
    'CXXFLAGS_MT_DLL':          '',
    'CXXFLAGS_MT_DLLDBG':       '',
    'CXXFLAGS_RELEASE':         '-O2 -MD',
    'CXXFLAGS_RTTI_OFF':        '',
    'CXXFLAGS_RTTI_ON':         '-GR',
    'CXXFLAGS_SHLIB':           '',
    'CXXFLAGS_STL_OFF':         '',
    'CXXFLAGS_STL_ON':          '-EHsc',
    'CXXFLAGS_THREAD':          '',
    'CXXFLAGS_WARN_OFF':        '-W0',
    'CXXFLAGS_WARN_ON':         '-W3 -w34100 -w34189',
    'DEFINES':                  'UNICODE WIN32 QT_LARGEFILE_SUPPORT',
    'DEL_FILE':                 'del',
    'EXTENSION_PLUGIN':         '',
    'EXTENSION_SHLIB':          '',
    'INCDIR':                   '',
    'INCDIR_OPENGL':            '',
    'INCDIR_QT':                'D:\\Qt\\4.6.2-static-vc90\\include',
    'INCDIR_X11':               '',
    'LFLAGS':                   '/NOLOGO',
    'LFLAGS_CONSOLE':           '/SUBSYSTEM:CONSOLE',
    'LFLAGS_CONSOLE_DLL':       '',
    'LFLAGS_DEBUG':             '/DEBUG',
    'LFLAGS_DLL':               '/DLL',
    'LFLAGS_OPENGL':            '',
    'LFLAGS_PLUGIN':            '',
    'LFLAGS_RELEASE':           '/INCREMENTAL:NO',
    'LFLAGS_SHLIB':             '',
    'LFLAGS_SONAME':            '',
    'LFLAGS_THREAD':            '',
    'LFLAGS_WINDOWS':           '''/SUBSYSTEM:WINDOWS "/MANIFESTDEPENDENCY:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' publicKeyToken='6595b64144ccf1df' language='*' processorArchitecture='*'"''',
    'LFLAGS_WINDOWS_DLL':       '',
    'LIB':                      'lib /NOLOGO',
    'LIBDIR':                   '',
    'LIBDIR_OPENGL':            '',
    'LIBDIR_QT':                'D:\\Qt\\4.6.2-static-vc90\\lib',
    'LIBDIR_X11':               '',
    'LIBS':                     '',
    'LIBS_CONSOLE':             '',
    'LIBS_CORE':                'kernel32.lib user32.lib shell32.lib uuid.lib ole32.lib advapi32.lib ws2_32.lib',
    'LIBS_GUI':                 'gdi32.lib comdlg32.lib oleaut32.lib imm32.lib winmm.lib winspool.lib ws2_32.lib ole32.lib user32.lib advapi32.lib',
    'LIBS_NETWORK':             'ws2_32.lib',
    'LIBS_OPENGL':              'opengl32.lib glu32.lib gdi32.lib user32.lib',
    'LIBS_RT':                  '',
    'LIBS_RTMT':                '',
    'LIBS_THREAD':              '',
    'LIBS_WEBKIT':              'shlwapi.lib version.lib',
    'LIBS_WINDOWS':             '',
    'LIBS_X11':                 '',
    'LINK':                     'link',
    'LINK_SHLIB':               '',
    'LINK_SHLIB_CMD':           '',
    'MAKEFILE_GENERATOR':       'MSVC.NET',
    'MKDIR':                    'mkdir',
    'MOC':                      'D:\\Qt\\4.6.2-static-vc90\\bin\\moc.exe',
    'RANLIB':                   '',
    'RPATH':                    '',
    'STRIP':                    ''
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
