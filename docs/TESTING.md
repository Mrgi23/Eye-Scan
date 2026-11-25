# Eye Scan - Testing & Validation Guide

## Overview
Testing strategies ensure the accuracy, reliability, and performance of the **Eye Scan**. Testing is divided into unit tests and coverage analysis, ensuring every component functions correctly in isolation and as part of the overall system. Real hardware components are mocked
since they cannot be tested without actual hardware.

## Testing Frameworks
- **Google Test (`gtest`), Google Mock (`gmock`)** – Testing framework for `C++`
- **GCOV (`gcov`)** – Code coverage analysis

### Compatibility Notice
For compatibility details, see the [Testing Framework Compatibility](COMPATIBILITY.md).

## Installing Dependencies
Ensure that all required dependencies are installed.

### Application Dependencies
- `EGL` & `GLES2` - Libraries for OpenGL ES rendering and context management
- `libcamera` - Library for camera control and image capture
- `OpenCV` - Library for computer vision and image processing

### Build Dependencies
- `conan` - System for compiling dependencies
- `cmake` - Build system for compiling tests
- `gcov` - Required for `C++` code coverage (**only works with `gcc` \ `g++`**)
- `ninja-build` - Build system for faster compilation

**Code coverage is only supported with GCC\G++**
- If using **Clang or MSVC**, code coverage will not be available.
- Windows users must run tests inside **WSL with GCC\G++**.

### Installation
```sh
sudo apt update
sudo apt install -y \
  build-essential cmake clang-format curl gcc g++ git gpg \
  lcov libavif-dev libcamera-dev libdrm-dev libegl1-mesa-dev libgles2-mesa-dev \
  libjpeg-dev libopenblas-dev libopenjp2-7-dev libopenexr-dev libpng-dev libwebp-dev \
  ninja-build pipx pkg-config qt6-base-dev qt6-tools-dev qt6-wayland

sudo pipx ensurepath && sudo pipx install conan
conan profile detect --force
conan remote add conancenter https://center2.conan.io --force
conan remote add artifactory https://conan.mrgi23.com/artifactory/api/conan/Conan-Index --force
```
Setup dependencies using the provided installation script:
```sh
./scripts/install-packages.sh
```

## Code Formatting
All `C++` source files must be formatted according to the project's `.clang-format` configuration before being committed using provided format script:
```sh
./scripts/clang-format.sh # <dir/file> to format specific directory or file
```

To check if all source files are properly formatted, use the `--check` option:
```sh
./scripts/clang-format.sh --check # <dir/file> to check specific directory or file
```

##  Running Tests
```sh
cd tests

conan install . --build=missing
cmake -G Ninja --preset conan-release
cmake --build --preset conan-release

cd build/Release
./tests
```
Run tests using the provided build scripts:
```sh
./scripts/build.sh --dir ./tests # --clean for clean build, --rebuild for building from scratch
./scripts/run-tests.sh # --output-dir <dir> for saving test results, --format <xml/json> for specifying output format
```

## **Code Coverage**
Code coverage ensures tests sufficiently exercise the codebase, identifying tested portions.

### Generating Coverage Reports
C++ code coverage is generated using **GCC's (`gcov`)**, which only works with **GCC\G++**.
**MSVC and MinGW are not supported for code coverage.**
- Linux: **Native support with GCC\G++**
- Windows: **Must use WSL with GCC\G++**
```sh
cd tests

conan install . --build=missing
cmake -G Ninja --preset conan-release
cmake --build --preset conan-release

cd build/Release
ninja coverage
```
Perform coverage analysis using provided build script:
```sh
./scripts/build.sh --dir ./tests # --clean for clean build, --rebuild for building from scratch
./scripts/run-tests.sh --coverage # --output-dir <dir> for saving coverage reports
```

### Coverage Report Locations
Default coverage report location is `./coverage/` directory. To view coverage report:
```sh
firefox ./coverage/index.html
```

## CI/CD Integration
The testing framework is integrated with **GitHub Actions**, ensuring automated testing and coverage reporting on every merge request.

### CI/CD Pipeline Overview
The pipeline is structured into the following stages:
| **Stage**    | **Purpose** |
|--------------|-------------|
| **Setup**    | Builds all dependencies pre-installed (`cmake`, `g++`, `gcc`, `googletest`, `googlemock`, `lcov`, etc.) |
| **Build**    | Compiles C++ tests |
| **Test**     | Runs C++ (Google Test) tests, and generates coverage reports using `gcov` |
| **Deploy**   | Creates new tag based on the release version and publishes reports via **GitHub Pages** (only `main`) |

### When Does CI/CD Run?
The CI/CD pipeline is triggered in the following cases:
- On every merge request to `develop` branch
- On every merge commit to `main` branch
- On manual pipeline execution from **GitHub UI**

**The pipeline is blocked if tests fail**, ensuring only validated code is merged.

### GitHub CI/CD Coverage Badge
[![Coverage](https://codecov.io/gh/Mrgi23/Eye-Scan/branch/main/graph/badge.svg)](https://app.codecov.io/gh/Mrgi23/Eye-Scan)

### GitHub Pages (Published Reports)
Deployed via GitHub Pages, coverage reports are accessible at:
- [C++ Code Coverage Report](https://mrgi23.github.io/Eye-Scan/)

## Next Steps
For general project information, see the [README](../README.md).

For information on data repository infrastructure, see the [Database Infrastructure](DATABASE.md).
