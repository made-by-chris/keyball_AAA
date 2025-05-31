#include "KeyboardLayoutDetector.h"
#include "HAL/Platform.h"
#include "InputCoreTypes.h" // For EKeys

#if PLATFORM_WINDOWS
#include "Windows/WindowsHWrapper.h"
#elif PLATFORM_MAC
#include <Carbon/Carbon.h>
#elif PLATFORM_LINUX
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#endif

FString UKeyboardLayoutDetector::GetCurrentKeyboardLayout()
{
    FString LayoutName = TEXT("Unknown");

#if PLATFORM_WINDOWS
    // Windows: Use GetKeyboardLayout to get the current layout
    HKL KeyboardLayout = GetKeyboardLayout(GetCurrentThreadId());
    if (KeyboardLayout)
    {
        WCHAR LayoutText[KL_NAMELENGTH] = { 0 };
        if (GetKeyboardLayoutNameW(LayoutText))
        {
            LayoutName = FString(LayoutText);
        }
    }
#elif PLATFORM_MAC
    // macOS: Use TISGetInputSourceProperty to get the current input source
    TISInputSourceRef CurrentInputSource = TISCopyCurrentKeyboardInputSource();
    if (CurrentInputSource == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("TISCopyCurrentKeyboardInputSource returned nullptr."));
        return LayoutName;
    }

    CFStringRef LayoutID = (CFStringRef)TISGetInputSourceProperty(CurrentInputSource, kTISPropertyInputSourceID);
    if (LayoutID == nullptr)
    {
        UE_LOG(LogTemp, Warning, TEXT("TISGetInputSourceProperty returned nullptr for kTISPropertyInputSourceID."));
        CFRelease(CurrentInputSource);
        return LayoutName;
    }

    // Use CFStringGetCString for safer conversion
    CFIndex Length = CFStringGetLength(LayoutID);
    if (Length == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Input source ID is empty."));
        CFRelease(CurrentInputSource);
        return LayoutName;
    }

    CFIndex MaxSize = CFStringGetMaximumSizeForEncoding(Length, kCFStringEncodingUTF8) + 1;
    char* Buffer = new char[MaxSize];
    if (!CFStringGetCString(LayoutID, Buffer, MaxSize, kCFStringEncodingUTF8))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to convert input source ID to UTF-8 string."));
        delete[] Buffer;
        CFRelease(CurrentInputSource);
        return LayoutName;
    }

    LayoutName = FString(UTF8_TO_TCHAR(Buffer));
    delete[] Buffer;
    CFRelease(CurrentInputSource);
#elif PLATFORM_LINUX
    // Linux: Use X11 to get the current keyboard layout
    Display* display = XOpenDisplay(nullptr);
    if (display)
    {
        XkbDescPtr xkb = XkbGetKeyboard(display, XkbAllComponentsMask, XkbUseCoreKbd);
        if (xkb && xkb->names)
        {
            Atom layoutAtom = xkb->names->groups[xkb->ctrls->num_groups - 1];
            char* layoutName = XGetAtomName(display, layoutAtom);
            if (layoutName)
            {
                LayoutName = FString(layoutName);
                XFree(layoutName);
            }
        }
        XkbFreeKeyboard(xkb, 0, True);
        XCloseDisplay(display);
    }
#endif

    return LayoutName;
}

FKey UKeyboardLayoutDetector::MapKeyToQWERTY(int32 KeyCode)
{
    FKey MappedKey = FKey();

    // Check for invalid or mouse-related keycodes
    if (KeyCode < 0 || KeyCode > 127) // Typical keyboard keycodes are 0-127
    {
        UE_LOG(LogTemp, Warning, TEXT("Nah bruh, that's a mouse or invalid keycode: %d"), KeyCode);
        return MappedKey; // Return empty FKey to skip processing
    }

#if PLATFORM_WINDOWS
    HKL CurrentLayout = GetKeyboardLayout(GetCurrentThreadId());
    HKL QWERTYLayout = LoadKeyboardLayoutW(L"00000409", KLF_ACTIVATE); // US QWERTY
    if (CurrentLayout && QWERTYLayout)
    {
        UINT ScanCode = MapVirtualKeyExW(KeyCode, MAPVK_VK_TO_VSC, CurrentLayout);
        UINT QWERTYKey = MapVirtualKeyExW(ScanCode, MAPVK_VSC_TO_VK, QWERTYLayout);
        TCHAR Char = MapVirtualKeyExW(QWERTYKey, MAPVK_VK_TO_CHAR, QWERTYLayout);
        if (Char != 0)
        {
            MappedKey = FKey(FName(*FString::Chr(Char)));
        }
    }
#elif PLATFORM_MAC
    // macOS: Use a lookup table for common keys, fallback to basic mapping
    static TMap<int32, FString> MacKeyMap = {
        { 6, TEXT("Z") },  // German 'Z' key (QWERTY 'Y' position)
        { 7, TEXT("Y") },  // German 'Y' key (QWERTY 'Z' position)
        { 0, TEXT("A") },
        { 12, TEXT("Q") },
        // Add more mappings as needed for your 10x4 grid
    };
    if (FString* QWERTYChar = MacKeyMap.Find(KeyCode))
    {
        MappedKey = FKey(FName(**QWERTYChar));
    }
    else
    {
        // Fallback: Assume key code maps directly (improve with UCKeyTranslate if needed)
        TCHAR Char = KeyCode; // Simplified, may need enhancement
        if (Char != 0)
        {
            MappedKey = FKey(FName(*FString::Chr(Char)));
        }
    }
#elif PLATFORM_LINUX
    // Linux: Use XKB to map to QWERTY
    Display* display = XOpenDisplay(nullptr);
    if (display)
    {
        XkbDescPtr xkb = XkbGetKeyboard(display, XkbAllComponentsMask, XkbUseCoreKbd);
        if (xkb)
        {
            KeySym keysym = XkbKeycodeToKeysym(display, KeyCode, 0, 0);
            if (keysym != NoSymbol)
            {
                char* keyName = XKeysymToString(keysym);
                if (keyName)
                {
                    MappedKey = FKey(FName(*FString(keyName)));
                    XFree(keyName);
                }
            }
        }
        XkbFreeKeyboard(xkb, 0, True);
        XCloseDisplay(display);
    }
#endif

    return MappedKey;
}