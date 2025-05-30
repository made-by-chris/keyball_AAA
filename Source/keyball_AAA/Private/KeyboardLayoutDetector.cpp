#include "KeyboardLayoutDetector.h"
#include "HAL/Platform.h"

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
    // [Previous implementation unchanged]
    FString LayoutName = TEXT("Unknown");

#if PLATFORM_WINDOWS
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
    TISInputSourceRef CurrentInputSource = TISCopyCurrentKeyboardInputSource();
    if (CurrentInputSource)
    {
        CFStringRef LayoutID = (CFStringRef)TISGetInputSourceProperty(CurrentInputSource, kTISPropertyInputSourceID);
        if (LayoutID)
        {
            LayoutName = FString(CFStringGetCStringPtr(LayoutID, kCFStringEncodingUTF8));
        }
        CFRelease(CurrentInputSource);
    }
#elif PLATFORM_LINUX
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

#if PLATFORM_WINDOWS
    HKL CurrentLayout = GetKeyboardLayout(GetCurrentThreadId());
    HKL QWERTYLayout = LoadKeyboardLayoutW(L"00000409", KLF_ACTIVATE); // US QWERTY
    if (CurrentLayout && QWERTYLayout)
    {
        UINT ScanCode = MapVirtualKeyExW(KeyCode, MAPVK_VK_TO_VSC, CurrentLayout);
        UINT QWERTYKey = MapVirtualKeyExW(ScanCode, MAPVK_VSC_TO_VK, QWERTYLayout);
        TCHAR Char = MapVirtualKeyExW(QWERTYKey, MAPVK_VK_TO_CHAR, QWERTYLayout);
        MappedKey = FKey(FName(*FString::Chr(Char)));
    }
#elif PLATFORM_MAC
    // macOS: Simplified mapping (requires UCCompareText for precise mapping)
    // Use a lookup table for common keys
    static TMap<int32, FString> MacKeyMap = {
        { 6, TEXT("Z") }, // German 'Z' key (QWERTY 'Y' position)
        { 7, TEXT("Y") }, // German 'Y' key (QWERTY 'Z' position)
    };
    if (FString* QWERTYChar = MacKeyMap.Find(KeyCode))
    {
        MappedKey = FKey(FName(**QWERTYChar));
    }
#elif PLATFORM_LINUX
    // Linux: Use XKB to map to QWERTY
    Display* display = XOpenDisplay(nullptr);
    if (display)
    {
        XkbDescPtr xkb = XkbGetKeyboard(display, XkbAllComponentsMask, XkbUseCoreKbd);
        if (xkb)
        {
            // Simplified: Map keycode to QWERTY symbol
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