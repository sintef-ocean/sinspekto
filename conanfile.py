from conans import ConanFile, CMake, tools
import os
import re


class SinspektoConan(ConanFile):
    name = "sinspekto"
    license = "GPLv3"
    url = "https://github.com/sintef-ocean/sinspekto"
    author = "SINTEF Ocean"
    homepage = "https://sintef-ocean.github.io/sinspekto"
    description = \
        "sinspekto is a library that provides a way to communicate with back-end"\
        " applications using Data Distributive Service (DDS)"
    topics = ("Qt", "QML", "DDS", "OpenSplice")
    exports = "version.txt, LICENSE"
    settings = "os", "compiler", "build_type", "arch"
    generators = ("virtualrunenv",
                  "virtualenv",
                  "cmake",
                  "cmake_paths",
                  "cmake_find_package",
                  "qt")
    build_subfolder = "build_subfolder"
    options = {
      "fPIC": [True, False],
      "with_examples": [True, False],
      "with_doc": [True, False]
    }
    default_options = (
      "fPIC=True",
      "with_examples=False",
      "with_doc=False"
    )
    requires = (
      "opensplice-ce/[>=6.9]@sintef/stable",
      "qt/5.15.2@bincrafters/stable"
    )
    build_requires = (
      "RatatoskIDL/0.2.0@sintef/stable",
      "boost/1.69.0")

    _cmake = None

    def requirements(self):

        if self.options.with_examples:
            self.options["qt"].qttools = True

    def build_requirements(self):

        if self.settings.os == "Android":
            self.output.warn("Android")
            self.build_requires("android-cmdline-tools/[>=6858069]@joakimono/testing")

    def configure(self):

        self.options["boost"].header_only = True
        self.options["qt"].shared = True
        self.options["qt"].with_mysql = False
        self.options["qt"].qtquickcontrols2 = True
        self.options["qt"].qtquick3d = True  # This is not needed
        self.options["qt"].qtcharts = True
        self.options["libpng"].shared = False  # Why?

        if self.settings.os != 'Windows':
            self.options["qt"].with_vulkan = True
        if self.settings.os == "Android":
            if self.settings.arch == 'x86_64':
                self.options["qt"].config = "-no-avx -no-avx2 -no-avx512"
            self.options["qt"].opengl = "es2"
            self.options["*"].with_glib = False
            self.options["android-cmdline-tools"].extra_packages = \
                "system-images;android-28;google_apis;x86_64,platforms;android-28,\
system-images;android-29;google_apis;x86_64,platforms;android-29,build-tools;28.0.3"

    def export_sources(self):

        self.copy("*", src="cmake", dst="cmake")
        self.copy("*", src="data", dst="data")
        self.copy("*", src="docs", dst="docs")
        self.copy("*", src="include", dst="include")
        self.copy("*", src="src", dst="src")
        self.copy("*", src="test_package", dst="test_package")
        self.copy("*", src="tools", dst="tools")
        self.copy("CMakeLists.txt")
        self.copy("version.txt")
        self.copy("LICENSE")
        self.copy("README.org")

    def set_version(self):
        self.version = tools.load(os.path.join(self.recipe_folder, "version.txt")).strip()

    def _configure_cmake(self):
        if self._cmake is None:
            self._cmake = CMake(self)
            if self.settings.os != "Windows":
                self._cmake.definitions["CMAKE_POSITION_INDEPENDENT_CODE"] = self.options.fPIC
            self._cmake.definitions["WITH_CONAN"] = True
            self._cmake.definitions["WITH_DOC"] = self.options.with_doc
            self._cmake.definitions["WITH_EXAMPLES"] = self.options.with_examples
            self._cmake.configure()
        return self._cmake

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()
        if self.options.with_doc:
            cmake.build(target="docs")
        cmake.install()

    def package(self):
        cmake = self._configure_cmake()
        if cmake is None:
            self.output.warn("cmake thing is NoneType in package")
        cmake.install()

        self.copy("LICENSE", dst="licenses", ignore_case=True, keep_path=False)

    def package_info(self):
        suffx = ""
        if self.settings.os == "Android":
            the_arch = os.environ.get('ANDROID_ABI')
            if the_arch is not None:
                suffx = "_" + the_arch
            else:
                the_arch = str(self.settings.arch)
            reg_x86 = re.compile("x86_64")
            reg_armv7 = re.compile("armv7*")
            reg_armv8 = re.compile("armv8*")

            if bool(reg_x86.match(the_arch)):
                suffx = "_x86_64"
            elif bool(reg_armv7.match(the_arch)):
                suffx = "_armeabi-v7a"
            elif bool(reg_armv8.match(the_arch)):
                suffx = "_arm64-v8a"
            # ANDROID_ABI: 'armeabi-v7a' or 'arm64-v8a', settings.arch: armv7. armv8.
            # Should really strip the suffix from the library. But how?

        self.cpp_info.libs = ["sinspekto" + suffx]
        if self.settings.build_type == "Debug":
            self.cpp_info.libs[0] += "_d"
        self.cpp_info.name = "sinspekto"

    def package_id(self):
        del self.info.options.with_CICD  # CICD shall not affect the package id

    def imports(self):
        # TODO: Should install this with the package
        self.copy("*", "licenses", "licenses", folder=True)
