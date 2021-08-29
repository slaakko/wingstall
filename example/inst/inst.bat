@echo off
wnginst -vd ..\x64\Release/*.exe ..\bin
wnginst -vd ..\x64\Debug\*.exe ..\bin
wnginst -vd ..\x64\Release\*.lib ..\lib
wnginst -vd ..\x64\Debug\*.lib ..\lib
