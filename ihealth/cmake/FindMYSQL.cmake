# Module for locating Intel's Threading Building Blocks (MYSQL).
#
# Customizable variables:
#   MYSQL_ROOT_DIR
#     Specifies MYSQL's root directory.
#
# Read-only variables:
#   MYSQL_FOUND
#     Indicates whether the library has been found.
#
#   MYSQL_INCLUDE_DIRS
#      Specifies MYSQL's include directory.
#
#   MYSQL_LIBRARIES
#     Specifies MYSQL libraries that should be passed to target_link_libararies.
#

INCLUDE (FindPackageHandleStandardArgs)

FIND_PATH (MYSQL_ROOT_DIR
  NAMES include/mysql.h
  PATHS ENV PATH
  DOC "MYSQL root directory")

FIND_PATH (MYSQL_INCLUDE_DIR
  NAMES mysql.h
  HINTS "${MYSQL_ROOT_DIR}/include"
  DOC "MYSQL include directory")

FIND_LIBRARY (MYSQL_LIBRARY_RELEASE
  NAMES libmysql
  HINTS ${MYSQL_ROOT_DIR}
  PATH_SUFFIXES lib
  DOC "MYSQL release library")

FIND_LIBRARY (MYSQL_LIBRARY_DEBUG
  NAMES libmysqld
  HINTS ${MYSQL_ROOT_DIR}
  PATH_SUFFIXES lib
  DOC "MYSQL debug library")

IF (MYSQL_LIBRARY_RELEASE AND MYSQL_LIBRARY_DEBUG)
  SET (MYSQL_LIBRARY optimized ${MYSQL_LIBRARY_RELEASE} debug ${MYSQL_LIBRARY_DEBUG}
    CACHE DOC "MYSQL library")
ELSEIF (MYSQL_LIBRARY_RELEASE)
  SET (MYSQL_LIBRARY ${MYSQL_LIBRARY_RELEASE} CACHE DOC "MYSQL library")
ENDIF (MYSQL_LIBRARY_RELEASE AND MYSQL_LIBRARY_DEBUG)

SET (MYSQL_LIBRARIES ${MYSQL_LIBRARY})
SET (MYSQL_INCLUDE_DIRS ${MYSQL_INCLUDE_DIR})


MARK_AS_ADVANCED (MYSQL_INCLUDE_DIR MYSQL_LIBRARY MYSQL_LIBRARY_RELEASE
  MYSQL_LIBRARY_DEBUG)


FIND_PACKAGE_HANDLE_STANDARD_ARGS (MYSQL REQUIRED_VARS MYSQL_ROOT_DIR
  MYSQL_INCLUDE_DIR MYSQL_LIBRARY)
