# CMakeLists.txt for ALURE
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
PROJECT(ALURE)
IF(COMMAND CMAKE_POLICY)
    CMAKE_POLICY(SET CMP0003 NEW)
ENDIF(COMMAND CMAKE_POLICY)

SET(CMAKE_MODULE_PATH "${ALURE_SOURCE_DIR}/cmake")

INCLUDE(CheckCCompilerFlag)
INCLUDE(CheckCSourceCompiles)
INCLUDE(CheckFunctionExists)
INCLUDE(CheckIncludeFile)
INCLUDE(CheckIncludeFiles)
INCLUDE(CheckLibraryExists)
INCLUDE(CheckSharedLibraryExists)
INCLUDE(FindPkgConfig)
INCLUDE(UsePkgConfig)
INCLUDE(CheckFileOffsetBits)



IF(WIN32)
    SET(LIBNAME ALURE32)
ELSE(WIN32)
    SET(LIBNAME alure)
ENDIF(WIN32)

SET(LIB_MAJOR_VERSION "1")
SET(LIB_MINOR_VERSION "0")
SET(LIB_BUILD_VERSION "72")
SET(LIB_VERSION "${LIB_MAJOR_VERSION}.${LIB_MINOR_VERSION}.${LIB_BUILD_VERSION}")
IF(NOT DEFINED LIB_INSTALL_DIR)
    SET(LIB_INSTALL_DIR "lib${LIB_SUFFIX}")
ENDIF(NOT DEFINED LIB_INSTALL_DIR)


INCLUDE_DIRECTORIES(include "${ALURE_BINARY_DIR}")


OPTION(NO_SHARED "Build a static library instead of shared" ON)

IF(NOT CMAKE_BUILD_TYPE)
    SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
        "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel."
        FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)
IF(NOT DEFINED CMAKE_DEBUG_POSTFIX)
    SET(CMAKE_DEBUG_POSTFIX "" CACHE STRING
        "Library postfix for debug builds. Normally left blank."
        FORCE)
ENDIF(NOT DEFINED CMAKE_DEBUG_POSTFIX)


ADD_DEFINITIONS(-DALURE_BUILD_LIBRARY -DHAVE_CONFIG_H)

CHECK_SHARED_LIBRARY_EXISTS(stdc++ memset "" HAS_STDCXX)
IF(HAS_STDCXX)
    SET(EXTRA_LIBS stdc++ ${EXTRA_LIBS})
ENDIF(HAS_STDCXX)

CHECK_FILE_OFFSET_BITS()
IF(_FILE_OFFSET_BITS)
    ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=${_FILE_OFFSET_BITS})
    SET(CMAKE_REQUIRED_DEFINITIONS ${CMAKE_REQUIRED_DEFINITIONS} -D_FILE_OFFSET_BITS=${_FILE_OFFSET_BITS})
ENDIF(_FILE_OFFSET_BITS)

CHECK_FUNCTION_EXISTS(fseeko HAVE_FSEEKO)

IF(MSVC)
    # ???
ELSE(MSVC)
    IF(NOT WIN32)
        CHECK_C_SOURCE_COMPILES("int foo() __attribute__((constructor));
                                 int main() {return 0;}" HAVE_GCC_CONSTRUCTOR)

        CHECK_C_COMPILER_FLAG(-fvisibility=hidden HAVE_GCC_VISIBILITY)
        IF(HAVE_GCC_VISIBILITY)
            ADD_DEFINITIONS(-fvisibility=hidden)
        ENDIF(HAVE_GCC_VISIBILITY)
    ENDIF(NOT WIN32)

    CHECK_C_COMPILER_FLAG(-Wextra HAVE_WEXTRA)
    IF(HAVE_WEXTRA)
        ADD_DEFINITIONS(-Wextra)
    ENDIF(HAVE_WEXTRA)
    ADD_DEFINITIONS(-Wall)
ENDIF(MSVC)


CHECK_INCLUDE_FILE(windows.h HAVE_WINDOWS_H -D_WIN32_WINNT=0x0500)
IF(NOT HAVE_WINDOWS_H)
    CHECK_C_COMPILER_FLAG(-pthread HAVE_PTHREAD)
    IF(HAVE_PTHREAD)
        ADD_DEFINITIONS(-pthread)
        SET(EXTRA_LIBS ${EXTRA_LIBS} -pthread)
    ENDIF(HAVE_PTHREAD)

    # We need pthreads outside of Windows
    CHECK_INCLUDE_FILE(pthread.h HAVE_PTHREAD_H)
    IF(NOT HAVE_PTHREAD_H)
        MESSAGE(FATAL_ERROR "PThreads is required for non-Windows builds!")
    ENDIF()
    # Some systems need pthread_np.h to get recursive mutexes
    CHECK_INCLUDE_FILES("pthread.h;pthread_np.h" HAVE_PTHREAD_NP_H)

    # _GNU_SOURCE is needed on some systems for extra attributes, and
    # _REENTRANT is needed for libc thread-safety
    ADD_DEFINITIONS(-D_GNU_SOURCE=1)
    CHECK_FUNCTION_EXISTS(pthread_create HAS_PTHREAD_CREATE)
    IF(NOT HAS_PTHREAD_CREATE)
        CHECK_SHARED_LIBRARY_EXISTS(pthread pthread_create "" HAVE_LIBPTHREAD)
        IF(NOT HAVE_LIBPTHREAD)
            MESSAGE(FATAL_ERROR "pthread_create not found on non-Windows target!")
        ENDIF(NOT HAVE_LIBPTHREAD)
        SET(EXTRA_LIBS pthread ${EXTRA_LIBS})
    ENDIF(NOT HAS_PTHREAD_CREATE)

    CHECK_FUNCTION_EXISTS(nanosleep HAVE_NANOSLEEP)
    IF(NOT HAVE_NANOSLEEP)
        MESSAGE(FATAL_ERROR "No sleep function found!")
    ENDIF(NOT HAVE_NANOSLEEP)
ENDIF(NOT HAVE_WINDOWS_H)


OPTION(SNDFILE   "SoundFile support (for various formats)" ON)
OPTION(VORBIS    "VorbisFile support (for Ogg Vorbis)"  ON)
OPTION(FLAC      "FLAC support (for FLAC and Ogg FLAC)" ON)
OPTION(MPG123    "MPG123 support (for MP1/MP2/MP3)" ON)
OPTION(GSTREAMER "GStreamer support" OFF)

IF(WIN32)
    ADD_DEFINITIONS(-D_WIN32)

    IF(CMAKE_SIZEOF_VOID_P MATCHES "8")
        SET(WIN_TYPE Win64)
        SET(XBITS x64)
    ELSE(CMAKE_SIZEOF_VOID_P MATCHES "8")
        SET(WIN_TYPE Win32)
        SET(XBITS x86)
    ENDIF(CMAKE_SIZEOF_VOID_P MATCHES "8")

    SET(OPENAL_SEARCH_PATHS
        ENV ALSDK_DIR
        ENV MINGDIR
        "C:/Program Files/OpenAL 1.1 SDK"
        "C:/Program Files (x86)/OpenAL 1.1 SDK"
        "E:/Programs/Dev/OpenAL"
        "E:/Program Files (x86)/OpenAL 1.1 SDK" )

    FIND_PATH(OPENAL_INCLUDE_DIR al.h
              PATHS ${OPENAL_SEARCH_PATHS}
              PATH_SUFFIXES include include/AL )
    IF(OPENAL_INCLUDE_DIR)
        INCLUDE_DIRECTORIES(${OPENAL_INCLUDE_DIR})
        FIND_LIBRARY(OPENAL_LIBRARIES OpenAL32
                     PATHS ${OPENAL_SEARCH_PATHS}
                     PATH_SUFFIXES lib libs/${WIN_TYPE} )
    ENDIF(OPENAL_INCLUDE_DIR)

    IF(NOT OPENAL_LIBRARIES)
        MESSAGE(FATAL_ERROR "OpenAL not found!\nPlease try setting the ALSDK_DIR environment variable and try again.")
    ENDIF(NOT OPENAL_LIBRARIES)

    MESSAGE(STATUS "OpenAL include: ${OPENAL_INCLUDE_DIR}")
    MESSAGE(STATUS "OpenAL lib: ${OPENAL_LIBRARIES}")
ELSE(WIN32)
    PKG_CHECK_MODULES(OPENAL openal)
    IF(NOT OPENAL_FOUND)
        IF(APPLE)
            CHECK_INCLUDE_FILE(OpenAL/al.h HAVE_OPENAL_AL_H)
            IF(HAVE_OPENAL_AL_H)
                SET(OPENAL_LIBRARIES "-framework OpenAL")
                SET(HAS_OPENAL 1)
            ENDIF(HAVE_OPENAL_AL_H)
        ELSE(APPLE)
            CHECK_INCLUDE_FILE(AL/al.h HAVE_AL_AL_H)
            IF(HAVE_AL_AL_H)
                CHECK_SHARED_LIBRARY_EXISTS(openal alcOpenDevice "" HAS_OPENAL)
                IF(HAS_OPENAL)
                    SET(OPENAL_LIBRARIES openal)
                ENDIF(HAS_OPENAL)
            ENDIF(HAVE_AL_AL_H)
        ENDIF(APPLE)
        IF(NOT HAS_OPENAL)
            MESSAGE(FATAL_ERROR "OpenAL not found!")
        ENDIF(NOT HAS_OPENAL)
    ELSE(NOT OPENAL_FOUND)
        INCLUDE_DIRECTORIES(${OPENAL_INCLUDE_DIRS})
        LINK_DIRECTORIES(${OPENAL_LIBRARY_DIRS})
    ENDIF(NOT OPENAL_FOUND)
ENDIF(WIN32)

# SndFile support
SET( SNDFILE 0 )
# IF(SNDFILE)
#     PKG_CHECK_MODULES(SNDFILE sndfile)
#     IF(NOT SNDFILE_FOUND)
#         CHECK_INCLUDE_FILE(sndfile.h HAVE_SNDFILE_H)
#         IF(HAVE_SNDFILE_H)
#             CHECK_SHARED_LIBRARY_EXISTS(sndfile sf_open "" HAS_SNDFILE)
#             IF(HAS_SNDFILE)
#                 SET(SNDFILE_LIBRARIES "sndfile")
#             ELSE(HAS_SNDFILE)
#                 CHECK_SHARED_LIBRARY_EXISTS(libsndfile-1 sf_open "" HAS_LIBSNDFILE1)
#                 IF(HAS_LIBSNDFILE1)
#                     SET(HAS_SNDFILE 1)
#                     SET(SNDFILE_LIBRARIES "libsndfile-1")
#                 ENDIF(HAS_LIBSNDFILE1)
#             ENDIF(HAS_SNDFILE)
#         ENDIF(HAVE_SNDFILE_H)
#     ELSE(NOT SNDFILE_FOUND)
#         SET(HAS_SNDFILE 1)
#         INCLUDE_DIRECTORIES(${SNDFILE_INCLUDE_DIRS})
#         LINK_DIRECTORIES(${SNDFILE_LIBRARY_DIRS})
#     ENDIF(NOT SNDFILE_FOUND)
# ELSE(SNDFILE)
#     SET(SNDFILE_LIBRARIES "")
# ENDIF(SNDFILE)

# Vorbis support
IF(VORBIS)
    PKG_CHECK_MODULES(VORBISFILE vorbisfile)
    IF(NOT VORBISFILE_FOUND)
        CHECK_INCLUDE_FILE(vorbis/vorbisfile.h HAVE_VORBIS_VORBISFILE_H)
        IF(HAVE_VORBIS_VORBISFILE_H)
            CHECK_SHARED_LIBRARY_EXISTS(vorbisfile ov_open "" HAS_VORBISFILE)
            IF(HAS_VORBISFILE)
                SET(VORBISFILE_LIBRARIES "vorbisfile")
            ENDIF(HAS_VORBISFILE)
        ENDIF(HAVE_VORBIS_VORBISFILE_H)
    ELSE(NOT VORBISFILE_FOUND)
        SET(HAS_VORBISFILE 1)
        INCLUDE_DIRECTORIES(${VORBISFILE_INCLUDE_DIRS})
        LINK_DIRECTORIES(${VORBISFILE_LIBRARY_DIRS})
    ENDIF(NOT VORBISFILE_FOUND)
ELSE(VORBIS)
    SET(VORBISFILE_LIBRARIES "")
ENDIF(VORBIS)

# FLAC support
SET( FLAC 0 )
# IF(FLAC)
#     #PKG_CHECK_MODULES(LIBFLAC flac)
#     #IF(NOT LIBFLAC_FOUND)
#         CHECK_INCLUDE_FILE(FLAC/all.h HAVE_FLAC_ALL_H)
#         IF(HAVE_FLAC_ALL_H)
#             CHECK_SHARED_LIBRARY_EXISTS(FLAC FLAC__stream_decoder_new "" HAS_FLAC)
#             IF(HAS_FLAC)
#                 SET(LIBFLAC_LIBRARIES "FLAC")
#             ENDIF(HAS_FLAC)
#         ENDIF(HAVE_FLAC_ALL_H)
#     #ELSE(NOT LIBFLAC)
#     #    SET(HAS_FLAC 1)
#     #    INCLUDE_DIRECTORIES(${LIBFLAC_INCLUDE_DIRS})
#     #    LINK_DIRECTORIES(${LIBFLAC_LIBRARY_DIRS})
#     #ENDIF(NOT LIBFLAC_FOUND)
# ELSE(FLAC)
#     SET(LIBFLAC_LIBRARIES "")
# ENDIF(FLAC)

# MPG123 support
IF(MPG123)
    PKG_CHECK_MODULES(MPG123 libmpg123)
    IF(NOT MPG123_FOUND)
        CHECK_INCLUDE_FILE(mpg123.h HAVE_MPG123_H)
        IF(HAVE_MPG123_H)
            CHECK_SHARED_LIBRARY_EXISTS(mpg123 mpg123_init "" HAS_MPG123)
            IF(HAS_MPG123)
                SET(MPG123_LIBRARIES "mpg123")
            ENDIF(HAS_MPG123)
        ENDIF(HAVE_MPG123_H)
    ELSE(NOT MPG123_FOUND)
        SET(HAS_MPG123 1)
        INCLUDE_DIRECTORIES(${MPG123_INCLUDE_DIRS})
        LINK_DIRECTORIES(${MPG123_LIBRARY_DIRS})
    ENDIF(NOT MPG123_FOUND)
ELSE(MPG123)
    SET(MPG123_LIBRARIES "")
ENDIF(MPG123)

# GStreamer support
IF(GSTREAMER)
    PKG_CHECK_MODULES(GST gstreamer-0.10;gstreamer-plugins-base-0.10)
    IF(GST_FOUND)
        CHECK_SHARED_LIBRARY_EXISTS(gstapp-0.10 gst_app_sink_pull_buffer "" HAS_GSTAPP)
        IF(HAS_GSTAPP)
            SET(HAS_GSTREAMER 1)
            INCLUDE_DIRECTORIES(${GST_INCLUDE_DIRS})
            LINK_DIRECTORIES(${GST_LIBRARY_DIRS})
            SET(EXTRA_LIBS gstapp-0.10 ${EXTRA_LIBS})
        ENDIF(HAS_GSTAPP)
    ELSE(GST_FOUND)
        SET(GST_SEARCH_PATHS
            ENV GSTSDK_DIR
            ENV MINGDIR
            "C:/Program Files/gstreamer"
            "C:/gstreamer" )

        MESSAGE(STATUS "Looking for glib.h")
        FIND_PATH(GLIB_INCLUDE_DIR glib.h
                  PATHS ${GST_SEARCH_PATHS}
                  PATH_SUFFIXES include include/glib-2.0)
        IF(GLIB_INCLUDE_DIR)
            MESSAGE(STATUS "Looking for glib.h - found: ${GLIB_INCLUDE_DIR}")
        ELSE(GLIB_INCLUDE_DIR)
            MESSAGE(STATUS "Looking for glib.h - not found")
        ENDIF(GLIB_INCLUDE_DIR)

        MESSAGE(STATUS "Looking for gst/gst.h")
        FIND_PATH(GST_INCLUDE_DIR gst/gst.h
                  PATHS ${GST_SEARCH_PATHS}
                  PATH_SUFFIXES include include/gstreamer-0.10)
        IF(GST_INCLUDE_DIR)
            MESSAGE(STATUS "Looking for gst/gst.h - found: ${GST_INCLUDE_DIR}")
        ELSE(GST_INCLUDE_DIR)
            MESSAGE(STATUS "Looking for gst/gst.h - not found")
        ENDIF(GST_INCLUDE_DIR)

        IF(GLIB_INCLUDE_DIR AND GST_INCLUDE_DIR)
            INCLUDE_DIRECTORIES("${GLIB_INCLUDE_DIR}" "${GST_INCLUDE_DIR}")
            FIND_LIBRARY(GST_LIBRARY1 NAMES gstapp-0.10
                         PATHS ${GST_SEARCH_PATHS} PATH_SUFFIXES lib )
            FIND_LIBRARY(GST_LIBRARY2 NAMES gstreamer-0.10
                         PATHS ${GST_SEARCH_PATHS} PATH_SUFFIXES lib )
            FIND_LIBRARY(GST_LIBRARY3 NAMES gthread-2.0
                         PATHS ${GST_SEARCH_PATHS} PATH_SUFFIXES lib )
            FIND_LIBRARY(GST_LIBRARY4 NAMES gmodule-2.0
                         PATHS ${GST_SEARCH_PATHS} PATH_SUFFIXES lib )
            FIND_LIBRARY(GST_LIBRARY5 NAMES gobject-2.0
                         PATHS ${GST_SEARCH_PATHS} PATH_SUFFIXES lib )
            FIND_LIBRARY(GST_LIBRARY6 NAMES glib-2.0
                         PATHS ${GST_SEARCH_PATHS} PATH_SUFFIXES lib )
            IF(GST_LIBRARY1 AND GST_LIBRARY2 AND GST_LIBRARY3 AND GST_LIBRARY4 AND GST_LIBRARY5 AND GST_LIBRARY6)
                SET(GST_LIBRARIES "${GST_LIBRARY1}" "${GST_LIBRARY2}" "${GST_LIBRARY3}" "${GST_LIBRARY4}" "${GST_LIBRARY5}" "${GST_LIBRARY6}")
                MESSAGE(STATUS "GStreamer libs: ${GST_LIBRARIES}")
                SET(HAS_GSTREAMER 1)
            ENDIF(GST_LIBRARY1 AND GST_LIBRARY2 AND GST_LIBRARY3 AND GST_LIBRARY4 AND GST_LIBRARY5 AND GST_LIBRARY6)
        ENDIF(GLIB_INCLUDE_DIR AND GST_INCLUDE_DIR)
    ENDIF(GST_FOUND)
ELSE(GSTREAMER)
    SET(GST_LIBRARIES "")
ENDIF(GSTREAMER)

# End configuration
CONFIGURE_FILE(
    "${ALURE_SOURCE_DIR}/alure_config.h.in"
    "${ALURE_BINARY_DIR}/alure_config.h")

SET(ALURE_OBJS  src/alure.cpp
                src/buffer.cpp
                src/istream.cpp
                src/stream.cpp
                src/streamplay.cpp
)

IF(NOT NO_SHARED)
    #build a shared library
    ADD_LIBRARY(${LIBNAME} SHARED ${ALURE_OBJS})
    SET_TARGET_PROPERTIES(${LIBNAME} PROPERTIES VERSION ${LIB_VERSION}
                                                SOVERSION ${LIB_MAJOR_VERSION})
    IF(WIN32)
        SET_TARGET_PROPERTIES(${LIBNAME} PROPERTIES PREFIX "")
    ENDIF(WIN32)
    TARGET_LINK_LIBRARIES(${LIBNAME} ${OPENAL_LIBRARIES} ${SNDFILE_LIBRARIES} ${VORBISFILE_LIBRARIES} ${LIBFLAC_LIBRARIES} ${MPG123_LIBRARIES} ${GST_LIBRARIES} ${EXTRA_LIBS})
ELSE(NOT NO_SHARED)
    #build a static library
    ADD_LIBRARY(${LIBNAME} STATIC ${ALURE_OBJS})
    SET_TARGET_PROPERTIES(${LIBNAME} PROPERTIES VERSION ${LIB_VERSION})
ENDIF(NOT NO_SHARED)


#add an install target here
IF(APPLE)
    SET(INCPATH OpenAL)
ELSE(APPLE)
    SET(INCPATH AL)
ENDIF(APPLE)

# INSTALL(TARGETS ${LIBNAME}
#         RUNTIME DESTINATION bin
#         LIBRARY DESTINATION ${LIB_INSTALL_DIR}
#         ARCHIVE DESTINATION ${LIB_INSTALL_DIR}
# )
# INSTALL(FILES include/AL/alure.h
#         DESTINATION "include/${INCPATH}"
# )
# INSTALL(FILES "${ALURE_BINARY_DIR}/alure.pc"
#         DESTINATION "${LIB_INSTALL_DIR}/pkgconfig"
# )


# MESSAGE(STATUS "\n********************** Configured options **********************")
# IF(NO_SHARED)
#     MESSAGE(STATUS "Building static library")
# ELSE(NO_SHARED)
#     MESSAGE(STATUS "Building shared library")
# ENDIF(NO_SHARED)
# MESSAGE(STATUS "")
# IF(HAS_SNDFILE)
#     MESSAGE(STATUS "SndFile support: enabled")
# ELSE(HAS_SNDFILE)
#     MESSAGE(STATUS "SndFile support: disabled")
# ENDIF(HAS_SNDFILE)
# IF(HAS_VORBISFILE)
#     MESSAGE(STATUS "VorbisFile support: enabled")
# ELSE(HAS_VORBISFILE)
#     MESSAGE(STATUS "VorbisFile support: disabled")
# ENDIF(HAS_VORBISFILE)
# IF(HAS_FLAC)
#     MESSAGE(STATUS "FLAC support: enabled")
# ELSE(HAS_FLAC)
#     MESSAGE(STATUS "FLAC support: disabled")
# ENDIF(HAS_FLAC)
# IF(HAS_MPG123)
#     MESSAGE(STATUS "MPG123 support: enabled")
# ELSE(HAS_MPG123)
#     MESSAGE(STATUS "MPG123 support: disabled")
# ENDIF(HAS_MPG123)
# IF(HAS_GSTREAMER)
#     MESSAGE(STATUS "GStreamer support: enabled")
# ELSE(HAS_GSTREAMER)
#     MESSAGE(STATUS "GStreamer support: disabled")
# ENDIF(HAS_GSTREAMER)
# MESSAGE(STATUS "")
