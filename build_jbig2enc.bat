reg Query "HKLM\Hardware\Description\System\CentralProcessor\0" | find /i "x86" > NUL && set OS=32BIT || set OS=64BIT
@if %OS%==32BIT call "%programfiles%\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86
@if %OS%==64BIT call "%programfiles% (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86

msbuild jbig2enc.sln /p:Configuration=Release /p:Platform=x86 /m
