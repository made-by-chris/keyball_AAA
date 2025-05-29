//#include "SDL.h"
//#include <stdio.h>
#include "Keyball_Function_Library.h"

//TMap<uint8, FKey> ScancodeToKey;
//ScancodeToKey.Add(16, FKey::Q);
////ScancodeToKey.Add(17, FKey::W);
//while (SDL_PollEvent(&event)) {
//    /* We are only worried about SDL_KEYDOWN and SDL_KEYUP events */
//    switch (event.type) {
//    case SDL_KEYDOWN:
//        printf("Key press detected\n");
//        break;
//
//    case SDL_KEYUP:
//        printf("Key release detected\n");
//        break;
//
//    default:
//        break;
//    }
//}

void UKeyball_Function_Library::GetKeyCode(FKey Key, int& KeyCode, int& CharacterCode)
{
    const uint32* KeyCodePtr;
    const uint32* CharCodePtr;
    FInputKeyManager::Get().GetCodesFromKey(Key, KeyCodePtr, CharCodePtr);
    
    KeyCode = KeyCodePtr ? *KeyCodePtr : 0;
    CharacterCode = CharCodePtr ? *CharCodePtr : 0;
}