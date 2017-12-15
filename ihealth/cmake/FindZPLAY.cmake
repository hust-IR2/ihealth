# Module for locating Intel's Threading Building Blocks (ZPLAY).
#
# Customizable variables:
#   ZPLAY_ROOT_DIR
#     Specifies ZPLAY's root directory.
#
# Read-only variables:
#   ZPLAY_FOUND
#     Indicates whether the library has been found.
#
#   ZPLAY_INCLUDE_DIRS
#      Specifies ZPLAY's include directory.
#
#   ZPLAY_LIBRARIES
#     Specifies ZPLAY libraries that should be passed to target_link_libararies.
#

INCLUDE (FindPackageHandleStandardArgs)

FIND_PATH (ZPLAY_ROOT_DIR
  NAMES libzplay.h
  PATHS ENV PATH
  DOC "ZPLAY root directory")

FIND_PATH (ZPLAY_INCLUDE_DIR
  NAMES libzplay.h
  HINTS "${ZPLAY_ROOT_DIR}/include"
  DOC "ZPLAY include directory")

FIND_LIBRARY (ZPLAY_LIBRARY_RELEASE
  NAMES libzplay
  HINTS ${ZPLAY_ROOT_DIR}
  PATH_SUFFIXES lib
  DOC "ZPLAY release library")

FIND_LIBRARY (ZPLAY_LIBRARY_DEBUG
  NAMES libzplayd
  HINTS ${ZPLAY_ROOT_DIR}
  PATH_SUFFIXES lib
  DOC "ZPLAY debug library")

IF (ZPLAY_LIBRARY_RELEASE AND ZPLAY_LIBRARY_DEBUG)
  SET (ZPLAY_LIBRARY optimized ${ZPLAY_LIBRARY_RELEASE} debug ${ZPLAY_LIBRARY_DEBUG}
    CACHE DOC "ZPLAY library")
ELSEIF (ZPLAY_LIBRARY_RELEASE)
  SET (ZPLAY_LIBRARY ${ZPLAY_LIBRARY_RELEASE} CACHE DOC "ZPLAY library")
ENDIF (ZPLAY_LIBRARY_RELEASE AND ZPLAY_LIBRARY_DEBUG)

SET (ZPLAY_LIBRARIES ${ZPLAY_LIBRARY})
SET (ZPLAY_INCLUDE_DIRS ${ZPLAY_INCLUDE_DIR})


MARK_AS_ADVANCED (ZPLAY_INCLUDE_DIR ZPLAY_LIBRARY ZPLAY_LIBRARY_RELEASE
  ZPLAY_LIBRARY_DEBUG)


FIND_PACKAGE_HANDLE_STANDARD_ARGS (ZPLAY REQUIRED_VARS ZPLAY_ROOT_DIR
  ZPLAY_INCLUDE_DIR ZPLAY_LIBRARY)
