# cmake - geo2d library
project(GEO2D)

cmake_minimum_required(VERSION 2.6)

set(QT_SUPPORT OFF CACHE BOOL "Enable Qt data types support flag")

add_library(geo2d areas.cpp base64.cpp bump.cpp complexspline.cpp
            dparadxgrid.cpp earcut.cpp geo2d.cpp inside.cpp offset.cpp isurf.cpp
            oldspline.cpp placement.cpp spline.cpp unione_path.cpp unisci.cpp
            xsarea.cpp intersect.cpp convexhull.cpp circle_pivoting.cpp poly2d.cpp
            rectscover.cpp packing.cpp path_matching.cpp mlbspi.cpp simplify.cpp)

if(QT_SUPPORT)
    cmake_minimum_required(VERSION 2.8.11)
    find_package(Qt5Core)
    find_package(Qt5Gui)
    add_definitions(-DGEO2D_QT_SUPPORT)
    target_link_libraries(geo2d Qt5::Core Qt5::Gui)
endif(QT_SUPPORT)

if(LIBRARY_OUTPUT_PATH)
    set (IDIR ${LIBRARY_OUTPUT_PATH})
else()
    set (IDIR ${CMAKE_CFG_INTDIR})
endif()

set(GNAME ${CMAKE_STATIC_LIBRARY_PREFIX}geo2d${CMAKE_STATIC_LIBRARY_SUFFIX})

add_custom_command(TARGET geo2d
                   POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${IDIR}/${GNAME} ${PROJECT_SOURCE_DIR}/libs/${GNAME}
                   COMMENT "geo2d: installing ${GNAME} from ${IDIR} in ${PROJECT_SOURCE_DIR}/libs/")

add_subdirectory(test)
