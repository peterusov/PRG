@echo off 
REM ============================
set appNumber=207350
REM =============================

set directory=W:\IDLPIC\
set wildcards=*.*


echo The variable is %directory%%appFiles%%wildcards%
DIR %directory%%appNumber%%wildcards%

PAUSE