# ===== Utils.cmake =====

# ===== Build =====
set(CMAKE_BUILD_TYPE Release CACHE STRING "Build Type")

# ===== Set Variables =====
set(INCLUDE_DIR "${ROOT_DIR}/inc" CACHE PATH "C++ Headers")
set(SOURCES_DIR "${ROOT_DIR}/src" CACHE PATH "C++ Sources")

# ===== Dependencies =====
find_package(PkgConfig REQUIRED)

# ----- EGL -----
find_package(OpenGL REQUIRED COMPONENTS EGL)

# ----- GLES2 -----
pkg_check_modules(GLES2 REQUIRED IMPORTED_TARGET glesv2)

# ----- libcamera -----
pkg_check_modules(LIBCAMERA REQUIRED IMPORTED_TARGET libcamera)

# ----- OpenCV (armv8) -----
find_package(OpenCV-armv8)

# ===== Sources =====
file(
  GLOB_RECURSE
  CXX_SOURCES
  CONFIGURE_DEPENDS
  "${INCLUDE_DIR}/**/*.hpp"
  "${SOURCES_DIR}/**/*.cpp"
)
set(CXX_SOURCES "${CXX_SOURCES}" CACHE STRING "Project Sources")
