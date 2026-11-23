# ===== Project Setup =====
cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===== Project =====
project(Tests)

# ===== Root Directory =====
get_filename_component(ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/.." ABSOLUTE)
set(CMAKE_CONFIG_DIR "${ROOT_DIR}/cmake" CACHE PATH "CMake Configurations")

# ===== Setup =====
include("${CMAKE_CONFIG_DIR}/Utils.cmake")

# ===== Additional Dependencies =====
# ----- Google Test & Google Mock -----
find_package(GTest REQUIRED)

# ===== Add Additional Sources =====
set(TESTS_DIR "${ROOT_DIR}/tests")
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS "${TESTS_DIR}/**/*.cpp" "${TESTS_DIR}/main.cpp")

# ===== Executable =====
add_executable(tests "${CXX_SOURCES}" "${TEST_SOURCES}")

target_include_directories(tests PRIVATE "${INCLUDE_DIR}")

target_link_libraries(
  tests PRIVATE
  GTest::gtest
  GTest::gtest_main
  GTest::gmock
  OpenCV-armv8::OpenCV-armv8
  OpenGL::EGL
  PkgConfig::GLES2
  PkgConfig::LIBCAMERA
)

# ===== Coverage =====
target_compile_options(tests PRIVATE --coverage -O0 -g -fno-elide-constructors)
target_link_options(tests PRIVATE --coverage)

set(REPORT_DIR "${ROOT_DIR}/coverage")
file(MAKE_DIRECTORY "${REPORT_DIR}")

add_custom_target(
  coverage
  COMMAND ./tests
  COMMAND lcov --capture --directory . -o raw.info --ignore-errors inconsistent,inconsistent --ignore-errors mismatch,mismatch --rc geninfo_unexecuted_blocks=1 > /dev/null
  COMMAND lcov -e raw.info "${INCLUDE_DIR}/*" "${SOURCES_DIR}/*" -o coverage.info --ignore-errors unused,unused > /dev/null
  COMMAND sed -i "/D0Ev/d" coverage.info
  COMMAND genhtml coverage.info -o "${REPORT_DIR}"
)
