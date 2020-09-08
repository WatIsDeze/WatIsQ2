# WatIsQ2
WhatIsQ2 is based on Paril's EGL 0.3.1. The aim is to improve the engine for modability.

Allowing a more easy workflow by using C++ instead of C, mimicking a more GoldSrc SDK approach. Adding support for RmlUI as menu and HUD interface renderer. 

Planned in the near future are creating a new game MOD adding:
- improve A.I so it has squad behavior and covers flanks. 
- player friendly A.I.
- recoil effect for weapons.
- reloading weapons.

Planned engine changes in the future are:
- replace mainmenu and console with RmlUI.
- support for .iqm model format. (maybe?)
- possible physics(maybe?)

## Features
- All features of EGL 0.3.1
- CMake support
- SDL(1.2) [windowing, user input]
- SDL and OpenAL audio
- C++
- Glew for OpenGL extensions. And a minimal requirement for OpenGL 2.0. No more qgl function pointer mess.
- Linux support. (FreeBSD code is there, but untested.)
