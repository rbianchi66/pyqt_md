import os

src_dir = @src_dir@
extras_dir = @extras_dir@
excludes = @excludes@
name = @name@
icons = @icons@
entrypoint = @entrypoint@
version = @version@
console = @console@

a = Analysis(['_boot.py',
              os.path.join(HOMEPATH,'support/_mountzlib.py'),
              os.path.join(HOMEPATH,'support/useUnicode.py'),
              entrypoint],
             pathex=[src_dir],
             excludes=excludes)
pyz = PYZ(a.pure, crypt="key.dat")

manifest = None
if os.name == "nt":
    name += ".exe"
    manifest = os.path.join(HOMEPATH, 'support/VC90CRT.manifest')

exe = EXE( pyz,
          a.scripts + [('u', '', 'OPTION')],
          a.zipfiles,
          a.binaries,
          a.datas,
          name=name,
          debug=0,
          strip=0,
          crypt=True,
          icon=icons,
          upx=1,
          console=console,
          manifest=manifest)

# FIXME: a regime lo specfile deve essere multipiattaforma; sotto windows il
# target BUNDLE non esegue alcuna operazione.
import sys
if sys.platform.startswith("darwin"):
    # L'argomento name e' il percorso dove sara' creato il bundle, relativo alla
    # directory "temp".
    app = BUNDLE(exe,
                 name="%s.app" % (name,),
                 appname=name,
                 version=version)
