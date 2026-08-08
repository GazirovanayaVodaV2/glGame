cmake_minimum_required(VERSION 3.30)
project(glGame VERSION 1.0.0)
set(IS_LIBRARY OFF)

set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

#set minimum lang standard
#dont forget to change it in .vscode/c_cpp_properties.json 
#and in .vscode/settings.json
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)


set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD_REQUIRED ON)


if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/git")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/git")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(FetchContent)

FetchContent_Declare(glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_SHALLOW TRUE
)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw)

FetchContent_Declare(nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(nlohmann_json)

FetchContent_Declare(glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(stb)

FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        master
    GIT_SHALLOW TRUE
)

FetchContent_MakeAvailable(assimp)

add_library(stb_header INTERFACE)
target_include_directories(stb_header INTERFACE ${stb_SOURCE_DIR})

set(GLAD_SOURCES "dependencies/glad/src/glad.c")
add_library(glad STATIC ${GLAD_SOURCES})

set(DEPS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dependencies")
if(EXISTS "${DEPS_DIR}" AND IS_DIRECTORY "${DEPS_DIR}" AND EXISTS "${DEPS_DIR}/CMakeLists.txt")
    add_subdirectory(dependencies)
else()
    message(STATUS "Directory 'dependencies' skipped: CMakeLists.txt not found.")
endif()

file(GLOB_RECURSE  SOURCES CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE  HEADERS CONFIGURE_DEPENDS "src/*.h" "src/*.hpp")

if (IS_LIBRARY)
    add_library(${PROJECT_NAME} "main.cpp"  ${SOURCES} ${HEADERS})
else()
    add_executable(${PROJECT_NAME} "main.cpp" ${SOURCES} ${HEADERS})
endif()

target_include_directories(${PROJECT_NAME} PRIVATE 
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
)
target_include_directories(glad PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/dependencies/glad/include")

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${nlohmann_json_SOURCE_DIR}/include)

target_compile_definitions(${PROJECT_NAME} PRIVATE 
    APP_NAME="${PROJECT_NAME}"
)

find_package(OpenGL REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE 
nlohmann_json::nlohmann_json
glm::glm glfw glad 
stb_header
assimp
${OPENGL_LIBRARIES})

set(SOURCE_ASSETS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/content")
set(DEST_ASSETS_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

if(EXISTS "${SOURCE_ASSETS_DIR}" AND IS_DIRECTORY "${SOURCE_ASSETS_DIR}")
    add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${SOURCE_ASSETS_DIR}
        ${DEST_ASSETS_DIR}
    COMMENT "Copying assets to build directory..."
)
else()
    message(STATUS "Directory 'content' skipped!")
endif()



set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "OUTPUT_BUILD")