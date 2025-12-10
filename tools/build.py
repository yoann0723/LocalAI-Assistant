#!/usr/bin/env python3
import argparse
import subprocess
import sys
import os
import shutil
import platform
import glob

# ---------------------------
# Argument parser
# ---------------------------
parser = argparse.ArgumentParser(description="Professional C++/Qt Project Builder")
parser.add_argument('--build_dir', default='build', help="Build directory")
parser.add_argument('--enable_ui', default='true', help="Enable UI components (true/false)")
parser.add_argument('--qt_dir', default='', help="Path to Qt installation. You can also set the QT_DIR environment variable.")
parser.add_argument('--config', default='Release', choices=['Debug', 'Release', 'RelWithDebInfo', 'MinSizeRel'])
parser.add_argument('--target', default=None, help="Specify CMake target")
parser.add_argument('--preset', default=None, help="CMake preset to use")
parser.add_argument('--clean', action='store_true', help="Clean build directory before building")
parser.add_argument('--run', action='store_true', help="Run the main frontend executable after build")
parser.add_argument('--capture_sdl2', default='true', help="Enable SDL2 support (true/false)")
args = parser.parse_args()

BUILD_DIR = args.build_dir
CONFIG = args.config
TARGET = args.target
PRESET = args.preset
CLEAN = args.clean
RUN = args.run
SOURCE_DIR = r"./src"
QTDIR = args.qt_dir or os.getenv("QT6_DIR", "")
ENABLE_UI = args.enable_ui.lower() == 'true'
CAPTURE_SDL2 = args.capture_sdl2.lower() == 'true'

# ---------------------------
# Paths
# ---------------------------
BIN_DIR = os.path.join(BUILD_DIR, "bin")

# ---------------------------
# Helper functions
# ---------------------------
def run(cmd, cwd=None):
    print("> " + " ".join(cmd))
    result = subprocess.run(cmd, cwd=cwd)
    if result.returncode != 0:
        sys.exit(result.returncode)

def clean_build():
    if os.path.exists(BUILD_DIR):
        print(f"Cleaning {BUILD_DIR} ...")
        shutil.rmtree(BUILD_DIR)

def cmake_configure(source_dir="."):
    cmd = ["cmake", "-S", source_dir, "-B", BUILD_DIR, 
           "-DSOURCE_DIR=" + os.path.abspath(source_dir),
           "-DBIN_DIR=" + os.path.abspath(BIN_DIR),
           "-DDEV_OUTPUT_DIR=" + os.path.join(BIN_DIR, CONFIG),
           "-DENABLE_UI=" + ("ON" if ENABLE_UI else "OFF"),
           "-DCAPTURE_SDL2=" + ("ON" if CAPTURE_SDL2 else "OFF")
           ]
    if PRESET:
        cmd += ["--preset", PRESET]
    else:
        cmd += ["-DCMAKE_BUILD_TYPE=" + CONFIG]
        
    if ENABLE_UI and QTDIR:
        cmd += ["-DQTDIR=" + os.path.abspath(QTDIR)]
        
    if platform.system() == "Windows":
        cmd += ["-DOS_WINDOWS=" + "1"]
        
    run(cmd)

def cmake_build():
    cmd = ["cmake", "--build", BUILD_DIR, "--config", CONFIG]
    if TARGET:
        cmd += ["--target", TARGET]
    run(cmd)

def find_modules(root_dir="."):
    # Scan all subdirectories with CMakeLists.txt
    print(f"Scanning for modules in {root_dir} ...")
    modules = []
    for d in os.listdir(root_dir):
        module_path = os.path.join(root_dir, d)
        if os.path.isdir(module_path) and os.path.exists(os.path.join(module_path, "CMakeLists.txt")):
            modules.append(d)
    return modules

def create_module_bin_dirs(modules):
    module_bins = []
    for m in modules:
        path = os.path.join(BIN_DIR, CONFIG, m)
        os.makedirs(path, exist_ok=True)
        module_bins.append(path)
    return module_bins

# ---------------------------
# Main workflow
# ---------------------------
if CLEAN:
    clean_build()

# Configure once at top-level
cmake_configure(SOURCE_DIR)

# Build all modules
cmake_build()

print(f"Build finished successfully. Module outputs in {os.path.join(BIN_DIR, CONFIG)}")
