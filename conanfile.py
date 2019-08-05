from conans import ConanFile
from conans import CMake


class Cis1CoreNative(ConanFile):
    name = "cis1-core-native"
    settings = "os", "arch", "compiler", "build_type"
    generators = "cmake"
    exports = "*"
    requires = "gtest/1.8.1@bincrafters/stable"

    def build(self):
        cmake = CMake(self)
        cmake.build()
