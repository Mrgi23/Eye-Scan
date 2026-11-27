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

# ----- Qt6 Test -----
find_package(Qt6 REQUIRED COMPONENTS Test)

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
  nlohmann_json::nlohmann_json
  OpenCV-armv8::OpenCV-armv8
  OpenGL::EGL
  PkgConfig::GLES2
  PkgConfig::LIBCAMERA
  Qt6::Core
  Qt6::Gui
  Qt6::Network
  Qt6::OpenGLWidgets
  Qt6::Test
  Qt6::Widgets
  Qt6::Xml
)

# ===== Coverage =====
target_compile_options(tests PRIVATE --coverage -O0 -g -fno-elide-constructors)
target_link_options(tests PRIVATE --coverage)
