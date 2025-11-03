@echo off
cmake -B build -S .
call "D:\VS\Common7\Tools\VsDevCmd.bat"
cmake --build build --config Release