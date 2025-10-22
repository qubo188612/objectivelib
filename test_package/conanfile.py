import os

from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.build import can_run
from conan.tools.files import copy


class alg_app_dmsTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    def package_id(self):
        #忽略编译器信息
        self.info.settings.rm_safe("compiler.version")

    def requirements(self):
        self.requires("alg_base_common/[^1.2.6]")
        self.requires("syopencv/4.6.0")
        self.requires(self.tested_reference_str)

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def layout(self):
        cmake_layout(self)

    def test(self):
        if can_run(self):
            cmd = os.path.join(self.cpp.build.bindir, "example")
            alg_app_ams = self.dependencies["alg_app_ams"]
            copy(self,"**/*.dll", alg_app_ams.package_folder, self.cpp.build.bindir,keep_path=False)
            self.run(cmd, env="conanrun")

