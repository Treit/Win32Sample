@echo off
mkdir ..\..\build
pushd ..\..\build
cl.exe -Zi -DUNICODE -D_UNICODE ..\Win32Sample\code\win32_testapp.cpp user32.lib gdi32.lib
popd