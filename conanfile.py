from conans import ConanFile
from conans import CMake

class Cis1CoreNative(ConanFile):
    name = "cis1-core-native"
    version = "0.0.1"
    description = "CIS1 core implementation."
    author = "MokinIA <mia@tomsksoft.com>"
    generators = "cmake"
    settings = "os", "arch", "compiler", "build_type"
    exports = []
    exports_sources = [
        "CMakeLists.txt",
        "include/*",
        "src/*",
        "Doxyfile.in",
        "version.txt",
        "component_version.txt",
        "cmake/*"]
    requires = ("gtest/1.8.1@bincrafters/stable",
                "boost_process/1.69.0@bincrafters/stable",
                "boost_filesystem/1.69.0@bincrafters/stable",
                "boost_system/1.69.0@bincrafters/stable",
                "boost_asio/1.69.0@bincrafters/stable",
                "boost_iostreams/1.69.0@bincrafters/stable",
                "cis1_cwu_transport/0.0.1@tomsksoft/cis1",
                "cis1_cwu_protocol/0.0.1@tomsksoft/cis1",
                "cis1_proto_utils/0.0.1@tomsksoft/cis1")

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        self.copy("*.h", dst="include", src="include")
        self.copy("startjob", dst="bin", src="bin")
        self.copy("setparam", dst="bin", src="bin")
        self.copy("getparam", dst="bin", src="bin")
        self.copy("setvalue", dst="bin", src="bin")
        self.copy("getvalue", dst="bin", src="bin")
        self.copy("libcis1_core.a", dst="lib", src="lib")
        self.copy("libcis1_core.lib", dst="lib", src="lib")
        self.copy("FindFilesystem.cmake", dst="cmake/modules", src="cmake/modules")

    def package_info(self):
        self.cpp_info.builddirs = ["", "cmake/modules"]
        self.cpp_info.libs = ["cis1_core"]
