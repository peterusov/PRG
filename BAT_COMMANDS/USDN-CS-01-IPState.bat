@echo off
:loop
set host=192.168.13.12
ping -n 1 %host% | find "TTL"
if not errorlevel 1 set error=up
if errorlevel 1 set error=down
cls
echo Result: %error%

IF "%error%"=="down" GOTO loop
 
pause