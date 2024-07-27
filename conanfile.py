from conan import ConanFile, tools
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
import re


def get_version():
    try:
        content = tools.load("CMakeLists.txt")
        version = re.search(r"project\([^\)]+VERSION (\d+\.\d+\.\d+)[^\)]*\)", content).group(1)
        return version.strip()
    except Exception:
        return None


class sc_machineRecipe(ConanFile):
    name = "sc-machine"
    package_type = "library"
    version = get_version()
    author = "OSTIS AI"
    license = "https://github.com/ostis-ai/sc-machine/blob/master/LICENSE"
    url = "https://github.com/ostis-ai/sc-machine"
    description = "Software implementation of semantic network storage"
    exports = ["LICENSE.md"]
    settings = "os", "compiler", "build_type", "arch"
    requires = ()
    options = {   # remove for a header-only library
        "shared": [True, False],
        "fPIC": [True, False]
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        }
    scm = {
        "type": "git",
        "url": "auto",
        "revision": "auto",
        "submodule": "recursive"
    }

    @property
    def _run_tests(self):
        return tools.get_env("CONAN_RUN_TESTS", False)

    def build_requirements(self):
        self.build_requires("websocketpp/0.8.2")
        self.build_requires("nlohmann_json/3.11.3")
        self.build_requires("glib/2.78.3")
        self.build_requires("antlr4-cppruntime/4.9.3")

    def layout(self):
        cmake_layout(self, build_folder="build")

    def test_requirements(self):
        if self._run_tests:
            self.build_requires("gtest/1.14.0")

    def generate(self):
        deps = CMakeDeps(self)
        # deps.build_context_activated = ["my_tool"]
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ['new-project']

    # uncomment for a header-only library
    # def package_id(self):
    #     self.info.header_only()
