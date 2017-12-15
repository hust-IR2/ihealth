# Module for locating Intel's Threading Building Blocks (MSC).
#
# Customizable variables:
#   MSC_ROOT_DIR
#     Specifies MSC's root directory.
#
# Read-only variables:
#   MSC_FOUND
#     Indicates whether the library has been found.
#
#   MSC_INCLUDE_DIRS
#      Specifies MSC's include directory.
#
#   MSC_LIBRARIES
#     Specifies MSC libraries that should be passed to target_link_libararies.
#

INCLUDE (FindPackageHandleStandardArgs)

FIND_PATH (MSC_ROOT_DIR
  NAMES include/msp_cmn.h
  PATHS ENV PATH
  DOC "MSC root directory")

FIND_PATH (MSC_INCLUDE_DIR
  NAMES msp_cmn.h
  HINTS "${MSC_ROOT_DIR}"
  DOC "MSC include directory")

FIND_LIBRARY (MSC_LIBRARY_RELEASE
  NAMES msc
  HINTS ${MSC_ROOT_DIR}
  DOC "MSC release library")

FIND_LIBRARY (MSC_LIBRARY_DEBUG
  NAMES mscd
  HINTS ${MSC_ROOT_DIR}
  DOC "MSC debug library")

IF (MSC_LIBRARY_RELEASE AND MSC_LIBRARY_DEBUG)
  SET (MSC_LIBRARY optimized ${MSC_LIBRARY_RELEASE} debug ${MSC_LIBRARY_DEBUG}
    CACHE DOC "MSC library")
ELSEIF (MSC_LIBRARY_RELEASE)
  SET (MSC_LIBRARY ${MSC_LIBRARY_RELEASE} CACHE DOC "MSC library")
ENDIF (MSC_LIBRARY_RELEASE AND MSC_LIBRARY_DEBUG)

SET (MSC_LIBRARIES ${MSC_LIBRARY})
SET (MSC_INCLUDE_DIRS ${MSC_INCLUDE_DIR})


MARK_AS_ADVANCED (MSC_INCLUDE_DIR MSC_LIBRARY MSC_LIBRARY_RELEASE
  MSC_LIBRARY_DEBUG)


FIND_PACKAGE_HANDLE_STANDARD_ARGS (MSC REQUIRED_VARS MSC_ROOT_DIR
  MSC_INCLUDE_DIR MSC_LIBRARY)
