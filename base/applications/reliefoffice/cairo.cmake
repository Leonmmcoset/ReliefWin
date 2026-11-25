set(_cairo ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cairo)
set(_pixman ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/pixman)
set(_cairo_generated ${_generated}/cairo)
set(_pixman_generated ${_generated}/pixman)
file(MAKE_DIRECTORY ${_cairo_generated} ${_pixman_generated})
file(CONFIGURE OUTPUT ${_cairo_generated}/cairo-features.h CONTENT "#ifndef CAIRO_FEATURES_H\n#define CAIRO_FEATURES_H\n#define CAIRO_HAS_IMAGE_SURFACE 1\n#define CAIRO_HAS_PNG_FUNCTIONS 1\n#define CAIRO_HAS_USER_FONT 1\n#define CAIRO_HAS_RECORDING_SURFACE 1\n#define CAIRO_HAS_OBSERVER_SURFACE 1\n#define CAIRO_HAS_MIME_SURFACE 1\n#endif\n")
file(CONFIGURE OUTPUT ${_cairo_generated}/config.h CONTENT "#define PACKAGE \"cairo\"\n#define PACKAGE_VERSION \"1.18.4\"\n#define SIZEOF_VOID_P ${CMAKE_SIZEOF_VOID_P}\n#define SIZEOF_INT 4\n#define SIZEOF_LONG 4\n#define SIZEOF_LONG_LONG 8\n#define SIZEOF_SIZE_T ${CMAKE_SIZEOF_VOID_P}\n#define HAVE_STDINT_H 1\n#define HAVE_INTTYPES_H 1\n#define HAVE_UINT64_T 1\n#define HAVE_FCNTL_H 1\n#define HAVE_SYS_STAT_H 1\n#define HAVE_TIME_H 1\n#define HAVE_SETJMP_H 1\n#define HAVE_SIGNAL_H 1\n#define HAVE_IO_H 1\n#define HAVE_CXX11_ATOMIC_PRIMITIVES 1\n#define HAS_PIXMAN_GLYPHS 1\n#define HAS_PIXMAN_r8g8b8_sRGB 1\n")
file(CONFIGURE OUTPUT ${_pixman_generated}/pixman-config.h CONTENT "#define PACKAGE \"pixman\"\n#define SIZEOF_LONG 4\n#define HAVE_BUILTIN_CLZ 1\n#define TLS __declspec(thread)\n")
set(PIXMAN_VERSION_MAJOR 0)
set(PIXMAN_VERSION_MINOR 44)
set(PIXMAN_VERSION_MICRO 2)
configure_file(${_pixman}/pixman/pixman-version.h.in ${_pixman_generated}/pixman-version.h @ONLY)
set(_pixman_sources
    ${_pixman}/pixman/pixman.c
    ${_pixman}/pixman/pixman-access.c
    ${_pixman}/pixman/pixman-access-accessors.c
    ${_pixman}/pixman/pixman-arm.c
    ${_pixman}/pixman/pixman-bits-image.c
    ${_pixman}/pixman/pixman-combine32.c
    ${_pixman}/pixman/pixman-combine-float.c
    ${_pixman}/pixman/pixman-conical-gradient.c
    ${_pixman}/pixman/pixman-edge.c
    ${_pixman}/pixman/pixman-edge-accessors.c
    ${_pixman}/pixman/pixman-fast-path.c
    ${_pixman}/pixman/pixman-filter.c
    ${_pixman}/pixman/pixman-glyph.c
    ${_pixman}/pixman/pixman-general.c
    ${_pixman}/pixman/pixman-gradient-walker.c
    ${_pixman}/pixman/pixman-image.c
    ${_pixman}/pixman/pixman-implementation.c
    ${_pixman}/pixman/pixman-linear-gradient.c
    ${_pixman}/pixman/pixman-matrix.c
    ${_pixman}/pixman/pixman-mips.c
    ${_pixman}/pixman/pixman-noop.c
    ${_pixman}/pixman/pixman-ppc.c
    ${_pixman}/pixman/pixman-radial-gradient.c
    ${_pixman}/pixman/pixman-region16.c
    ${_pixman}/pixman/pixman-region32.c
    ${_pixman}/pixman/pixman-riscv.c
    ${_pixman}/pixman/pixman-solid-fill.c
    ${_pixman}/pixman/pixman-timer.c
    ${_pixman}/pixman/pixman-trap.c
    ${_pixman}/pixman/pixman-utils.c
    ${_pixman}/pixman/pixman-x86.c
)
add_library(reliefoffice_pixman STATIC ${_pixman_sources})
target_include_directories(reliefoffice_pixman PUBLIC ${_pixman}/pixman ${_pixman_generated})
target_compile_definitions(reliefoffice_pixman PRIVATE HAVE_CONFIG_H PIXMAN_API=)
set(_cairo_sources
    ${_cairo}/src/cairo-analysis-surface.c
    ${_cairo}/src/cairo-arc.c
    ${_cairo}/src/cairo-array.c
    ${_cairo}/src/cairo-atomic.c
    ${_cairo}/src/cairo-base64-stream.c
    ${_cairo}/src/cairo-base85-stream.c
    ${_cairo}/src/cairo-bentley-ottmann-rectangular.c
    ${_cairo}/src/cairo-bentley-ottmann-rectilinear.c
    ${_cairo}/src/cairo-bentley-ottmann.c
    ${_cairo}/src/cairo-botor-scan-converter.c
    ${_cairo}/src/cairo-boxes-intersect.c
    ${_cairo}/src/cairo-boxes.c
    ${_cairo}/src/cairo-cache.c
    ${_cairo}/src/cairo-clip-boxes.c
    ${_cairo}/src/cairo-clip-polygon.c
    ${_cairo}/src/cairo-clip-region.c
    ${_cairo}/src/cairo-clip-surface.c
    ${_cairo}/src/cairo-clip-tor-scan-converter.c
    ${_cairo}/src/cairo-clip.c
    ${_cairo}/src/cairo-color.c
    ${_cairo}/src/cairo-composite-rectangles.c
    ${_cairo}/src/cairo-compositor.c
    ${_cairo}/src/cairo-contour.c
    ${_cairo}/src/cairo-damage.c
    ${_cairo}/src/cairo-debug.c
    ${_cairo}/src/cairo-default-context.c
    ${_cairo}/src/cairo-device.c
    ${_cairo}/src/cairo-error.c
    ${_cairo}/src/cairo-fallback-compositor.c
    ${_cairo}/src/cairo-fixed.c
    ${_cairo}/src/cairo-font-face-twin-data.c
    ${_cairo}/src/cairo-font-face-twin.c
    ${_cairo}/src/cairo-font-face.c
    ${_cairo}/src/cairo-font-options.c
    ${_cairo}/src/cairo-freed-pool.c
    ${_cairo}/src/cairo-freelist.c
    ${_cairo}/src/cairo-gstate.c
    ${_cairo}/src/cairo-hash.c
    ${_cairo}/src/cairo-hull.c
    ${_cairo}/src/cairo-image-compositor.c
    ${_cairo}/src/cairo-image-info.c
    ${_cairo}/src/cairo-image-source.c
    ${_cairo}/src/cairo-image-surface.c
    ${_cairo}/src/cairo-line.c
    ${_cairo}/src/cairo-lzw.c
    ${_cairo}/src/cairo-mask-compositor.c
    ${_cairo}/src/cairo-matrix.c
    ${_cairo}/src/cairo-mempool.c
    ${_cairo}/src/cairo-mesh-pattern-rasterizer.c
    ${_cairo}/src/cairo-misc.c
    ${_cairo}/src/cairo-mono-scan-converter.c
    ${_cairo}/src/cairo-mutex.c
    ${_cairo}/src/cairo-no-compositor.c
    ${_cairo}/src/cairo-observer.c
    ${_cairo}/src/cairo-output-stream.c
    ${_cairo}/src/cairo-paginated-surface.c
    ${_cairo}/src/cairo-path-bounds.c
    ${_cairo}/src/cairo-path-fill.c
    ${_cairo}/src/cairo-path-fixed.c
    ${_cairo}/src/cairo-path-in-fill.c
    ${_cairo}/src/cairo-path-stroke-boxes.c
    ${_cairo}/src/cairo-path-stroke-polygon.c
    ${_cairo}/src/cairo-path-stroke-traps.c
    ${_cairo}/src/cairo-path-stroke-tristrip.c
    ${_cairo}/src/cairo-path-stroke.c
    ${_cairo}/src/cairo-path.c
    ${_cairo}/src/cairo-pattern.c
    ${_cairo}/src/cairo-pen.c
    ${_cairo}/src/cairo-polygon-intersect.c
    ${_cairo}/src/cairo-polygon-reduce.c
    ${_cairo}/src/cairo-polygon.c
    ${_cairo}/src/cairo-raster-source-pattern.c
    ${_cairo}/src/cairo-recording-surface.c
    ${_cairo}/src/cairo-rectangle.c
    ${_cairo}/src/cairo-rectangular-scan-converter.c
    ${_cairo}/src/cairo-region.c
    ${_cairo}/src/cairo-rtree.c
    ${_cairo}/src/cairo-scaled-font.c
    ${_cairo}/src/cairo-shape-mask-compositor.c
    ${_cairo}/src/cairo-slope.c
    ${_cairo}/src/cairo-spans-compositor.c
    ${_cairo}/src/cairo-spans.c
    ${_cairo}/src/cairo-spline.c
    ${_cairo}/src/cairo-stroke-dash.c
    ${_cairo}/src/cairo-stroke-style.c
    ${_cairo}/src/cairo-surface-clipper.c
    ${_cairo}/src/cairo-surface-fallback.c
    ${_cairo}/src/cairo-surface-observer.c
    ${_cairo}/src/cairo-surface-offset.c
    ${_cairo}/src/cairo-surface-snapshot.c
    ${_cairo}/src/cairo-surface-subsurface.c
    ${_cairo}/src/cairo-surface-wrapper.c
    ${_cairo}/src/cairo-surface.c
    ${_cairo}/src/cairo-time.c
    ${_cairo}/src/cairo-tor-scan-converter.c
    ${_cairo}/src/cairo-tor22-scan-converter.c
    ${_cairo}/src/cairo-toy-font-face.c
    ${_cairo}/src/cairo-traps-compositor.c
    ${_cairo}/src/cairo-traps.c
    ${_cairo}/src/cairo-tristrip.c
    ${_cairo}/src/cairo-unicode.c
    ${_cairo}/src/cairo-user-font.c
    ${_cairo}/src/cairo-version.c
    ${_cairo}/src/cairo-wideint.c
    ${_cairo}/src/cairo.c
    ${_cairo}/src/cairo-cff-subset.c
    ${_cairo}/src/cairo-scaled-font-subsets.c
    ${_cairo}/src/cairo-truetype-subset.c
    ${_cairo}/src/cairo-type1-fallback.c
    ${_cairo}/src/cairo-type1-glyph-names.c
    ${_cairo}/src/cairo-type1-subset.c
    ${_cairo}/src/cairo-type3-glyph-surface.c
    ${_cairo}/src/cairo-pdf-operators.c
    ${_cairo}/src/cairo-pdf-shading.c
    ${_cairo}/src/cairo-tag-attributes.c
    ${_cairo}/src/cairo-tag-stack.c
    ${_cairo}/src/cairo-deflate-stream.c
    ${_cairo}/src/cairo-png.c
)
add_library(reliefoffice_cairo STATIC ${_cairo_sources})
target_include_directories(reliefoffice_cairo BEFORE PUBLIC ${_cairo}/src ${_cairo_generated})
target_compile_definitions(reliefoffice_cairo PRIVATE HAVE_CONFIG_H PUBLIC CAIRO_WIN32_STATIC_BUILD)
target_link_libraries(reliefoffice_cairo PUBLIC reliefoffice_pixman reliefoffice_png libkernel32)
if(CMAKE_C_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(reliefoffice_cairo PRIVATE -Wno-error)
    target_compile_options(reliefoffice_pixman PRIVATE -Wno-error)
endif()
