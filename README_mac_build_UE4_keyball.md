# Building `keyball_AAA` on macOS with Unreal Engine 4.26

## 🛠 Prerequisites

- macOS (preferably ≤ Monterey)
- Xcode 12.4 installed in `/Applications`
- Command Line Tools installed:
  ```bash
  xcode-select --install
  ```
- Mono installed (e.g., via Homebrew):
  ```bash
  brew install mono
  ```

---

## 📁 Setup
download xcode 12.4 which is the latest version that supports UE4.26
https://developer.apple.com/download/all/?q=xcode%2012.4 
(it will only be runnable with the below commands, no GUI)


Clone or sync your project, then run the following commands from Terminal:

---

## 1️⃣ Generate Xcode Project Files

```bash
chmod +x mac_run_keyball_build.py
./mac_run_keyball_build.py
```

This creates the `.xcworkspace` and configures the Unreal build system:

```bash
"/Users/Shared/Epic Games/UE_4.26/Engine/Build/BatchFiles/Mac/GenerateProjectFiles.sh" \
  -project="/Users/basiclaser/projects/keyball/keyball.uproject" -game -engine
```

> ⚠️ Make sure you have `Source/keyball_AAA.Target.cs` **and**
> `Source/keyball_AAAEditor/keyball_AAAEditor.Target.cs` before doing this.

---

## 2️⃣ Build the Project (no Xcode GUI needed)

```bash
"/Users/Shared/Epic Games/UE_4.26/Engine/Build/BatchFiles/Mac/Build.sh" \
  keyball_AAAEditor Mac Development "/Users/basiclaser/projects/keyball/keyball.uproject"
```

This compiles your project for use in the UE4 Editor.

---

## ▶️ Launch the Editor with Your Project

Once built, launch the editor from Terminal:

```bash
"/Users/Shared/Epic Games/UE_4.26/Engine/Binaries/Mac/UE4Editor.app/Contents/MacOS/UE4Editor" \
  "/Users/basiclaser/projects/keyball/keyball.uproject"
```

---

## ✅ Done!

You're now ready to edit, run, and playtest your project in the UE 4.26 editor — all without ever opening Xcode manually.
