cmake_minimum_required(VERSION 3.16)
project(UCGretina)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Use large file support
add_compile_definitions(_FILE_OFFSET_BITS=64 G4V11)

find_package(Geant4 REQUIRED)
include(${Geant4_USE_FILE})

find_package(Git QUIET)
if(GIT_FOUND)
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    OUTPUT_VARIABLE GIT_HASH OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    OUTPUT_VARIABLE GIT_BRANCH OUTPUT_STRIP_TRAILING_WHITESPACE)
else()
    set(GIT_HASH "unknown")
    set(GIT_BRANCH "unknown")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/Git_Hash.hh.in
               ${CMAKE_CURRENT_BINARY_DIR}/Git_Hash.hh @ONLY)
include_directories(${CMAKE_CURRENT_BINARY_DIR} include)

file(GLOB SRC_FILES src/*.cc)
add_library(gretina SHARED ${SRC_FILES})

# Link against Geant4 libraries
TARGET_LINK_LIBRARIES(gretina PUBLIC ${Geant4_LIBRARIES})

macro(add_gretina_exe exe source)
    add_executable(${exe} ${source})
    target_link_libraries(${exe} gretina)
endmacro()

add_gretina_exe(UCGretina UCGretina.cc)
add_gretina_exe(UCGretina_LH UCGretina_LH.cc)
target_compile_definitions(UCGretina_LH PRIVATE LHTARGET)
add_gretina_exe(UCGretina_Scan UCGretina_Scan.cc)
target_compile_definitions(UCGretina_Scan PRIVATE SCANNING)
add_gretina_exe(UCGretina_Pol UCGretina_Pol.cc)
target_compile_definitions(UCGretina_Pol PRIVATE POL)
add_gretina_exe(UCGretina_LH_Pol UCGretina_LH.cc)
target_compile_definitions(UCGretina_LH_Pol PRIVATE LHTARGET POL)
add_gretina_exe(UCGretina_Scan_Pol UCGretina_Scan.cc)
target_compile_definitions(UCGretina_Scan_Pol PRIVATE SCANNING POL)

install(TARGETS UCGretina UCGretina_LH UCGretina_Scan UCGretina_Pol
        UCGretina_LH_Pol UCGretina_Scan_Pol gretina
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)
