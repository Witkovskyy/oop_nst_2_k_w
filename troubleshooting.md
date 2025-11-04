# If cmake fails to compile

Install vcpkg


In x64 Native Tools for Visual Studio
Try powershell```
rd /s /q build\win-vs 2>nul

cmake -S . -B build\win-vs ^
  -G "Visual Studio 17 2022" -A x64 ^
  -DCMAKE_TOOLCHAIN_FILE=C:\vcpkg\scripts\buildsystems\vcpkg.cmake ^
  -DVCPKG_TARGET_TRIPLET=x64-windows

cmake --build build\win-vs --config Debug
```
