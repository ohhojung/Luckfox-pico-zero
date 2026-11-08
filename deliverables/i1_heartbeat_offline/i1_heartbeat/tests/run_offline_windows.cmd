@echo off
setlocal
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat" >nul
if errorlevel 1 exit /b 1
cd /d "%~dp0.."
if not exist build\tests mkdir build\tests
cl /nologo /W4 /WX /D_CRT_SECURE_NO_WARNINGS /TC tests\test_protocol.c src\protocol.c /Febuild\tests\test_protocol.exe /Fobuild\tests\
if errorlevel 1 exit /b 1
build\tests\test_protocol.exe
exit /b %errorlevel%
