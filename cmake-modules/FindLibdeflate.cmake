# libdeflate is required by a special build of libtiff required by Nyxus
#
# Dependencies: C++11 support and threading library
#
# The Libdeflate_CXX_FLAGS should be added to the CMAKE_CXX_FLAGS
#
# This module defines
#  Libdeflate_INCLUDE_DIR
#  Libdeflate_LIBRARIES
#  Libdeflate_CXX_FLAGS
#  Libdeflate_FOUND
#

# Ensure C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif (MSVC)

# Try to found Libdeflate
SET(Libdeflate_FOUND ON)

FIND_PATH(Libdeflate_INCLUDE_DIR libdeflate/libdeflate.h
        lib/libdeflate
        )


IF (NOT Libdeflate_INCLUDE_DIR)
    SET(Libdeflate_FOUND OFF)
    MESSAGE(STATUS "Could not find Libdeflate includes. Libdeflate_FOUND now off")
ENDIF ()

IF (Libdeflate_FOUND)
    IF (NOT Libdeflate_FIND_QUIETLY)
        MESSAGE(STATUS "Found Libdeflate include: ${Libdeflate_INCLUDE_DIR}, CXX_FLAGS: ${Libdeflate_CXX_FLAGS}, Libs: ${Libdeflate_LIBRARIES}")
    ENDIF (NOT Libdeflate_FIND_QUIETLY)
ELSE (Libdeflate_FOUND)
    IF (Libdeflate_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could not find Libdeflate header files, please set the cmake variable Libdeflate_INCLUDE_DIR")
    ENDIF (Libdeflate_FIND_REQUIRED)
ENDIF (Libdeflate_FOUND)

MARK_AS_ADVANCED(Libdeflate_INCLUDE_DIR)
