@echo off
cd %CD%
For /f "tokens=2-4 delims=/ " %%a in ('date /t') do (set mydate=%%c-%%a-%%b)
For /f "tokens=1-2 delims=/:" %%a in ("%TIME%") do (set mytime=%%a%%b)
cd ./data
mkdir log
mkdir captures
mkdir output
cd ../
cd ./data/log/
mkdir %mydate%
cd ../../
PhotoBooth > ./data/log/%mydate%/log_%mydate%_%mytime%.log