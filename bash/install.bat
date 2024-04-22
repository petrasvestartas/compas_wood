@echo off
setlocal

echo Installing Anaconda...
bitsadmin /transfer "AnacondaDownload" /download /priority normal https://repo.anaconda.com/archive/Anaconda3-2023.01-Windows-x86_64.exe "%cd%\Anaconda3-Installer.exe"
start /wait "" "%cd%\Anaconda3-Installer.exe" /InstallationType=JustMe /RegisterPython=0 /S /D=%UserProfile%\Anaconda3

echo Installing Visual Studio Code...
bitsadmin /transfer "VSCodeDownload" /download /priority normal https://update.code.visualstudio.com/latest/win32-x64/stable "%cd%\VSCodeSetup.exe"
start /wait "" "%cd%\VSCodeSetup.exe" /VERYSILENT /MERGETASKS=!runcode

echo Checking for C:\code directory...
if not exist "C:\code\" mkdir "C:\code"

echo Cloning the repository...
cd /d C:\code
git clone https://github.com/petrasvestartas/compas_wood.git
cd compas_wood

echo Creating Anaconda environment...
call %UserProfile%\Anaconda3\Scripts\conda.bat create -n compas_wood_3_9_10 python=3.9.10 compas -y

echo Activating environment...
call %UserProfile%\Anaconda3\Scripts\activate.bat compas_wood_3_9_10

echo Installing Python requirements...
pip install -r requirements.txt
pip install -e .

echo Cleaning up installation files...
del "%cd%\Anaconda3-Installer.exe"
del "%cd%\VSCodeSetup.exe"

echo All set!
pause
