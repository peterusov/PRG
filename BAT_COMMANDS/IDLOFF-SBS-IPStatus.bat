@echo off
:loop
set host=idlofficeinc.com
ping -n 1 %host% | find "TTL"
if not errorlevel 1 set error=%host% is UP
if errorlevel 1 set error=%host% is Down
cls
echo Result: %error%

IF "%error%"=="down" GOTO loop
 
pause