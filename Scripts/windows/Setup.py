import os
import shutil
import subprocess
import sys

import Vulkan
import Utils

CMAKE_INSTALLER_URL = 'https://github.com/Kitware/CMake/releases/download/v3.30.5/cmake-3.30.5-windows-x86_64.msi'
CMAKE_MSI_PATH = 'Scripts/windows/CMakeInstaller.msi'

def CheckCMake():
    if shutil.which('cmake') is not None:
        result = subprocess.run(['cmake', '--version'], capture_output=True, text=True)
        print(result.stdout.splitlines()[0])
        return True

    print("CMake was not found on your PATH.")
    print("Would you like to download and install it now?")
    if not Utils.YesOrNo():
        return False

    Utils.DownloadFile(CMAKE_INSTALLER_URL, CMAKE_MSI_PATH)
    print("Running CMake installer (this will prompt for admin rights)...")
    subprocess.call(['msiexec', '/i', os.path.abspath(CMAKE_MSI_PATH), '/passive', 'ADD_CMAKE_TO_PATH=System'])
    print("CMake installed. Re-run Setup.bat to continue (PATH needs a fresh terminal to take effect).")
    return False

def main():
    os.chdir(os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))

    print("=== Checking Vulkan SDK ===")
    if not Vulkan.CheckVulkanSDK():
        print("\nSetup cannot continue without the Vulkan SDK.")
        sys.exit(1)

    print("\n=== Checking CMake ===")
    if not CheckCMake():
        sys.exit(1)

    print("\n=== Configuring project ===")
    build_dir = "build"
    os.makedirs(build_dir, exist_ok=True)

    result = subprocess.call([
        "cmake",
        "-S", ".",
        "-B", build_dir,
        "-DCMAKE_BUILD_TYPE=Debug"
    ])

    if result != 0:
        print("\nCMake configuration failed. Check the errors above.")
        sys.exit(1)

    print(f"\nDone! Open the .sln in {build_dir}/ with Visual Studio, or run: cmake --build {build_dir}")

if __name__ == "__main__":
    main()