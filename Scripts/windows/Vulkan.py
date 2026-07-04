
import os
import re
import subprocess

import Utils

FH_VULKAN_VERSION = '1.4.0.0'
VULKAN_SDK_INSTALLER_URL = 'https://sdk.lunarg.com/sdk/download/latest/windows/vulkan_sdk.exe'
VULKAN_SDK_EXE_PATH = 'Scripts/windows/VulkanSDK.exe'

def _parse_version(v):
    return tuple(int(x) for x in re.findall(r'\d+', v)[:4])

def GetInstalledVulkanVersion():
    vulkan_sdk = os.environ.get('VULKAN_SDK')
    if vulkan_sdk is None:
        return None, None

    try:
        result = subprocess.run(['vulkaninfo', '--summary'], capture_output=True, text=True, timeout=10)
        match = re.search(r'apiVersion\s*=\s*([\d.]+)', result.stdout)
        if match:
            return vulkan_sdk, match.group(1)
    except (FileNotFoundError, subprocess.TimeoutExpired):
        pass

    match = re.search(r'([\d]+\.[\d]+\.[\d]+\.[\d]+)', vulkan_sdk)
    if match:
        return vulkan_sdk, match.group(1)

    return vulkan_sdk, None

def InstallVulkanSDK():
    Utils.DownloadFile(VULKAN_SDK_INSTALLER_URL, VULKAN_SDK_EXE_PATH)
    print("Running Vulkan SDK installer...")
    print("Make sure to select 'Shader Toolchain Debug Symbols - 64-bit' during install.")
    os.startfile(os.path.abspath(VULKAN_SDK_EXE_PATH))
    print("Re-run Setup.bat after the installer finishes.")

def InstallVulkanPrompt():
    print("Would you like to download and install the Vulkan SDK now?")
    if Utils.YesOrNo():
        InstallVulkanSDK()
        return True
    return False

def CheckVulkanSDK():
    path, version = GetInstalledVulkanVersion()

    if path is None:
        print("You don't have the Vulkan SDK installed (VULKAN_SDK is not set).")
        return InstallVulkanPrompt()

    if version is None:
        print(f"Found Vulkan SDK at {path}, but couldn't determine its version. Proceeding.")
        return True

    if _parse_version(version) < _parse_version(FH_VULKAN_VERSION):
        print(f"Located Vulkan SDK {version} at {path}")
        print(f"Fahren requires at least {FH_VULKAN_VERSION}.")
        return InstallVulkanPrompt()

    print(f"Vulkan SDK {version} located at {path}")
    return True