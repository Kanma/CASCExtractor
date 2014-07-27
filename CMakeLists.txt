project(CASCEXTRACTOR)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CASCEXTRACTOR_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CASCEXTRACTOR_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CASCEXTRACTOR_BINARY_DIR}/bin")


if (NOT EXISTS "${CASCEXTRACTOR_SOURCE_DIR}/CascLib/CMakeLists.txt")
    message(FATAL_ERROR
"Missing dependency: CascLib
CASCExtractor requires the CascLib library.
It is provided as a GIT submodule of this repository.
Did you forgot to execute the following commands?
   git submodule init
   git submodule update")
endif()


add_subdirectory(CascLib)

include_directories("${CASCEXTRACTOR_SOURCE_DIR}/CascLib/src/"
                    "${CASCEXTRACTOR_SOURCE_DIR}/include/"
)

add_executable(CASCExtractor main.cpp)
target_link_libraries(CASCExtractor casc)

# Set the RPATH
if (APPLE)
    set_target_properties(CASCExtractor PROPERTIES LINK_FLAGS "-Wl,-rpath,@loader_path/.")
elseif (UNIX)
    set_target_properties(CASCExtractor PROPERTIES INSTALL_RPATH ".")
endif()
