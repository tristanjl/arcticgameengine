cmake_minimum_required(VERSION 3.14)

project(ATEngine LANGUAGES C CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SOIL CONFIGURE_DEPENDS "Private/Dependencies/SOIL/*.c" "Private/Dependencies/SOIL/*.h")

file(GLOB ATEnginge_private CONFIGURE_DEPENDS "Private/Include/*.h" "Private/Source/*.cpp")

IF (WIN32)
    file(GLOB ATEnginge_private_platform CONFIGURE_DEPENDS "Private/Include/Windows/*.h" "Private/Source/Windows/*.cpp" "Private/Include/Multiplatform/*.h" "Private/Source/Multiplatform/*.cpp")
ELSE()
    file(GLOB ATEnginge_private_platform CONFIGURE_DEPENDS "Private/Include/Linux/*.h" "Private/Source/Linux/*.cpp" "Private/Include/Multiplatform/*.h" "Private/Source/Multiplatform/*.cpp")
ENDIF()

file(GLOB ATEnginge_public CONFIGURE_DEPENDS "Public/Include/*.h")

add_library(ATEngine STATIC
    ${SOIL}
    ${ATEnginge_private}
    ${ATEnginge_private_platform}
    ${ATEnginge_public}
)

target_include_directories(ATEngine PRIVATE "Private/Dependencies")
target_include_directories(ATEngine PRIVATE "Private/Include")
target_include_directories(ATEngine PUBLIC "Public/Include")

target_compile_definitions(ATEngine PRIVATE ATENGINE_LIBRARY)

cmake_minimum_required(VERSION 3.12)
