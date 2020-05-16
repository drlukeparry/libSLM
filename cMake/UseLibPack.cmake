# ================================================================================
# == Win32 is default behaviour use the LibPack copied in Source tree ============

# --------------------------------------------------------------------------------
# General includes

link_directories(${amVDB_LIBPACK_DIR}/lib)
include_directories(${amVDB_LIBPACK_DIR}/include)

# OpenGL
set(OPENGL_gl_LIBRARY opengl32 glu32)

# Boost
set(Boost_INCLUDE_DIRS ${amVDB_LIBPACK_DIR}/include/boost/boost-1_55)
set(Boost_LIBRARIES
    optimized boost_filesystem-vc110-mt-1_55.lib
    optimized boost_system-vc110-mt-1_55.lib
    optimized boost_graph-vc110-mt-1_55.lib
    optimized boost_program_options-vc110-mt-1_55.lib
    optimized boost_regex-vc110-mt-1_55.lib
    optimized boost_signals-vc110-mt-1_55.lib
    optimized boost_thread-vc110-mt-1_55.lib
)
set(Boost_DEBUG_LIBRARIES
    debug boost_filesystem-vc110-mt-gd-1_55.lib
    debug boost_system-vc110-mt-gd-1_55.lib
    debug boost_graph-vc110-mt-gd-1_55.lib
    debug boost_program_options-vc110-mt-gd-1_55.lib
    debug boost_regex-vc110-mt-gd-1_55.lib
    debug boost_signals-vc110-mt-gd-1_55.lib
    debug boost_thread-vc110-mt-gd-1_55.lib
)
set(Boost_FOUND TRUE)

# OpenEXR Half Libraries
set(OPENEXR_INCLUDE_PATHS  ${amVDB_LIBPACK_DIR}/include/openexr/ilmbase/Half)
set(OPENEXR_LIBRARIES  Half.lib)
set(OPENEXR_FOUND TRUE)

# Zlib
set(ZLIB_INCLUDE_DIR ${amVDB_LIBPACK_DIR}/include/zlib-1.2.3)
set(ZLIB_LIBRARIES  zlibstat.lib)
set(ZLIB_FOUND TRUE)

# openVDB
set(OPENVDB_INCLUDE_DIR ${amVDB_LIBPACK_DIR}/include/openvdb-2.3)
set(OPENVDB_LIBRARIES openvdb.lib)
set(OPENVDB_FOUND TRUE)

# Intel Theaded Building Blocks (TBB)
set(TBB_INCLUDE_DIR ${amVDB_LIBPACK_DIR}/include/tbb)
set(TBB_LIBRARIES
	tbb.lib
	tbb_preview.lib
	tbbmalloc.lib
	tbbmalloc_proxy.lib
	tbbproxy.lib
)

set(TBB_DEBUG_LIBRARIES
	tbb_debug.lib
	tbb_preview_debug.lib
	tbbmalloc_debug.lib
	tbbmalloc_proxy_debug.lib
	tbbproxy_debug.lib
)

set(TBB_FOUND TRUE)

