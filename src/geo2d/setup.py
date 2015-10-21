from distutils.core import setup
from distutils.extension import Extension
import sipdistutils
import os

undef_macros = []
if not os.environ.get("BUILD_DEPLOY", 0):
    undef_macros = ["NDEBUG"]


def check_depends(paths):
    for f in paths:
        if not os.path.exists(f):
            raise IOError("File %r listed in 'depends' is missing!" % f)
    return paths

setup(
  name = 'geo2d._geo2dcpp',
  ext_modules=[
    Extension("_geo2dcpp",
              sources =
              [
                "sip/_geo2dcpp.sip",
                "src/geo2d.cpp",
                "src/offset.cpp",
                "src/simplify.cpp",
                "src/unisci.cpp",
                "src/earcut.cpp",
                "src/spline.cpp",
                "src/dparadxgrid.cpp",
                "src/placement.cpp",
                "src/inside.cpp",
                "src/intersect.cpp",
                "src/xsarea.cpp",
                "src/bump.cpp",
                "src/areas.cpp",
                "src/oldspline.cpp",
                "src/complexspline.cpp",
                "src/unione_path.cpp",
                "src/base64.cpp",
                "src/convexhull.cpp",
                "src/packing.cpp",
                "src/rectscover.cpp",
                "src/path_matching.cpp",
                "src/bbox.cpp",
                "src/isurf.cpp",
                "src/memorytest.cpp",
                "src/poly2d.cpp",
                "src/mlbspi.cpp",
                "src/minimumbbox.cpp"
              ],
              depends = check_depends(
              [
                "src/geo2d.h",
                "src/offset.h",
                "src/simplify.h",
                "src/unisci.h",
                "src/earcut.h",
                "src/spline.h",
                "src/dpara.h",
                "src/dumptocpz.h",
                "src/dxgrid.h",
                "src/placement.h",
                "src/areas.h",
                "src/inside.h",
                "src/intersect.h",
                "src/packing.h",
                "src/rectscover.h",
                "src/oldspline.h",
                "src/complexspline.h",
                "src/unione_path.h",
                "src/base64.h",
                "src/marshaller_geo2d.h",
                "src/convexhull.h",
                "src/path_matching.h",
                "src/bbox.h",
                "src/isurf.h",
                "src/memorytest.h",
                "src/mlbspi.h",
                "src/ndvect.h",
                "sip/geo2dp.sip",
                "sip/geo2dx.sip",
                "sip/geo2dpath.sip",
                "sip/offset.sip",
                "sip/simplify.sip",
                "sip/convexhull.sip",
                "sip/unisci.sip",
                "sip/earcut.sip",
                "sip/spline.sip",
                "sip/dpara.sip",
                "sip/placement.sip",
                "sip/inside.sip",
                "sip/intersect.sip",
                "sip/area.sip",
                "sip/xsarea.sip",
                "sip/bump.sip",
                "sip/areas.sip",
                "sip/oldspline.sip",
                "sip/complexspline.sip",
                "sip/unione_path.sip",
                "sip/stlextra.sip",
                "sip/packing.sip",
                "sip/rectscover.sip",
                "sip/path_matching.sip",
                "sip/bbox.sip",
                "sip/isurf.sip",
                "sip/memorytest.sip",
                "sip/poly2d.sip",
                "sip/mlbspi.sip",
                "sip/minimumbbox.sip"
              ]),
              include_dirs=["src"],
              define_macros=[
                ("USE_PYMALLOC", "1"),
              ],

              # Scommentare per compilare questa estensione in debug (msvc only)
##              extra_compile_args=[
##               "/Od", "/Zi",
##              ],
##              extra_link_args=[
##               "/DEBUG",
##              ],

              # Scommentare per compilare questa estensione in debug (g++ only)
##              extra_compile_args=[
##               "-O0", "-g3", "-D_GLIBCXX_DEBUG", "-D_DEBUG",
##              ],
##              extra_link_args=[
##               "-g3",
##              ],
              undef_macros=undef_macros,
        ),
    ],
  cmdclass = {'build_ext': sipdistutils.build_ext}
)


# Quando trova comcom2 compila il supporto per C2Stream e il wrapper sip dei
# marshaller geo2d
if os.path.exists("../comcom2"):
    print 'Extending geo2d with comcom2 support.'
    marshaller_cfg = {}
    execfile("../comcom2/marshaller.cfg", {'path':'../comcom2/'}, marshaller_cfg)
    setup(
      name = 'geo2d.marshaller',
      ext_modules=[
        Extension("marshaller",
                  [
                    "comcom2/marshaller_geo2d.sip",
                    "src/geo2d.cpp",
                    "src/base64.cpp",
                    "src/inside.cpp",
                  ] + marshaller_cfg['cpps'],
                  depends =
                  [
                    "src/geo2d.h",
                    "src/marshaller_geo2d.h",
                    "sip/geo2dp.sip",
                    "sip/geo2dx.sip",
                    "sip/geo2dpath.sip",
                    "comcom2/marshaller_geo2d.sip",
                  ] + marshaller_cfg['deps'],
                  include_dirs=["src", "comcom2"] + marshaller_cfg['incl_dirs'],
                  define_macros=[
                    ("USE_PYMALLOC", "1"),
                  ],
                  libraries = marshaller_cfg['libs'],
                  # Scommentare per compilare questa estensione in debug (msvc only)
    ##              extra_compile_args=[
    ##               "/Od", "/Zi",
    ##              ],
    ##              extra_link_args=[
    ##               "/DEBUG",
    ##              ],

                  # Scommentare per compilare questa estensione in debug (g++ only)
    ##              extra_compile_args=[
    ##               "-O1", "-g3", "-D_GLIBCXX_DEBUG",
    ##              ],
    ##              extra_link_args=[
    ##               "-g3",
    ##              ],

                  undef_macros=undef_macros,
            ),
        ],
      cmdclass = {'build_ext': sipdistutils.build_ext}
    )
