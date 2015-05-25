@echo off 

set archiveDir=E:\WEB_APP\COM\IDLICENSE\WWW\UPLOADS\
set webDir=C:\WEB_APP\COM\IDLICENSE\WWW\UPLOADS\
set VPNDir=\\tsclient\R\200000-250000\
set wildcards=*.*

:MENU
ECHO.
ECHO -----------------------------------------------
ECHO PRESS 1 OR 2 to select your task, or 3 to EXIT.
ECHO ----------------------------------------------
ECHO.
ECHO 1 - Copy applications' files from Archive to Web and VPN;
ECHO 2 - Copy applications' files from Web to VPN;
ECHO 3 - EXIT
ECHO.
SET /P M=Type 1, 2 or 3 for EXIT, then press ENTER:
IF %M%==1 GOTO START
IF %M%==2 GOTO VPNONLY
IF %M%==3 GOTO EOF

:START
CLS
REM ============================
set /P appNumber=Enter application number:
REM =============================

COPY %archiveDir%%appNumber%%wildcards% %webDir%

echo.  
echo ---- Copy to VPN Folder --------
echo.

IF NOT EXIST %VPNDir% GOTO NOWINDIR
COPY %archiveDir%%appNumber%%wildcards% %VPNDir%   
GOTO MENU

:VPNONLY
CLS
set /P appNumber=Enter application number for copy from Web to VPN:
COPY %webDir%%appNumber%%wildcards% %VPNDir%
GOTO MENU


:NOWINDIR
echo.
echo Can't copy to VPN directory: %VPNDir% IS NOT EXIST!!!
echo.
GOTO MENU

 
:EOF
