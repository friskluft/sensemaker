# libTIFF is required by Nyxus
#
# Dependencies: C++11 support and threading library
#
# The Libtiff_CXX_FLAGS should be added to the CMAKE_CXX_FLAGS
#
# This module defines
#  Libtiff_INCLUDE_DIR
#  Libtiff_LIBRARIES
#  Libtiff_CXX_FLAGS
#  Libtiff_FOUND
#

# Ensure C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif (MSVC)

# Try to found Libtiff
SET(Libtiff_FOUND ON)

FIND_PATH(Libtiff_INCLUDE_DIR libtiff/tiffio.h
	/usr/local
        lib/libtiff
        )


IF (NOT Libtiff_INCLUDE_DIR)
    SET(Libtiff_FOUND OFF)
    MESSAGE(STATUS "Could not find Libtiff includes. Libtiff_FOUND now off")
ENDIF ()

IF (Libtiff_FOUND)
    IF (NOT Libtiff_FIND_QUIETLY)
        MESSAGE(STATUS "Found Libtiff include: ${Libtiff_INCLUDE_DIR}, CXX_FLAGS: ${Libtiff_CXX_FLAGS}, Libs: ${Libtiff_LIBRARIES}")
    ENDIF (NOT Libtiff_FIND_QUIETLY)
ELSE (Libtiff_FOUND)
    IF (Libtiff_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find Libtiff header files, please set the cmake variable Libtiff_INCLUDE_DIR")
    ENDIF (Libtiff_FIND_REQUIRED)
ENDIF (Libtiff_FOUND)

MARK_AS_ADVANCED(Libtiff_INCLUDE_DIR)
