git submodule update --init --recursive
@if %errorlevel% NEQ 0 GOTO ERROR

rmdir /S /Q Release
@if %errorlevel% NEQ 0 GOTO ERROR

msbuild tesseract.sln /p:Configuration=Release /p:Platform=x64 /m
@if %errorlevel% NEQ 0 GOTO ERROR

rmdir /S /Q Debug
@if %errorlevel% NEQ 0 GOTO ERROR

msbuild tesseract.sln /p:Configuration=Debug /p:Platform=x64 /m
@if %errorlevel% NEQ 0 GOTO ERROR

@GOTO END
:ERROR
@ECHO "Program failed, please check this log file for errors ..." 
@ECHO Errorlevel: %errorlevel%
@EXIT /B %errorlevel%
:END