from conans.model.conan_file import ConanFile
from conans import CMake


class Cis1CoreNativeTests(ConanFile):
    settings = "os", "compiler", "arch", "build_type"
    generators = "cmake"
    requires = "gtest/1.8.1@bincrafters/stable"

    def build(self):
        self.cmake = CMake(self)
        self.cmake.configure()
        self.cmake.build()

    def test(self):
        target_test = "RUN_TESTS" if self.settings.os == "Windows" else "test"
        self.cmake.build(target=target_test)

    def imports(self):
        self.copy("libcis1_core.a", dst="lib", src="lib")
        self.copy("libcis1_core.lib", dst="lib", src="lib")
        self.copy("FindFilesystem.cmake", dst="cmake/modules", src="cmake/modules")
