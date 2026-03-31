@echo off
for /f "tokens=1-4 delims=/ " %%a in ("%date%") do set mydate=%%c-%%a-%%b
for /f "tokens=1-2 delims=: " %%a in ("%time%") do set mytime=%%a%%b
set timestamp=%mydate%_%mytime%
git add .
git commit -m "backup: %timestamp%"
git push origin main
pause