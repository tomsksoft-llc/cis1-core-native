from conans import ConanFile
from conans import CMake


class Cis1CoreNative(ConanFile):
    name = "cis1-core-native"
    version = "0.0.0"
    settings = "os", "arch", "compiler", "build_type"
    generators = "cmake"
    exports = "*"
    requires = ("gtest/1.8.1@bincrafters/stable",
                "boost_process/1.69.0@bincrafters/stable",
                "boost_filesystem/1.69.0@bincrafters/stable",
                "boost_system/1.69.0@bincrafters/stable",
                "boost_asio/1.69.0@bincrafters/stable")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("*.h", dst="include", src="include")
        self.copy("startjob", dst="bin", src="bin")
