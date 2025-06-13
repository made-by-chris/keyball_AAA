#!/usr/bin/env python3
import os
import shutil
import subprocess
import argparse

# === CONFIGURATION ===
PROJECT_NAME = "keyball"
TARGET_NAME = "keyball_AAA"
PROJECT_PATH = f"/Users/basiclaser/projects/{PROJECT_NAME}"
UPROJECT_PATH = os.path.join(PROJECT_PATH, f"{PROJECT_NAME}.uproject")
ENGINE_PATH = "/Users/Shared/Epic Games/UE_4.26"
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
            shutil.rmtree(d)
        else:
            print(f"Not found: {d}")

# === MAIN STEPS ===
def main():
    parser = argparse.ArgumentParser(description='Build and launch UE4 project')
    parser.add_argument('--rebuild', action='store_true', help='Perform a full rebuild including cleaning temp folders and launching the editor')
    args = parser.parse_args()

    # Step 0: Clean up (only if --rebuild is specified)
    if args.rebuild:
        clean_temp_folders()
        generate_script = os.path.join(ENGINE_PATH, "Engine/Build/BatchFiles/Mac/GenerateProjectFiles.sh")
        run(f'"{generate_script}" -project="{UPROJECT_PATH}" -game -engine')

    # Step 2: Build the project
    build_script = os.path.join(ENGINE_PATH, "Engine/Build/BatchFiles/Mac/Build.sh")
    run(f'"{build_script}" {TARGET_NAME}Editor Mac {BUILD_CONFIG} "{UPROJECT_PATH}"')

    # Step 3: Launch UE4 Editor (only if --rebuild is specified)
    if args.rebuild:
        editor_exe = os.path.join(ENGINE_PATH, "Engine/Binaries/Mac/UE4Editor.app/Contents/MacOS/UE4Editor")
        run(f'"{editor_exe}" "{UPROJECT_PATH}"')
        print("\n✅ All done. Editor should be launching now.\n")
    else:
        print("\n✅ Build completed successfully.\n")

if __name__ == "__main__":
    main()
