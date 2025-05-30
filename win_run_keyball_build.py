#!/usr/bin/env python3
import os
import shutil
import subprocess

# === CONFIGURATION ===
PROJECT_NAME = "keyball"
TARGET_NAME = "keyball_AAA"
PROJECT_PATH = f"C:\\Users\\basiclaser\\projects\\{PROJECT_NAME}"  # Adjust to your actual path
UPROJECT_PATH = os.path.join(PROJECT_PATH, f"{PROJECT_NAME}.uproject")
ENGINE_PATH = "C:\\Program Files\\Epic Games\\UE_4.26"  # Adjust if Unreal Engine is installed elsewhere
BUILD_CONFIG = "Development"

# === UTILITY FUNCTIONS ===
def run(command):
    print(f"\n[RUNNING] {command}\n")
    subprocess.run(command, shell=True, check=True)

def clean_temp_folders():
    print("\n[CLEANING TEMP FOLDERS]")
    temp_dirs = [
        os.path.join(PROJECT_PATH, "Intermediate"),
        os.path.join(PROJECT_PATH, "Saved"),
        os.path.join(PROJECT_PATH, "DerivedDataCache"),
        os.path.join(PROJECT_PATH, "Binaries")
    ]
    for d in temp_dirs:
        if os.path.exists(d):
            print(f"Deleting: {d}")
            shutil.rmtree(d, ignore_errors=True)  # ignore_errors for Windows permission issues
        else:
            print(f"Not found: {d}")

# === MAIN STEPS ===

# Step 0: Clean up
clean_temp_folders()

# Step 1: Generate project files
generate_script = os.path.join(ENGINE_PATH, "Engine\\Build\\BatchFiles\\GenerateProjectFiles.bat")
run(f'"{generate_script}" -project="{UPROJECT_PATH}" -game -engine')

# Step 2: Build the project
build_script = os.path.join(ENGINE_PATH, "Engine\\Build\\BatchFiles\\Build.bat")
run(f'"{build_script}" {TARGET_NAME}Editor Win64 {BUILD_CONFIG} "{UPROJECT_PATH}"')

# Step 3: Launch UE4 Editor
editor_exe = os.path.join(ENGINE_PATH, "Engine\\Binaries\\Win64\\UE4Editor.exe")
run(f'"{editor_exe}" "{UPROJECT_PATH}"')

print("\n✅ All done. Editor should be launching now.\n")