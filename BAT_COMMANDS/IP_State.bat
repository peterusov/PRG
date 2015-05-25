@echo off
cls
set ip=%1
ping -n 1 %ip% | find "TTL"
if not errorlevel 1 set error=win
if errorlevel 1 set error=fail
cls
echo Result: %error%
pause