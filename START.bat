@echo off
title GSV VARMA IVR — Dashboard Server
color 0A
echo.
echo  ╔══════════════════════════════════════════╗
echo  ║   GSV ELECTRICAL — V-VARMA IVR SYSTEM   ║
echo  ║   Starting Web Dashboard Server...       ║
echo  ╚══════════════════════════════════════════╝
echo.
echo  Opening: http://localhost:3000
echo  Login  : admin / GSVIVR001
echo.
start "" "http://localhost:3000"
cd /d "%~dp0web\backend"
npm start
pause
