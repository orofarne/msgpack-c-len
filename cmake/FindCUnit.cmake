include(FindCurses)

FIND_PATH(CUNIT_INCLUDE_DIR Basic.h
  /usr/local/include/CUnit
  /usr/include/CUnit
  /opt/local/include/CUnit
)

FIND_PATH(CUNIT_SHARE_DIR CUnit-List.dtd
  /usr/local/share/CUnit
  /usr/share/CUnit
  /opt/local/share/CUnit
)

# On unix system, debug and release have the same name
FIND_LIBRARY(CUNIT_LIBRARY cunit
             ${CUNIT_INCLUDE_DIR}/../../lib
             /usr/local/lib
             /usr/lib
             )
FIND_LIBRARY(CUNIT_DEBUG_LIBRARY cunit
             ${CUNIT_INCLUDE_DIR}/../../lib
             /usr/local/lib
             /usr/lib
             )

IF(CUNIT_INCLUDE_DIR)
  IF(CUNIT_LIBRARY)
    SET(CUNIT_FOUND "YES")
    if (WIN32)
      SET(CUNIT_LIBRARIES ${CUNIT_LIBRARY})
      SET(CUNIT_DEBUG_LIBRARIES ${CUNIT_DEBUG_LIBRARY})
    else (WIN32)
      SET(CUNIT_LIBRARIES ${CUNIT_LIBRARY} ${CURSES_LIBRARY})
      SET(CUNIT_DEBUG_LIBRARIES ${CUNIT_DEBUG_LIBRARY} ${CURSES_DEBUG_LIBRARY})
    endif (WIN32)
  ENDIF(CUNIT_LIBRARY)
  IF(CUNIT_INCLUDE_DIR)
    if (WIN32)
      SET(CUNIT_INCLUDE_DIRS ${CUNIT_INCLUDE_DIR})
    else (WIN32)
      MESSAGE(STATUS "Found CUNIT: ${CUNIT_INCLUDE_DIR}")
      SET(CUNIT_INCLUDE_DIRS ${CUNIT_INCLUDE_DIR} ${CURSES_INCLUDE_DIR})
    endif (WIN32)
  ENDIF(CUNIT_INCLUDE_DIR)
ENDIF(CUNIT_INCLUDE_DIR)
