@ECHO ON
IF [%1]==[--fortify] (
    (set fortify=sourceanalyzer -b tesseract)
    shift
) else (
    (set fortify=)
)
if [%1]==[--coverity] (
    (set coverity=c:\cygwin64\bin\python2.7.exe /usr/bin/coverity check --)
    shift
) else (
    (set coverity=)
)
IF [%1]==[] (
    (set vcvarsallq="%programfiles% (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvarsall.bat")
) ELSE (
    (set vcvarsallq="%~1")
)
(SET projdir=%CD%)
CALL %vcvarsallq% x86 || GOTO ERROR
@ECHO ON
CD "%projdir%"

rmdir /S /Q Release > nul 2>&1
rmdir /S /Q Debug > nul 2>&1

IF NOT [%fortify%]==[] (
    %fortify% -clean || GOTO ERROR
)

%fortify% %coverity% msbuild tesseract.sln /t:Rebuild /p:Configuration=Debug /p:Platform=x86 || GOTO ERROR

IF NOT [%fortify%]==[] (
    %fortify% -scan -format fpr -f tesseract.fpr || GOTO ERROR
)

@GOTO END
:ERROR
@ECHO Program failed, please check this log file for errors ...
@ECHO Errorlevel: %errorlevel%
@EXIT /B %errorlevel%
:END
