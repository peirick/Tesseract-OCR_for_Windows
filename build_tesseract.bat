git submodule update --init --recursive
@if %errorlevel% NEQ 0 GOTO ERROR

call "%programfiles% (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86
@if %errorlevel% NEQ 0 GOTO ERROR

rmdir /S /Q Binaries
@if %errorlevel% NEQ 0 GOTO ERROR

msbuild tesseract.sln /p:Configuration=Release /p:Platform=x86 /m
@if %errorlevel% NEQ 0 GOTO ERROR

@GOTO END
:ERROR
@ECHO "Program failed, please check this log file for errors ..." 
@ECHO Errorlevel: %errorlevel%
@EXIT /B %errorlevel%
:END