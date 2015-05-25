@echo off 

REM ignoreDir=E:\WEB_APP\COM\IDLICENSE\WWW\UPLOADS\
REM gitignoreFileLocation=C:\WEB_APP\COM\IDLICENSE\WWW\UPLOADS\

:MENU
ECHO.
ECHO -----------------------------------------------
ECHO PRESS 1 OR 2 to select your task, or 3 to EXIT.
ECHO ----------------------------------------------
ECHO.
ECHO 1 - Create a file named .gitignore in directory;
ECHO 2 - Not active yet;
ECHO 3 - EXIT
ECHO.
SET /P M=Type 1, 2 or 3 for EXIT, then press ENTER:
IF %M%==1 GOTO START
IF %M%==2 GOTO EMPTY
IF %M%==3 GOTO EOF


:START
CLS
REM ============================
set /P rootIgnoreDir=Enter root Ignore Directory with backslash (\) on end :
REM =============================

IF NOT EXIST %rootIgnoreDir% GOTO NOWINDIR

DIR %rootIgnoreDir% /ad /b >  %rootIgnoreDir%.gitignore 
GOTO MENU


:NOWINDIR
echo.
echo  %rootIgnoreDir% IS NOT EXIST !!!
echo.
GOTO MENU




:EOF
