cmake_minimum_required(VERSION 3.16)

project(libtiled LANGUAGES CXX)

option(TILED_WINDOWS_LAYOUT "Enable Windows layout" OFF)
option(TILED_STATIC_ZSTD "Link zstd statically" OFF)

set(TILED_LIB_DIR "lib" CACHE STRING "Library install directory")

# enable moc/autogen for this subproject
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui)

find_package(PkgConfig)

set(HAVE_ZSTD FALSE)

if(PkgConfig_FOUND)
    pkg_check_modules(ZSTD libzstd)
    if(ZSTD_FOUND)
        set(HAVE_ZSTD TRUE)
    endif()
endif()

if(NOT HAVE_ZSTD)
    find_package(zstd CONFIG QUIET)
    if(zstd_FOUND)
        set(HAVE_ZSTD TRUE)
    endif()
endif()

set(TILED_SOURCES
    compression.cpp
    fileformat.cpp
    filesystemwatcher.cpp
    gidmapper.cpp
    grouplayer.cpp
    hex.cpp
    hexagonalrenderer.cpp
    imagecache.cpp
    imagelayer.cpp
    imagereference.cpp
    isometricrenderer.cpp
    layer.cpp
    logginginterface.cpp
    map.cpp
    mapformat.cpp
    mapobject.cpp
    mapreader.cpp
    maprenderer.cpp
    maptovariantconverter.cpp
    mapwriter.cpp
    minimaprenderer.cpp
    obliquerenderer.cpp
    object.cpp
    objectgroup.cpp
    objecttemplate.cpp
    objecttemplateformat.cpp
    objecttypes.cpp
    orthogonalrenderer.cpp
    plugin.cpp
    pluginmanager.cpp
    properties.cpp
    propertytype.cpp
    savefile.cpp
    staggeredrenderer.cpp
    templatemanager.cpp
    tile.cpp
    tileanimationdriver.cpp
    tiled.cpp
    tilelayer.cpp
    tileset.cpp
    tilesetformat.cpp
    tilesetmanager.cpp
    tmxmapformat.cpp
    varianttomapconverter.cpp
    wangset.cpp
    world.cpp
)

set(TILED_HEADERS
    compression.h
    containerhelpers.h
    fileformat.h
    filesystemwatcher.h
    gidmapper.h
    grid.h
    grouplayer.h
    hex.h
    hexagonalrenderer.h
    imagecache.h
    imagelayer.h
    imagereference.h
    isometricrenderer.h
    layer.h
    logginginterface.h
    map.h
    mapformat.h
    mapobject.h
    mapreader.h
    maprenderer.h
    maptovariantconverter.h
    mapwriter.h
    minimaprenderer.h
    obliquerenderer.h
    object.h
    objectgroup.h
    objecttemplate.h
    objecttemplateformat.h
    objecttypes.h
    orthogonalrenderer.h
    plugin.h
    pluginmanager.h
    properties.h
    propertytype.h
    savefile.h
    staggeredrenderer.h
    templatemanager.h
    tile.h
    tileanimationdriver.h
    tiled.h
    tiled_global.h
    tilelayer.h
    tileset.h
    tilesetformat.h
    tilesetmanager.h
    tmxmapformat.h
    varianttomapconverter.h
    wangset.h
    world.h
)

add_library(libtiled SHARED
    ${TILED_SOURCES}
    ${TILED_HEADERS}
)

set_target_properties(libtiled PROPERTIES
    AUTOMOC ON
    AUTOUIC ON
    AUTORCC ON
)

# make libtiled output its runtime files to HyEditor's folder
set_target_properties(libtiled PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY $<TARGET_FILE_DIR:HyEditor>
  RUNTIME_OUTPUT_DIRECTORY_DEBUG $<TARGET_FILE_DIR:HyEditor>
  RUNTIME_OUTPUT_DIRECTORY_RELEASE $<TARGET_FILE_DIR:HyEditor>
)

target_compile_features(libtiled PUBLIC cxx_std_17)

target_include_directories(libtiled
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<INSTALL_INTERFACE:include/libtiled>
)

target_compile_definitions(libtiled
    PRIVATE
        TILED_LIBRARY
        TILED_LIB_DIR="${TILED_LIB_DIR}"
        QT_NO_CAST_FROM_ASCII
        QT_NO_CAST_TO_ASCII
        QT_NO_URL_CAST_FROM_STRING
        QT_DISABLE_DEPRECATED_BEFORE=0x050F00
        QT_NO_DEPRECATED_WARNINGS
        _USE_MATH_DEFINES
)

if(TILED_WINDOWS_LAYOUT)
    target_compile_definitions(libtiled PRIVATE TILED_WINDOWS_LAYOUT)
endif()

target_link_libraries(libtiled
    PUBLIC
        Qt6::Core
        Qt6::Gui
)

if(HAVE_ZSTD)
    target_compile_definitions(libtiled PRIVATE TILED_ZSTD_SUPPORT)

    if(TARGET zstd::libzstd_shared)
        target_link_libraries(libtiled PRIVATE zstd::libzstd_shared)
    elseif(TARGET zstd::libzstd_static)
        target_link_libraries(libtiled PRIVATE zstd::libzstd_static)
    elseif(ZSTD_FOUND)
        target_include_directories(libtiled PRIVATE ${ZSTD_INCLUDE_DIRS})
        target_link_directories(libtiled PRIVATE ${ZSTD_LIBRARY_DIRS})
        target_compile_options(libtiled PRIVATE ${ZSTD_CFLAGS_OTHER})
        target_link_libraries(libtiled PRIVATE ${ZSTD_LIBRARIES})
    endif()
endif()

if(MSVC)
    target_compile_options(libtiled PRIVATE /W3)
else()
    target_compile_options(libtiled PRIVATE
        -fvisibility=hidden
        -fvisibility-inlines-hidden
    )
endif()

set_target_properties(libtiled PROPERTIES
    OUTPUT_NAME libtiled
)