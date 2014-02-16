@echo off
if defined 386 (set DRIVER_DIR_NAME=i386) else (set DRIVER_DIR_NAME=amd64)
mkdir ..\output\bin\%_BuildType%
mkdir ..\output\pdb\%_BuildType%
copy .\obj%BUILD_ALT_DIR%\%DRIVER_DIR_NAME%\%DriverName%.sys ..\output\bin\%_BuildType%\%DriverName%_%BUILD_ALT_DIR%.sys
copy .\obj%BUILD_ALT_DIR%\%DRIVER_DIR_NAME%\%DriverName%.pdb ..\output\pdb\%_BuildType%\%DriverName%_%BUILD_ALT_DIR%.pdb
copy .\obj%BUILD_ALT_DIR%\%DRIVER_DIR_NAME%\%DriverName%.pdb %SymbolDir%\private\%DriverName%.pdb
echo on
exit