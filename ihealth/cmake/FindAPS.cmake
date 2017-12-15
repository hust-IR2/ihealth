# Module for locating Intel's Threading Building Blocks (APS).
#
# Customizable variables:
#   APS_ROOT_DIR
#     Specifies APS's root directory.
#
# Read-only variables:
#   APS_FOUND
#     Indicates whether the library has been found.
#
#   APS_INCLUDE_DIRS
#      Specifies APS's include directory.
#
#   APS_LIBRARIES
#     Specifies APS libraries that should be passed to target_link_libararies.
#

INCLUDE (FindPackageHandleStandardArgs)

FIND_PATH (APS_ROOT_DIR
  NAMES include/APS168.h
  PATHS ENV PATH
  DOC "APS root directory")

FIND_PATH (APS_INCLUDE_DIR
  NAMES APS168.h
  HINTS "${APS_ROOT_DIR}/include"
  DOC "APS include directory")

FIND_LIBRARY (APS_LIBRARY_RELEASE
  NAMES APS168
  HINTS ${APS_ROOT_DIR}
  PATH_SUFFIXES lib
  DOC "APS release library")

FIND_LIBRARY (APS_LIBRARY_DEBUG
  NAMES APS168d
  HINTS ${APS_ROOT_DIR}
  PATH_SUFFIXES lib
  DOC "APS debug library")

IF (APS_LIBRARY_RELEASE AND APS_LIBRARY_DEBUG)
  SET (APS_LIBRARY optimized ${APS_LIBRARY_RELEASE} debug ${APS_LIBRARY_DEBUG}
    CACHE DOC "APS library")
ELSEIF (APS_LIBRARY_RELEASE)
  SET (APS_LIBRARY ${APS_LIBRARY_RELEASE} CACHE DOC "APS library")
ENDIF (APS_LIBRARY_RELEASE AND APS_LIBRARY_DEBUG)

SET (APS_LIBRARIES ${APS_LIBRARY})
SET (APS_INCLUDE_DIRS ${APS_INCLUDE_DIR})


MARK_AS_ADVANCED (APS_INCLUDE_DIR APS_LIBRARY APS_LIBRARY_RELEASE
  APS_LIBRARY_DEBUG)


FIND_PACKAGE_HANDLE_STANDARD_ARGS (APS REQUIRED_VARS APS_ROOT_DIR
  APS_INCLUDE_DIR APS_LIBRARY)
