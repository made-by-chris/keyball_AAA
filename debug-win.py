import os

# Define directories and files to check
directories = [
    r"C:\Program Files\Epic Games\UE_4.26",  # Unreal Engine installation
    r"C:\Users\Christopher\projects\keyball_AAA",  # Your project directory
    r"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.19041.0\um\x64",  # Windows SDK 10.0.19041.0
    r"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.22621.0\um\x64"   # Windows SDK 10.0.22621.0
]

vs_toolchain = r"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Tools\MSVC\14.29.30133\bin\HostX64\x64"  # VS 2019 toolchain
project_files = [
    r"C:\Users\Christopher\projects\keyball_AAA\keyball_AAA.sln",
    r"C:\Users\Christopher\projects\keyball_AAA\keyball.vcxproj"
]
ubt_log_dir = r"C:\Users\Christopher\projects\keyball_AAA\Saved\Logs"

# Function to check if a directory or file exists
def check_path(path, is_dir=True):
    return os.path.exists(path) if is_dir else os.path.isfile(path)

# Check directories
print("Checking directory existence at", os.path.abspath(__file__))
for directory in directories:
    exists = check_path(directory)
    status = "exists" if exists else "does not exist"
    print(f"Directory: {directory} - {status}")

# Check user32.lib in SDK paths
sdk_dirs = [d for d in directories if "Windows Kits" in d]
for sdk_dir in sdk_dirs:
    user32_path = os.path.join(sdk_dir, "user32.lib")
    user32_exists = check_path(user32_path, is_dir=False)
    status = "exists" if user32_exists else "does not exist"
    print(f"File: {user32_path} - {status}")

# Check Visual Studio 2019 toolchain
toolchain_exists = check_path(vs_toolchain)
status = "exists" if toolchain_exists else "does not exist"
print(f"Toolchain: {vs_toolchain} - {status}")

# Check project files
for file_path in project_files:
    file_exists = check_path(file_path, is_dir=False)
    status = "exists" if file_exists else "does not exist"
    print(f"File: {file_path} - {status}")

# Check for recent UBT logs
log_exists = check_path(ubt_log_dir)
if log_exists:
    logs = [f for f in os.listdir(ubt_log_dir) if f.startswith("UnrealVersionSelector") and f.endswith(".log")]
    if logs:
        latest_log = max(logs, key=lambda x: os.path.getmtime(os.path.join(ubt_log_dir, x)))
        print(f"Latest UBT log: {latest_log} - exists")
    else:
        print(f"Directory: {ubt_log_dir} - No UnrealVersionSelector logs found")
else:
    print(f"Directory: {ubt_log_dir} - does not exist")