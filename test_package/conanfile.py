from conans.model.conan_file import ConanFile
from conans import CMake


class Cis1CoreNativeTests(ConanFile):
    name = "cis1-core-native-tests"
    settings = "os", "compiler", "arch", "build_type"
    generators = "cmake"
    cmake = None
    requires = "gtest/1.8.1@bincrafters/stable"

    def build(self):
        self.cmake = CMake(self)
        self.cmake.configure()
        self.cmake.build()

    def test(self):
        target_test = "RUN_TESTS" if self.settings.os == "Windows" else "test"
        self.cmake.build(target=target_test)
