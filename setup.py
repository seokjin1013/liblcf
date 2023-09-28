#!/usr/bin/env python3

import os, sys, platform
import zipfile
import subprocess
import re
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
from distutils.command.bdist import bdist as _bdist
from distutils.command.sdist import sdist as _sdist
from distutils.command.install_lib import install_lib as _install_lib


if not os.path.exists('/home/seokj/workspace/liblcf3/liblcf'):
    os.makedirs('/home/seokj/workspace/liblcf3/liblcf', exist_ok=True)


# This is necessary to inform distutils that we are building
# a binary extension module, but we don't need to do anything.
class CMakeExtension(Extension):
    def __init__(self, name, **kwa):
        Extension.__init__(self, name, sources=[], **kwa)


class cmake_build_ext(build_ext):
    def run(self):
        self.build_temp = '/home/seokj/workspace/liblcf3/liblcf'
        self.build_lib = '/home/seokj/workspace/liblcf3/liblcf/pylcf2xml.wheel/pylcf2xml'
        if platform.system() == 'Linux':
            self.fix_linux_loader_paths()

    def fix_linux_loader_paths(self):
        for f in os.listdir('/home/seokj/workspace/liblcf3/liblcf/pylcf2xml.wheel/pylcf2xml'):
            print(f"cmake_build_ext: Fixing {f}")
            file_path = os.path.join('/home/seokj/workspace/liblcf3/liblcf/pylcf2xml.wheel/pylcf2xml', f)
            if f.endswith('.so'):
                self.spawn(['patchelf', '--set-rpath', '$ORIGIN', file_path])


class bdist(_bdist):
    def finalize_options(self):
        _bdist.finalize_options(self)
        self.dist_dir = '/home/seokj/workspace/liblcf3/liblcf'
        self.bdist_base = '/home/seokj/workspace/liblcf3/liblcf/pylcf2xml.wheel'


class sdist(_sdist):
    def finalize_options(self):
        _sdist.finalize_options(self)
        self.dist_dir = '/home/seokj/workspace/liblcf3/liblcf'


class install_lib(_install_lib):
    def finalize_options(self):
        _install_lib.finalize_options(self)
        self.build_dir = '/home/seokj/workspace/liblcf3/liblcf/pylcf2xml.wheel'


setup(
    name = "pylcf2xml",
    version = "0.0.1",
    description = "lcf2xml python build.",
    author = "seokjin1013",
    url = "https://github.com/EasyRPG/liblcf",

    ext_modules = [CMakeExtension("pylcf2xml")],
    keywords = "pylcf2xml",
    python_requires = ">=3.8",
    install_requires = [
        
    ],
    package_dir = {"": "pylcf2xml.wheel"},
    packages = ["pylcf2xml"],
    package_data = {
        "pylcf2xml": ["*"]
    },

    cmdclass = {
        'bdist': bdist,
        'sdist': sdist,
        'install_lib': install_lib,
        'build_ext': cmake_build_ext,
    },
)
