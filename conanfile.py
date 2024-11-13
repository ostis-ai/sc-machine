from conan import ConanFile, tools
from conan.tools.cmake import cmake_layout, CMakeDeps, CMakeToolchain, CMake
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
    exports_sources = "*", "!.venv", "!build", "!.cache", "!kb", "!kb.bin", "!.env", "!ConanPresets.json", "!docs", "!.git"
    settings = "os", "compiler", "build_type", "arch"
    requires = ()
    options = {
        "shared": [True],
        "fPIC": [True, False],
    }
    default_options = {
        "shared": True,
        "fPIC": True,
    }

    @property
    def _run_tests(self):
        return tools.get_env("CONAN_RUN_TESTS", False)
    
    def requirements(self):
        self.requires("websocketpp/0.8.2", options={"asio": "standalone"})
        self.requires("nlohmann_json/3.11.3")
        self.requires("glib/2.76.3")
        # TODO(FallenChromium): use this instead of thirdparty/antlr4 
        # self.requires("antlr4-cppruntime/4.9.3")

    def build_requirements(self):
        self.build_requires("libxml2/2.13.4")
        self.test_requires("gtest/1.14.0")
        self.test_requires("benchmark/1.9.0")

    def layout(self):
        cmake_layout(self)

    def build(self):
        cmake = CMake(self)
        cmake.configure()  # equivalent to self.run("cmake . <other args>")
        cmake.build()

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.user_presets_path = "ConanPresets.json"
        tc.generate()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_id(self):
        del self.info.settings.os
        del self.info.settings.compiler
        del self.info.settings.build_type

    def parse_version(self):
        content = tools.files.load(self, self.recipe_folder + "/CMakeLists.txt")
        version = re.search(r"project\([^\)]+VERSION (\d+\.\d+\.\d+)[^\)]*\)", content).group(1)
        return version.strip()

    def package_info(self): 
        self.cpp_info.set_property("cmake_find_mode", "none") # Do NOT generate any files
        self.cpp_info.builddirs.append("lib/cmake/sc-machine")
        self.cpp_info.builddirs.append("build/" + str(self.settings.build_type)) # Provides correct install path for conan editable
