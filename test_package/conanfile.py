#!/usr/bin/env python
# -*- coding: utf-8 -*-

from conans import ConanFile, CMake, tools, RunEnvironment
import os


class SinpektoTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = ("cmake_paths", "cmake", "virtualenv")

    def build(self):
        if not tools.cross_building(self.settings):
            if 'DOCKER_CONTAINER' not in os.environ:
                env_build = RunEnvironment(self)
                with tools.environment_append(env_build.vars):
                    cmake = CMake(self)
                    cmake.configure()
                    cmake.build()
                    cmake.test()

    def imports(self):
        pass

    def test(self):
        if not tools.cross_building(self.settings):
            print("SUCCESS")
        else:
            print("NOT_RUN (cross-building)")
