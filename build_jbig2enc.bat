call "%programfiles% (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" x86
msbuild jbig2enc.sln /p:Configuration=Release /p:Platform=x86 /m
