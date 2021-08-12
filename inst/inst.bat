@echo off
cminst -v ..\x64\Debug\*.exe ..\bin
cminst -v ..\x64\Release\*.exe ..\bin
cminst -v ..\x64\Debug\*.lib ..\lib
cminst -v ..\x64\Release\*.lib ..\lib
