import os
import subprocess

output_lines = []

# Get list of files tracked by git
result = subprocess.run(["git", "ls-files"], stdout=subprocess.PIPE, text=True)
git_files = result.stdout.splitlines()

for path in git_files:
    if os.path.isfile(path):
        try:
            size_bytes = os.path.getsize(path)
            size_mb = size_bytes / (1024 * 1024)
            output_lines.append((size_mb, path))
        except Exception as e:
            print(f"Error reading {path}: {e}")

# Sort by size descending
output_lines.sort(reverse=True, key=lambda x: x[0])

# Write to all-files.txt
with open("all-files.txt", "w", encoding="utf-8") as f:
    for size_mb, path in output_lines:
        f.write(f"{size_mb:8.2f} MB  |  {path}\n")

print("✅ Git-tracked file sizes written to all-files.txt")
