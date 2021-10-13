@echo off
xcopy  /Y ..\x64\Release\wnginst.exe .
if NOT EXIST ..\bin mkdir ..\bin
if NOT EXIST ..\lib mkdir ..\lib
wnginst -v ..\x64\Debug\*.exe ..\bin
wnginst -v ..\x64\Release\*.exe ..\bin
wnginst -v ..\x64\Debug\*.pdb ..\bin
wnginst -v ..\x64\Release\*.pdb ..\bin
wnginst -v ..\x64\Debug\*.lib ..\lib
wnginst -v ..\x64\Release\*.lib ..\lib
