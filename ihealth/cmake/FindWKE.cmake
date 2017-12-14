# Module for locating Intel's Threading Building Blocks (WKE).
#
# Customizable variables:
#   WKE_ROOT_DIR
#     Specifies WKE's root directory.
#
# Read-only variables:
#   WKE_FOUND
#     Indicates whether the library has been found.
#
#   WKE_INCLUDE_DIRS
#      Specifies WKE's include directory.
#
#   WKE_LIBRARIES
#     Specifies WKE libraries that should be passed to target_link_libararies.
#

INCLUDE (FindPackageHandleStandardArgs)

FIND_PATH (WKE_ROOT_DIR
  NAMES include/wke.h
  PATHS ENV PATH
  DOC "WKE root directory")

FIND_PATH (WKE_INCLUDE_DIR
  NAMES wke.h
  HINTS "${WKE_ROOT_DIR}"
  DOC "WKE include directory")

FIND_LIBRARY (WKE_LIBRARY_RELEASE
  NAMES WKE
  HINTS ${WKE_ROOT_DIR}
  DOC "WKE release library")

FIND_LIBRARY (WKE_LIBRARY_DEBUG
  NAMES WKEd
  HINTS ${WKE_ROOT_DIR}
  DOC "WKE debug library")

IF (WKE_LIBRARY_RELEASE AND WKE_LIBRARY_DEBUG)
  SET (WKE_LIBRARY optimized ${WKE_LIBRARY_RELEASE} debug ${WKE_LIBRARY_DEBUG}
    CACHE DOC "WKE library")
ELSEIF (WKE_LIBRARY_RELEASE)
  SET (WKE_LIBRARY ${WKE_LIBRARY_RELEASE} CACHE DOC "WKE library")
ENDIF (WKE_LIBRARY_RELEASE AND WKE_LIBRARY_DEBUG)

SET (WKE_LIBRARIES ${WKE_LIBRARY})
SET (WKE_INCLUDE_DIRS ${WKE_INCLUDE_DIR})


MARK_AS_ADVANCED (WKE_INCLUDE_DIR WKE_LIBRARY WKE_LIBRARY_RELEASE
  WKE_LIBRARY_DEBUG)


FIND_PACKAGE_HANDLE_STANDARD_ARGS (WKE REQUIRED_VARS WKE_ROOT_DIR
  WKE_INCLUDE_DIR WKE_LIBRARY)
