@echo off
xcopy  /Y ..\x64\Release\wnginst.exe .
wnginst -v ..\x64\Debug\*.exe ..\bin
wnginst -v ..\x64\Release\*.exe ..\bin
wnginst -v ..\x64\Debug\*.lib ..\lib
wnginst -v ..\x64\Release\*.lib ..\lib
