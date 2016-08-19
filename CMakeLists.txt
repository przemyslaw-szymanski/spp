cmake_minimum_required(VERSION 3.5)

set(SPP_LIB_NAME "S++")
set(SPP_LIB_NAME_REL "${SPP_LIB_NAME}")
set(SPP_LIB_NAME_DBG "${SPP_LIB_NAME}_d")
set(STANDALONE_NAME "S++Standalone")
set(DEMO_NAME "S++Test")
set(DEMO_NAME_REL "${DEMO_NAME}")
set(DEMO_NAME_DBG "${DEMO_NAME}_d")


MACRO(SUBDIRLIST result curdir)
  FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
  SET(dirlist "")
  FOREACH(child ${children})
    IF(IS_DIRECTORY ${curdir}/${child})
        LIST(APPEND dirlist ${child})
    ENDIF()
  ENDFOREACH()
  SET(${result} ${dirlist})
ENDMACRO()

MACRO(SET_INCLUDE_FILES)

    set(INC_FILES_PATTERN
        "${INC_DIR}/*.h"
        "${INC_DIR}/*.inl"
        "${INC_DIR}/*.hpp")

    set(SRC_FILES_PATTERN
        "${SRC_DIR}/*.c"
        "${SRC_DIR}/*.cpp"
        "${SRC_DIR}/*.h"
        "${SRC_DIR}/*.inl"
        "${SRC_DIR}/*.hpp")

    file(GLOB_RECURSE INCLUDE_FILES ${INC_FILES_PATTERN})
    file(GLOB_RECURSE SOURCE_FILES ${SRC_FILES_PATTERN})
    #file(GLOB_RECURSE ALL_FILES ${INC_FILES})

ENDMACRO()

MACRO(COMPILER_OPTIONS)
    if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
        add_definitions(-DSPP_WINDOWS)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
        add_definitions(-DSPP_LINUX)
    elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
        add_definitions(-DSPP_ANDROID)
    else()
        message(FATAL_ERROR "Unknown system")
    endif()

    if (ANDROID)
        add_definitions(-std=c++11)
        include_directories(android-windowing)
        add_subdirectory(android-windowing)
    else()
        if (${CMAKE_GENERATOR} MATCHES "Visual Studio")
            set(CPP_FLAGS "/MP /W4 /fp:fast /GR- /GF")
            set(CPP_DEBUG_FLAGS "/MTd /EHsc /arch:IA32 /Qpar-")
            set(CPP_RELEASE_FLAGS "/MT /arch:AVX2 /Oi /Qpar")
        else()
            set(CPP_FLAGS "-fms-extensions -std=c++11 -Wall")
            set(CPP_DEBUG_FLAGS "")
            set(CPP_RELEASE_FLAGS "")
            add_definitions(-std=c++11)
        endif() # ${CMAKE_GENERATOR} MATCHES "Visual Studio"
    endif()
    
    set(CMAKE_C_FLAGS_RELEASE	         "${CMAKE_CXX_FLAGS_RELEASE} ${CPP_FLAGS} ${CPP_RELEASE_FLAGS}")
    set(CMAKE_C_FLAGS_DEBUG		         "${CMAKE_CXX_FLAGS_DEBUG} ${CPP_FLAGS}  ${CPP_DEBUG_FLAGS}")
    set(CMAKE_CXX_FLAGS_RELEASE	         "${CMAKE_CXX_FLAGS_RELEASE} ${CPP_FLAGS} ${CPP_RELEASE_FLAGS}")
    set(CMAKE_CXX_FLAGS_DEBUG	         "${CMAKE_CXX_FLAGS_DEBUG} ${CPP_FLAGS}  ${CPP_DEBUG_FLAGS}")
ENDMACRO()

MACRO(CREATE_PROJECT_DIRS)
    foreach(f ${INCLUDE_FILES})
        # Get the path of the file relative to ${DIRECTORY},
        # then alter it (not compulsory)
        file(RELATIVE_PATH GRP ${INC_DIR} ${f})
        set(GRP "include/${GRP}")
        # Extract the folder, ie remove the filename part
        string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" GRP ${GRP})
        # Source_group expects \\ (double antislash), not / (slash)
        string(REPLACE / \\ GRP ${GRP})
        source_group("${GRP}" FILES ${f})
    endforeach()

    foreach(f ${SOURCE_FILES})
        # Get the path of the file relative to ${DIRECTORY},
        # then alter it (not compulsory)
        file(RELATIVE_PATH GRP ${SRC_DIR} ${f})
        set(GRP "src/${GRP}")
        # Extract the folder, ie remove the filename part
        string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" GRP ${GRP})
        # Source_group expects \\ (double antislash), not / (slash)
        string(REPLACE / \\ GRP ${GRP})
        source_group("${GRP}" FILES ${f})
    endforeach()
ENDMACRO()

project(${SPP_LIB_NAME} C CXX)

set(BIN_DIR "${CMAKE_CURRENT_SOURCE_DIR}/bin")
set(LIB_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src/Spp")
set(DEMO_DIR_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Test")
#SET(PROJECT_BINARY_DIR "${BIN_DIR}")
#SET(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}")

MESSAGE("Paths ${SRC_FILES}")

COMPILER_OPTIONS()
add_definitions(-DSPP_API_EXPORT)

SET_INCLUDE_FILES()
CREATE_PROJECT_DIRS()

set(PROJECT_FILES
    "${INCLUDE_FILES}"
    "${SOURCE_FILES}"
)
message("proj files ${SOURCE_FILES}")

include_directories(${INC_DIR})

add_library(${SPP_LIB_NAME} SHARED
    ${PROJECT_FILES}
)

SET(TARGET_NAME_REL "${SPP_LIB_NAME_REL}")
SET(TARGET_NAME_DBG "${SPP_LIB_NAME_DBG}")
set_target_properties(${SPP_LIB_NAME}
    PROPERTIES
    OUTPUT_NAME_DEBUG						${TARGET_NAME_DBG}
    OUTPUT_NAME_RELEASE						${TARGET_NAME_REL}
    DEBUG_OUTPUT_NAME						${TARGET_NAME_DBG}
    RELEASE_OUTPUT_NAME						${TARGET_NAME_REL}
    
    LIBRARY_OUTPUT_DIRECTORY_DEBUG			${LIB_DIR}
    IMPORT_LIBRARY_OUTPUT_DIRECTORY_DEBUG	${LIB_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG			${BIN_DIR}
    EXECUTABLE_OUTPUT_DIRECTORY_DEBUG		${BIN_DIR}
    LIBRARY_OUTPUT_DIRECTORY_RELEASE		${LIB_DIR}
    IMPORT_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${LIB_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE		${BIN_DIR}
    EXECUTABLE_OUTPUT_DIRECTORY_RELEASE		${BIN_DIR}
    BINARY_DIR								${BIN_DIR}
    LIBRARY_OUTPUT_DIRECTORY				${LIB_DIR}
    ARCHIVE_OUTPUT_DIRECTORY				${LIB_DIR}
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG			${LIB_DIR}
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE		${LIB_DIR}
)

SET(LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})
SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_DIR})
#SET(ARCHIVE_OUTPUT_DIRECTORY ${LIB_DIR})
MESSAGE("lib out dirs ${LIB_DIR}")

set_target_properties(${SPP_LIB_NAME} PROPERTIES LINKER_LANGUAGE CXX)

#set(LINK_LIB optimized ${SPP_LIB_NAME_REL} debug ${SPP_LIB_NAME_DBG})
#target_link_libraries(${SPP_LIB_NAME} ${LINK_LIB})

######## TEST DEMO ###########
project(${DEMO_NAME} C CXX)
set(${PROJECT_NAME} "${SPP_LIB_NAME}")
COMPILER_OPTIONS()
#SET_INCLUDE_FILES()
set(DEMO_FILES
    ${INCLUDE_FILES}
    ${SOURCE_FILES}
)

INCLUDE_DIRECTORIES(${LIB_DIR})
LINK_DIRECTORIES(${LIB_DIR})

include_directories(${INC_DIR} ${DEMO_DIR_PATH})
add_executable(${DEMO_NAME} 
    "${DEMO_DIR_PATH}/main.cpp"
)

add_dependencies(${DEMO_NAME} ${SPP_LIB_NAME})

 
SET(TARGET_NAME_REL "${DEMO_NAME_REL}")
SET(TARGET_NAME_DBG "${DEMO_NAME_DBG}")
set_target_properties(${DEMO_NAME}
    PROPERTIES
    OUTPUT_NAME_DEBUG						${TARGET_NAME_DBG}
    OUTPUT_NAME_RELEASE						${TARGET_NAME_REL}
    DEBUG_OUTPUT_NAME						${TARGET_NAME_DBG}
    RELEASE_OUTPUT_NAME						${TARGET_NAME_REL}
    
    LIBRARY_OUTPUT_DIRECTORY_DEBUG			${LIB_DIR}
    IMPORT_LIBRARY_OUTPUT_DIRECTORY_DEBUG	${LIB_DIR}
    RUNTIME_OUTPUT_DIRECTORY_DEBUG			${BIN_DIR}
    EXECUTABLE_OUTPUT_DIRECTORY_DEBUG		${BIN_DIR}
    LIBRARY_OUTPUT_DIRECTORY_RELEASE		${LIB_DIR}
    IMPORT_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${LIB_DIR}
    RUNTIME_OUTPUT_DIRECTORY_RELEASE		${BIN_DIR}
    EXECUTABLE_OUTPUT_DIRECTORY_RELEASE		${BIN_DIR}
    BINARY_DIR								${BIN_DIR}
    LIBRARY_OUTPUT_DIRECTORY				${LIB_DIR}
    ARCHIVE_OUTPUT_DIRECTORY				${LIB_DIR}
)


set_target_properties(${DEMO_NAME} PROPERTIES LINKER_LANGUAGE CXX)
    
set(LINK_LIB optimized ${SPP_LIB_NAME_REL} debug ${SPP_LIB_NAME_DBG})
target_link_libraries(${DEMO_NAME} ${LINK_LIB})
