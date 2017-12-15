# Module for locating Intel's Threading Building Blocks (NI).
#
# Customizable variables:
#   NI_ROOT_DIR
#     Specifies NI's root directory.
#
# Read-only variables:
#   NI_FOUND
#     Indicates whether the library has been found.
#
#   NI_INCLUDE_DIRS
#      Specifies NI's include directory.
#
#   NI_LIBRARIES
#     Specifies NI libraries that should be passed to target_link_libararies.
#

INCLUDE (FindPackageHandleStandardArgs)

FIND_PATH (NI_ROOT_DIR
  NAMES include/NIDAQmx.h
  PATHS ENV PATH
  DOC "NI root directory")

FIND_PATH (NI_INCLUDE_DIR
  NAMES NIDAQmx.h
  HINTS "${NI_ROOT_DIR}/include"
  DOC "NI include directory")

FIND_LIBRARY (NI_LIBRARY_RELEASE
  NAMES NIDAQmx
  HINTS ${NI_ROOT_DIR}
  PATH_SUFFIXES lib
  DOC "NI release library")

FIND_LIBRARY (NI_LIBRARY_DEBUG
  NAMES NIDAQmxd
  HINTS ${NI_ROOT_DIR}
  PATH_SUFFIXES lib
  DOC "NI debug library")

IF (NI_LIBRARY_RELEASE AND NI_LIBRARY_DEBUG)
  SET (NI_LIBRARY optimized ${NI_LIBRARY_RELEASE} debug ${NI_LIBRARY_DEBUG}
    CACHE DOC "NI library")
ELSEIF (NI_LIBRARY_RELEASE)
  SET (NI_LIBRARY ${NI_LIBRARY_RELEASE} CACHE DOC "NI library")
ENDIF (NI_LIBRARY_RELEASE AND NI_LIBRARY_DEBUG)

SET (NI_LIBRARIES ${NI_LIBRARY})
SET (NI_INCLUDE_DIRS ${NI_INCLUDE_DIR})


MARK_AS_ADVANCED (NI_INCLUDE_DIR NI_LIBRARY NI_LIBRARY_RELEASE
  NI_LIBRARY_DEBUG)


FIND_PACKAGE_HANDLE_STANDARD_ARGS (NI REQUIRED_VARS NI_ROOT_DIR
  NI_INCLUDE_DIR NI_LIBRARY)
