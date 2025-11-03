@echo off

echo 如果出现输入用户名和密码的提示，输入用户名ADMIN 、密码 LSpower 。
echo 输入完成后关闭Windows资源管理器并继续。
timeout 3 > NUL
start \\192.168.72.125\vcpkg
timeout 3 > NUL
pause
net use \\192.168.72.125\vcpkg /user:ADMIN LSpower
set SRC_DIR=\\192.168.72.125\vcpkg\vcpkg

:SET_DEST
set /p DEST_DIR=请输入目标路径:
if not exist %DEST_DIR% (
    echo 输入路径不存在！
    goto SET_DEST
)

robocopy %SRC_DIR% "%DEST_DIR%\vcpkg" /e /mt:16 /z /xd downloads /xd buildtrees /xx /xj /np /Ns /nc /nfl /ndl
IF %ERRORLEVEL% GEQ 8 GOTO ERROR

net use \\192.168.72.125\vcpkg /del /y > NUL

setx VCPKG_ROOT "%DEST_DIR%/vcpkg"

echo VCPKG环境复制完成。
pause
exit 0

:ERROR
echo VCPKG环境复制失败。
pause
exit /b 1