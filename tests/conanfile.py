from conan import ConanFile
from conan.tools.cmake import cmake_layout

class EyeScanTestsConan(ConanFile):
    name = "eyescan-tests"
    version = "1.0"

    requires = (
      "gtest/1.18.0",
      "nlohmann_json/3.12.0",
    	"opencv-armv8/4.14.0@mrgi/release"
    )

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def configure(self):
      self.settings.compiler.cppstd = "20"

    def layout(self):
      cmake_layout(self)
